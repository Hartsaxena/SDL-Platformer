#include <stdio.h>
#include <SDL2\\SDL.h>

#include "front.h"
#include "debug.h"


SDL_Window* front_Window;
SDL_Renderer* front_Renderer;
SDL_Rect front_WinRect = {0, 0, front_SCREENX, front_SCREENY};


void front_Quit()
{
    if (front_Window != NULL) {
        SDL_DestroyWindow(front_Window);
        SDL_DestroyRenderer(front_Renderer);
    }
    SDL_Quit();
    exit(0);
}


int front_Init()
{
    if (DEBUG_MODE)
        printf("Initializing SDL... ");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { // Initialize SDL
        printf("FATAL ERROR: %s\n", SDL_GetError());
        front_Quit();
    }
    if (DEBUG_MODE)
        printf("Success!\n");

    if (DEBUG_MODE)
        printf("Creating Display Window... ");
    front_Window = SDL_CreateWindow(front_WINDOWTITLE,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    front_SCREENX, front_SCREENY,
                                    SDL_WINDOW_SHOWN);
    if (front_Window == NULL) {
        printf("FATAL ERROR: Window could not be displayed.\n");
        front_Quit();
    }
    front_Renderer = SDL_CreateRenderer(front_Window, -1, SDL_RENDERER_ACCELERATED);
    if (DEBUG_MODE)
        printf("Success!\n");

    return 0;
}