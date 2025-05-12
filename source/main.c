#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>

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
#include "../include/sounds.h"

#define NrOfButton 3

void background();
char main_menu();

enum Weapons { ROCK = 0, SCISSORS, PAPER };

int main(int argv, char** args){
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDLNet_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_VolumeMusic(70);
    init_music_system("audio/soundtrack-main_menu.mp3");
    int client=0;
    bool isRunning = true;
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


   
    while(isRunning){
        char choice = main_menu(&renderController, &isRunning);
        if (choice == 's'){
            close_music_system();
            server_main();
        } else if (choice == 'c'){
            client= client_main(&renderController);
            if(client==1) return 1;
            else if(client==2);
        } else if (choice == 'q'){
            close_music_system();
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
    int Textoffset[NrOfButton]={0};

    while(quit==false){
        int x,y;
        SDL_GetMouseState(&x, &y);
        SDL_RenderClear(renderController->renderer);
        SDL_RenderCopy(renderController->renderer, renderController->background, NULL, NULL);
        //Ändra Namnet på spelet här och försök få den att va på mitten också då jag inte gjorde en center funktion till den.
        create_textarea(renderController->renderer, 50, 100, 60, NULL, "Rock Paper Scissors Arena PLUS+", (SDL_Color){0,0,0,255});
        /* För den gamla versionen
        for(int i=0; i<NrOfButton; i++){
            if (ret_button_hover_state(Button[i])) ret_button_hover(ret_button_rect(Button[i]), renderController->renderer);
            else ret_button_normal(ret_button_rect(Button[i]), renderController->renderer);
        }  
        */
        for(int i=0; i<NrOfButton; i++){
            if(ret_button_hover_state(Button[i])) load_button_image(Button[i], renderController->renderer, 0, 1);
            else if(ret_button_click_state(Button[i])) load_button_image(Button[i], renderController->renderer, 0, 2);
            else load_button_image(Button[i], renderController->renderer, 0, 3);
           
        }
        
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
            if(event.type == SDL_MOUSEBUTTONUP){
                if(is_in_button_rect(x, y, ret_button_rect(Button[0]))) {
                    howtoplay=1^howtoplay;
                }
            }
            if(event.type == SDL_MOUSEBUTTONDOWN){
                for(int i=0; i<NrOfButton; i++){
                    if(is_in_button_rect(x, y, ret_button_rect(Button[i]))) {
                       set_button_click_true(Button[i]);
                       set_button_hover_false(Button[i]);
                       Textoffset[i]+=10;
                    }
                }  
            }

            if(event.type == SDL_MOUSEBUTTONUP){
                for(int i=0; i<NrOfButton; i++){
                    if(ret_button_click_state(Button[i]))Textoffset[i]-=10;
                    set_button_click_false(Button[i]);
                    set_button_hover_false(Button[i]);
                    
                }  
            }
        }
        //Ändra how to play texten här! 
        if(howtoplay){
            create_textarea_linebreaks(renderController->renderer, 1000,  500, 23, NULL, "This section willl explain what keys to use to play the game.", (SDL_Color){0,0,0,255});
           
        }
        
        create_textarea( renderController->renderer,ret_button_rect(Button[2]).x+10,ret_button_rect(Button[2]).y+Textoffset[2], 75,NULL,"Server",(SDL_Color){0,0,0,255});
        create_textarea( renderController->renderer,ret_button_rect(Button[1]).x+10,ret_button_rect(Button[1]).y+Textoffset[1], 75,NULL,"Client",(SDL_Color){0,0,0,255});
        create_textarea( renderController->renderer,ret_button_rect(Button[0]).x+10,ret_button_rect(Button[0]).y+Textoffset[0], 75,NULL,"Help?",(SDL_Color){0,0,0,255});
       
        SDL_RenderPresent(renderController->renderer);
    }
    for(int i=0; i<NrOfButton; i++) button_destroy(Button[i]);
    close_music_system();
    Mix_CloseAudio();
}

