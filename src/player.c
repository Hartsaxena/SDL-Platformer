/*
This is where all the methods relating to the Player the User plays as.
Exceptions:
    Rendering (still in render.c)

The player.c file is very special: It is not organized the same as other objects (Platforms, Barriers, Entities, etc.)
The player_Player structure is defined in the player.h header file, as opposed to other objects, whose
structures are defined in the obj.h header file.

All frame-update related methods are also defined in this file, as opposed to other objects, which have their update methods defined in update.c
Because of this independence from the update.c file, This file should be completely separate from update.c, and should use no methods from it. It should only ever use constant variable macros defined in the update.h header file.

Basically, everything related to the Player (Exceptions listed above) is written here.
*/


#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "calc.h"
#include "front.h"
#include "game_config.h"
#include "parse.h"
#include "player.h"
#include "update.h"


struct player_Player player_Init()
{
    SDL_Rect NewPlayerRect = {
        PLAYER_SPAWN_X, PLAYER_SPAWN_Y,
        PLAYER_HEIGHT, PLAYER_WIDTH
    };
    struct player_Player NewPlayer = (struct player_Player) {
        .Hitbox = NewPlayerRect,
        .vx = 0, .vy = 0,
        .ax = 0, .ay = 0,
        .state = PLAYER_STATE_IDLE,
        .alive = true,
        .render = true,
    };

    return NewPlayer;
}


bool player_PlayerCheckWindowCollision(SDL_Rect Hitbox)
{
    // Only disallow hitbox to traverse pass the left and right edges of the window: If the player falls, that's a different story.
    if (Hitbox.x < 0) {
        return true;
    }
    if (Hitbox.x + Hitbox.w > front_SCREENX) {
        return true;
    }

    return false;
}


bool player_PlayerCheckFell(player_Player* Player)
{
    return (Player->Hitbox.y + Player->Hitbox.h > front_SCREENY);
}


bool player_PlayerCheckCollision(obj_Barrier* BarriersHead, SDL_Rect Hitbox)
{
    if (player_PlayerCheckWindowCollision(Hitbox)) {
        return true;
    }

    for (obj_Barrier* BarrierPtr = BarriersHead; BarrierPtr != NULL; BarrierPtr = BarrierPtr->next) {
        if (SDL_HasIntersection(&BarrierPtr->Rect, &Hitbox)) {
            return true;
        }
    }

    return false;
}


bool player_PlayerCheckEnemyEntityCollision(obj_Entity* EntitiesHead, SDL_Rect Hitbox)
{
    for (obj_Entity* EntityPtr = EntitiesHead; EntityPtr != NULL; EntityPtr = EntityPtr->next) {
        if (EntityPtr->IsEnemy && SDL_HasIntersection(&EntityPtr->Hitbox, &Hitbox)) {
            return true;
        }
    }
    return false;
}


void player_DoInputs(struct player_Player* Player, bool InputKeys[322])
{
    Player->vx = 0;
    bool Up = InputKeys[SDLK_w];
    bool Left = InputKeys[SDLK_a];
    bool Down = InputKeys[SDLK_s];
    bool Right = InputKeys[SDLK_d];
    bool Space = InputKeys[SDLK_SPACE];
    if (Up || Space) {
        bool IsIdle = (Player->state == PLAYER_STATE_IDLE || Player->state == PLAYER_STATE_WALK);
        bool InAir = (Player->vy != 0);
        if (IsIdle && !InAir) {
            Player->state = PLAYER_STATE_JUMP_START;
        }
    }
    if (Down) {
        if (Player->state == PLAYER_STATE_JUMP_ACCELERATE) {
            Player->state = PLAYER_STATE_ACCEL_FALL;
            Player->vy += PLAYER_ACCEL_FALL_STEP;
        }
    }
    if (Left) {
        if (Player->state != PLAYER_STATE_ACCEL_FALL) {
            Player->vx -= PLAYER_BASE_VX;
        }
    }
    if (Right) {
        if (Player->state != PLAYER_STATE_ACCEL_FALL) {
            Player->vx += PLAYER_BASE_VX;
        }
    }
}


void player_DoPhysics(struct player_Player* Player, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead)
{
    SDL_Rect NewHitbox = {
        Player->Hitbox.x,
        Player->Hitbox.y,
        Player->Hitbox.h,
        Player->Hitbox.w
    };

    if (Player->state == PLAYER_STATE_JUMP_START) {
        Player->vy -= PLAYER_JUMP_START_BOOST;
        Player->state = PLAYER_STATE_JUMP_ACCELERATE;
    }
    if (Player->state == PLAYER_STATE_JUMP_ACCELERATE) {
        Player->state = PLAYER_STATE_IDLE;
        Player->vy -= PLAYER_JUMP_STEP;
    }

    Player->vy += CONFIG_GRAVITY_AY; // Oh look here's gravity.
    // Detect Collision for Y axis
    Player->vy = MIN(Player->vy, CONFIG_MAX_GRAVITY_VY);
    NewHitbox.y += Player->vy;
    if (player_PlayerCheckCollision(BarriersHead, NewHitbox)) {
        Player->vy = 0;
        NewHitbox.y = Player->Hitbox.y;
    }
    // ... X axis
    NewHitbox.x += Player->vx;
    if (player_PlayerCheckCollision(BarriersHead, NewHitbox)) {
        Player->vx = 0;
        NewHitbox.x = Player->Hitbox.x;
    }

    Player->Hitbox = NewHitbox;
    // Check if the player has fallen
    if (player_PlayerCheckFell(Player)) {
        Player->alive = false;
    } else if (player_PlayerCheckEnemyEntityCollision(EntitiesHead, Player->Hitbox)) { // Check if the player has touched another enemy entity
        Player->alive = false;
    }
}


void player_UpdatePlayer(struct player_Player* Player, bool InputKeys[322], obj_Barrier* Barriers, obj_Entity* EntitiesHead)
{
    if (Player->alive) {
        player_DoInputs(Player, InputKeys);
        player_DoPhysics(Player, Barriers, EntitiesHead);
    } else {
        // Player->render = false;

        // For now, just instantly respawn the player when they die
        *Player = player_Init();
    }
}