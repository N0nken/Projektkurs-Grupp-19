#include <SDL2/SDL.h>
#include <stdlib.h>


#include "../include/player.h"
#include "../include/movement.h" 
#include "../include/vector2.h"


void update_player_position(Player *player, const Uint8 *keystates, Vector2 *Velocity) {


    int speed = 5;

    if (keystates[SDL_SCANCODE_W]) {
        Vector2_set_x(Velocity, 5);
    }

    Player_set_position(player, Velocity);
}
