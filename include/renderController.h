#ifndef RENDER_CONTROLLER_H_
#define RENDER_CONTROLLER_H_
#include <SDL2/SDL.h>

struct RenderController {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;
    SDL_Texture* playerSpritesheet;
}; typedef struct RenderController RenderController;

#endif