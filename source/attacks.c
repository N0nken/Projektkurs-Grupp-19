#include <stdlib.h>

#include "../include/attacks.h"
#include "../include/player.h"
#include "../include/collision.h"
#include "../include/input_logger.h"
#include "../include/sounds.h"

#define BASEDAMAGE 5
#define DAMAGEBONUSMULTIPLIER 2
#define DAMAGEPENALTYMULTIPLIER 0.5

enum Weapons {ROCK, SCISSORS, PAPER};

void deal_damage(Player *player, int damage) {
    Player_set_hp(player, Player_get_hp(player) - damage);
    if (Player_get_hp(player) <= 0) {
        Player_set_isAlive(player, 0);
    }
    play_sound_effect("audio/sfx-hurt.wav", 60);
}

void attack(Player *attackingPlayer, Player *allPlayers[], int activePlayerCount) {
    // play attack animation
    Collider *attackHitbox = Player_get_attackHitbox(attackingPlayer);
    Vector2 *origin = create_Vector2(Vector2_get_x(Collider_get_position(attackHitbox)), Vector2_get_y(Collider_get_position(attackHitbox)));
    Vector2 *offset = create_Vector2(Vector2_get_x(Collider_get_position(attackHitbox)) * Player_get_direction(attackingPlayer), Vector2_get_y(Collider_get_position(attackHitbox)));
    Collider_set_position(attackHitbox, Vector2_addition(Player_get_position(attackingPlayer), offset));
    destroy_Vector2(offset);
    for (int i = 0; i < activePlayerCount; i++) {
        if (allPlayers[i] == attackingPlayer) continue;
        Player *defendingPlayer = allPlayers[i];
        if (is_colliding(Player_get_attackHitbox(attackingPlayer), Player_get_collider(defendingPlayer), -1)) {
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

void handle_attack_input(Player *allPlayers[], int activePlayerCount) {
    for (int i = 0; i < activePlayerCount; i++) {
        Player *p = allPlayers[i];
        InputLogger *logger = Player_get_inputs(p);
        if (InputLogger_is_action_just_pressed(logger, "attack")) {
            attack(p, allPlayers, activePlayerCount);
            play_sound_effect("audio/sfx-hit.wav", 70);
        }
    }
}