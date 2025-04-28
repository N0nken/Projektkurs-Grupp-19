#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "../include/input_logger.h"
#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/movement.h"

#define MAXCLIENTS 4
#define CLIENTPORT 50000
#define SERVERPORT 50001
#define MAXPACKETSRECEIVEDPERFRAME 4
#define TARGETFPS 60
#define GAMEOVERCOOLDOWN 30000 // 30 seconds cooldown before match restarts after game over
#define GAMESTARTTIMER 15000 // 15 seconds timer before match starts after all players are connected

enum MatchStates {WAITING,PLAYING,GAME_OVER};

struct Server {
    int clientCount;
    IPaddress clientIPs[MAXCLIENTS];
    UDPsocket socket;
    UDPpacket *sendPacket;
    UDPpacket *recvPacket;
    int framesSinceLatestPacket[MAXCLIENTS]; // Number of frames since the last packet was received from each client. Assume client has disconnected when it reaches X frames.
    int packetsReceived; // Number of packets received in the current frame
    Uint64 gameOverStartTime;    // timer for server
}; typedef struct Server Server;

struct GameState {
    int matchState; // waiting, playing, game over
    int playerAliveCount; // number of players left alive
    int winnerID; // identify winner
    Player *players[MAXCLIENTS]; // array of players
}; typedef struct GameState GameState;

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
    int playerAliveCount; //send info about how many players are alive
    int winnerID;
    int gameOverTimerMs; // how many milisec until game restarts
}; typedef struct SimulationData SimulationData;

int server_main();
int init_server();

void server_waiting();
void server_playing();
void server_game_over();

void send_server_game_state_to_all_clients();
void receive_player_inputs();
int save_client();
int get_player_id_from_ip();

void show_debug_info_server(GameState *gameState, Server *server) {
    printf("Match State: %d\n", gameState->matchState);
    printf("Alive Players: %d\n", gameState->playerAliveCount);
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (gameState->players[i]) {
            printf("Player %d - HP: %d, Weapon: %d, Position: (%.2f, %.2f)\n", i, Player_get_hp(gameState->players[i]), Player_get_weapon(gameState->players[i]), 
                Vector2_get_x(Player_get_position(gameState->players[i])), Vector2_get_y(Player_get_position(gameState->players[i])));
            InputLogger_print_inputs(Player_get_inputs(gameState->players[i]));
        }
    }
}

int server_main() {
    Server server;
    GameState gameState;
    if (init_server(&server, &gameState)) {
        return 1;
    }
    
    int running = 1;
    while (running) {
        server.clientCount = 0; // Reset client count for each new game
        gameState.matchState = WAITING;
        server_waiting(&server, &gameState);
        server_playing(&server, &gameState);
        server_game_over(&server, &gameState);
    }
    // Cleanup
    SDLNet_FreePacket(server.sendPacket);
    SDLNet_FreePacket(server.recvPacket);
    SDLNet_UDP_Close(server.socket);
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (gameState.players[i] != NULL) {
            destroy_Player(gameState.players[i]);
        }
    }
    return 0;
}

int init_server(Server *server, GameState *gameState) {
    server->socket = SDLNet_UDP_Open(SERVERPORT);
    if (!server->socket) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return 1;
    }
    server->sendPacket = SDLNet_AllocPacket(512);
    if (!server->sendPacket) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }
    server->recvPacket = SDLNet_AllocPacket(512);
    if (!server->recvPacket) {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }
    server->clientCount = 0;

    gameState->matchState = WAITING;
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

void server_waiting(Server *server, GameState *gameState) {
    // Wait for players to connect
    gameState->matchState = WAITING;
    while (server->clientCount < MAXCLIENTS) {
        while (SDLNet_UDP_Recv(server->socket, server->recvPacket)) {
            save_client(server, server->recvPacket->address);
        }
        send_server_game_state_to_all_clients(server, gameState);
        printf("Waiting for players... (%d)\n", server->clientCount);
        SDL_Delay(500); // Wait for 1 second before checking again
    }
    // All players connected, start the game after a short countdown
    Uint64 deltaTime = SDL_GetTicks64();
    int totalTime = 0;
    while (totalTime < GAMESTARTTIMER) {
        deltaTime = SDL_GetTicks64() - deltaTime;
        totalTime += deltaTime;
        receive_player_inputs(server, gameState);
        send_server_game_state_to_all_clients(server, gameState);
    }
    gameState->matchState = PLAYING;
}

void server_playing(Server *server, GameState *gameState) {
    // Game logic for playing state
    Collider *platform1 = create_Collider(create_Vector2(100, 410), create_Vector2(120, 20), 0, 1);  
    Collider *platform2 = create_Collider(create_Vector2(290, 410-100), create_Vector2(120, 20), 0, 1);  //x led fungerar tvärtom i collider och sdl rect
    Collider *platform3 = create_Collider(create_Vector2(480, 410), create_Vector2(120, 20), 0, 1);  //x led fungerar tvärtom i collider och sdl rect
    Uint64 lastTicks = SDL_GetTicks64();
    while (gameState->matchState == PLAYING) {
        receive_player_inputs(server, gameState);
        show_debug_info_server(gameState, server);
        // Update game state logic here
        Uint64 currentTicks = SDL_GetTicks64();             // nu i ms
        Uint64 elapsedTicks = currentTicks - lastTicks;     // skillnad i ms
        lastTicks = currentTicks;
        float deltaTime = elapsedTicks * 0.001f;
        // run simulation
        for (int i = 0; i < MAXCLIENTS; i++) {
            handle_movement(gameState->players[i], PLAYERSPEED, platform1, platform2, platform3, deltaTime);
            // handle_weapon_switching(gameState->players[i]);
        }
        handle_attack_input(gameState->players, MAXCLIENTS);

        int alive = 0;
        int lastAlive = -1;
        for(int i = 0; i < MAXCLIENTS; i++){
            if(Player_get_isAlive(gameState->players[i])){
                alive++;
                lastAlive = i;
            }
        }
        gameState->playerAliveCount = alive;

        if(alive <= 1){ //if only 1 player is left -> GAME_OVER
            gameState->matchState = GAME_OVER;
            gameState->winnerID = lastAlive;
            server->gameOverStartTime = SDL_GetTicks64();
            send_server_game_state_to_all_clients(server, gameState);
            break;
        }

        // Broadcast game state to all clients
        send_server_game_state_to_all_clients(server, gameState);
        SDL_Delay(1000 / TARGETFPS); // Run at 60 FPS
    }
}

void server_game_over(Server *server, GameState *gameState) {
    Uint64 start = server->gameOverStartTime;

    // Skicka GAME_OVER en första gång
    send_server_game_state_to_all_clients(server, gameState);

    // Vänta 5 sekunder och skicka state varje frame
    while (SDL_GetTicks64() - start < 5000) {
        SDL_Delay(1000 / TARGETFPS);
        send_server_game_state_to_all_clients(server, gameState);
    }

    //gå tillbaka till WAITING
    gameState->matchState = WAITING;
}

int save_client(Server *server, IPaddress ip) {
    for (int i = 0; i < server->clientCount; i++) {
        if (server->clientIPs[i].host == ip.host && server->clientIPs[i].port == ip.port) return -1; // Client already exists}
    }
    server->clientIPs[(server->clientCount)++] = ip;
    return 1; // Client saved successfully
}

void receive_player_inputs(Server *server, GameState *gameState) {
    ClientInput clientInput;
    while (SDLNet_UDP_Recv(server->socket, server->recvPacket) && server->packetsReceived < MAXPACKETSRECEIVEDPERFRAME) {
        printf("Packet received from %s\n", SDLNet_ResolveIP(&server->recvPacket->address));
        int playerID = get_player_id_from_ip(server, server->recvPacket->address);
        if (playerID == -1) {
            save_client(server, server->recvPacket->address); // Player not found, save new client
            playerID = server->clientCount - 1; // Get the new player ID
        }
        if(sizeof(server->recvPacket->data) != sizeof(ClientInput)){
            continue;
        }
        memcpy(&clientInput, server->recvPacket->data, sizeof(ClientInput));
        InputLogger *playerInputLogger = Player_get_inputs(gameState->players[playerID]);
        for (int i = 0; i < 3; i++) {
            InputLogger_set_action_state(playerInputLogger, "move_up", i, clientInput.up[i]);
            InputLogger_set_action_state(playerInputLogger, "move_up", i, clientInput.down[i]);
            InputLogger_set_action_state(playerInputLogger, "move_left", i, clientInput.left[i]);
            InputLogger_set_action_state(playerInputLogger, "move_right", i, clientInput.right[i]);
            InputLogger_set_action_state(playerInputLogger, "attack", i, clientInput.attack[i]);
            InputLogger_set_action_state(playerInputLogger, "switch_to_rock", i, clientInput.switchToRock[i]);
            InputLogger_set_action_state(playerInputLogger, "switch_to_paper", i, clientInput.switchToPaper[i]);
            InputLogger_set_action_state(playerInputLogger, "switch_to_scissors", i, clientInput.switchToScissors[i]);
        }
    }
}

int get_player_id_from_ip(Server *server, IPaddress ip) {
    for (int i = 0; i < server->clientCount; i++) {
        if (server->clientIPs[i].host == ip.host && server->clientIPs[i].port == ip.port) {
            return i;
        }
    }
    return -1; // Player not found
}

void send_server_game_state_to_all_clients(Server *server, GameState *gameState) {
    SimulationData simulationData;
    simulationData.matchState = gameState->matchState;
    simulationData.playerAliveCount = gameState->playerAliveCount;
    simulationData.winnerID = gameState->winnerID;

    int remMs = 0;
    if (gameState->matchState == GAME_OVER) {
        Uint64 elapsed = SDL_GetTicks64() - server->gameOverStartTime;
        remMs = (elapsed < 5000 ? (int)(5000 - elapsed) : 0);
    }
    simulationData.gameOverTimerMs = remMs;

    // Prepare simulation status packet and...
    for (int j = 0; j < MAXCLIENTS; j++) {
        if (gameState->players[j]) {
            simulationData.players[j].isAlive = Player_get_isAlive(gameState->players[j]);
            simulationData.players[j].hp = Player_get_hp(gameState->players[j]);
            simulationData.players[j].weapon = Player_get_weapon(gameState->players[j]);
            simulationData.players[j].posX = Vector2_get_x(Player_get_position(gameState->players[j]));
            simulationData.players[j].posY = Vector2_get_y(Player_get_position(gameState->players[j]));
            simulationData.players[j].direction = Player_get_direction(gameState->players[j]);
            simulationData.players[j].state = Player_get_state(gameState->players[j]);
            printf("%d %d %d %.2f %.2f %d\n", simulationData.players[j].isAlive, simulationData.players[j].hp, simulationData.players[j].weapon, simulationData.players[j].posX, simulationData.players[j].posY, simulationData.players[j].direction);
        }
    }
    // ...send to each client
    for (int i = 0; i < MAXCLIENTS; i++) {
        simulationData.playerID = i;
        server->sendPacket->address = server->clientIPs[i];
        server->sendPacket->len = sizeof(SimulationData);
        memcpy(server->sendPacket->data, &simulationData, sizeof(SimulationData));
        SDLNet_UDP_Send(server->socket, -1, server->sendPacket);
    }
    ClientInput clientInputs;
    for (int i = 0; i < MAXCLIENTS; i++) {
        // Prepare client inputs packet and...
        clientInputs.playerID = i;
        for (int j = 0; j < 3; j++) {
            clientInputs.up[j] = InputLogger_get_action_state(Player_get_inputs(gameState->players[i]), "move_up", j);
            clientInputs.down[j] = InputLogger_get_action_state(Player_get_inputs(gameState->players[i]), "move_down", j);
            clientInputs.left[j] = InputLogger_get_action_state(Player_get_inputs(gameState->players[i]), "move_left", j);
            clientInputs.right[j] = InputLogger_get_action_state(Player_get_inputs(gameState->players[i]), "move_right", j);
            clientInputs.attack[j] = InputLogger_get_action_state(Player_get_inputs(gameState->players[i]), "attack", j);
            clientInputs.switchToRock[j] = InputLogger_get_action_state(Player_get_inputs(gameState->players[i]), "switch_to_rock", j);
            clientInputs.switchToPaper[j] = InputLogger_get_action_state(Player_get_inputs(gameState->players[i]), "switch_to_paper", j);
            clientInputs.switchToScissors[j] = InputLogger_get_action_state(Player_get_inputs(gameState->players[i]), "switch_to_scissors", j);
        }
        // ...load packet and...
        server->sendPacket->len = sizeof(ClientInput);
        memcpy(server->sendPacket->data, &clientInputs, sizeof(ClientInput));
        // ...send to each client...
        for (int j = 0; j < MAXCLIENTS; j++) {
            if (i == j) continue; // ...except for the client whose input it is
            server->sendPacket->address = server->clientIPs[j];
            SDLNet_UDP_Send(server->socket, -1, server->sendPacket);
        }
    }
}