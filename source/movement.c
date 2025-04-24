#include <SDL2/SDL.h>
#include <stdlib.h>

#include "../include/player.h"
#include "../include/movement.h" 
#include "../include/vector2.h"
#include "../include/input_logger.h"

#define MOVEUP 0

void move_player(Player *player, Vector2 *velocity) {
    // Hämta nuvarande position
    Vector2 *pos = Player_get_position(player);
    
    // Uppdatera positionen
    move_and_collide(Player_get_collider(player), velocity, 1);
    Vector2 *newPosition = copy_Vector2(Collider_get_position(Player_get_collider(player)));
    Player_set_position(player, newPosition);
}

void handle_movement(Player *player, float speed, Collider *ground) {
    Vector2 *direction = create_Vector2(0.0f, 0.0f);
    static float vertical_velocity = 0;  // Behåller hastighet mellan frames
    const float gravity = 0.6f;
    const float jump_force = -12.0f;
    float dashspeed = speed * 2;
    Player_set_can_dash(player, 1);
    int isJumping = 0;

    InputLogger *logger = Player_get_inputs(player);
    if (InputLogger_is_action_pressed(logger, "move_left")) {
        Vector2_set_x(direction, -1.0f);
    }
    else if (InputLogger_is_action_pressed(logger, "move_right")) {
        Vector2_set_x(direction, 1.0f);
    }

    if ((InputLogger_is_action_pressed(logger, "move_up") && is_colliding(Player_get_collider(player), ground, 1)))
    {
        vertical_velocity = jump_force;
        isJumping = 1;
    }

    /*if(keystates[SDL_SCANCODE_LSHIFT])
    {   
        if(Player_get_can_dash(player)==1)
        {
            speed = speed * 6;
            Player_set_can_dash(player, 0);
        }
    }*/

    vertical_velocity += gravity;

    Vector2 *velocity = create_Vector2(
        Vector2_get_x(direction) * speed,
        vertical_velocity
    );

    if(is_colliding(Player_get_collider(player), ground, 1)) { //ska checka alla plattformar, har bara en nu
        if(isJumping==0){
            Player_set_yposition(player, Collider_get_yposition(ground));
            vertical_velocity = 0;  // Nollställ fallhastighet
            Vector2_set_y(velocity, vertical_velocity);
        }
    }

    move_player(player, velocity); 

    destroy_Vector2(direction);
    destroy_Vector2(velocity);
}

