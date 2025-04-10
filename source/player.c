#include <stdlib.h>

#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/attacks.h"
#include <SDL2/SDL.h>


struct Player {
    Vector2 *position;
    Collider *collider;
    Collider *hurtbox;
    Collider *attackHitbox;
    int hp;
    int weapon;
    int isAlive;
    SDL_Rect *rect; //la till pga behövs för textur
};

enum Weapons {ROCK, SCISSORS, PAPER};

Player *create_Player(Vector2 *position, Collider *collider, Collider *hurtbox, Collider *attackHitbox, int hp, int weapon, int isAlive) {
    Player *newPlayer = malloc(sizeof(struct Player));
    newPlayer->position = position;
    newPlayer->collider = collider;
    newPlayer->hurtbox = hurtbox;
    newPlayer->attackHitbox = attackHitbox;
    newPlayer->hp = hp;
    newPlayer->weapon = weapon;
    newPlayer->isAlive = isAlive;

    newPlayer->rect = malloc(sizeof(struct SDL_Rect));
    newPlayer->rect->x = (int)Vector2_get_x(position);
    newPlayer->rect->y = (int)Vector2_get_y(position);
    newPlayer->rect->w = 64;
    newPlayer->rect->h = 64;


    return newPlayer;
}
int destroy_Player(Player *p) {
    destroy_Vector2(p->position);
    destroy_Collider(p->collider);
    destroy_Collider(p->hurtbox);
    destroy_Collider(p->attackHitbox);
    free(p->rect);
    free(p);
    return 0;
}

/* Setters */
void Player_set_position(Player *p, struct Vector2 *position) {
    destroy_Vector2(p->position);
    p->position = position;
    
    p->rect->x = (int)Vector2_get_x(position);
    p->rect->y = (int)Vector2_get_y(position);  //flytta på rect
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
Vector2 *Player_get_position(Player *p) {
    return p->position;
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

void SwitchPlayerWeapon(Player *p, int Key){
    switch (Key){
    case 1:
        p->weapon = ROCK;
        break;
    case 2:
        p->weapon = SCISSORS;
        break;
    case 3:
        p->weapon = PAPER;    
        break;
    }   
}

void SwitchPlayerWeaponSprite(Player *p, int Key, int *pCurrentImage){
    switch (Key){
    case 1:
        //currentWeaponImage = rockImage;
        break;
    case 2:
        //currentWeaponImage = scissorImage;
        break;
    case 3:
        //currentWeaponImage = paperImage;
        break;
    }
}