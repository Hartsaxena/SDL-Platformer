#include <stdio.h>
#include <SDL2\\SDL.h>

#include "front.h"


void frontTools_ColorFill(SDL_Renderer* Renderer, int Color[3], int Alpha)
{
    SDL_SetRenderDrawColor(Renderer, Color[0], Color[1], Color[2], Alpha);
    SDL_RenderClear(Renderer);
}


void frontTools_DrawRect(SDL_Renderer* Renderer, SDL_Rect* Rect, int Color[3], int Alpha)
{
    SDL_RenderDrawRect(Renderer, Rect);
    SDL_SetRenderDrawColor(Renderer, Color[0], Color[1], Color[2], Alpha);
    SDL_RenderFillRect(Renderer, Rect);
}