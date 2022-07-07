#include <stdio.h>
#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "front.h"
#include "game_config.h"
#include "obj.h"
#include "parse.h"
#include "player.h"
#include "render.h"
#include "update.h"


int main(int argc, char* argv[])
{
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
    player_Player Player = player_Init(); // Initialize Player
    printf("Parsing map data... \n");
    parse_ParseResult ParseResult = parse_ParseMapFolder("maps\\test");
    obj_Barrier* BarriersHead = ParseResult.BarriersParseResult;
    obj_Entity* EntitiesHead = ParseResult.EntitiesParseResult;
    printf("Succesfully parsed map data!\n");


    printf("\n"); // Separate game errors from initializing errors.
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
        update_UpdateBarriers(BarriersHead);
        player_UpdatePlayer(&Player, InputKeys, BarriersHead, EntitiesHead);
        update_UpdateEntities(EntitiesHead, BarriersHead);

        // Rendering Code Here
        render_FillScreenColor(front_Renderer, BGColor, SDL_ALPHA_OPAQUE);

        render_RenderPlayer(front_Renderer, &Player);
        render_RenderBarriers(front_Renderer, BarriersHead);
        render_RenderEntities(front_Renderer, EntitiesHead);

        SDL_RenderPresent(front_Renderer);
        SDL_Delay(front_FRAMEPAUSEDELAY);
    }

    front_Quit();
    return 0;
}