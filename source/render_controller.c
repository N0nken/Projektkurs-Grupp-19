#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/render_controller.h"

#define MAXCLIENTS 4

struct RenderController {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *playerTextures[MAXCLIENTS];
}; typedef struct RenderController RenderController;

int init_rendering() {
    
}