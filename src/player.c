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


#include <stdio.h>
#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "calc.h"
#include "front.h"
#include "game_config.h"
#include "parse.h"
#include "player.h"
#include "update.h"
#include "debug.h"


player_Player player_Init()
{
    /*
    This function creates a new player_Player object, and returns it. This is called when the game starts and when the player respawns (at the time of writing this. I plan to add a respawn function later).
    */
    SDL_Rect NewPlayerRect = {
        PLAYER_SPAWN_X, PLAYER_SPAWN_Y,
        PLAYER_HEIGHT, PLAYER_WIDTH
    };
    player_Player NewPlayer = (player_Player) {
        .Hitbox = NewPlayerRect,
        .vx = 0, .vy = 0,
        .ax = 0, .ay = 0,
        .Direction = true,
        .State = PLAYER_STATE_IDLE,
        .Alive = true,
        .Render = true,
        .BulletsHead = NULL,
        .BulletDelay = PLAYER_BULLET_DELAY
    };

    return NewPlayer;
}


bool player_PlayerCheckWindowCollision(SDL_Rect Hitbox)
{
    // Only disallow hitbox to traverse pass the left and right edges of the window: If the player falls, that's a different story.
    // return (Hitbox.x < 0);
    return false;
}


int player_PlayerCheckCollision(obj_Barrier* BarriersHead, SDL_Rect Hitbox)
{
    /*
    This function checks if the player's hitbox collides with any of the barriers in the map.
    */
    if (player_PlayerCheckWindowCollision(Hitbox)) {
        return PLAYER_WINDOW_COLLISION_ID;
    }

    for (obj_Barrier* BarrierPtr = BarriersHead; BarrierPtr != NULL; BarrierPtr = BarrierPtr->next) {
        if (BarrierPtr->Type == OBJ_BARRIER_TYPE_PLATFORM) {
            // For platforms, we need to check for collision with only the top of the platform.
            SDL_Rect PlatformTopRect = {
                BarrierPtr->Rect.x, BarrierPtr->Rect.y,
                BarrierPtr->Rect.w, 1 // Make the height of the Platform Rect to 1px.
            };
            if (SDL_HasIntersection(&Hitbox, &PlatformTopRect)) {
                return BarrierPtr->Type;
            }
        } else if (SDL_HasIntersection(&BarrierPtr->Rect, &Hitbox)) {
            return BarrierPtr->Type;
        }
    }

    return -1;
}


bool player_PlayerCheckEnemyEntityCollision(obj_Entity* EntitiesHead, SDL_Rect Hitbox)
{
    /*
    This function checks if the player's hitbox collides with any of the enemies.
    */
    for (obj_Entity* EntityPtr = EntitiesHead; EntityPtr != NULL; EntityPtr = EntityPtr->next) {
        if (EntityPtr->IsEnemy && EntityPtr->Alive && SDL_HasIntersection(&EntityPtr->Hitbox, &Hitbox)) {
            return true;
        }
    }
    return false;
}


bool player_PlayerCheckFell(player_Player* Player)
{
    /*
    Checks if the player has fallen below the map.
    Note that this function doesn't check if the player has touched the bottom of the map, but rather if the player has fallen below the entire map. This is intentional.
    */
    return (Player->Hitbox.y > front_SCREENY);
}


void player_CreateNewBullet(player_Player* Player)
{
    /*
    This function creates a new player bullet and adds it to the player's list of bullets to update.
    */
    player_Bullet* NewBullet = malloc(sizeof(player_Bullet));
    if (NewBullet == NULL) {
        printf("Error: Could not allocate memory for bullet.\n");
        front_Quit();
    }

    NewBullet->Hitbox = (SDL_Rect) {
        .x = Player->Hitbox.x,
        .y = Player->Hitbox.y,
        .w = PLAYER_BULLET_LENGTH,
        .h = PLAYER_BULLET_LENGTH,
    };
    NewBullet->Speed = 14; // the speed member should be changed in the bullet update function, but isn't yet.
    NewBullet->Direction = Player->Direction;
    NewBullet->Active = true;

    NewBullet->next = Player->BulletsHead;
    Player->BulletsHead = NewBullet;
}


void player_DoInputs(player_Player* Player, bool InputKeys[286])
{
    /*
    This function handles the player's inputs. InputKeys is an array of booleans, where each index corresponds to a key on the keyboard.
    */
    Player->vx = 0; // Reset X-axis velocity.
    bool Up = InputKeys[SDL_SCANCODE_W];
    bool Left = InputKeys[SDL_SCANCODE_A];
    bool Down = InputKeys[SDL_SCANCODE_S];
    bool Right = InputKeys[SDL_SCANCODE_D];
    bool Space = InputKeys[SDL_SCANCODE_SPACE];
    bool K = InputKeys[SDL_SCANCODE_K];
    // bool LShift = InputKeys[SDL_SCANCODE_LSHIFT]; // Not implemented yet.
    if (Up || Space) {
        bool IsIdle = (Player->State == PLAYER_STATE_IDLE || Player->State == PLAYER_STATE_WALK);
        bool InAir = (Player->vy != 0);
        if (IsIdle && !InAir) {
            Player->State = PLAYER_STATE_JUMP;
        }
    }
    if (Down) {
        // Ducking is not implemented yet.
    }
    if (Left) {
        Player->vx -= PLAYER_BASE_VX;
        Player->Direction = false;
    }
    if (Right) {
        Player->vx += PLAYER_BASE_VX;
        Player->Direction = true;
    }
    if (K && Player->BulletDelay <= 0) {
        player_CreateNewBullet(Player);
        Player->BulletDelay = PLAYER_BULLET_DELAY;
    } else {
        Player->BulletDelay--;
    }
}


void player_DoPhysics(player_Player* Player, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead)
{
    /*
    This function renders the player's physics interactions (collision, gravity, etc.).
    Player state changes are also handled here (for example, if the function detects that the player has fallen, it will set the player's Alive flag to false).
    */
    SDL_Rect NewHitbox = {
        Player->Hitbox.x,
        Player->Hitbox.y,
        Player->Hitbox.h,
        Player->Hitbox.w
    };

    if (PLAYER_STATE_JUMP == Player->State) {
        Player->vy -= PLAYER_JUMP_START_BOOST + PLAYER_JUMP_STEP;
        Player->State = PLAYER_STATE_IDLE;
    }

    Player->vy += CONFIG_GRAVITY_AY; // Oh look here's gravity.
    // Detect Collision for Y axis
    Player->vy = MIN(Player->vy, PLAYER_MAX_VY); // Clamp velocity to max.
    NewHitbox.y += Player->vy;
    switch (player_PlayerCheckCollision(BarriersHead, NewHitbox)) {
        case PLAYER_WINDOW_COLLISION_ID:
        case OBJ_BARRIER_TYPE_WALL: {
            Player->vy = 0;
            NewHitbox.y = Player->Hitbox.y;
            break;
        }
        case OBJ_BARRIER_TYPE_VOID: {
            Player->Alive = false;
            if (DEBUG_MODE)
                printf("Y CHECK: Player touched void barrier.\n");
            break;
        }
        case OBJ_BARRIER_TYPE_PLATFORM: {
            if (Player->vy > 0) {
                Player->vy = 0;
                NewHitbox.y = Player->Hitbox.y;
            }
            break;
        }
    }

    // ... X axis
    NewHitbox.x += Player->vx;
    switch (player_PlayerCheckCollision(BarriersHead, NewHitbox)) {
        case PLAYER_WINDOW_COLLISION_ID:
        case OBJ_BARRIER_TYPE_WALL: {
            Player->vx = 0;
            NewHitbox.x = Player->Hitbox.x;
            break;
        }
        case OBJ_BARRIER_TYPE_VOID: {
            Player->Alive = false;
            if (DEBUG_MODE)
                printf("X CHECK: Player touched void barrier.\n");
            break;
        }
    }

    Player->Hitbox = NewHitbox;
    // Check if the player has fallen
    if (player_PlayerCheckFell(Player)) {
        if (DEBUG_MODE)
            printf("Player fell!\n");
        Player->Alive = false;
    } else if (player_PlayerCheckEnemyEntityCollision(EntitiesHead, Player->Hitbox)) { // Check if the player has touched another enemy entity
        Player->Alive = false;
        if (DEBUG_MODE) {
            printf("Player has touched an enemy entity.\n");
        }
    }
}


int player_BulletCheckCollision(player_Bullet* Bullet, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead)
{
    /*
    This function checks if the bullet has collided with any of the barriers or entities.
    */

    // Check if the bullet has collided with any of the barriers.
    int CollidedBarrierID = update_RectCheckBarrierCollision(BarriersHead, Bullet->Hitbox);
    if (CollidedBarrierID != OBJ_NONE) {
        if (DEBUG_MODE)
            printf("Bullet collided with Barrier Type: %d.\n", CollidedBarrierID);
        return CollidedBarrierID;
    }

    // Check if the bullet has collided with any of the entities.
    for (obj_Entity* Entity = EntitiesHead; Entity != NULL; Entity = Entity->next) {
        if (SDL_HasIntersection(&Bullet->Hitbox, &Entity->Hitbox)) {
            if (DEBUG_MODE)
                printf("Player Bullet has collided with entity and has been deactivated.\n");
            Entity->Alive = false;
            Bullet->Active = false;
            return Entity->Type;
        }
    }

    return OBJ_NONE;
}


void player_UpdateBullets(player_Player* Player, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead)
{
    /*
    This function updates all the player's bullets.
    if a bullet has collided with a barrier or an entity, it will be deactivated and removed from the Player's bullet list.
    */
   player_Bullet* PrevBulletPtr = NULL;
   player_Bullet* BulletPtr;
    for (BulletPtr = Player->BulletsHead; BulletPtr != NULL; BulletPtr = BulletPtr->next) {
        if (BulletPtr->Direction) {
            BulletPtr->Hitbox.x += BulletPtr->Speed;
        } else {
            BulletPtr->Hitbox.x -= BulletPtr->Speed;
        }

        switch (player_BulletCheckCollision(BulletPtr, BarriersHead, EntitiesHead)) {
            // Note: only barrier collisions are handled in this switch statement. The entity collisions are handled in the player_BulletCheckCollision function.
            // Although all types of barriers do the same thing in this switch statement at the time of writing this, it is possible that in the future, different barriers will have different effects.
            case OBJ_BARRIER_TYPE_WALL:
            case OBJ_BARRIER_TYPE_VOID:
            case OBJ_BARRIER_TYPE_PLATFORM: {
                BulletPtr->Active = false;
                break;
            }
        }
        if (!BulletPtr->Active) {
            if (PrevBulletPtr == NULL)
                Player->BulletsHead = BulletPtr->next;
            else
                PrevBulletPtr->next = BulletPtr->next;
        }
        PrevBulletPtr = BulletPtr;
    }
}


void player_UpdatePlayer(player_Player* Player, bool InputKeys[286], obj_Barrier* BarriersHead, obj_Entity* EntitiesHead)
{
    if (!Player->Alive) {
        // Player->render = false;

        // For now, just instantly respawn the player when they die
        *Player = player_Init();
        if (DEBUG_MODE)
            printf("Player respawned.\n");
    }

    // Update functions. Note that the order of these functions is important.
    player_DoInputs(Player, InputKeys);
    player_DoPhysics(Player, BarriersHead, EntitiesHead);
    player_UpdateBullets(Player, BarriersHead, EntitiesHead);
}