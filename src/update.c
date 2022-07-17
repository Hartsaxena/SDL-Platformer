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
#include "debug.h"


int update_RectCheckBarrierCollision(obj_Barrier* BarriersHead, SDL_Rect Rect)
{
    /*
    This function checks if a given rectangle collides with any barriers in the game.
    */
    for (obj_Barrier* BarrierPtr = BarriersHead; BarrierPtr != NULL; BarrierPtr = BarrierPtr->next) {
        SDL_Rect BarrierRect = BarrierPtr->Rect;
        if (SDL_HasIntersection(&Rect, &BarrierRect)) {
            return BarrierPtr->Type;
        }
    }

    return OBJ_NONE;
}


int update_RectCheckEntityCollision(obj_Entity* EntitiesHead, SDL_Rect Rect)
{
    /*
    This function checks if a given rectangle collides with any entities in the game.
    */
    for (obj_Entity* EntityPtr = EntitiesHead; EntityPtr != NULL; EntityPtr = EntityPtr->next) {
        SDL_Rect EntityRect = EntityPtr->Hitbox;
        if (SDL_HasIntersection(&Rect, &EntityRect)) {
            return EntityPtr->Type;
        }
    }

    return OBJ_NONE;
}


bool update_ConfineRect(SDL_Rect Rect, SDL_Rect ConfineRect)
{
    /*
    This function confines a rectangle inside a given rectangle.
    */
    bool Confined = false;
    if (Rect.x < ConfineRect.x) {
        Rect.x = ConfineRect.x;
        Confined = true;
    }
    if (Rect.y < ConfineRect.y) {
        Rect.y = ConfineRect.y;
        Confined = true;
    }
    if (Rect.x + Rect.w > ConfineRect.x + ConfineRect.w) {
        Rect.x = ConfineRect.x + ConfineRect.w - Rect.w;
        Confined = true;
    }
    if (Rect.y + Rect.h > ConfineRect.y + ConfineRect.h) {
        Rect.y = ConfineRect.y + ConfineRect.h - Rect.h;
        Confined = true;
    }
    return Confined;
}


bool update_ConfineEntity(obj_Entity* EntityPtr)
{
    /*
    This function confines an entity inside their own domain.
    */
    bool Confined = false;
    if (EntityPtr->Hitbox.x < EntityPtr->Domain_left && EntityPtr->Domain_left != -1) {
        EntityPtr->Hitbox.x = EntityPtr->Domain_left;
        Confined = true;
    } else if (EntityPtr->Hitbox.x + EntityPtr->Hitbox.w > EntityPtr->Domain_right && EntityPtr->Domain_right != -1) {
        EntityPtr->Hitbox.x = EntityPtr->Domain_right - EntityPtr->Hitbox.w;
        Confined = true;
    }
    if (EntityPtr->Hitbox.y < EntityPtr->Domain_top && EntityPtr->Domain_top != -1) {
        EntityPtr->Hitbox.y = EntityPtr->Domain_top;
        Confined = true;
    } else if (EntityPtr->Hitbox.y + EntityPtr->Hitbox.h > EntityPtr->Domain_bottom && EntityPtr->Domain_bottom != -1) {
        EntityPtr->Hitbox.y = EntityPtr->Domain_bottom - EntityPtr->Hitbox.h;
        Confined = true;
    }
    return Confined;
}


int update_EntityCheckCollision(obj_Barrier* BarriersHead, SDL_Rect EntityRect)
{
    /*
    This function checks if a given Entity collides with any barriers in the game.
    */
    int CollisionType = update_RectCheckBarrierCollision(BarriersHead, EntityRect);
    return CollisionType;
}


void update_UpdateBarriers(obj_Barrier* BarriersHead)
{
    return; // Do nothing. This function is only here to make the code structure more consistent.
}


void update_UpdateEntity(obj_Entity* EntityPtr, obj_Barrier* BarriersHead)
{
    /*
    This function runs every frame and update the position of an entity. It also checks if the entity collides with any barriers.
    Note that it does not check whether it has collided with a player. Player-Entity collision are handled in player_UpdatePlayer().
    */
    if (!EntityPtr->Alive) {
        return;
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
            if (DEBUG_MODE)
                printf("Entity State %d is not implemented.\n", EntityPtr->State);
            abort();
        }
    }

    EntityPtr->vy += CONFIG_GRAVITY_AY;
    // Detect Collision for Y axis
    EntityPtr->vy = MIN(EntityPtr->vy, CONFIG_MAX_GRAVITY_VY);
    NewHitbox.y += EntityPtr->vy;
    switch (update_EntityCheckCollision(BarriersHead, NewHitbox)) {
        case OBJ_BARRIER_TYPE_VOID: { // The fact that the OBJ_BARRIER_TYPE_VOID case is first is important.
            EntityPtr->Alive = false;
            break;
        }
        case OBJ_BARRIER_TYPE_WALL: {
            EntityPtr->vy = 0;
            NewHitbox.y = EntityPtr->Hitbox.y;
            break;
        }
        case OBJ_BARRIER_TYPE_PLATFORM: {
            // Detect if the entity fell on the platform or came through below it
            if (EntityPtr->vy > 0) {
                EntityPtr->vy = 0;
                NewHitbox.y = EntityPtr->Hitbox.y;
            }
            break;
        }
    }
    // ... X axis
    if (EntityPtr->Direction) {
        EntityPtr->vx = EntityPtr->Speed;
    } else {
        EntityPtr->vx = -EntityPtr->Speed;
    }
    NewHitbox.x += EntityPtr->vx;
    switch (update_EntityCheckCollision(BarriersHead, NewHitbox)) {
        case OBJ_BARRIER_TYPE_VOID: {
            EntityPtr->Alive = false;
            break;
        }
        case OBJ_BARRIER_TYPE_WALL: {
            EntityPtr->vx = 0;
            NewHitbox.x = EntityPtr->Hitbox.x;
            break;
        }
    }

    EntityPtr->Hitbox = NewHitbox;
    if (update_ConfineEntity(EntityPtr)) {
        if (DEBUG_MODE)
            printf("Entity with Rect {%d, %d, %d, %d} was confined to its domain {%d - %d}, {%d - %d}.\n", EntityPtr->Hitbox.x, EntityPtr->Hitbox.y, EntityPtr->Hitbox.w, EntityPtr->Hitbox.h, EntityPtr->Domain_left, EntityPtr->Domain_right, EntityPtr->Domain_top, EntityPtr->Domain_bottom);
        EntityPtr->Direction = !EntityPtr->Direction;
    }
}


void update_UpdateEntities(obj_Entity** EntitiesHead, obj_Barrier* BarriersHead)
{
    /*
    This function updates all entities in a given Linked List of obj_Entity's.
    The function deletes entites it finds that are not alive. This runs after updating each entity.
    Note that EntitiesHead is a double pointer. This is because the function may need to delete the first element / head of the list.
    The function returns immediately if there are no entities in the list.
    */
    if (*EntitiesHead == NULL)
        return;
    obj_Entity* PrevEntityPtr = NULL;
    obj_Entity* EntityPtr;
    for (EntityPtr = *EntitiesHead; EntityPtr != NULL; EntityPtr = EntityPtr->next) {
        update_UpdateEntity(EntityPtr, BarriersHead);
        if (!EntityPtr->Alive) {
            if (DEBUG_MODE)
                printf("Found and Deleted Entity with Rect {%d, %d, %d, %d}.\n", EntityPtr->Hitbox.x, EntityPtr->Hitbox.y, EntityPtr->Hitbox.w, EntityPtr->Hitbox.h);
            if (PrevEntityPtr == NULL)
                *EntitiesHead = EntityPtr->next;
            else
                PrevEntityPtr->next = EntityPtr->next;
        }
        PrevEntityPtr = EntityPtr;
    }
}