#ifndef BARRIER_H
#define BARRIER_H

#include <stdbool.h>
#include <SDL2\\SDL.h>

#define PI 3.14159265358979323846

#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)
#define DEG_TO_RAD(x) ((x) * (PI / 180))


bool calc_IsPointInsideRect(int x, int y, SDL_Rect Rect);
double calc_Distance(int x1, int y1, int x2, int y2);


#endif