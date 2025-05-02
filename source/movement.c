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
}

void handle_movement(Player *player, float speed, Collider *platform1, Collider *platform2, Collider *platform3, float deltaTime) {
    Vector2 *direction = create_Vector2(0.0f, 0.0f);
    const float gravity = 0.6f;
    const float jump_force = -16.0f;

    static float dashTimeLeft     = 0.0f;
    static float dashCooldownLeft = 0.0f;

    const float dashDuration   = 0.6f;  // sekunder
    const float dashCooldown   = 3.0f;  // sekunder

    int isJumping = 0;

    
    InputLogger *logger = Player_get_inputs(player);
    if (InputLogger_is_action_pressed(logger, "move_left")) {
        Vector2_set_x(direction, -1.0f);
    }
    else if (InputLogger_is_action_pressed(logger, "move_right")) {
        Vector2_set_x(direction, 1.0f);
    }

    if (InputLogger_is_action_pressed(logger, "move_up") &&
    (is_colliding(Player_get_collider(player), platform1, 1) ||
     is_colliding(Player_get_collider(player), platform2, 1) || is_colliding(Player_get_collider(player), platform3, 1)) && Player_get_vertical_velocity(player)>=0)
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


    //vertical_velocity += gravity;
    Player_set_vertical_velocity(player, Player_get_vertical_velocity(player) + gravity);

    float currentSpeed = speed;
    if (dashTimeLeft > 0.0f){
        currentSpeed *= 3.0f;
        //vertical_velocity = 0;
        Player_set_vertical_velocity(player, 0);
    }

    Vector2 *velocity = create_Vector2(
        Vector2_get_x(direction) * currentSpeed,
        Player_get_vertical_velocity(player)
    );

    if(((is_colliding(Player_get_collider(player), platform1, 1)) && Player_get_vertical_velocity(player)>=0)) { //ska checka alla plattformar, har bara en nu
        if(isJumping==0){
            //vertical_velocity = 0;  // Nollställ fallhastighet
            Player_set_vertical_velocity(player, 0);
            Vector2_set_y(velocity, Player_get_vertical_velocity(player));
            //Player_set_yposition(player, Collider_get_yposition(platform1)); //vet ej varför -22 men det blir fel annars
            
        }
    }
    else if(((is_colliding(Player_get_collider(player), platform2, 1)) && Player_get_vertical_velocity(player)>=0)) { //ska checka alla plattformar, har bara en nu
        if(isJumping==0){
            Player_set_vertical_velocity(player, 0);  // Nollställ fallhastighet
            Vector2_set_y(velocity, Player_get_vertical_velocity(player));
            //Player_set_yposition(player, Collider_get_yposition(platform2));
            
        }
    }
    else if(((is_colliding(Player_get_collider(player), platform3, 1)) && Player_get_vertical_velocity(player)>=0)) { //ska checka alla plattformar, har bara en nu
        if(isJumping==0){
            Player_set_vertical_velocity(player, 0);  // Nollställ fallhastighet
            Vector2_set_y(velocity, Player_get_vertical_velocity(player));
            //Player_set_yposition(player, Collider_get_yposition(platform3));
            
        }
    }
    move_player(player, velocity); 

    destroy_Vector2(direction);
    destroy_Vector2(velocity);
}

