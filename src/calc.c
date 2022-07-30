#include <stdbool.h>
#include <SDL2\\SDL.h>
#include <math.h>

#include "calc.h"

// Note: Some functions are defined as macros in calc.h

bool calc_IsPointInsideRect(int x, int y, SDL_Rect Rect)
{
    return (x >= Rect.x && x <= Rect.x + Rect.w && y >= Rect.y && y <= Rect.y + Rect.h);
}


double calc_Distance(int x1, int y1, int x2, int y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}