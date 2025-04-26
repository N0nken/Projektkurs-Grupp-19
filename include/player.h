#ifndef PLAYER_H_   /* Include guard */
#define PLAYER_H_

#include "input_logger.h"
#include "collision.h"
#include "vector2.h"
#include <SDL2/SDL.h>

#define PLAYERWIDTH 64
#define PLAYERHEIGHT 64
#define PLAYERATTACKHITBOXWIDTH 20
#define PLAYERATTACKHITBOXHEIGHT 20
#define PLAYERATTACKHITBOXOFFSETX 20
#define PLAYERATTACKHITBOXOFFSETY 20

typedef struct Player Player;
typedef struct Frame frame;

Player *create_Player(Vector2 *position, Collider *collider, Collider *attackHitbox, int hp, int weapon, int isAlive, Player *allPlayers[], int *activePlayerCount);
/* Deallocates all memory to the struct object including struct fields */
int destroy_Player(Player *p);

/* Setters */
/* */
void Player_set_position(Player *p, Vector2 *position);
void Player_set_yposition(Player *p, float y);
void Player_set_can_dash(Player *p, int yes);
void Player_set_collider(Player *p, Collider *collider);
void Player_set_attackHitbox(Player *p, Collider *attackHitbox);
void Player_set_hp(Player *p, int hp);
void Player_set_weapon(Player *p, int weapon);
void Player_set_isAlive(Player *p, int isAlive);
void Player_set_direction(Player *p, int direction);

/* Getters */
/* */
InputLogger *Player_get_inputs(Player *p);
Vector2 *Player_get_position(Player *p);
float Player_get_yposition(Player *p);
int Player_get_can_dash(Player *p);
Collider *Player_get_collider(Player *p);
Collider *Player_get_attackHitbox(Player *p);
int Player_get_hp(Player *p);
int Player_get_weapon(Player *p);
int Player_get_isAlive(Player *p);
SDL_Rect *Player_get_rect(Player *p);
SDL_Texture *Player_get_weapon_sprite(Player *p, int weapon);
int Player_get_direction(Player *p);
int Player_get_state(Player *p);

void deal_damage(Player *player, int damage);

void switch_player_weapon(Player *p, int keyPressed);
void switch_player_weapon_sprite(Player *p, int Key, int *pCurrentImage);

SDL_Rect *get_Player_Frame(frame *f, int weapon, int animationCounter);
int get_animation_offset(InputLogger *logger);
int get_Animation_Counter(InputLogger *logger);
int get_Number_Of_Frames(InputLogger *logger);

#endif