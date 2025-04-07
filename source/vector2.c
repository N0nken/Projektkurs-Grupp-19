#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/vector2.h"

struct Vector2 {
    float x;
    float y;
}; 

Vector2* create_Vector2(float x, float y) {
    Vector2 *v = malloc(sizeof(struct Vector2));
    v->x = x;
    v->y = y;
    return v;
}

int kill_Vector2(Vector2 *v) {
    free(v);
    return 0;
}

void print_Vector2(Vector2 *v) {
    printf("%.2f %.2f", v->x, v->y);
}

void Vector2_set(Vector2 *v, float x, float y) {
    v->x = x;
    v->y = y;
}

/* The distance between two Vector2 treated as points */
float distance_to(Vector2 *pos1, Vector2 *pos2) {
    return sqrtf(powf(pos1->x - pos2->x, 2.0) + powf(pos1->y - pos2->y, 2.0));
}
/* Returns the length/magnitude of a Vector2 */
float magnitude(Vector2 *v) {
    Vector2 *origo = create_Vector2(0.0, 0.0);
    return distance_to(v, origo);
}
/* Dot product of two Vector2 */
float dot_product(Vector2 *v1, Vector2 *v2) {
    return v1->x * v2->x + v1->y * v2->y;
}
/* Angle between two Vector2 */
float angle_to(Vector2 *v1, Vector2 *v2) {
    float preCos = dot_product(v1, v2)/(magnitude(v1)*magnitude(v2));
    if (preCos < 0.0) preCos *= -1.0;
    return cosf(preCos);
}
/* Angle between a Vector2 and X-Axis. Functions the same as angle_to(v1, Vector2(1,0))*/
float angle_of(Vector2 *v) {
    Vector2 *xAxis = create_Vector2(1.0,0.0);
    return angle_to(v, xAxis);
}
/* Converts a Vector2 into a unit vector with the same direction */
void normalize(Vector2 *v) {
    float mag = magnitude(v);
    Vector2_set(v, v->x / mag, v->y / mag);
}