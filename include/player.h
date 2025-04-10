#ifndef PLAYER_H_   /* Include guard */
#define PLAYER_H_

#include "collision.h"
#include "vector2.h"

typedef struct Player Player;

Player *create_Player(Vector2 *position, Collider *collider, Collider *hurtbox, Collider *attackHitbox, int hp, int weapon, int isAlive);
/* Deallocates all memory to the struct object including struct fields */
int destroy_Player(Player *p);

/* Setters */

void Player_set_position(Player *p, Vector2 *position);
void Player_set_collider(Player *p, Collider *collider);
void Player_set_hurtbox(Player *p, Collider *hurtbox);
void Player_set_attackHitbox(Player *p, Collider *attackHitbox);
void Player_set_hp(Player *p, int hp);
void Player_set_weapon(Player *p, int weapon);
void Player_set_isAlive(Player *p, int isAlive);

/* Getters */

Vector2 *Player_get_position(Player *p);
Collider *Player_get_collider(Player *p);
Collider *Player_get_hurtbox(Player *p);
Collider *Player_get_attackHitbox(Player *p);
int Player_get_hp(Player *p);
int Player_get_weapon(Player *p);
int Player_get_isAlive(Player *p);

void deal_damage(Player *player, int damage);

void switch_player_weapon(Player *p, int keyPressed);
void SwitchPlayerWeaponSprite(Player *p, int Key, int *pCurrentImage);

#endif