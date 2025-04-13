#ifndef ATTACKS_H_   /* Include guard */
#define ATTACKS_H_

#include "player.h"

void deal_damage(Player *player, int damage);
void attack(Player *attackingPlayer, Player *allPlayers[], int activePlayerCount);
void handle_attack_input(Player *allPlayers[], int activePlayerCount);

#endif