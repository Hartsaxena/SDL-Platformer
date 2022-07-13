#ifndef FRONT_CONFIG_H
#define FRONT_CONFIG_H

#include <stdbool.h>
#include <SDL2\\SDL.h>

#define front_SCREENX 1000
#define front_SCREENY 1000

#define front_FPS 60
#define front_FRAMEPAUSEDELAY (1000 / front_FPS)

#define front_WINDOWTITLE "Platformer"
extern SDL_Window* front_Window;
extern SDL_Renderer* front_Renderer;
extern SDL_Rect front_WinRect;

void front_Quit();
int front_Init();
bool front_HandleInputs(SDL_Event* InputEvent, bool InputKeys[286]);


#endif