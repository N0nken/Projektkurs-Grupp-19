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
int destroy_Vector2(Vector2 *v) {
    free(v);
    return 0;
}

void print_Vector2(Vector2 *v) {
    printf("%.2f %.2f", v->x, v->y);
}

/* Setters and Getters */
void Vector2_set_x(Vector2 *v, float x) {
    v->x = x;
}
void Vector2_set_y(Vector2 *v, float y) {
    v->y = y;
}
float Vector2_get_x(Vector2 *v) {
    return v->x;
}
float Vector2_get_y(Vector2 *v) {
    return v->y;
}

/* 2D Vector math */
float distance_to(Vector2 *pos1, Vector2 *pos2) {
    return sqrtf(powf(pos1->x - pos2->x, 2.0) + powf(pos1->y - pos2->y, 2.0));
}
float magnitude(Vector2 *v) {
    Vector2 *origin = create_Vector2(0.0, 0.0);
    return distance_to(v, origin);
}
float dot_product(Vector2 *v1, Vector2 *v2) {
    return v1->x * v2->x + v1->y * v2->y;
}
float angle_to(Vector2 *v1, Vector2 *v2) {
    float preCos = dot_product(v1, v2)/(magnitude(v1)*magnitude(v2));
    if (preCos < 0.0) preCos *= -1.0;
    return cosf(preCos);
}
float angle_of(Vector2 *v) {
    Vector2 *xAxis = create_Vector2(1.0,0.0);
    return angle_to(v, xAxis);
}
void normalize(Vector2 *v) {
    float mag = magnitude(v);
    Vector2_set(v, v->x / mag, v->y / mag);
}