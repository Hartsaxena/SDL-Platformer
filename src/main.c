#include <stdio.h>
#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "front.h"
#include "front_tools.h"
#include "game_config.h"
#include "player.h"
#include "level.h"


int main(int argc, char* argv[]) {
    front_Init(); // Initializing Frontend (windows and stuff)


    struct player_Player Player = player_Init(); // Initialize Player
    printf("Reading map... ");
    struct level_BarrierMap* Barriers = level_ParseMapFile("maps\\alone.map");
    if (Barriers != NULL && Barriers->next != NULL) {
        printf("Successfully created Barrier array of length 2\n");
    }
    printf("Success!\n");
    int BarrierColor[3] = {255, 255, 255};


    // Initializing some variables before the game loop
    SDL_Event InputEvent;
    bool InputKeys[322]; // Contains bools for all SDL keys. true = key is pressed, false = key isn't pressed.
    for(int i = 0; i < 322; i++) { // Init them all to false first
        InputKeys[i] = false;
    }
    int BGColor[3] = {0, 0, 0};
    bool IsRunning = true;


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
        player_Update(&Player, InputKeys, Barriers);

        // Rendering Code Here
        frontTools_ColorFill(front_Renderer, BGColor, SDL_ALPHA_OPAQUE);

        player_DrawPlayer(front_Renderer, &Player);
        for (struct level_BarrierMap* Temp = Barriers; Temp != NULL; Temp = Temp->next) {
            frontTools_DrawRect(front_Renderer, &Temp->Rect, BarrierColor, 255);
        }

        SDL_RenderPresent(front_Renderer);
        SDL_Delay(front_FRAMEPAUSEDELAY);
    }

    front_Quit();

    return 0;
}