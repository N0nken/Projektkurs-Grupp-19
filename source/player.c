#include <SDL2/SDL.h>
#include <stdlib.h>

#include "../include/input_logger.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/sounds.h"

#define PLAYER_SIZE 32

struct Player {
    Vector2 *position;
    Collider *collider;
    Collider *attackHitbox;
    int hp;
    int weapon;
    int isAlive;
    SDL_Texture *spriteSheet;
    int canDash;
    SDL_Rect *rect; //la till pga behövs för textur
    InputLogger *logger;
    int direction; // left -1, right 1
    int state; // 0 = idle, 1 = moving, 2 = attacking
    float vertical_velocity;
};

enum Weapons {ROCK, SCISSORS, PAPER};

Player *create_Player(Vector2 *position, Collider *collider, Collider *attackHitbox, int hp, int weapon, int isAlive, Player *allPlayers[], int *activePlayerCount) {
    Player *newPlayer = malloc(sizeof(struct Player));
    newPlayer->position = position;
    newPlayer->collider = collider;
    newPlayer->attackHitbox = attackHitbox;
    newPlayer->hp = hp;
    newPlayer->weapon = weapon;
    newPlayer->isAlive = isAlive;
    newPlayer->canDash = 1;
    newPlayer->logger = create_InputLogger();

    newPlayer->rect = malloc(sizeof(struct SDL_Rect));
    newPlayer->rect->x = (int)Vector2_get_x(position) - (int)Vector2_get_x(Collider_get_dimensions(newPlayer->collider));
    newPlayer->rect->y = (int)Vector2_get_y(position) - (int)Vector2_get_y(Collider_get_dimensions(collider));
    newPlayer->rect->w = 64;
    newPlayer->rect->h = 64;
    allPlayers[(*activePlayerCount)++] = newPlayer;
    newPlayer->direction = 1; // default direction to right    
    return newPlayer;
}
int destroy_Player(Player *p) {
    destroy_Vector2(p->position);
    destroy_Collider(p->collider);
    destroy_Collider(p->attackHitbox);
    destroy_InputLogger(p->logger);
    free(p->rect);
    free(p);
    return 0;
}

/* Setters */
void Player_set_position(Player *p, struct Vector2 *position) {
    destroy_Vector2(p->position);

    p->position = position;    
    Collider_set_position(Player_get_collider(p), position);
    p->rect->x = (int)Vector2_get_x(position) - (int)Vector2_get_x(Collider_get_dimensions(p->collider));
    p->rect->y = (int)Vector2_get_y(position) - (int)Vector2_get_y(Collider_get_dimensions(p->collider));  //flytta på rect
}
void Player_set_yposition(Player *p, float y) {
    float current_x = Vector2_get_x(p->position);
    Vector2 *new_pos = create_Vector2(current_x, y);
    Player_set_position(p, new_pos);
}
void Player_set_vertical_velocity(Player *p, float velocity){
    p->vertical_velocity = velocity;
}
void Player_set_can_dash(Player *p, int yes) {
    p->canDash = yes; //yes 1 = kan dasha
}
void Player_set_collider(Player *p, Collider *collider) {
    destroy_Collider(p->collider);
    p->collider = collider;
}
void Player_set_attackHitbox(Player *p, Collider *attackHitbox) {
    destroy_Collider(p->attackHitbox);
    p->attackHitbox = attackHitbox;
}
void Player_set_hp(Player *p, int hp) {
    p->hp = hp;
}
void Player_set_weapon(Player *p, int weapon) {
    p->weapon = weapon;
}
void Player_set_isAlive(Player *p, int isAlive) {
    p->isAlive = isAlive;
}
void Player_set_direction(Player *p, int direction) {
    p->direction = direction;
}

/* Getters */
InputLogger *Player_get_inputs(Player *p) {
    return p->logger;
}
Vector2 *Player_get_position(Player *p) {
    return p->position;
}
float Player_get_yposition(Player *p) {
    return Vector2_get_y(p->position);
}
float Player_get_vertical_velocity(Player *p){
    return p->vertical_velocity;
}
int Player_get_can_dash(Player *p){
    return p->canDash;
}
Collider *Player_get_collider(Player *p) {
    return p->collider;
}
Collider *Player_get_attackHitbox(Player *p) {
    return p->attackHitbox;
}
int Player_get_hp(Player *p) {
    return p->hp;
}
int Player_get_weapon(Player *p) {
    return p->weapon;
}
int Player_get_isAlive(Player *p) {
    return p->isAlive;
}
SDL_Rect *Player_get_rect(Player *p) {
    return p->rect;
}
SDL_Texture *Player_get_weapon_sprite(Player *p, int weapon){
    switch (weapon)
    {
        case ROCK: return p->spriteSheet; 
        case SCISSORS: return p->spriteSheet;
        case PAPER: return p->spriteSheet;
    }
}
int Player_get_direction(Player *p) {
    return p->direction;
}
int Player_get_state(Player *p) {
    return p->state;
}

void switch_player_weapon(Player *p){
    if (InputLogger_is_action_just_pressed(p->logger, "switch_to_rock")) {
        Player_set_weapon(p, ROCK);
    }
    if (InputLogger_is_action_just_pressed(p->logger, "switch_to_scissors")) {
        Player_set_weapon(p, SCISSORS);
    }
    if (InputLogger_is_action_just_pressed(p->logger, "switch_to_paper")) {
        Player_set_weapon(p, PAPER);
    }

}


void health_bar(Player *p, SDL_Renderer *renderer){
    if(p->hp>0){
        SDL_Rect healthBar = {0, 0, 100, 10};
        healthBar.x = (int)Vector2_get_x(p->position)-40;
        healthBar.y = (int)Vector2_get_y(p->position)-60;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &healthBar);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        healthBar.w = p->hp;
        SDL_RenderFillRect(renderer, &healthBar);
    }
}

struct Frame{
    int x;
    int y;
    int h;
    int w;
} typedef frame;

SDL_Rect *get_Player_Frame(frame *f, int weapon, int animationCounter){

    f->x = animationCounter * PLAYER_SIZE;
    f->y = weapon * PLAYER_SIZE;
    f->w = PLAYER_SIZE;
    f->h = PLAYER_SIZE;

    return (SDL_Rect*)f;
}

int get_Animation_Counter(InputLogger *logger, int direction) {
    static Uint64 lastUpdate = 0;
    static int animationCounter = 0;
    static int offset = 0;
    static int frames = 0;

    static int inAttack = 0;          // 1 if currently in attack animation
    static Uint64 attackStartTime = 0;
    const Uint64 ATTACK_FRAME_DURATION = 200; // ms per frame
    const int ATTACK_FRAMES = 2;
    const int ATTACK_OFFSET = 4;

    Uint64 now = SDL_GetTicks64();

    // Start attack if pressed and not already in attack animation
    if (!inAttack && InputLogger_is_action_pressed(logger, "attack") > 0) {
        inAttack = 1;
        attackStartTime = now;
        animationCounter = 0;
        lastUpdate = now;
        switch (direction)
        {
            case 1:
                offset = ATTACK_OFFSET;
                break;
            case -1:
                offset = (ATTACK_OFFSET+2);
                break;
        }
        play_sound_effect("audio/sfx-hit.wav", 70);
        frames = ATTACK_FRAMES;
    }

    // If in attack animation
    if (inAttack) {
        if (now - lastUpdate >= ATTACK_FRAME_DURATION) {
            animationCounter++;
            lastUpdate = now;
        }

        // End attack animation after all frames played
        if (animationCounter >= ATTACK_FRAMES) {
            inAttack = 0;
            animationCounter = 0;
        }
        return offset + (animationCounter % ATTACK_FRAMES);
    }

    // Normal movement animations
    offset = get_animation_offset(logger);
    frames = get_Number_Of_Frames(logger);

    if ((InputLogger_is_action_pressed(logger, "move_right") > 0 || 
         InputLogger_is_action_pressed(logger, "move_left") > 0) && 
         now - lastUpdate >= 200) {
        animationCounter = (animationCounter + 1) % frames;
        lastUpdate = now;
    }

    return animationCounter + offset;
}


//behövs offset för att bestämma vart i x-led i spritesheeten
int get_animation_offset(InputLogger *logger) {
    if (InputLogger_is_action_pressed(logger, "move_right")==1) {return 0;}
    else if (InputLogger_is_action_pressed(logger, "move_left")==1){return 8; }
    else if (InputLogger_is_action_pressed(logger, "attack")==1){return 4; }
    return 0;
}

//behöver antalet frames den ska växla mellan, attack är 2 och gå är 4
int get_Number_Of_Frames(InputLogger *logger){

    if (InputLogger_is_action_pressed(logger, "move_right")==1) {return 4;}
    else if (InputLogger_is_action_pressed(logger, "move_left")==1){return 4; }
    else if (InputLogger_is_action_pressed(logger, "attack")==1){return 2;}   
}