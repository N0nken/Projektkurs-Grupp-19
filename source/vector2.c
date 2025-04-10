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
    printf("%.2f %.2f\n", Vector2_get_x(v), Vector2_get_y(v));
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
Vector2 *Vector2_addition(Vector2 *v1, Vector2 *v2) {
    return create_Vector2(Vector2_get_x(v1) + Vector2_get_x(v2), Vector2_get_y(v1) + Vector2_get_y(v2));
}
Vector2 *Vector2_subtraction(Vector2 *v1, Vector2 *v2) {
    return create_Vector2(Vector2_get_x(v1) - Vector2_get_x(v2), Vector2_get_y(v1) - Vector2_get_y(v2));
}
Vector2 *Vector2_const_multiplication(Vector2 *v, int constant) {
    return create_Vector2(Vector2_get_x(v) * constant, Vector2_get_y(v) * constant);
}
float distance_to(Vector2 *pos1, Vector2 *pos2) {
    return sqrtf(powf(Vector2_get_x(pos1) - Vector2_get_x(pos2), 2.0) + powf(Vector2_get_y(pos1) - Vector2_get_y(pos2), 2.0));
}
float magnitude(Vector2 *v) {
    Vector2 *origin = create_Vector2(0.0, 0.0);
    float distance = distance_to(v, origin);
    free(origin);
    return distance;
}
float dot_product(Vector2 *v1, Vector2 *v2) {
    float dotProduct = Vector2_get_x(v1) * Vector2_get_x(v2) + Vector2_get_y(v1) * Vector2_get_y(v2);
    return dotProduct;
}
float angle_to(Vector2 *v1, Vector2 *v2) {
    if (Vector2_get_x(v1) == Vector2_get_x(v2) || Vector2_get_y(v1) == Vector2_get_y(v2)) {
        return 0;
    }
    float dotProduct = dot_product(v1, v2);
    float magnitude1 = magnitude(v1);
    float magnitude2 = magnitude(v2);
    float preCos = dotProduct/(magnitude1*magnitude2);
    if (preCos < 0.0) preCos *= -1.0;
    return cosf(preCos);
}
float angle_of(Vector2 *v) {
    Vector2 *xAxis = create_Vector2(1.0,0.0);
    float angle = angle_to(v, xAxis);
    free(xAxis);
    return angle;
}
void normalize(Vector2 *v) {
    float mag = magnitude(v);
    if (mag != 0) {
        Vector2_set_x(v, Vector2_get_x(v) / mag);
        Vector2_set_y(v, Vector2_get_y(v) / mag);
    }
}