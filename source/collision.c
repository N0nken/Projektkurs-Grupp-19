#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../include/collision.h"
#include "../include/vector2.h"
#include "../include/mathex.h"

#define VELOCITYFRAMESIZE 5
#define MAXCOLLIDERCOUNT 50 // increase if necessary
Collider *allColliders[MAXCOLLIDERCOUNT];
int activeColliderCount = 0;

struct Collider {
    Vector2 *position;
    Vector2 *dimensions;
    int isTrigger; // triggers allow other colliders to pass through them
    int id;
};

Collider* create_Collider(Vector2 *position, Vector2 *dimensions, int isTrigger) {
    if (activeColliderCount >= MAXCOLLIDERCOUNT) {
        printf("Maximum number of colliders reached. Increase MAXCOLLIDERCOUNT if this is intentional.\n");
        return NULL;
    }
    Collider *newCollider = malloc(sizeof(struct Collider));
    newCollider->dimensions = dimensions;
    newCollider->position = position;
    newCollider->isTrigger = isTrigger;
    allColliders[activeColliderCount] = newCollider;
    newCollider->id = activeColliderCount++;
    return newCollider;
}

int destroy_Collider(Collider *collider) {
    destroy_Vector2(Collider_get_position(collider));
    destroy_Vector2(Collider_get_dimensions(collider));
    for (int i = Collider_get_id(collider); i < activeColliderCount-1; i++) {
        allColliders[i] = allColliders[i+1];
    }
    activeColliderCount--;
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
void Collider_set_trigger(Collider *collider, int isTrigger) {
    collider->isTrigger = isTrigger;
}

/* Getters */
Vector2 *Collider_get_position(Collider *collider) {
    return collider->position;
}
Vector2 *Collider_get_dimensions(Collider *collider) {
    return collider->dimensions;
}
int Collider_is_trigger(Collider *collider) {
    return collider->isTrigger;
}
int Collider_get_id(Collider *collider) {
    return collider->id;
}

void print_Collider(Collider *collider) {
    printf("Position: ");
    print_Vector2(Collider_get_position(collider));

    printf("Dimensions: ");
    print_Vector2(Collider_get_dimensions(collider));

    printf("isTrigger: %d\n", Collider_is_trigger(collider));

    printf("ID: %d\n", Collider_get_id(collider));
}

int is_colliding(Collider *collider1, Collider *collider2) {
    if (Vector2_equals(Collider_get_position(collider1), Collider_get_position(collider2))) {
        return 1;
    }
    float yMax1 = Vector2_get_y(Collider_get_position(collider1)) + Vector2_get_y(Collider_get_dimensions(collider1));
    float xMax1 = Vector2_get_x(Collider_get_position(collider1)) + Vector2_get_x(Collider_get_dimensions(collider1));
    float yMin1 = Vector2_get_y(Collider_get_position(collider1)) - Vector2_get_y(Collider_get_dimensions(collider1));
    float xMin1 = Vector2_get_x(Collider_get_position(collider1)) - Vector2_get_x(Collider_get_dimensions(collider1));

    float yMax2 = Vector2_get_y(Collider_get_position(collider2)) + Vector2_get_y(Collider_get_dimensions(collider2));
    float xMax2 = Vector2_get_x(Collider_get_position(collider2)) + Vector2_get_x(Collider_get_dimensions(collider2));
    float yMin2 = Vector2_get_y(Collider_get_position(collider2)) - Vector2_get_y(Collider_get_dimensions(collider2));
    float xMin2 = Vector2_get_x(Collider_get_position(collider2)) - Vector2_get_x(Collider_get_dimensions(collider2));

    if (
        (((yMin2 <= yMin1 && yMin1 <= yMax2) || (yMin2 <= yMax1 && yMax1 <= yMax2)) && 
            ((xMin2 <= xMin1 && xMin1 <= xMax2) || (xMin2 <= xMax1 && xMax1 <= xMax2))) ||
        (((yMin1 <= yMin2 && yMin2 <= yMax1) || (yMin1 <= yMax2 && yMax2 <= yMax1)) && 
            ((xMin1 <= xMin2 && xMin2 <= xMax1) || (xMin1 <= xMax2 && xMax2 <= xMax1)))) {
        return 1;
    }
    return 0;

    /*
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
    */
}

void move_and_collide(Collider *collider, Vector2 *velocity) {
    if (Collider_is_trigger(collider)) {
        Collider_set_position(collider, Vector2_addition(Collider_get_position(collider), velocity));
        return;
    }
    if (magnitude(velocity) == 0) {
        return;
    }
    int numberOfMoves = magnitude(velocity) / VELOCITYFRAMESIZE;
    Vector2 *velocityFrame = create_Vector2(Vector2_get_x(velocity) / magnitude(velocity) * VELOCITYFRAMESIZE, Vector2_get_y(velocity) / magnitude(velocity) * VELOCITYFRAMESIZE);
    for (int i = 0; i < numberOfMoves; i++) {
        Collider_set_position(collider, Vector2_addition(Collider_get_position(collider), velocityFrame));
        for (int i = 0; i < MAXCOLLIDERCOUNT; i++) {
            if (allColliders[i] == NULL) break;
            if (allColliders[i] == collider) continue;
            if (Collider_is_trigger(allColliders[i])) continue;
            if (is_colliding(collider, allColliders[i])) {
                destroy_Vector2(velocityFrame);
                return;
            }
        }
    }
    destroy_Vector2(velocityFrame);
    /*Vector2 *shortestMove = create_Vector2(Vector2_get_x(velocity), Vector2_get_y(velocity));
    Vector2 *origin = create_Vector2(Vector2_get_x(Collider_get_position(collider)), Vector2_get_y(Collider_get_position(collider)));
    for (int i = 0; i < activeColliderCount; i++) {
        if (allColliders[i] == collider) continue;
        if (Collider_is_trigger(allColliders[i])) continue;
        Collider *secondaryCollider = allColliders[i];
        /*
            Closest point (x,y) on the line collider.position->collider.position + velocity to currentCollider.position:
            x = (b(bx0 - ay0) - ac) / (a^2 + b^2)
            y = (a(-bx0 + ay0) - bc) / (a^2 + b^2)

            a = velocity.y / velocity.x
            b = 1
            c = velocity.y - a * velocity.x
        /
        float a = Vector2_get_y(velocity) / Vector2_get_x(velocity);
        float c = Vector2_get_y(velocity) - a * Vector2_get_x(velocity);
        Vector2 *secondaryColliderNormalisedPosition = Vector2_subtraction(Collider_get_position(secondaryCollider), origin);
        float x0 = Vector2_get_x(secondaryColliderNormalisedPosition);
        float y0 = Vector2_get_y(secondaryColliderNormalisedPosition);
        Vector2 *closestPosition = create_Vector2(((x0 - a*y0) - a*c) / (a*a), (a*(-x0 + a*y0) - c) / (a*a));
        Collider_set_position(collider, closestPosition);
        if (is_colliding(collider, secondaryCollider)) {
            if (magnitude(shortestMove) > magnitude(closestPosition)) {
                float xDiff = (Vector2_get_x(Collider_get_dimensions(collider)) - Vector2_get_x(Collider_get_dimensions(secondaryCollider))) - (Vector2_get_x(secondaryColliderNormalisedPosition) - Vector2_get_x(closestPosition));
                float yDiff = (Vector2_get_y(Collider_get_dimensions(collider)) - Vector2_get_y(Collider_get_dimensions(secondaryCollider))) - (Vector2_get_y(secondaryColliderNormalisedPosition) - Vector2_get_y(closestPosition));
                Vector2_set_x(shortestMove, Vector2_get_x(closestPosition) - xDiff);
                Vector2_set_y(shortestMove, Vector2_get_y(closestPosition) - yDiff);
            }
        }
        Collider_set_position(collider, origin);
        destroy_Vector2(secondaryColliderNormalisedPosition);
    }
    Vector2 *newPosition = Vector2_addition(origin, shortestMove);
    Collider_set_position(collider, newPosition);
    free(shortestMove);*/
}