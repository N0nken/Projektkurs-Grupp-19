#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "../include/menu.h"



struct button{
    SDL_Rect area;
    SDL_Color color;
    TTF_Font* font;
    bool is_hovering;
    bool is_clicked; 
};
typedef struct button button;

//creat and destroy
button *button_create(int x, int y, int w,int h, TTF_Font *Font, SDL_Renderer *Renderer, SDL_Window* Window){
    button *Button= malloc(sizeof(button));
    

    Button->area = (SDL_Rect){x, y, w, h };
    Button->is_hovering =false;
    Button->is_clicked =false;
    Button->color= (SDL_Color){0,0,0,255};
    if(!Font){
        Button->font= TTF_OpenFont("fonts/p.ttf", 20);
        if(!Button->font){
            printf("The fonts for buttons failed%s\n",TTF_GetError());
        }
    }
    else Button->font=Font;
    
    
    return Button;
}

void button_destroy(button *Button){
    TTF_CloseFont(Button->font);
    free(Button);
}


//hover

void set_button_hover_true(button *Button){
    Button->is_hovering=true;
}

void set_button_hover_false(button *Button){
    Button->is_hovering=false;
}

bool ret_button_hover_state(button *Button){
    return Button->is_hovering;
}

//click

void set_button_click_true(button *Button){
    Button->is_clicked=true;
}

void set_button_click_false(button *Button){
    Button->is_clicked=false;
}

bool ret_button_click_state(button * Button){
    return Button->is_clicked;
}


// rect

bool is_in_button_rect(int x, int y, SDL_Rect r) {
    if (x>r.x && x<(r.x+r.w) && y>r.y && y<(r.y+r.h))
    {
        return true;
    }
    else
    {
        return false;
    }
}

SDL_Rect ret_button_rect(button *Button){
    return Button->area;
}


// states

void ret_button_normal(SDL_Rect Area, SDL_Renderer *Renderer){
    SDL_SetRenderDrawColor(Renderer, 230, 230, 230, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(Renderer, &Area);

}

void ret_button_hover(SDL_Rect Area, SDL_Renderer *Renderer){
    SDL_SetRenderDrawColor(Renderer, 220, 220, 220, 255);
    SDL_RenderFillRect(Renderer, &Area);
}

void ret_button_clicked(SDL_Rect Area, SDL_Renderer *Renderer){
    SDL_SetRenderDrawColor(Renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(Renderer, &Area);
}

void ret_button_image(SDL_Rect Area, SDL_Renderer *Renderer, SDL_Surface* img){
    SDL_Texture* img_Texture= SDL_CreateTextureFromSurface(Renderer, img);
    SDL_RenderCopy(Renderer,img_Texture, NULL, &Area);

}

void button_state_select(int state, SDL_Rect Area, SDL_Renderer *Renderer){
    if(state==0){
        ret_button_normal(Area,Renderer);
    }
    else if(state==1){
        ret_button_hover(Area, Renderer);
    }
    else if(state==2){
        ret_button_clicked(Area, Renderer);
    }
    else printf("You are inputting the wrong thing to the button_state_select function");
}

// ttf
void load_Button_Text(button *Button, const char *text, SDL_Renderer* Renderer){
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Button->font, text, Button->color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(Renderer, surfaceMessage);
    SDL_RenderCopy(Renderer, Message, NULL, &Button->area);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void center_button(button *Button,SDL_Window *Window){
    int winW, winH;
    SDL_GetWindowSize(Window, &winW, &winH);
    Button->area.x=  960-Button->area.w;
}
void load_button_image(button *Button, SDL_Renderer *renderer, int buttonNr, int state){
    SDL_Surface* button=NULL;
    SDL_Texture* textureButton=NULL;
    SDL_Rect ButtonState;
    if(state==3){
        SDL_Rect temp={0,0,48,16};
        ButtonState=temp;
    }
    else if(state==2){  
        SDL_Rect temp={48,0,48,16};
        ButtonState=temp;

    }
    else if(state==1){
        SDL_Rect temp={96,0,48,16};
        ButtonState=temp;
    }


    switch(buttonNr){
        case 0:
        button= IMG_Load("images/Button/[0]LightBlue.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,&ButtonState, &Button->area);
        break;
        case 1:
        button= IMG_Load("images/Button/[1] Red.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,&ButtonState, &Button->area);
        break;
        case 2:
        button= IMG_Load("images/Button/[2] Yellow.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,&ButtonState, &Button->area);
        break;
        case 3:
        button= IMG_Load("images/Button/[3] Green.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,&ButtonState, &Button->area);
        break;
        case 4:
        button= IMG_Load("images/Button/[4] Blue.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,&ButtonState, &Button->area);
        break;
        case 5:
        button= IMG_Load("images/Button/[5] Silver.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,&ButtonState, &Button->area);
        break;
        case 6:
        button= IMG_Load("images/Button/[6] Gold.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,&ButtonState, &Button->area);
        break;
        case 7:
        button= IMG_Load("images/Button/[7] Wood.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,&ButtonState, &Button->area);
        break;
        case 8:
        button= IMG_Load("images/Button/arrow-square-left.png");
        textureButton= SDL_CreateTextureFromSurface(renderer,button);
        SDL_RenderCopy(renderer,textureButton,NULL, &Button->area);
        break;




    }
    if(!button){
        printf("Failed to get button: %s",SDL_GetError());
    }

    
    SDL_DestroyTexture(textureButton);
    SDL_FreeSurface(button);
}