#include <stdlib.h>
#include <math.h>
#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/mathex.h"

struct Collider {
    Vector2 *position;
    Vector2 *dimensions;
    int isTrigger; // triggers allow other colliders to pass through them
};

Collider* create_Collider(Vector2 *position, Vector2 *dimensions, int isTrigger) {
    Collider *newCollider = malloc(sizeof(struct Collider));
    newCollider->dimensions = dimensions;
    newCollider->position = position;
    newCollider->isTrigger = isTrigger;
    return newCollider;
}

int destroy_Collider(Collider *collider) {
    free(collider);
    return 1;
}

int are_colliding(Collider *collider1, Collider *collider2) {
    Vector2 *vBetween = create_Vector2(Vector2_get_x(collider1->position) - Vector2_get_x(collider2->position), Vector2_get_y(collider1->position) - Vector2_get_y(collider2->position));
    float angleBetween = angle_of(vBetween);
    float xMulti = clampf(cosf(angleBetween*2),0.0,1.0);
    float yMulti = clampf(sinf(angleBetween*2),0.0,1.0);
    float dist = distance_to(collider1->position, collider2->position);
    float maxX = (Vector2_get_x(collider1->dimensions) + Vector2_get_x(collider2->dimensions))*xMulti;
    float maxY = (Vector2_get_y(collider1->dimensions) + Vector2_get_y(collider2->dimensions))*yMulti;
    float maxDist = sqrtf(powf(maxX, 2.0) + powf(maxY, 2.0));
    if (dist < maxDist) {
        return 1;
    }
    return 0;
}
