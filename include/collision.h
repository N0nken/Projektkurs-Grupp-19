#ifndef COLLISION_H_   /* Include guard */
#define COLLISION_H_
#include "vector2.h"

typedef struct Collider Collider;

Collider* create_Collider(Vector2 *position, Vector2 *dimensions, int isTrigger);
/* Deallocates all memory to the struct object including struct fields */
int destroy_Collider(Collider *collider);
void print_Collider(Collider *collider);
/* Setters */

/* Getters */
Vector2 *Collider_get_position(Collider *collider);
Vector2 *Collider_get_dimensions(Collider *collider);


/* Checks if two colliders are colliding */
int is_colliding(Collider *collider1, Collider *collider2);

#endif