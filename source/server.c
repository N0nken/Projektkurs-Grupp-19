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

enum MatchStates {WAITING,PLAYING,GAME_OVER};

struct Server {
    int clientCount;
    IPaddress clientIPs[MAXCLIENTS];
    UDPsocket socket;
    UDPpacket *sendPacket;
    UDPpacket *recvPacket;
    int framesSinceLatestPacket[MAXCLIENTS]; // Number of frames since the last packet was received from each client. Assume client has disconnected when it reaches X frames.
}; typedef struct Server Server;

struct GameState {
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
}; typedef struct UDPplayer UDPplayer;

struct ServerData {
    int matchState; // waiting, playing, game over
    UDPplayer players[MAXCLIENTS];
    int playerID; // ID of the player receiving the data
}; typedef struct ServerData ServerData;

struct ClientData {
    int up[3];
    int down[3];
    int left[3];
    int right[3];
    int attack[3];
    int switchToRock[3];
    int switchToPaper[3];
    int switchToScissors[3];
}; typedef struct ClientData ClientData;

int server_main();
int init_server();

void server_waiting();
void server_playing();
void server_game_over();

void send_server_game_state_to_all_clients();
void receive_player_inputs();
int save_client();
int get_player_id_from_ip();

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
    }
    return 0;
}

void server_waiting(Server *server, GameState *gameState) {
    gameState->matchState = WAITING;
    while (server->clientCount < MAXCLIENTS) {
        while (SDLNet_UDP_Recv(server->socket, server->recvPacket)) {
            save_client(server, server->recvPacket->address);
        }
        send_server_game_state_to_all_clients(server, gameState);
        printf("Waiting for players... (%d)\n", server->clientCount);
        SDL_Delay(500); // Wait for 1 second before checking again
    }
    gameState->matchState = PLAYING;
}

void server_playing(Server *server, GameState *gameState) {
    // Game logic for playing state
    while (gameState->matchState == PLAYING) {
        receive_player_inputs(server, gameState);
        // Update game state logic here
        for (int i = 0; i < MAXCLIENTS; i++) {
            //handle_movement();
            handle_attack_input(gameState->players, MAXCLIENTS);
        }

        send_server_game_state_to_all_clients(server, gameState);
        SDL_Delay(1000 / 60); // Run at 60 FPS
    }
}

void server_game_over(Server *server, GameState *gameState) {
    // Game logic for game over state
    while (gameState->matchState == GAME_OVER) {
        // Handle game over logic here
    }
}

int save_client(Server *server, IPaddress ip) {
    for (int i = 0; i < server->clientCount; i++) {
        if (server->clientIPs[i].host == ip.host && server->clientIPs[i].port == ip.port) return -1; // Client already exists}
    }
    server->clientIPs[(server->clientCount)++] = ip;
    return 1; // Client saved successfully
}

void receive_player_inputs(Server *server, GameState *gameState) {
    ClientData clientData;
    while (SDLNet_UDP_Recv(server->socket, server->recvPacket)) {
        printf("Packet received from %s\n", SDLNet_ResolveIP(&server->recvPacket->address));
        memcpy(&clientData, server->recvPacket->data, sizeof(ClientData));
        int playerID = get_player_id_from_ip(server, server->recvPacket->address);
        if (playerID == -1) {
            save_client(server, server->recvPacket->address); // Player not found, save new client
            playerID = server->clientCount - 1; // Get the new player ID
        }
        InputLogger *playerInputLogger = Player_get_inputs(gameState->players[playerID]);
        for (int i = 0; i < 3; i++) {
            InputLogger_set_action_state(playerInputLogger, "move_up", i, clientData.up[i]);
            InputLogger_set_action_state(playerInputLogger, "move_up", i, clientData.down[i]);
            InputLogger_set_action_state(playerInputLogger, "move_left", i, clientData.left[i]);
            InputLogger_set_action_state(playerInputLogger, "move_right", i, clientData.right[i]);
            InputLogger_set_action_state(playerInputLogger, "attack", i, clientData.attack[i]);
            InputLogger_set_action_state(playerInputLogger, "switch_to_rock", i, clientData.switchToRock[i]);
            InputLogger_set_action_state(playerInputLogger, "switch_to_paper", i, clientData.switchToPaper[i]);
            InputLogger_set_action_state(playerInputLogger, "switch_to_scissors", i, clientData.switchToScissors[i]);
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
    ServerData serverData;
    serverData.matchState = gameState->matchState;
    for (int i = 0; i < server->clientCount; i++) {
        serverData.playerID = i;
        for (int j = 0; j < 4; j++) {
            if (gameState->players[j]) {
                // prepare game state data for sending
                serverData.players[j].isAlive = Player_get_isAlive(gameState->players[j]);
                serverData.players[j].hp = Player_get_hp(gameState->players[j]);
                serverData.players[j].weapon = Player_get_weapon(gameState->players[j]);
                serverData.players[j].posX = Vector2_get_x(Player_get_position(gameState->players[j]));
                serverData.players[j].posY = Vector2_get_y(Player_get_position(gameState->players[j]));
                serverData.players[j].direction = Player_get_direction(gameState->players[j]);
                //printf("%d %d %d %d %.2f %.2f %d\n", serverData.players[j].isAlive, serverData.players[j].hp, serverData.players[j].weapon, serverData.players[j].posX, serverData.players[j].posY, serverData.players[j].direction);
            }
            // Prepare packet and send
            server->sendPacket->address = server->clientIPs[i];
            server->sendPacket->len = sizeof(ServerData);
            memcpy(server->sendPacket->data, &serverData, sizeof(ServerData));
            SDLNet_UDP_Send(server->socket, -1, server->sendPacket);
        }
    }
}