#include <stdlib.h>

#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"

#define BASEDAMAGE 5
#define DAMAGEBONUSMULTIPLIER 2
#define DAMAGEPENALTYMULTIPLIER 0.5

enum Weapons {ROCK, SCISSORS, PAPER};

void deal_damage(Player *player, int damage) {
    Player_set_hp(player, Player_get_hp(player) - damage);
    if (Player_get_hp(player) <= 0) {
        Player_set_isAlive(player, 0);
    }
}

void attack(Player *attackingPlayer, Player *allPlayers[], int activePlayerCount) {
    Collider *attackHitbox = Player_get_attackHitbox(attackingPlayer);
    Vector2 *origin = create_Vector2(Vector2_get_x(Collider_get_position(attackHitbox)), Vector2_get_y(Collider_get_position(attackHitbox)));
    Collider_set_position(attackHitbox, Vector2_addition(Player_get_position(attackingPlayer), Collider_get_position(attackHitbox)));
    for (int i = 0; i < activePlayerCount; i++) {
        if (allPlayers[i] == attackingPlayer) continue;
        Player *defendingPlayer = allPlayers[i];
        if (is_colliding(Player_get_attackHitbox(attackingPlayer), Player_get_hurtbox(defendingPlayer))) {
            int damage = BASEDAMAGE;
            int attackingPlayerWeapon = Player_get_weapon(attackingPlayer);
            int defendingPlayerWeapon = Player_get_weapon(defendingPlayer);
            if ((attackingPlayerWeapon == ROCK && defendingPlayerWeapon == SCISSORS) || 
                (attackingPlayerWeapon == SCISSORS && defendingPlayerWeapon == PAPER) ||
                (attackingPlayerWeapon == PAPER && defendingPlayerWeapon == ROCK)) {
                damage *= DAMAGEBONUSMULTIPLIER;
            } else if ((attackingPlayerWeapon == ROCK && defendingPlayerWeapon == PAPER) || 
                (attackingPlayerWeapon == SCISSORS && defendingPlayerWeapon == ROCK) ||
                (attackingPlayerWeapon == PAPER && defendingPlayerWeapon == SCISSORS)) {
                damage *= DAMAGEPENALTYMULTIPLIER;
            }
            deal_damage(defendingPlayer, damage);
        }
    }
    Collider_set_position(attackHitbox, origin);
}