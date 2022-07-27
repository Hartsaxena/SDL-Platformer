#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include <stdbool.h>

#include "front.h"
#include "obj.h"
#include "player.h"
#include "weather.h"

void editor_Update(player_Player* Player, bool InputKeys[286], front_MouseState* MouseState, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead, weather_Weather* WeatherInstance);


#endif