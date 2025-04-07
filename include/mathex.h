#ifndef MATHEX_H_   /* Include guard */
#define MATHEX_H_

/* Forces x into the range min-max (including both end points) */
int clampi(int i, int min, int max);
/* Forces x into the range min-max (including both end points) */
float clampf(float f, float min, float max);

#endif