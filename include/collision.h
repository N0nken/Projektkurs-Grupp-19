#ifndef COLLISION_H_   /* Include guard */
#define COLLISION_H_
#include "vector2.h"

typedef struct Collider Collider;

Collider* create_Collider(Vector2 *position, Vector2 *dimensions, int isTrigger);
int destroy_Collider(Collider *collider);
/* Checks if two colliders are colliding */
int are_colliding(Collider *collider1, Collider *collider2);

#endif