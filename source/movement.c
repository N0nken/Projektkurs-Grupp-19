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



void handle_movement(Player *player, float speed, const Uint8 *keystates) {
    Vector2 *direction = create_Vector2(0.0f, 0.0f);
    static float vertical_velocity = 0;  // Behåller hastighet mellan frames
    const float gravity = 0.6f;
    const float jump_force = -12.0f;
    float dashspeed = speed * 2;
    Player_set_can_dash(player, 1);

    if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT]) {
        Vector2_set_x(direction, -1.0f);
    }
    else if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {
        Vector2_set_x(direction, 1.0f);
    }

    if ((keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP]) && Player_get_yposition(player)>=400) //här måste ändras när plattformar läggs till, använd sdl_HASintersect
    {
        vertical_velocity = jump_force;
    }

    if(keystates[SDL_SCANCODE_LSHIFT])
    {   
        if(Player_get_can_dash(player)==1)
        {
            speed = speed * 6;
            Player_set_can_dash(player, 0);
        }
    }

    vertical_velocity += gravity;

    Vector2 *velocity = create_Vector2(
        Vector2_get_x(direction) * speed,
        vertical_velocity
    );

    update_player_position(player, velocity); 

    if(Player_get_yposition(player) > 400) { //måste uppdateras, ska använda SDL_has_intersect
        Player_set_yposition(player, 400);
        vertical_velocity = 0;  // Nollställ fallhastighet
    }

    destroy_Vector2(direction);
    destroy_Vector2(velocity);
}


