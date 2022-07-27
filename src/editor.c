#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "calc.h"
#include "debug.h"
#include "front.h"
#include "obj.h"
#include "player.h"
#include "weather.h"


static obj_Barrier* SelectedBarrier = NULL;
static obj_Entity* SelectedEntity = NULL;
void editor_Update(player_Player* Player, bool InputKeys[286], front_MouseState* MouseState, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead, weather_Weather* WeatherInstance)
{
    if (MouseState->ButtonStates[SDL_BUTTON_LEFT]) {
        if (SelectedBarrier == NULL) {
            for (obj_Barrier* BarrierPtr = BarriersHead; BarrierPtr != NULL; BarrierPtr = BarrierPtr->next) {
                if (calc_IsPointInsideRect(MouseState->x, MouseState->y, BarrierPtr->Rect)) {
                    SelectedBarrier = BarrierPtr;
                    break;
                }
            }
        }

        if (SelectedEntity == NULL) {
            for (obj_Entity* EntityPtr = EntitiesHead; EntityPtr != NULL; EntityPtr = EntityPtr->next) {
                if (calc_IsPointInsideRect(MouseState->x, MouseState->y, EntityPtr->Hitbox)) {
                    SelectedEntity = EntityPtr;
                    break;
                }
            }
        }
    } else {
        SelectedBarrier = NULL;
        SelectedEntity = NULL;
    }

    if (SelectedBarrier != NULL) {
        // Move barrier relative to the mouse position
        SelectedBarrier->Rect.x = MouseState->x - SelectedBarrier->Rect.w / 2;
        SelectedBarrier->Rect.y = MouseState->y - SelectedBarrier->Rect.h / 2;
    }
    if (SelectedEntity != NULL) {
        // Move entity relative to the mouse position
        SelectedEntity->Hitbox.x = MouseState->x - SelectedEntity->Hitbox.w / 2;
        SelectedEntity->Hitbox.y = MouseState->y - SelectedEntity->Hitbox.h / 2;
    }
}