#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

#include "../include/input_logger.h"
#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/movement.h"

#define PACKETLOSSLIMIT 10 // Give up sending packets to server after this many failed attempts
#define MAXCLIENTS 4
#define CLIENTPORT 50000
#define SERVERPORT 50001
#define MAXPACKETSRECEIVEDPERFRAME 10
#define TARGETFPS 60

enum MatchStates {WAITING,PLAYING,GAME_OVER};

struct Client {
    IPaddress serverIP;
    UDPsocket socket;
    UDPpacket *sendPacket;
    UDPpacket *recvPacket;
    int failedPackets; // Number of packets that failed to send
    int packetsReceived; // Number of packets received in the current frame
}; typedef struct Client Client;

struct GameState {
    int playerID; // ID of the clients player
    int matchState; // waiting, playing, game over
    int playerAliveCount; // number of players left alive
    Player *players[MAXCLIENTS]; // array of players
}; typedef struct GameState GameState;

struct UDPplayer {
    int isAlive;
    int hp;
    int weapon;
    float posX;
    float posY;
    int direction;
    int state; // 0 = idle, 1 = moving, 2 = attacking
}; typedef struct UDPplayer UDPplayer;

struct SimulationData {
    int matchState; // waiting, playing, game over
    UDPplayer players[MAXCLIENTS];
    int playerID; // ID of the player receiving the data
}; typedef struct SimulationData SimulationData;

struct ClientInput {
    int playerID; // ID of the player whose input is being sent. = -1 when sent from client
    int up[3];
    int down[3];
    int left[3];
    int right[3];
    int attack[3];
    int switchToRock[3];
    int switchToPaper[3];
    int switchToScissors[3];
}; typedef struct ClientInput ClientInput;

int client_main();
int init_client();

int client_waiting();
int client_playing();
int client_game_over();

int send_player_input();
void sync_game_state_with_server();

void show_debug_info_client(GameState *gameState, Client *client) {
    printf("Player ID: %d\n", gameState->playerID);
    printf("Match State: %d\n", gameState->matchState);
    printf("Alive Players: %d\n", gameState->playerAliveCount);
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (gameState->players[i]) {
            printf("Player %d - HP: %d, Weapon: %d, Position: (%.2f, %.2f)\n", i, Player_get_hp(gameState->players[i]), Player_get_weapon(gameState->players[i]), 
                Vector2_get_x(Player_get_position(gameState->players[i])), Vector2_get_y(Player_get_position(gameState->players[i])));
        }
        printf("Player %d ", gameState->playerID);
        InputLogger_print_inputs(Player_get_inputs(gameState->players[i]));
    }
}

int client_main() {
    Client client;
    GameState gameState;

    if (init_client(&client, &gameState)) return 1;

    while (1) {
        if(client_waiting(&client, &gameState)) {
            break;
        }
        if(client_playing(&client, &gameState)) {
            break;
        }
        if(client_game_over(&client, &gameState)) {
            break;
        }
    }

    return 0;
}

int init_client(Client *client, GameState *gameState) {
    client->socket = SDLNet_UDP_Open(0);
    if (!client->socket) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return 1;
    }
    client->sendPacket = SDLNet_AllocPacket(512);
    if (!client->sendPacket) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }
    client->recvPacket = SDLNet_AllocPacket(512);
    if (!client->recvPacket) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }
    gameState->matchState = WAITING;
    gameState->playerID = -1;
    gameState->playerAliveCount = 0;
    for (int i = 0; i < MAXCLIENTS; i++) {
        gameState->players[i] = create_Player(create_Vector2(0, 0), 
            create_Collider(create_Vector2(0, 0), create_Vector2(PLAYERWIDTH, PLAYERHEIGHT), 0, PLAYERCOLLISIONLAYER), 
            create_Collider(create_Vector2(PLAYERATTACKHITBOXOFFSETX, PLAYERATTACKHITBOXOFFSETY), create_Vector2(PLAYERATTACKHITBOXWIDTH, PLAYERATTACKHITBOXHEIGHT), 0, PLAYERATTACKLAYER), 
            100, 0, 1, gameState->players, &gameState->playerAliveCount);
        InputLogger_reset_all_actions(Player_get_inputs(gameState->players[i]));
    }
    return 0;
}

int client_waiting(Client *client, GameState *gameState) {
    char targetIPaddress[16];
    printf("Enter server IP address: ");
    scanf("%s", &targetIPaddress);
    SDLNet_ResolveHost(&client->serverIP, targetIPaddress, SERVERPORT);
    printf("server: %s\n", SDLNet_ResolveIP(&client->serverIP));
    SDL_Event event;
    while (gameState->matchState == WAITING) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return 1;
            }
        }
        if (gameState->playerID == -1) {
            printf("Attempting to connect to server...\n");
            client->sendPacket->address = client->serverIP;
            client->sendPacket->len = sizeof(1); // No data to send
            *(client->sendPacket->data) = 1;
            SDLNet_UDP_Send(client->socket, -1, client->sendPacket);
        } else {
            printf("Connected to server %s\n", SDLNet_ResolveIP(&client->serverIP));
            send_player_input(client, gameState);
            printf("Player input sent\n");
        }
        sync_game_state_with_server(client, gameState);
        SDL_Delay(1000);
    }
    return 0;
}

int client_playing(Client *client, GameState *gameState) {
    SDL_Window *window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALWAYS_ON_TOP);
    if (!window) {                           /* ← Fångar fel! */
        SDL_Log("CreateWindow: %s", SDL_GetError());
        return 1;
    }
    SDL_RaiseWindow(window);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, /*0*/ SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {                                  /* <— FELKOLL */
        SDL_Log("CreateRenderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }
    SDL_Surface *background = IMG_Load("images/background.png");
    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
    Collider *ground = create_Collider(create_Vector2(400, 400), create_Vector2(400, 10), 0, GROUNDCOLLISIONLAYER);
    SDL_Event event;
    while (gameState->matchState == PLAYING) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    gameState->matchState = GAME_OVER;
                    return 1;
            }
        }

        show_debug_info_client(gameState, client);
        // Update player input...
        if (Player_get_isAlive(gameState->players[gameState->playerID])) {
            InputLogger_update_all_actions(Player_get_inputs(gameState->players[gameState->playerID]), SDL_GetKeyboardState(NULL));
            InputLogger_print_inputs(Player_get_inputs(gameState->players[gameState->playerID]));
            // ...and send it to the server
            while (!send_player_input(client, gameState) && client->failedPackets < PACKETLOSSLIMIT) {
                printf("Failed to send player input\n");
                client->failedPackets++;
            }
        } else {
            printf("Player %d is dead\n", gameState->playerID);
        }
        
        // run simulation
        for (int i = 0; i < MAXCLIENTS; i++) {
            handle_movement(gameState->players[i], PLAYERSPEED, ground);
            // handle_weapon_switching(gameState->players[i]);
        }
        handle_attack_input(gameState->players, MAXCLIENTS);

        // sync simulation with server
        sync_game_state_with_server(client, gameState);

        // Render current frame
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
        
        SDL_Delay(1000 / TARGETFPS); // Run at target FPS
    }
    return 0;
}

int client_game_over(Client *client, GameState *gameState) {
    SDL_Event event;
    while (gameState->matchState == GAME_OVER) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return 1;
            }
        }
        // Render game over screen
        printf("Game Over\n");
        SDL_Delay(1000 / TARGETFPS); // Run at target FPS
    }
    return 0;
}

int send_player_input(Client *client, GameState *gameState) {
    ClientInput clientInput;
    printf("Preparing sending packet\n");
    InputLogger *playerInputLogger = Player_get_inputs(gameState->players[gameState->playerID]);
    clientInput.playerID = -1;
    for (int i = 0; i < 3; i++) {
        clientInput.up[i] = InputLogger_get_action_state(playerInputLogger, "move_up", i);
        clientInput.down[i] = InputLogger_get_action_state(playerInputLogger, "move_down", i);
        clientInput.left[i] = InputLogger_get_action_state(playerInputLogger, "move_left", i);
        clientInput.right[i] = InputLogger_get_action_state(playerInputLogger, "move_right", i);
        clientInput.attack[i] = InputLogger_get_action_state(playerInputLogger, "attack", i);
        clientInput.switchToRock[i] = InputLogger_get_action_state(playerInputLogger, "switch_to_rock", i);
        clientInput.switchToPaper[i] = InputLogger_get_action_state(playerInputLogger, "switch_to_paper", i);
        clientInput.switchToScissors[i] = InputLogger_get_action_state(playerInputLogger, "switch_to_scissors", i);
    }
    memcpy(client->sendPacket->data, &clientInput, sizeof(ClientInput));
    client->sendPacket->address = client->serverIP;
    client->sendPacket->len = sizeof(clientInput);
    printf("sending packet\n");
    return SDLNet_UDP_Send(client->socket, -1, client->sendPacket);
}

void sync_game_state_with_server(Client *client, GameState *gameState) {
    SimulationData simulationData;
    ClientInput clientInput;
    // Receive all packets from the server
    while (SDLNet_UDP_Recv(client->socket, client->recvPacket) && client->packetsReceived < MAXPACKETSRECEIVEDPERFRAME) {
        client->packetsReceived++;
        // sync player data
        if (client->recvPacket->len == sizeof(SimulationData)) {
            printf("Simulation packet received from: %s\n", SDLNet_ResolveIP(&client->recvPacket->address));
            memcpy(&simulationData, client->recvPacket->data, sizeof(SimulationData));
            gameState->matchState = simulationData.matchState;
            gameState->playerID = simulationData.playerID;
            gameState->playerAliveCount = MAXCLIENTS;
            for (int i = 0; i < MAXCLIENTS; i++) {
                if (simulationData.players[i].isAlive == 0) {
                    gameState->playerAliveCount--;
                    continue;
                }
                //printf("%d %d %d %d %.2f %.2f %d\n", simulationData.players[i].isAlive, simulationData.players[i].hp, simulationData.players[i].weapon, simulationData.players[i].posX, simulationData.players[i].posY, simulationData.players[i].direction);
                Player_set_isAlive(gameState->players[i], simulationData.players[i].isAlive);
                Player_set_hp(gameState->players[i], simulationData.players[i].hp);
                Player_set_weapon(gameState->players[i], simulationData.players[i].weapon);
                Vector2_set_x(Player_get_position(gameState->players[i]), simulationData.players[i].posX);
                Vector2_set_y(Player_get_position(gameState->players[i]), simulationData.players[i].posY);
                Player_set_direction(gameState->players[i], simulationData.players[i].direction);
                printf("%d %d %d %.2f %.2f %d\n", simulationData.players[i].isAlive, simulationData.players[i].hp, simulationData.players[i].weapon, simulationData.players[i].posX, simulationData.players[i].posY, simulationData.players[i].direction);
            }
        // sync player input data
        } else if (client->recvPacket->len == sizeof(ClientInput)) {
            printf("Client input packet received from: %s\n", SDLNet_ResolveIP(&client->recvPacket->address));
            memcpy(&clientInput, client->recvPacket->data, sizeof(ClientInput));
            InputLogger *targetLogger = Player_get_inputs(gameState->players[clientInput.playerID]);
            for (int i = 0; i < 3; i++) {
                InputLogger_set_action_state(targetLogger, "move_up", i, clientInput.up[i]);
                InputLogger_set_action_state(targetLogger, "move_down", i, clientInput.down[i]);
                InputLogger_set_action_state(targetLogger, "move_left", i, clientInput.left[i]);
                InputLogger_set_action_state(targetLogger, "move_right", i, clientInput.right[i]);
                InputLogger_set_action_state(targetLogger, "attack", i, clientInput.attack[i]);
                InputLogger_set_action_state(targetLogger, "switch_to_rock", i, clientInput.switchToRock[i]);
                InputLogger_set_action_state(targetLogger, "switch_to_paper", i, clientInput.switchToPaper[i]);
                InputLogger_set_action_state(targetLogger, "switch_to_scissors", i, clientInput.switchToScissors[i]);
            }
        } else {
            printf("Unknown packet type received\n");
        }
    }
    client->packetsReceived = 0;
}