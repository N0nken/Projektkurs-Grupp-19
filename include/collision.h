#ifndef COLLISION_H_   /* Include guard */
#define COLLISION_H_
#include "vector2.h"

typedef struct Collider Collider;

Collider* create_Collider(Vector2 *position, Vector2 *dimensions, int isTrigger, int layer);

/* Deallocates all memory to the struct object including struct fields */
int destroy_Collider(Collider *collider);
void print_Collider(Collider *collider);
/* Setters */
void Collider_set_position(Collider *collider, Vector2 *position);
void Collider_set_dimensions(Collider *collider, Vector2 *dimensions);
void Collider_set_trigger(Collider *collider, int isTrigger);


/* Getters */
Vector2 *Collider_get_position(Collider *collider);
Vector2 *Collider_get_dimensions(Collider *collider);
int Collider_is_trigger(Collider *collider);
int Collider_get_id(Collider *collider);
int Collider_get_layer(Collider *collider);

/* Checks if two colliders are colliding */
int is_colliding(Collider *collider1, Collider *collider2, int layer);
/* Moves a collider along a given vector2 until it collides with another collider */
void move_and_collide(Collider *collider, Vector2 *velocity, int layer);

int is_colliding_any(Collider *collider, int layer);

#endif