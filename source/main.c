#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_net.h>

#include "../include/server.h"
#include "../include/client.h"

#include "../include/input_logger.h"
#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/movement.h"
#include "../include/menu.h"
#include "../include/dynamic_textarea.h"

struct Game{
    SDL_Window* Window;
    SDL_Renderer* renderer;
}; typedef struct Game Game;

#define NrOfButton 3

void background(SDL_Renderer* Renderer);
char main_menu(SDL_Renderer* Renderer, SDL_Window* Window,bool *mainQuit);

int main(int argv, char** args){
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDLNet_Init();

    SDL_Window* Window=SDL_CreateWindow("Window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,960,600,SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(Window, -1,0);
  

    bool isRunning = true;
    char choice = main_menu(renderer,  Window, &isRunning);
     
    while(isRunning){
        if (choice == 's'){
            server_main();
        } else if (choice == 'c'){
            client_main(Window, renderer);
        } else if (choice == 'q'){
            isRunning = 0;
        } else {
            printf("Invalid choice. Please enter 's' for server or 'c' for client.\n");
            continue;
        }
    }
    SDL_Quit();

    return 0;
}


void background(SDL_Renderer* renderer){
    SDL_Surface* surface= IMG_Load("images/background.png");
    SDL_Texture* texture= SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


char main_menu(SDL_Renderer* renderer, SDL_Window* Window, bool *mainQuit){
    button *Button[NrOfButton];
    Button[0] = button_create(0, 600, 400, 100, NULL, renderer, Window);
    Button[1]= button_create(0, 500, 400, 100, NULL, renderer, Window);
    Button[2]= button_create(0, 400, 400, 100, NULL, renderer, Window);
    center_button(Button[0],Window);
    center_button(Button[1],Window);
    center_button(Button[2],Window);
    bool howtoplay=false;
    bool quit;
    
    while(quit==false){
        SDL_RenderClear(renderer);
        background(renderer);
        //Ändra Namnet på spelet här och försök få den att va på mitten också då jag inte gjorde en center funktion till den.
        create_textarea(renderer, 450-120,  100, 120, NULL, "Game name", (SDL_Color){0,0,0,255});

        for(int i=0; i<NrOfButton; i++){
            if (ret_button_hover_state(Button[i])) ret_button_hover(ret_button_rect(Button[i]), renderer);
            else ret_button_normal(ret_button_rect(Button[i]), renderer);
        }  
        int x,y;
        SDL_GetMouseState(&x, &y);
        SDL_Event event;
        while(SDL_PollEvent(&event)!= 0){
            if(event.type == SDL_QUIT) {
                quit = true;
                *mainQuit=false;
                return 'q';
            }

            if(event.type == SDL_MOUSEMOTION){
                for(int i=0; i<NrOfButton; i++){
                    if(is_in_button_rect(x, y, ret_button_rect(Button[i]))) set_button_hover_true(Button[i]);
                    else set_button_hover_false(Button[i]);
                }
            }
            if(event.type == SDL_MOUSEBUTTONUP){
                if(is_in_button_rect(x, y, ret_button_rect(Button[2]))) {
                    return 's';
                }
            }
            if(event.type == SDL_MOUSEBUTTONUP){
                if(is_in_button_rect(x, y, ret_button_rect(Button[1]))) {
                    return 'c';
                }
            }
            if(event.type == SDL_MOUSEBUTTONDOWN){
                if(is_in_button_rect(x, y, ret_button_rect(Button[0]))) {
                    howtoplay=1^howtoplay;
                }
            }
        }
        //Ändra how to play texten här! 
        if(howtoplay){
            create_textarea_linebreaks(renderer, 1000,  500, 23, NULL, "This section willl explain what keys to use to play the game.", (SDL_Color){0,0,0,255});
           
        }
        load_Button_Text(Button[2], "Server", renderer);
        load_Button_Text(Button[1], "Client", renderer);
        load_Button_Text(Button[0], "How to play?", renderer);
        SDL_RenderPresent(renderer);
    }
    for(int i=0; i<NrOfButton; i++) button_destroy(Button[i]);
}



