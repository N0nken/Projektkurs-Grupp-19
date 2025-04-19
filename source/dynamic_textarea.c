#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../include/dynamic_textarea.h"



void Islinked(void){
    printf("WEll that went bad");
}

void create_textarea(SDL_Renderer* renderer,int x, int y, int w, int h, TTF_Font* font,  int size, const char *text, SDL_Color color){
    
    if(!font){
        font = TTF_OpenFont("fonts/p.ttf", size);
    }
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect area={x, y, w, h};
    SDL_RenderCopy(renderer, Message, NULL, &area);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}