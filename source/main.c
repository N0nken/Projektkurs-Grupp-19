#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL_net.h>

#include "../include/server.h"
#include "../include/client.h"

#include "../include/input_logger.h"
#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/movement.h"

enum Weapons { ROCK = 0, SCISSORS, PAPER };

int main(int argv, char** args){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    int isRunning = 1;
    while(isRunning){
        char choice = 0;
        printf("server or client (s/c): ");
        scanf("%c", &choice);
        if (choice == 's'){
            server_main();
        } else if (choice == 'c'){
            client_main();
        } else if (choice == 'q'){
            isRunning = 0;
        } else {
            printf("Invalid choice. Please enter 's' for server or 'c' for client.\n");
            continue;
        }
    }
    IMG_Quit();
    SDL_Quit();
    return 0;
}