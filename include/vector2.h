#ifndef ENGINE_H_   /* Include guard */
#define ENGINE_H_

typedef struct Vector2 Vector2;

Vector2* create_Vector2(float x, float y);

int kill_Vector2(Vector2 *v);

void print_Vector2(Vector2 *v);

/* Set x and y values for a Vector2 */
void Vector2_set(Vector2 *v, float x, float y);

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

#endif // FOO_H_