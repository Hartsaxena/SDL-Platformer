/*
Driver file of the code. This is where everything is put together into an actual game.
*/


#include <stdio.h>
#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "debug.h"
#include "front.h"
#include "game_config.h"
#include "obj.h"
#include "parse.h"
#include "player.h"
#include "render.h"
#include "update.h"


int main(int argc, char* argv[])
{
    printf("Loading...\n");
    if (front_Init() != 0) {
        printf("Failed to initialize frontend.\n");
        front_Quit();
    }

    // Initializing Frontend Variables
    SDL_Event InputEvent;
    bool InputKeys[286]; // Contains bools for all SDL scancodes. true = key is pressed, false = key isn't pressed.
    for (int i = 0; i < 286; i++) { // Init them all to false first
        InputKeys[i] = false;
    }
    int BGColor[3] = {0, 0, 0};
    bool IsRunning = true;


    // Initializing Game Variables
    if (DEBUG_MODE) {
        printf("Debug Mode Enabled.\n");
        printf("Initializing Game Variables...\n");
    }
    if (DEBUG_MODE)
        printf("Initializing Player... ");
    player_Player Player = player_Init(); // Initialize Player
    if (DEBUG_MODE)
        printf("Success!\n");

    if (DEBUG_MODE)
        printf("Parsing map data... \n");
    obj_Map* Map = parse_ParseMapFolder("maps\\test");
    if (DEBUG_MODE)
        printf("Succesfully parsed map data!\n");


    printf("Finished Loading!\n\n"); // Separate game errors from initializing errors.
    // Actual Game Loop
    while (IsRunning) {

        // Handle Input Events
        IsRunning = front_HandleInputs(&InputEvent, InputKeys);

        // Update code here
        update_UpdateBarriers(Map->BarriersHead);
        player_UpdatePlayer(&Player, InputKeys, Map->BarriersHead, Map->EntitiesHead);
        update_UpdateEntities(&Map->EntitiesHead, Map->BarriersHead);

        // Rendering Code Here
        // Offset the camera to the player's position.
        SDL_RenderClear(front_Renderer);
        render_UpdateCamera(&Player);
        render_FillScreenColor(front_Renderer, BGColor, SDL_ALPHA_OPAQUE);

        render_RenderPlayer(front_Renderer, &Player);
        render_RenderBarriers(front_Renderer, Map->BarriersHead);
        render_RenderEntities(front_Renderer, Map->EntitiesHead);

        SDL_RenderPresent(front_Renderer);
        SDL_Delay(front_FRAMEPAUSEDELAY);
    }

    front_Quit();
    return 0;
}