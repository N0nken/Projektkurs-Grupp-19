#include <SDL2/SDL.h>
#include <stdlib.h>

#include "../include/player.h"
#include "../include/movement.h" 
#include "../include/vector2.h"

#define MOVEUP 0

void update_player_position(Player *player, Vector2 *velocity) {
    // Hämta nuvarande position
    Vector2 *pos = Player_get_position(player);
    
    // Uppdatera positionen
    Player_set_position(player, Vector2_addition(pos, velocity));
}



