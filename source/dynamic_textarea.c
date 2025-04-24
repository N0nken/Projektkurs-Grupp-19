#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../include/dynamic_textarea.h"


void create_textarea(SDL_Renderer* renderer,int x, int y, int size, TTF_Font* font,   const char *text, SDL_Color color){
    
    if(!font){
        font = TTF_OpenFont("fonts/p.ttf", size);
    }
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect area={x, y, surfaceMessage->w, surfaceMessage->h};
    SDL_RenderCopy(renderer, Message, NULL, &area);
    TTF_CloseFont(font);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
    
}


void create_textarea_linebreaks(SDL_Renderer* renderer,int x, int y, int size, TTF_Font* font,  const char *text, SDL_Color color){
    if(!font){
        font = TTF_OpenFont("fonts/p.ttf", size);
    }
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended_Wrapped(font, text, color,500);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect area={x, y, surfaceMessage->w, surfaceMessage->h};
    SDL_RenderCopy(renderer, Message, NULL, &area);
    TTF_CloseFont(font);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
    
}