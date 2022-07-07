/*
All Non-Player Entities, Platforms, Barriers, etc. have `update` functions that run every frame. These functions are defined here.
These functions do not do any rendering.
Those functions are generally found in the `render.c` file at the time of this being written.
*/


#include <SDL2\\SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "calc.h"
#include "game_config.h"
#include "parse.h"
#include "update.h"


bool update_RectCheckCollision(obj_Barrier* BarriersHead, SDL_Rect Hitbox)
{
    for (obj_Barrier* Barrier = BarriersHead; Barrier != NULL; Barrier = Barrier->next) {
        SDL_Rect BarrierRect = Barrier->Rect;
        if (SDL_HasIntersection(&Hitbox, &BarrierRect)) {
            return true;
        }
    }

    return false;
}


bool update_EntityCheckCollision(obj_Barrier* BarriersHead, SDL_Rect Hitbox)
{
    return update_RectCheckCollision(BarriersHead, Hitbox);
}


void update_UpdateBarrier(obj_Barrier* BarrierPtr)
{
    return; // Do Nothing for now. Maybe change when implementing Screen-Scrolling.
}


void update_UpdateBarriers(obj_Barrier* BarriersHead)
{
    for (obj_Barrier* BarrierPtr = BarriersHead; BarrierPtr != NULL; BarrierPtr = BarrierPtr->next) {
        update_UpdateBarrier(BarrierPtr);
    }
}


void update_UpdateEntity(obj_Entity* EntityPtr, obj_Barrier* BarriersHead)
{
    if (!EntityPtr->Alive) {
        return; // Just a placeholder for now.
    }

    SDL_Rect NewHitbox = {
        EntityPtr->Hitbox.x,
        EntityPtr->Hitbox.y,
        EntityPtr->Hitbox.h,
        EntityPtr->Hitbox.w
    };

    switch (EntityPtr->State) {
        case ENTITY_STATE_IDLE: {
            EntityPtr->vx = 0;
            break;
        }
        default: {
            fprintf(stderr, "FATAL ERROR: Entity->State not defined.\n");
            abort();
        }
    }

    EntityPtr->vy += CONFIG_GRAVITY_AY;
    // Detect Collision for Y axis
    EntityPtr->vy = MIN(EntityPtr->vy, CONFIG_MAX_GRAVITY_VY);
    NewHitbox.y += EntityPtr->vy;
    if (update_EntityCheckCollision(BarriersHead, NewHitbox)) {
        EntityPtr->vy = 0;
        NewHitbox.y = EntityPtr->Hitbox.y;
    }
    // ... X axis
    NewHitbox.x += EntityPtr->vx;
    if (update_EntityCheckCollision(BarriersHead, NewHitbox)) {
        EntityPtr->vx = 0;
        NewHitbox.x = EntityPtr->Hitbox.x;
    }

    EntityPtr->Hitbox = NewHitbox;
}


void update_UpdateEntities(obj_Entity* EntitiesHead, obj_Barrier* BarriersHead)
{
    for (obj_Entity* EntityPtr = EntitiesHead; EntityPtr != NULL; EntityPtr = EntityPtr->next) {
        update_UpdateEntity(EntityPtr, BarriersHead);
    }
}