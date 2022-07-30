#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include <stdbool.h>

#include "front.h"
#include "obj.h"
#include "player.h"
#include "weather.h"

#define EDITOR_OBJ_DISTANCE_SNAP 15

#define EDITOR_SELECTION_ID_NONE 0
#define EDITOR_SELECTION_ID_TOPLEFT 1
#define EDITOR_SELECTION_ID_TOPRIGHT 2
#define EDITOR_SELECTION_ID_BOTTOMLEFT 3
#define EDITOR_SELECTION_ID_BOTTOMRIGHT 4
#define EDITOR_SELECTION_ID_CENTER 5

#define EDITOR_DEFAULT_WBARRIER_WIDTH 100
#define EDITOR_DEFAULT_WBARRIER_HEIGHT 100

#define EDITOR_SAVE_DELAY_MS 5000

typedef struct editor_Selection {
    obj_Barrier* SelectedBarrier;
    obj_Entity* SelectedEntity;
    int SelectedPointID;
} editor_Selection;

void editor_DeleteNEWFolder();
void editor_Update(player_Player* Player, bool InputKeys[286], front_MouseState* MouseState, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead, weather_Weather* WeatherInstance);


#endif