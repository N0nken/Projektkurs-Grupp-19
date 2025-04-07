#include "../include/mathex.h"

/* Clamp functions */
int clampi(int i, int min, int max) {
    if (i < min) {
        i = min;
    } else if (i > max) {
        i = max;
    }
    return i;
}
float clampf(float f, float min, float max) {
    if (f < min) {
        f = min;
    } else if (f > max) {
        f = max;
    }
    return f;
}