#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "player.h"  
#include "vector2.h"

void update_player_position(Player *player, const Uint8 *keystates, Vector2 *Velocity);

#endif