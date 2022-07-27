/*
This file contains methods that manage the SDL2 library, which is used as the frontend of the game. This includes window creation, rendering, and event handling.
*/
#include <stdio.h>
#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "front.h"
#include "debug.h"


SDL_Window* front_Window;
SDL_Renderer* front_Renderer;
SDL_Rect front_WinRect = {0, 0, front_SCREENX, front_SCREENY};


void front_Quit()
{
    /*
    This function quits the SDL2 library and the program as a whole.
    */
    if (front_Window != NULL) {
        SDL_DestroyWindow(front_Window);
        SDL_DestroyRenderer(front_Renderer);
    }
    SDL_Quit();
    exit(0);
}


int front_Init()
{
    /*
    This function initializes the SDL2 library. It creates a window and renderer, and returns 0 if successful, or -1 if not.
    */
    if (DEBUG_MODE)
        printf("Initializing SDL... ");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { // Initialize SDL
        printf("FATAL ERROR: %s\n", SDL_GetError());
        return -1;
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
        return -1;
    }
    front_Renderer = SDL_CreateRenderer(front_Window, -1, SDL_RENDERER_ACCELERATED);
    if (DEBUG_MODE)
        printf("Success!\n");

    return 0;
}


void front_InitMouseState(front_MouseState* MouseState)
{
    /*
    This function initializes the MouseState struct.
    */
    for (int i = 0; i < 5; i++)
        MouseState->ButtonStates[i] = false;
    MouseState->x = 0;
    MouseState->y = 0;
}


bool front_HandleInputs(SDL_Event* InputEvent, bool InputKeys[286], front_MouseState* MouseState)
{
    /*
    This function handles all input events. It returns true if the game should continue running, false if it should quit.
    */
    bool IsRunning = true;
    while (SDL_PollEvent(InputEvent) > 0) {
        switch (InputEvent->type) {
            case SDL_KEYDOWN: {
                InputKeys[InputEvent->key.keysym.scancode] = true;
                break;
            }
            case SDL_KEYUP: {
                InputKeys[InputEvent->key.keysym.scancode] = false;
                break;
            }
            
            case SDL_MOUSEBUTTONDOWN: {
                MouseState->ButtonStates[InputEvent->button.button] = true;
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                MouseState->ButtonStates[InputEvent->button.button] = false;
                break;
            }
            case SDL_MOUSEMOTION: {
                MouseState->x = InputEvent->motion.x;
                MouseState->y = InputEvent->motion.y;
                break;
            }

            case SDL_QUIT: {
                IsRunning = false;
                break;
            }
            default: {
                break;
            }
        }
    }
    return IsRunning;
}