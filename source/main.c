#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_net.h>


#include "../include/input_logger.h"
#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/movement.h"
#include "../include/menu.h"
#include "../include/dynamic_textarea.h"

#define NrOfButton 3

void background(SDL_Renderer* Renderer);
void main_menu(SDL_Renderer* Renderer, SDL_Window* Window,bool *mainQuit);

int main(int argv, char** args){

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_Window *window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    


    
    SDL_Surface *background= IMG_Load("images/background.png");
    SDL_Texture *backgroundtexture = SDL_CreateTextureFromSurface(renderer,background);

    SDL_Surface *playerSurface = IMG_Load("images/char.png"); //texture för spelare
    if (!playerSurface) {
        printf("Error loading player image: %s\n", IMG_GetError());
    }
    SDL_Texture *playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);



    bool isRunning = true;
    main_menu( renderer,  window, &isRunning);
    SDL_Event event;

    Player *allPlayers[4];
    int activePlayerCount = 0;

    Player *player1 = create_Player(
        create_Vector2(50, 50), create_Collider(create_Vector2(10, 10), create_Vector2(10, 10), 1, 0), 
        create_Collider(create_Vector2(10, 10), create_Vector2(10, 10), 1, 0), 
        create_Collider(create_Vector2(10, 10), create_Vector2(10, 10), 1, 0), 
    100, 1, 1, allPlayers, &activePlayerCount);
    //SDL_Rect *rect1 = Player_get_rect(player1);
    //float deltaTime = 1;

    /* avkommentera för att testa move_and_collide()
    Collider *col1 = create_Collider(create_Vector2(0, 0), create_Vector2(5, 5), 0);
    Collider *col2 = create_Collider(create_Vector2(100, 100), create_Vector2(5, 5), 0);
    move_and_collide(col1, create_Vector2(1000, 1000));
    print_Collider(col1);
    */

    /* avkommentera för att testa attack */
    Player *p1 = create_Player(
        create_Vector2(0, 0), create_Collider(create_Vector2(0, 0), create_Vector2(10, 10), 0, 0), 
        create_Collider(create_Vector2(0, 0), create_Vector2(10, 10), 1, 0), 
        create_Collider(create_Vector2(0, 0), create_Vector2(10, 10), 1, 0),
        100, 0, 1, allPlayers, &activePlayerCount);
    Player *p2 = create_Player(
        create_Vector2(0, 0), create_Collider(create_Vector2(0, 0), create_Vector2(10, 10), 0, 0), 
        create_Collider(create_Vector2(0, 0), create_Vector2(10, 10), 1, 0), 
        create_Collider(create_Vector2(0, 0), create_Vector2(10, 10), 1, 0),
        100, 0, 1, allPlayers, &activePlayerCount);
    
    

    Uint64 deltaTime = SDL_GetTicks64();
    while(isRunning){

        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
                case SDL_QUIT: isRunning = false; break;
            }
            
            if (event.type==SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_1: switch_player_weapon(player1, SDLK_1); break;
                    case SDLK_2: switch_player_weapon(player1, SDLK_2); break;
                    case SDLK_3: switch_player_weapon(player1, SDLK_3); break;
                }
            }
            
        }
        printf("%d\n", Player_get_hp(p2));
        handle_attack_input(allPlayers, activePlayerCount);
        printf("%d\n", Player_get_hp(p2));
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        printf("A");
        handle_movement(player1, 5.0f, keystates);
        printf("B");
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundtexture, NULL, NULL);
        SDL_RenderCopy(renderer, playerTexture, NULL, Player_get_rect(player1));
        SDL_RenderPresent(renderer);
        /*deltaTime = SDL_GetTicks64() - deltaTime;
        SDL_Delay(1000/60 - deltaTime); // 60 fps
        */
        Input_Logger *p1Logger = Player_get_inputs(p1);
        SDL_Delay(1000/60);
        Input_Logger *logger = Player_get_inputs(p1);
        Input_Logger_update_all_actions(logger, keystates);
        printf("attack just pressed %d\n", Input_Logger_is_action_just_pressed(p1Logger, "attack"));
        printf("attack pressed %d\n", Input_Logger_is_action_pressed(p1Logger, "attack"));
        printf("attack just released %d\n", Input_Logger_is_action_just_released(p1Logger, "attack"));
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
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


void main_menu(SDL_Renderer* renderer, SDL_Window* Window, bool *mainQuit){
    button *Button[NrOfButton];
    Button[0] = button_create(0, 600, 400, 100, NULL, renderer, Window);
    Button[1]= button_create(0, 500, 400, 100, NULL, renderer, Window);
    Button[2]= button_create(0, 400, 400, 100, NULL, renderer, Window);
    center_button(Button[0],Window);
    center_button(Button[1],Window);
    center_button(Button[2],Window);
    bool quit;
    SDL_Event event;
    while(quit==false){
        SDL_RenderClear(renderer);
        background(renderer);
        
        create_textarea( renderer, 500,  100,  500, 200 , NULL,  40, "Game name", (SDL_Color){0,0,0,255});

        for(int i=0; i<NrOfButton; i++){
            if (ret_button_hover_state(Button[i])) ret_button_hover(ret_button_rect(Button[i]), renderer);
            else ret_button_normal(ret_button_rect(Button[i]), renderer);
        }  
        int x,y;
        SDL_GetMouseState(&x, &y);
        
        while(SDL_PollEvent(&event)!= 0){
            if(event.type == SDL_QUIT) {
                quit = true;
                *mainQuit=false;
            }

            else if(event.type == SDL_MOUSEMOTION){
                for(int i=0; i<NrOfButton; i++){
                    if(is_in_button_rect(x, y, ret_button_rect(Button[i]))) set_button_hover_true(Button[i]);
                    else set_button_hover_false(Button[i]);
                }
            }
            else if(event.type == SDL_MOUSEBUTTONUP){
                if(is_in_button_rect(x, y, ret_button_rect(Button[2]))) quit=true;;
            }
        }
        load_Button_Text(Button[2], "Single player", renderer);
        load_Button_Text(Button[1], "Lobby", renderer);
        load_Button_Text(Button[0], "How to play?", renderer);
        SDL_RenderPresent(renderer);
    }
    for(int i=0; i<NrOfButton; i++) button_destroy(Button[i]);
}


//void lobby(SDL_Renderer* renderer, SDL_Window* Window){}
