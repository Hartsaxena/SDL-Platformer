#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "calc.h"

// Note: Some functions are defined as macros in calc.h

bool calc_IsPointInsideRect(int x, int y, SDL_Rect Rect)
{
    return (x >= Rect.x && x <= Rect.x + Rect.w && y >= Rect.y && y <= Rect.y + Rect.h);
}