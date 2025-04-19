#include <SDL2/SDL.h>
#include <stdlib.h>

#include "../include/input_logger.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"

struct Player {
    Vector2 *position;
    Collider *collider;
    Collider *hurtbox;
    Collider *attackHitbox;
    int hp;
    int weapon;
    int isAlive;
    SDL_Texture *spriteSheet;
    int canDash;
    SDL_Rect *rect; //la till pga behövs för textur
    Input_Logger *logger;
};

enum Weapons {ROCK, SCISSORS, PAPER};

Player *create_Player(Vector2 *position, Collider *collider, Collider *hurtbox, Collider *attackHitbox, int hp, int weapon, int isAlive, Player *allPlayers[], int *activePlayerCount) {
    Player *newPlayer = malloc(sizeof(struct Player));
    newPlayer->position = position;
    newPlayer->collider = collider;
    newPlayer->hurtbox = hurtbox;
    newPlayer->attackHitbox = attackHitbox;
    newPlayer->hp = hp;
    newPlayer->weapon = weapon;
    newPlayer->isAlive = isAlive;
    newPlayer->canDash = 1;
    newPlayer->logger = create_Input_Logger();

    newPlayer->rect = malloc(sizeof(struct SDL_Rect));
    newPlayer->rect->x = (int)Vector2_get_x(position);
    newPlayer->rect->y = (int)Vector2_get_y(position);
    newPlayer->rect->w = 64;
    newPlayer->rect->h = 64;
    allPlayers[(*activePlayerCount)++] = newPlayer;
    
    return newPlayer;
}
int destroy_Player(Player *p) {
    destroy_Vector2(p->position);
    destroy_Collider(p->collider);
    destroy_Collider(p->hurtbox);
    destroy_Collider(p->attackHitbox);
    destroy_Input_Logger(p->logger);
    free(p->rect);
    free(p);
    return 0;
}

/* Setters */
void Player_set_position(Player *p, struct Vector2 *position) {
    printf("g");
    destroy_Vector2(p->position);

    Vector2 *hurtboxposition = create_Vector2(Vector2_get_x(position), Vector2_get_y(position));
    p->position = position;    
    p->rect->x = (int)Vector2_get_x(position);
    p->rect->y = (int)Vector2_get_y(position);  //flytta på rect
    //destroy_Vector2(Collider_get_position(Player_get_hurtbox(p)));
    //Collider_set_position(Player_get_hurtbox(p), hurtboxposition);
    printf("h");
}
void Player_set_yposition(Player *p, float y) {
    float current_x = Vector2_get_x(p->position);
    Vector2 *new_pos = create_Vector2(current_x, y);
    Player_set_position(p, new_pos);
}
void Player_set_can_dash(Player *p, int yes) {
    p->canDash = yes; //yes 1 = kan dasha
}
void Player_set_collider(Player *p, Collider *collider) {
    destroy_Collider(p->collider);
    p->collider = collider;
}
void Player_set_hurtbox(Player *p, Collider *hurtbox) {
    destroy_Collider(p->hurtbox);
    p->hurtbox = hurtbox;
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


/* Getters */
Input_Logger *Player_get_inputs(Player *p) {
    return p->logger;
}
Vector2 *Player_get_position(Player *p) {
    return p->position;
}
float Player_get_yposition(Player *p) {
    return Vector2_get_y(p->position);
}
int Player_get_can_dash(Player *p){
    return p->canDash;
}
Collider *Player_get_collider(Player *p) {
    return p->collider;
}
Collider *Player_get_hurtbox(Player *p) {
    return p->hurtbox;
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

void switch_player_weapon(Player *p, int keyPressed){
    switch (keyPressed){
        case SDLK_1: Player_set_weapon(p, ROCK);break;
        case SDLK_2: Player_set_weapon(p, SCISSORS);break;
        case SDLK_3: Player_set_weapon(p, PAPER);break;
    }   
}

void switch_player_weapon_sprite(Player *p, int weapon, int *pCurrentWeaponImage){
    switch (weapon){
    case 0:
        //currentWeaponImage() = rockImage;
        break;
    case 1:
        //currentWeaponImage() = scissorImage;
        break;
    case 2:
        //currentWeaponImage() = paperImage;
        break;
    }
}