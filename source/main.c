#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL_net.h>

#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/movement.h" 

int main(int argv, char** args){

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface *background= IMG_Load("images/BG_Prototype.png");
    SDL_Texture *backgroundtexture = SDL_CreateTextureFromSurface(renderer,background);

    SDL_Surface *playerSurface = IMG_Load("images/char.png"); //texture för spelare
    if (!playerSurface) {
        printf("Error loading player image: %s\n", IMG_GetError());
    }
    SDL_Texture *playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);

    bool isRunning = true;
    SDL_Event event;
    Vector2 *direction = create_Vector2(0, 0);

    Player *player1 = create_Player(create_Vector2(50, 50), create_Collider(create_Vector2(10, 10), create_Vector2(10, 10), 1), create_Collider(create_Vector2(10, 10), create_Vector2(10, 10), 1), create_Collider(create_Vector2(10, 10), create_Vector2(10, 10), 1), 100, 1, 1);
    //SDL_Rect *rect1 = Player_get_rect(player1);
    //float deltaTime = 1;

    while(isRunning){
        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
                case SDL_QUIT: isRunning = false;
                
            }
        }

        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP]) {
            Vector2_set_y(direction, -1.0f);  // Gå upp
        } else if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN]) {
            Vector2_set_y(direction, 1.0f);   // Gå ner
        } else {
            Vector2_set_y(direction, 0.0f);   // Ingen rörelse i Y-led
        }
        
        if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT]) {
            Vector2_set_x(direction, -1.0f);  // Gå vänster
        } else if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {
            Vector2_set_x(direction, 1.0f);   // Gå höger
        } else {
            Vector2_set_x(direction, 0.0f);   // Ingen rörelse i X-led
        }
        
        //print_Vector2(direction);
        normalize(direction);
        float speed = 5;
        Vector2 *velocity = Vector2_const_multiplication(direction, speed);
        update_player_position(player1, velocity); // Anropa funktionen från movement.c
        destroy_Vector2(velocity);
        print_Vector2(Player_get_position(player1));
        
        //reset direction
        Vector2_set_x(direction, 0.0f);
        Vector2_set_y(direction, 0.0f);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundtexture, NULL, NULL);
        SDL_RenderCopy(renderer, playerTexture, NULL, Player_get_rect(player1));
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/60); // 60 fps
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}