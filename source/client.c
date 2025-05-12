#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>

#include "../include/input_logger.h"
#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/movement.h"
#include "../include/dynamic_textarea.h"
#include "../include/renderController.h"
#include "../include/menu.h"
#include "../include/sounds.h"

#define PACKETLOSSLIMIT 10 // Give up sending packets to server after this many failed attempts
#define MAXCLIENTS 2
#define CLIENTPORT 50000
#define SERVERPORT 50001
#define MAXPACKETSRECEIVEDPERFRAME 10
#define TARGETFPS 60

typedef struct {
    float cx, cy;  // plattformens centrum
    float hw, hh;  // half-width, half-height
} PlatformDef;

#define PLATFORM_COUNT 4

static const PlatformDef platformDefs[PLATFORM_COUNT] = {
    { 360.0f, 785.0f, 120.0f, 10.0f },
    { 910.0f, 480.0f, 120.0f, 10.0f },
    { 1490.0f, 785.0f, 120.0f, 10.0f },
    { 960.0f, 975.0f, 825.0f, 30.0f }
};//MÅSTE MATCHA MED SERVER

enum MatchStates {WAITING,PLAYING,GAME_OVER};

//  Basic struct for sdl_net
struct Client {
    IPaddress serverIP;
    UDPsocket socket;
    UDPpacket *sendPacket;
    UDPpacket *recvPacket;
    int failedPackets; // Number of packets that failed to send
    int packetsReceived; // Number of packets received in the current frame
}; typedef struct Client Client;

// contains the current state of the game. E.g. if theres an active match, how many players are alive, which player this client controls etc.
struct GameState {
    int playerID; // ID of the clients player
    int matchState; // waiting, playing, game over
    int playerAliveCount; // number of players left alive
    int winnerID;
    int gameOverTimerMs;
    Player *players[MAXCLIENTS]; // array of players
}; typedef struct GameState GameState;

// Struct for receiving player data from the server
struct UDPplayer {
    int isAlive;
    int hp;
    int weapon;
    float posX;
    float posY;
    int direction;
    int state; // 0 = idle, 1 = moving, 2 = attacking
}; typedef struct UDPplayer UDPplayer;

// Struct for receiving game state data from the server
struct SimulationData {
    int matchState; // waiting, playing, game over
    UDPplayer players[MAXCLIENTS];
    int playerID; // ID of the player receiving the data
    int playerAliveCount;
    int winnerID;
    int gameOverTimerMs;
}; typedef struct SimulationData SimulationData;

// Struct for sending/receiving player inputs to/from the server. !!!!!Update when new actions are added!!!!!
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
    int dash[3];
}; typedef struct ClientInput ClientInput;

struct Frame{
    int x;
    int y;
    int h;
    int w;
} typedef frame;

int client_main();
int init_client();

int client_waiting();
int client_playing();
int client_game_over();

int send_player_input();
void sync_game_state_with_server();
void client_background();
int client_lobby();

void draw_player_hitbox(Player *p, RenderController *renderController) {
    SDL_Rect hitbox = {Vector2_get_x(Player_get_position(p)), Vector2_get_y(Player_get_position(p)), Vector2_get_x(Collider_get_dimensions(Player_get_collider(p))), Vector2_get_y(Collider_get_dimensions(Player_get_collider(p)))};
    SDL_Rect attackHitbox = {Vector2_get_x(Player_get_position(p)) + Vector2_get_x(Collider_get_position(Player_get_attackHitbox(p))), 
        Vector2_get_y(Player_get_position(p)) + Vector2_get_y(Collider_get_position(Player_get_attackHitbox(p))), 
        Vector2_get_x(Collider_get_dimensions(Player_get_attackHitbox(p))), 
        Vector2_get_y(Collider_get_dimensions(Player_get_attackHitbox(p)))};
    SDL_SetRenderDrawColor(renderController->renderer, 255, 0, 255, 255);
    SDL_RenderFillRect(renderController->renderer, &hitbox);
    SDL_SetRenderDrawColor(renderController->renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderController->renderer, &attackHitbox);
}

void draw_colliders(RenderController *renderController, Collider *c) {
    SDL_Rect collider = {
        (int)(Vector2_get_x(Collider_get_position(c)) - Vector2_get_x(Collider_get_dimensions(c))), // x
        (int)(Vector2_get_y(Collider_get_position(c)) - Vector2_get_y(Collider_get_dimensions(c))), // y
        (int)(2 * Vector2_get_x(Collider_get_dimensions(c))),                  // width
        (int)(2 * Vector2_get_y(Collider_get_dimensions(c)))                   // height
    };
    
    SDL_SetRenderDrawColor(renderController->renderer, 255, 0, 255, 255);
    SDL_RenderFillRect(renderController->renderer, &collider);
}

void draw_platforms(RenderController *renderController, Collider *c, SDL_Texture *platform) {
    SDL_Rect collider = {
        (int)(Vector2_get_x(Collider_get_position(c)) - Vector2_get_x(Collider_get_dimensions(c))), // x
        (int)(Vector2_get_y(Collider_get_position(c)) - Vector2_get_y(Collider_get_dimensions(c))), // y
        (int)(2 * Vector2_get_x(Collider_get_dimensions(c))),                  // width
        (int)(2 * Vector2_get_y(Collider_get_dimensions(c)))                   // height
    };
    SDL_RenderCopy(renderController->renderer,
        platform,
        NULL,              // ta hela texturen
        &collider // rita in i denna rektangel
    ); 
}

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

int client_main(RenderController* renderController) {
    Client client;
    GameState gameState;
    int lobby=0;

    char targetIP[16];
    lobby=client_lobby(renderController, targetIP);
    if (lobby==1) return 1;
    else if (lobby==2) return 2;
    if (init_client(&client, &gameState)) return 1;
    init_music_system("audio/soundtrack-match.mp3");
    while (1) {
        if (client_waiting(&client, &gameState, renderController, targetIP)) break;
        if (client_playing(&client, &gameState, renderController)) break;
        if (client_game_over(&client, &gameState, renderController)) break;
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
        gameState->players[i] = create_Player(create_Vector2(240, 0), 
            create_Collider(create_Vector2(240, 0), create_Vector2(PLAYERWIDTH, PLAYERHEIGHT), 0, PLAYERCOLLISIONLAYER), 
            create_Collider(create_Vector2(PLAYERATTACKHITBOXOFFSETX, PLAYERATTACKHITBOXOFFSETY), create_Vector2(PLAYERATTACKHITBOXWIDTH, PLAYERATTACKHITBOXHEIGHT), 0, PLAYERATTACKLAYER), 
            100, 0, 1, gameState->players, &gameState->playerAliveCount);
        InputLogger_reset_all_actions(Player_get_inputs(gameState->players[i]));
    }
    return 0;
}

// connects to the server at the given ip address and waits until the server says that the match has begun
int client_waiting(Client *client, GameState *gameState, RenderController* renderController, char targetIPaddress[]) {
    SDLNet_ResolveHost(&client->serverIP, targetIPaddress, SERVERPORT);
    printf("server: %s\n", SDLNet_ResolveIP(&client->serverIP));
    SDL_Event event;
    while (gameState->matchState == WAITING) {
        SDL_RenderClear(renderController->renderer);
        SDL_RenderCopy(renderController->renderer, renderController->background, NULL, NULL);
        SDL_RenderPresent(renderController->renderer);
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
            //printf("Player input sent\n");
        }
        sync_game_state_with_server(client, gameState);
        SDL_Delay(1000);
    }
    return 0;
}

// main match loop
int client_playing(Client *client, GameState *gameState, RenderController* renderController) {
    SDL_Texture *platformTexture = IMG_LoadTexture(renderController->renderer, "images/platform.png");
    if (!platformTexture) {
        printf("Failed to load platform.png: %s\n", IMG_GetError());
        return 1;
    }
    int spriteWidth = 32,spriteHeight = 32, animationCounter=0;
    frame playerFrame = {0};

    Collider *colls[PLATFORM_COUNT];

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        float cx = platformDefs[i].cx;
        float cy = platformDefs[i].cy;
        float hw = platformDefs[i].hw;
        float hh = platformDefs[i].hh;

        colls[i] = create_Collider(
            create_Vector2(cx, cy),
            create_Vector2(hw, hh),
            0,  1
        );
    }
    SDL_Event event;
    Uint64 lastTicks = SDL_GetTicks64();
    while (gameState->matchState == PLAYING) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    gameState->matchState = GAME_OVER;
                    return 1;
            }
        }
        
        // Update player input...
        if (Player_get_isAlive(gameState->players[gameState->playerID])) {
            InputLogger_update_all_actions(Player_get_inputs(gameState->players[gameState->playerID]), SDL_GetKeyboardState(NULL));
            //InputLogger_print_inputs(Player_get_inputs(gameState->players[gameState->playerID]));
            // ...and send it to the server
            while (!send_player_input(client, gameState) && client->failedPackets < PACKETLOSSLIMIT) {
                //printf("Failed to send player input\n");
                client->failedPackets++;
            }
        } else {
            //printf("Player %d is dead\n", gameState->playerID); //fixes issue that makes server thinks that connection has failed when killed
            client->sendPacket->address = client->serverIP;
            client->sendPacket->len = sizeof(1);
            *(client->sendPacket->data) = 1;
            SDLNet_UDP_Send(client->socket, -1, client->sendPacket);
        }
        
        Uint64 currentTicks = SDL_GetTicks64();             // nu i ms
        Uint64 elapsedTicks = currentTicks - lastTicks;     // skillnad i ms
        lastTicks = currentTicks;
        float deltaTime = elapsedTicks * 0.001f;
        // run simulation
        for (int i = 0; i < MAXCLIENTS; i++) {
            handle_movement(gameState->players[i], PLAYERSPEED, colls[0], colls[1], colls[2], colls[3], deltaTime);
            //handle_weapon_switching(gameState->players[i]);
            switch_player_weapon(gameState->players[i]);
        }
        handle_attack_input(gameState->players, MAXCLIENTS);

        // sync simulation with server
        sync_game_state_with_server(client, gameState);

        // clear renderer
        SDL_RenderClear(renderController->renderer);
        
        // draw background
        SDL_RenderCopy(renderController->renderer, renderController->background, NULL, NULL);

        
        // draw platforms
        draw_platforms(renderController, colls[0], platformTexture);
        draw_platforms(renderController, colls[1], platformTexture);
        draw_platforms(renderController, colls[2], platformTexture);
        //draw_colliders(renderController, Player_get_collider(gameState->players[0]));

        //draw_colliders(renderController, colls[3]);

        // draw all players
        for (int i = 0; i < MAXCLIENTS; i++) {
            health_bar(gameState->players[i], renderController->renderer);
            if(Player_get_isAlive(gameState->players[i])){
                SDL_QueryTexture(renderController->playerSpritesheet, NULL , NULL, &spriteHeight, &spriteWidth);        
                SDL_RenderCopy(renderController->renderer, renderController->playerSpritesheet,
                    get_Player_Frame(&playerFrame,
                        Player_get_weapon(gameState->players[i]),
                        get_Animation_Counter(Player_get_inputs(gameState->players[i]), Player_get_direction(gameState->players[i]))),
                    Player_get_rect(gameState->players[i]));
            }
        }
        
        SDL_RenderPresent(renderController->renderer);
        
        SDL_Delay(1000 / TARGETFPS); // Run at target FPS
        //SDL_DestroyTexture(platformTexture);
    }
    return 0;
    
}

// shows the winner and after a certain time restarts (end?) the match
int client_game_over(Client *client, GameState *gameState, RenderController* renderController) {
    SDL_Event event;
    SDL_Surface* winnerImage;
    close_music_system();
    init_music_system("audio/soundtrack-gameover.mp3");
    Mix_VolumeMusic(70);
    switch (gameState->winnerID) {
        case 0:
            winnerImage = IMG_Load("images/char.png");
            break;
        case 1:
            winnerImage = IMG_Load("images/char.png");
            break;
        case 2:
            winnerImage = IMG_Load("images/char.png");
            break;
        case 3:
            winnerImage = IMG_Load("images/char.png");
            break;
        default:
            winnerImage = IMG_Load("images/char.png");
    }
    SDL_Texture* winnerImageTexture = SDL_CreateTextureFromSurface(renderController->renderer, winnerImage);
    SDL_FreeSurface(winnerImage);
    SDL_Rect targetRect = {960/2,600/2,200,200};
    client->sendPacket->address = client->serverIP;
    client->sendPacket->len = sizeof(1);
    *(client->sendPacket->data) = 1;
    while (gameState->matchState == GAME_OVER) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return 1;
            }
        }
        SDL_RenderClear(renderController->renderer);
        SDL_RenderCopy(renderController->renderer, renderController->background, NULL, NULL);
        SDL_RenderCopy(renderController->renderer, winnerImageTexture, NULL, &targetRect);
        switch (gameState->winnerID) {
            case 0:
                create_textarea(renderController->renderer, 200, 50, 20, NULL, "Player One Won!", (SDL_Color){0, 0, 0, 255});
                break;
            case 1:
                create_textarea(renderController->renderer, 200, 50, 20, NULL, "Player Two Won!", (SDL_Color){0, 0, 0, 255});
                break;
            case 2:
                create_textarea(renderController->renderer, 200, 50, 20, NULL, "Player Three Won!", (SDL_Color){0, 0, 0, 255});
                break;
            case 3:
                create_textarea(renderController->renderer, 200, 50, 20, NULL, "Player Four Won!", (SDL_Color){0, 0, 0, 255});
                break;
        }
        SDLNet_UDP_Send(client->socket, -1, client->sendPacket);
        SDL_RenderPresent(renderController->renderer);
        // Render game over screen
        //printf("Game Over\n");
        SDL_Delay(1000 / TARGETFPS); // Run at target FPS
        
        
    }
    return 0;
}

// sends this clients input to the server
int send_player_input(Client *client, GameState *gameState) {
    ClientInput clientInput;
    //printf("Preparing sending packet\n");
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
        clientInput.dash[i] = InputLogger_get_action_state(playerInputLogger, "dash", i);
    }
    memcpy(client->sendPacket->data, &clientInput, sizeof(ClientInput));
    client->sendPacket->address = client->serverIP;
    client->sendPacket->len = sizeof(clientInput);
    //printf("sending packet\n");
    return SDLNet_UDP_Send(client->socket, -1, client->sendPacket);
}

// corrects this clients simulation with the simulation on the server. Also collects all inputs from the other clients to predict the coming frame(s) until the next server sync
void sync_game_state_with_server(Client *client, GameState *gameState) {
    SimulationData simulationData;
    ClientInput   clientInput;

    while (SDLNet_UDP_Recv(client->socket, client->recvPacket)
           && client->packetsReceived < MAXPACKETSRECEIVEDPERFRAME) {

        client->packetsReceived++;

        if (client->recvPacket->len == sizeof(SimulationData)) {
            // Läs in hela paketet från servern
            memcpy(&simulationData, client->recvPacket->data, sizeof(simulationData));

            // Spegla serverns state rakt av 
            gameState->matchState       = simulationData.matchState;
            gameState->playerID         = simulationData.playerID;
            gameState->playerAliveCount = simulationData.playerAliveCount;
            gameState->winnerID         = simulationData.winnerID;
            gameState->gameOverTimerMs  = simulationData.gameOverTimerMs;  // se till att GameState har detta fält

            // Uppdatera alla spelare
            for (int i = 0; i < MAXCLIENTS; i++) {
                Player_set_isAlive   (gameState->players[i], simulationData.players[i].isAlive);
                Player_set_hp        (gameState->players[i], simulationData.players[i].hp);
                Player_set_weapon    (gameState->players[i], simulationData.players[i].weapon);
                Vector2_set_x        (Player_get_position(gameState->players[i]), simulationData.players[i].posX);
                Vector2_set_y        (Player_get_position(gameState->players[i]), simulationData.players[i].posY);
                Player_set_direction (gameState->players[i], simulationData.players[i].direction);
            }

        } else if (client->recvPacket->len == sizeof(ClientInput)) {
            // Läs in input-paketet
            memcpy(&clientInput, client->recvPacket->data, sizeof(clientInput));

            InputLogger *logger = Player_get_inputs(gameState->players[clientInput.playerID]);
            for (int i = 0; i < 3; i++) {
                InputLogger_set_action_state(logger, "move_up",          i, clientInput.up[i]);
                InputLogger_set_action_state(logger, "move_down",        i, clientInput.down[i]);
                InputLogger_set_action_state(logger, "move_left",        i, clientInput.left[i]);
                InputLogger_set_action_state(logger, "move_right",       i, clientInput.right[i]);
                InputLogger_set_action_state(logger, "attack",           i, clientInput.attack[i]);
                InputLogger_set_action_state(logger, "switch_to_rock",   i, clientInput.switchToRock[i]);
                InputLogger_set_action_state(logger, "switch_to_paper",  i, clientInput.switchToPaper[i]);
                InputLogger_set_action_state(logger, "switch_to_scissors",i, clientInput.switchToScissors[i]);
                InputLogger_set_action_state(logger, "dash",              i, clientInput.dash[i]);
            }

        } /*else {
            printf("Unknown packet type received\n");
        }*/
    }

    client->packetsReceived = 0;
}

// menu for the player to enter the ip address of the server
int client_lobby(RenderController* renderController, char targetIPaddress[]) {
    button *Button=button_create(1400, 20, 50, 50, NULL, renderController->renderer, renderController->window);
    strcpy(targetIPaddress, "127.0.0.1_____");
    int max=15, count=9;
    // run till player quits or enters an IP address
    int rctrl=0, lctrl=0;
    while(1){
        int x,y;
        SDL_GetMouseState(&x, &y);

        TTF_Font* font = TTF_OpenFont("fonts/poppins.regular.ttf", 50);
        SDL_RenderClear(renderController->renderer);
        SDL_RenderCopy(renderController->renderer, renderController->background, NULL, NULL);
        load_button_image(Button, renderController->renderer, 8, 0);
        create_textarea(renderController->renderer, 450-120,  100, 50, NULL, "Enter the server ip", (SDL_Color){0,0,0,255});
        create_textarea(renderController->renderer, 450-120,  300, 50, font, targetIPaddress, (SDL_Color){0,0,0,255});
        
        SDL_RenderPresent(renderController->renderer);
       
        SDL_StartTextInput();
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                button_destroy(Button);
                return 1;
            case SDL_MOUSEBUTTONDOWN:
                if(is_in_button_rect(x, y, ret_button_rect(Button))){
                    button_destroy(Button);
                    return 2; 
                }
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.scancode == SDL_SCANCODE_RCTRL) { rctrl = 1; }
                else if(event.key.keysym.scancode == SDL_SCANCODE_LCTRL) { lctrl = 1; }
                break;
            case SDL_KEYUP:
                if(event.key.keysym.scancode == SDL_SCANCODE_RCTRL) { rctrl = 0; }
                else if(event.key.keysym.scancode == SDL_SCANCODE_LCTRL) { lctrl = 0; }
                break;
           
               
           
            case SDL_TEXTINPUT:
                printf("%s",event.text.text);
                if(count<max){
                    printf("%d",count);
                    targetIPaddress[count]= *event.text.text;
                    count++;
                }
                break;
           
        }   

        if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE && event.type==SDL_KEYDOWN){
            if(count>-1){
                if(count!=0)count--;
                targetIPaddress[count]='_';
                
            }
        }
        if(event.key.keysym.scancode == SDL_SCANCODE_V && event.type==SDL_KEYDOWN && (rctrl || lctrl)){
            char *clipboardText = SDL_GetClipboardText();
            if (strlen(clipboardText)<max) {
                strcpy(targetIPaddress, clipboardText);
                count=strlen(clipboardText);
                SDL_free(clipboardText); 
                for(int i = count; i < max; i++) {
                    targetIPaddress[i] = '_';
                }
            }

        }
       
        else if(event.key.keysym.scancode == SDL_SCANCODE_RETURN){
            button_destroy(Button);
            return 0; // ip adress entered, DONT quit program
        }
    }
}