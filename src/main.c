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
    front_Init(); // Initializing Frontend (windows and stuff)

    // Initializing Frontend Variables
    SDL_Event InputEvent;
    bool InputKeys[322]; // Contains bools for all SDL keys. true = key is pressed, false = key isn't pressed.
    for(int i = 0; i < 322; i++) { // Init them all to false first
        InputKeys[i] = false;
    }
    int BGColor[3] = {0, 0, 0};
    bool IsRunning = true;


    // Initializing Game Variables
    if (DEBUG_MODE) {
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
        while (SDL_PollEvent(&InputEvent) > 0) {
            switch (InputEvent.type) {
                case SDL_KEYDOWN: {
                    InputKeys[InputEvent.key.keysym.sym] = true;
                    break;
                }
                case SDL_KEYUP: {
                    InputKeys[InputEvent.key.keysym.sym] = false;
                    break;
                }

                case SDL_QUIT: {
                    IsRunning = false;
                }
                default: {
                    break;
                }
            }
        }


        // Update code here
        update_UpdateBarriers(Map->BarriersHead);
        player_UpdatePlayer(&Player, InputKeys, Map->BarriersHead, Map->EntitiesHead);
        update_UpdateEntities(Map->EntitiesHead, Map->BarriersHead);

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