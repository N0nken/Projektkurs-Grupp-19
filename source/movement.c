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
    move_and_collide(Player_get_collider(player), velocity, 0);
    Vector2 *newPosition = copy_Vector2(Collider_get_position(Player_get_collider(player)));
    Player_set_position(player, newPosition);

    if((int)Player_get_yposition(player)>1100){ // dödar spelare om man ramlar
        Player_set_isAlive(player, 0);
    }
}

void handle_movement(Player *player, float speed, Collider *platform1, Collider *platform2, Collider *platform3, Collider *platform4, float deltaTime) {
    Vector2 *direction = create_Vector2(0.0f, 0.0f);
    const float gravity = 0.6f;
    const float jump_force = -25.0f;

    static float dashTimeLeft     = 0.0f;
    static float dashCooldownLeft = 0.0f;

    const float dashDuration   = 0.6f;  // sekunder
    const float dashCooldown   = 3.0f;  // sekunder

    int isJumping = 0;

    
    InputLogger *logger = Player_get_inputs(player);
    if (InputLogger_is_action_pressed(logger, "move_left")) {
        Vector2_set_x(direction, -1.0f);
        Player_set_direction(player, -1);
    }
    else if (InputLogger_is_action_pressed(logger, "move_right")) {
        Vector2_set_x(direction, 1.0f);
        Player_set_direction(player, 1);
    }

    if (InputLogger_is_action_pressed(logger, "move_up") &&
    (is_standing_on(Player_get_collider(player),platform1) ||
    is_standing_on(Player_get_collider(player),platform2) || is_standing_on(Player_get_collider(player),platform3) || is_standing_on(Player_get_collider(player),platform4)) && Player_get_vertical_velocity(player)>=0)
    {
        Player_set_vertical_velocity(player, jump_force);
        isJumping = 1;
    }
    if(Player_get_vertical_velocity(player)<0){
        isJumping = 1;
    }

    
    if (InputLogger_is_action_just_pressed(logger, "dash") &&
        dashCooldownLeft <= 0.0f) {
        dashTimeLeft     = dashDuration;
        dashCooldownLeft = dashCooldown;
        Player_set_can_dash(player, 0);
    }

    // Tidsnerdragning för dash
    if (dashTimeLeft > 0.0f){
        dashTimeLeft     -= deltaTime;
    }
    if (dashTimeLeft < 0.0f){
        dashTimeLeft = 0.0f;
    }
    if (dashCooldownLeft > 0.0f) {
        dashCooldownLeft -= deltaTime;
        if (dashCooldownLeft <= 0.0f){
            Player_set_can_dash(player, 1);
            dashCooldownLeft=0.0f;
        }
    }


    Player_set_vertical_velocity(player, Player_get_vertical_velocity(player) + gravity); //applicera vertikal hastighet med gravity

    float currentSpeed = speed;
    if (dashTimeLeft > 0.0f){ //om dash pågår öka hastighet med 3
        currentSpeed *= 3.0f;
        Player_set_vertical_velocity(player, 0);
    }

    Vector2 *velocity = create_Vector2(
        Vector2_get_x(direction) * currentSpeed,
        Player_get_vertical_velocity(player)
    );


    if((is_standing_on(Player_get_collider(player),platform1) ||
    is_standing_on(Player_get_collider(player),platform2) || is_standing_on(Player_get_collider(player),platform3) || is_standing_on(Player_get_collider(player),platform4)) && Player_get_vertical_velocity(player)>=0){
        Player_set_vertical_velocity(player, 0);  // Nollställ fallhastighet i spelaren
        Vector2_set_y(velocity, Player_get_vertical_velocity(player)); //nollställ velocity
    }

    move_player(player, velocity); 

    destroy_Vector2(direction);
    destroy_Vector2(velocity);
}

