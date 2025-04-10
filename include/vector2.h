#ifndef VECTOR2_H_   /* Include guard */
#define VECTOR2_H_

typedef struct Vector2 Vector2;

Vector2* create_Vector2(float x, float y);

/* Deallocates all memory to the struct object including struct fields */
int destroy_Vector2(Vector2 *v);

void print_Vector2(Vector2 *v);

void Vector2_set_x(Vector2 *v, float x);
void Vector2_set_y(Vector2 *v, float y);
float Vector2_get_x(Vector2 *v);
float Vector2_get_y(Vector2 *v);

/* Add two 2D vectors */
Vector2 *Vector2_add(Vector2 *v1, Vector2 *v2);

/* The distance between two Vector2 treated as points */
float distance_to(Vector2 *pos1, Vector2 *pos2);

/* Returns the length/magnitude of a Vector2 */
float magnitude(Vector2 *v);

/* Dot product of two Vector2 */
float dot_product(Vector2 *v1, Vector2 *v2);

/* Angle between two Vector2 */
float angle_to(Vector2 *v1, Vector2 *v2);

/* Angle between a Vector2 and X-Axis. Functions the same as angle_to(v1, Vector2(1,0))*/
float angle_of(Vector2 *v);

/* Normalize vector */
void normalize(Vector2 *v);

#endif