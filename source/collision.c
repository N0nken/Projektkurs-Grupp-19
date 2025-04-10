#include <stdio.h>
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
    destroy_Vector2(Collider_get_position(collider));
    destroy_Vector2(Collider_get_dimensions(collider));
    free(collider);
    return 0;
}

/* Setters */
void Collider_set_position(Collider *collider, Vector2 *position) {
    destroy_Vector2(collider->position);
    collider->position = position;
}
void Collider_set_dimensions(Collider *collider, Vector2 *dimensions) {
    destroy_Vector2(collider->dimensions);
    collider->dimensions = dimensions;
}
void Collider_set_isTrigger(Collider *collider, int isTrigger) {
    collider->isTrigger = isTrigger;
}

/* Getters */
Vector2 *Collider_get_position(Collider *collider) {
    return collider->position;
}
Vector2 *Collider_get_dimensions(Collider *collider) {
    return collider->dimensions;
}
int Collider_get_isTrigger(Collider *collider) {
    return collider->isTrigger;
}

void print_Collider(Collider *collider) {
    printf("Position: ");
    print_Vector2(Collider_get_position(collider));

    printf("Dimensions: ");
    print_Vector2(Collider_get_dimensions(collider));

    printf("isTrigger: %d\n", Collider_get_isTrigger(collider));
}

int is_colliding(Collider *collider1, Collider *collider2) {
    if (Vector2_get_x(Collider_get_position(collider1)) == Vector2_get_x(Collider_get_position(collider2)) &&
        Vector2_get_y(Collider_get_position(collider1)) ==  Vector2_get_y(Collider_get_position(collider2))) {
        return 1;
    }
    Vector2 *vBetween = Vector2_subtraction(Collider_get_position(collider1), Collider_get_position(collider2));
    float angleBetween = angle_of(vBetween);
    float xMulti = clampf(cosf(angleBetween*2),0.0,1.0);
    float yMulti = clampf(sinf(angleBetween*2),0.0,1.0);
    float dist = distance_to(Collider_get_position(collider1), Collider_get_position(collider2));
    float maxX = (Vector2_get_x(Collider_get_dimensions(collider1)) + Vector2_get_x(Collider_get_dimensions(collider2)))*xMulti;
    float maxY = (Vector2_get_y(Collider_get_dimensions(collider1)) + Vector2_get_y(Collider_get_dimensions(collider2)))*yMulti;
    float maxDist = sqrtf(powf(maxX, 2.0) + powf(maxY, 2.0));
    if (dist < maxDist) {
        return 1;
    }
    return 0;
}
