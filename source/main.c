#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>

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
#include "../include/renderController.h"

#define NrOfButton 3

void background();
char main_menu();

enum Weapons { ROCK = 0, SCISSORS, PAPER };

int main(int argv, char** args){
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDLNet_Init();
  
    RenderController renderController;
    renderController.window = SDL_CreateWindow("Window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,960,600,SDL_WINDOW_RESIZABLE);
    renderController.renderer = SDL_CreateRenderer(renderController.window, -1,0);
    SDL_Surface* backgroundSurface = IMG_Load("images/background.png");
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderController.renderer, backgroundSurface);
    renderController.background = backgroundTexture;
    SDL_FreeSurface(backgroundSurface);
    SDL_Surface* playerSpritesheetSurface = IMG_Load("images/animationer.png");
    SDL_Texture* playerSpritesheetTexture = SDL_CreateTextureFromSurface(renderController.renderer, playerSpritesheetSurface);
    SDL_FreeSurface(playerSpritesheetSurface);
    renderController.playerSpritesheet = playerSpritesheetTexture;

    bool isRunning = true;
    char choice = main_menu(&renderController, &isRunning);
    while(isRunning){
        if (choice == 's'){
            server_main();
        } else if (choice == 'c'){
            if(!client_main(&renderController)) return 1;
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

char main_menu(RenderController* renderController, bool *mainQuit){
    button *Button[NrOfButton];
    Button[0]= button_create(0, 600, 400, 100, NULL, renderController->renderer, renderController->window);
    Button[1]= button_create(0, 500, 400, 100, NULL, renderController->renderer, renderController->window);
    Button[2]= button_create(0, 400, 400, 100, NULL, renderController->renderer, renderController->window);
    center_button(Button[0],renderController->window);
    center_button(Button[1],renderController->window);
    center_button(Button[2],renderController->window);
    bool howtoplay=false;
    bool quit;
    
    while(quit==false){
        SDL_RenderClear(renderController->renderer);
        SDL_RenderCopy(renderController->renderer, renderController->background, NULL, NULL);
        //Ändra Namnet på spelet här och försök få den att va på mitten också då jag inte gjorde en center funktion till den.
        create_textarea(renderController->renderer, 450-120,  100, 120, NULL, "Game name", (SDL_Color){0,0,0,255});

        for(int i=0; i<NrOfButton; i++){
            if (ret_button_hover_state(Button[i])) ret_button_hover(ret_button_rect(Button[i]), renderController->renderer);
            else ret_button_normal(ret_button_rect(Button[i]), renderController->renderer);
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
            create_textarea_linebreaks(renderController->renderer, 1000,  500, 23, NULL, "This section willl explain what keys to use to play the game.", (SDL_Color){0,0,0,255});
           
        }
        load_Button_Text(Button[2], "Server", renderController->renderer);
        load_Button_Text(Button[1], "Client", renderController->renderer);
        load_Button_Text(Button[0], "How to play?", renderController->renderer);
        SDL_RenderPresent(renderController->renderer);
    }
    for(int i=0; i<NrOfButton; i++) button_destroy(Button[i]);
}

