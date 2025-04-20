#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "../include/input_logger.h"
#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/movement.h"
#include "../include/render_controller.h"

#define PACKETLOSSLIMIT 10 // Give up sending packets to server after this many failed attempts
#define MAXCLIENTS 4
#define CLIENTPORT 50000
#define SERVERPORT 50001
#define MAXPACKETSRECEIVEDPERFRAME 4

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

void client_waiting();
void client_playing();
void client_game_over();

int send_player_input();
void sync_game_state_with_server();

int client_main() {
    Client client;
    GameState gameState;
    RenderController *renderController;

    if (init_client(&client, &gameState)) return 1;
    if (init_rendering(renderController)) return 1; // Initialize rendering (if needed)

    while (1) {
        client_waiting(&client, &gameState, renderController);
        client_playing(&client, &gameState, renderController);
        client_game_over(&client, &gameState, renderController);
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

int init_rendering(RenderController *renderController) {
    
}

void client_waiting(Client *client, GameState *gameState, RenderController *renderController) {
    char targetIPaddress[16];
    printf("Enter server IP address: ");
    scanf("%s", &targetIPaddress);
    SDLNet_ResolveHost(&client->serverIP, targetIPaddress, SERVERPORT);
    printf("server: %s\n", SDLNet_ResolveIP(&client->serverIP));
    while (gameState->matchState == WAITING) {
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
}

void client_playing(Client *client, GameState *gameState, RenderController *renderController) {
    Collider *ground = create_Collider(create_Vector2(400, 400), create_Vector2(400, 10), 0, GROUNDCOLLISIONLAYER);
    while (gameState->matchState == PLAYING) {
        // sync simulation with server
        sync_game_state_with_server(client, gameState);
        // Handle player input
        printf("updating client input\n");
        InputLogger_update_all_actions(Player_get_inputs(gameState->players[gameState->playerID]), SDL_GetKeyboardState(NULL));
        printf("finished updating client input\n");
        while (!send_player_input(client, gameState) && client->failedPackets < PACKETLOSSLIMIT) {
            printf("Failed to send player input\n");
            client->failedPackets++;
        }
        
        // run simulation
        for (int i = 0; i < MAXCLIENTS; i++) {
            printf("A---");
            handle_movement(gameState->players[i], PLAYERSPEED, ground); // Assuming ground is NULL for now
            printf("B---");
            // handle_weapon_switching(player);
        }
        handle_attack_input(gameState->players, MAXCLIENTS);
        printf("C---\n");
        // Render current frame
        

        SDL_Delay(1000 / 60); // Run at 60 FPS
    }
}

void client_game_over(Client *client, GameState *gameState, RenderController *renderController) {

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
            gameState->playerAliveCount = 0;
            for (int i = 0; i < MAXCLIENTS; i++) {
                if (!simulationData.players[i].isAlive) {
                    continue;
                }
                gameState->playerAliveCount++;
                //printf("%d %d %d %d %.2f %.2f %d\n", simulationData.players[i].isAlive, simulationData.players[i].hp, simulationData.players[i].weapon, simulationData.players[i].posX, simulationData.players[i].posY, simulationData.players[i].direction);
                Player_set_isAlive(gameState->players[i], simulationData.players[i].isAlive);
                Player_set_hp(gameState->players[i], simulationData.players[i].hp);
                Player_set_weapon(gameState->players[i], simulationData.players[i].weapon);
                Vector2_set_x(Player_get_position(gameState->players[i]), simulationData.players[i].posX);
                Vector2_set_y(Player_get_position(gameState->players[i]), simulationData.players[i].posY);
                Player_set_direction(gameState->players[i], simulationData.players[i].direction);
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