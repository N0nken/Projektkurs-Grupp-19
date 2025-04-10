#include <stdlib.h>
#include <SDL2/SDL.h> 

#include "../include/player.h"
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/attacks.h"


struct Player {
    Vector2 *position;
    Collider *collider;
    Collider *hurtbox;
    Collider *attackHitbox;
    int hp;
    int weapon;
    int isAlive;
    SDL_Texture *spriteSheet;
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
    return newPlayer;
}
int destroy_Player(Player *p) {
    destroy_Vector2(p->position);
    destroy_Collider(p->collider);
    destroy_Collider(p->hurtbox);
    destroy_Collider(p->attackHitbox);
    free(p);
    return 0;
}

/* Setters */
void Player_set_position(Player *p, Vector2 *position) {
    destroy_Vector2(p->position);
    p->position = position;
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


void Player_get_weapon_sprite(Player *p, int weapon){
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

void Switch_Player_Weapon_Sprite(Player *p, int weapon, int *pCurrentWeaponImage){
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