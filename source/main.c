#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL_net.h>

#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"

int main(int argv, char** args){

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface *background= IMG_Load("images/BG_Prototype.png");
    SDL_Texture *backgroundtexture = SDL_CreateTextureFromSurface(renderer,background);

    bool isRunning = true;
    SDL_Event event;

    while(isRunning){
        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
                case SDL_QUIT: isRunning = false;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundtexture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}