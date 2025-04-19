#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "player.h"  
#include "vector2.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include "input_logger.h"

void move_player(Player *player, Vector2 *velocity);
void handle_movement(Player *player, float speed, Input_Logger *logger, Collider *ground);
#endif