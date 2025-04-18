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

struct Client {
    IPaddress serverIP;
    UDPsocket socket;
    UDPpacket *sendPacket;
    UDPpacket *recvPacket;
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

    if (init_client(&client, &gameState)) return 1;

    while (1) {
        client_waiting(&client, &gameState);
        client_playing(&client, &gameState);
        client_game_over(&client, &gameState);
    }

    return 0;
}

int init_client(Client *client, GameState *gameState) {
    client->socket = SDLNet_UDP_Open(CLIENTPORT);
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
    }
    return 0;
}

void client_waiting(Client *client, GameState *gameState) {
    char targetIPaddress[16];
    printf("Enter server IP address: ");
    scanf("%s", &targetIPaddress);
    SDLNet_ResolveHost(&client->serverIP, targetIPaddress, SERVERPORT);
    printf("server: %s\n", SDLNet_ResolveIP(&client->serverIP));
    while (gameState->matchState == WAITING) {
        if (gameState->playerID == -1) {
            printf("Attempting to connect to server...\n");
            client->sendPacket->address = client->serverIP;
            client->sendPacket->len = 0; // No data to send
            client->sendPacket->data = NULL;
            SDLNet_UDP_Send(client->socket, -1, client->sendPacket);
        } else {
            printf("Connected to server %s\n", SDLNet_ResolveIP(&client->serverIP));
            send_player_input(client, gameState);
        }
        sync_game_state_with_server(client, gameState);
        SDL_Delay(1000); // Wait for 1 second before checking again
    }
}

void client_playing(Client *client, GameState *gameState) {
    while (gameState->matchState == PLAYING) {
        // Handle player input
        InputLogger_update_all_actions(Player_get_inputs(gameState->players[gameState->playerID]), SDL_GetKeyboardState(NULL));
        while (!send_player_input(client, gameState));
        
        // run simulation
        handle_movement(gameState->players[gameState->playerID], 5.0f, SDL_GetKeyboardState(NULL));
        handle_attack_input(gameState->players, MAXCLIENTS);

        // sync simulation with server
        sync_game_state_with_server(client, gameState);
    }
}

void client_game_over() {

}

int send_player_input(Client *client, GameState *gameState) {
    printf("in send_player_input\n");
    ClientData clientData;
    InputLogger *playerInputLogger = Player_get_inputs(gameState->players[gameState->playerID]);
    for (int i = 0; i < 3; i++) {
        clientData.up[i] = InputLogger_get_action_state(playerInputLogger, "move_up", i);
        clientData.down[i] = InputLogger_get_action_state(playerInputLogger, "move_down", i);
        clientData.left[i] = InputLogger_get_action_state(playerInputLogger, "move_left", i);
        clientData.right[i] = InputLogger_get_action_state(playerInputLogger, "move_right", i);
        clientData.attack[i] = InputLogger_get_action_state(playerInputLogger, "attack", i);
        clientData.switchToRock[i] = InputLogger_get_action_state(playerInputLogger, "switch_to_rock", i);
        clientData.switchToPaper[i] = InputLogger_get_action_state(playerInputLogger, "switch_to_paper", i);
        clientData.switchToScissors[i] = InputLogger_get_action_state(playerInputLogger, "switch_to_scissors", i);
    }
    printf("A\n");
    memcpy(client->sendPacket->data, &clientData, sizeof(ClientData));
    printf("B\n");
    client->sendPacket->address = client->serverIP;
    client->sendPacket->len = sizeof(ClientData);
    printf("packet sending\n");
    return SDLNet_UDP_Send(client->socket, -1, client->sendPacket);
}

void sync_game_state_with_server(Client *client, GameState *gameState) {
    printf("in sync_game_state_with_server\n");
    ServerData serverData;
    // Receive all packets from the server
    while (SDLNet_UDP_Recv(client->socket, client->recvPacket)) {
        printf("Packet received from: %s\n", SDLNet_ResolveIP(&client->recvPacket->address));
        memcpy(&serverData, client->recvPacket->data, sizeof(ServerData));
        gameState->matchState = serverData.matchState;
        gameState->playerID = serverData.playerID;
        gameState->playerAliveCount = 0;
        for (int i = 0; i < MAXCLIENTS; i++) {
            if (!serverData.players[i].isAlive) {
                continue;
            }
            gameState->playerAliveCount++;
            printf("%d %d %d %d %.2f %.2f %d\n", serverData.players[i].isAlive, serverData.players[i].hp, serverData.players[i].weapon, serverData.players[i].posX, serverData.players[i].posY, serverData.players[i].direction);
            Player_set_isAlive(gameState->players[i], serverData.players[i].isAlive);
            Player_set_hp(gameState->players[i], serverData.players[i].hp);
            Player_set_weapon(gameState->players[i], serverData.players[i].weapon);
            Vector2_set_x(Player_get_position(gameState->players[i]), serverData.players[i].posX);
            Vector2_set_y(Player_get_position(gameState->players[i]), serverData.players[i].posY);
            Player_set_direction(gameState->players[i], serverData.players[i].direction);
        }
    }
}