#include <SDL2/SDL.h>
#include <stdlib.h>


#include "../include/player.h"
#include "../include/movement.h" 
#include "../include/vector2.h"

void update_player_position(Player *player, const Uint8 *keystates, Vector2 *Velocity) {
    float speed = 5.0;

    // Nollställ velocity först varje frame

    // Lägg till input-baserad rörelse
    if (keystates[SDL_SCANCODE_W]) {
        Vector2_set_y(Velocity, -speed);
    }
    if (keystates[SDL_SCANCODE_S]) {
        Vector2_set_y(Velocity, speed);
    }
    if (keystates[SDL_SCANCODE_A]) {
        Vector2_set_x(Velocity, -speed);
    }
    if (keystates[SDL_SCANCODE_D]) {
        Vector2_set_x(Velocity, speed);
    }

    // Hämta nuvarande position
    Vector2 *pos = Player_get_position(player);

    // Uppdatera positionen
    Vector2_set_x(pos, Vector2_get_x(pos) + Vector2_get_x(Velocity));
    Vector2_set_y(pos, Vector2_get_y(pos) + Vector2_get_y(Velocity));

    // Uppdatera spelarens position & rect
    Player_set_position(player, pos);
}



