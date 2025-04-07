#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_net.h>

#include "../include/collision.h"
#include "../include/player.h"
#include "../include/vector2.h"

int main(int argv, char** args){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    bool isRunning = true;
    SDL_Event event;

    Player *newPlayer = create_Player(
        create_Vector2(0, 0), 
        create_Collider(create_Vector2(0, 0), create_Vector2(1,1), 0), 
        create_Collider(create_Vector2(0, 0), create_Vector2(1,1), 1),
        create_Collider(create_Vector2(0, 0), create_Vector2(1,1), 1),
        100, 0
    );

    while(isRunning){
        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
                case SDL_QUIT: isRunning = false;
            }
        }
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}