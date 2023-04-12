/*
This file contains methods that render all objects in the game. Calculations for updating positions of objects, and the like, are done in the update.c file. Game updates and rendering should be done separately.
*/
#include <stdio.h>
#include <SDL2\\SDL.h>

#include "debug.h"
#include "front.h"
#include "game_config.h"
#include "obj.h"
#include "parse.h"
#include "player.h"

static int CameraX = 0, CameraY = 0;
int* render_CameraX = &CameraX;
int* render_CameraY = &CameraY;

void render_FillScreenColor(SDL_Renderer* Renderer, int Color[3], int Alpha)
{
    SDL_SetRenderDrawColor(Renderer, Color[0], Color[1], Color[2], Alpha);
    SDL_RenderClear(Renderer);
}


void render_DrawRect(SDL_Renderer* Renderer, SDL_Rect Rect, int Color[3], int Alpha)
{
    SDL_Rect OffsetRect = {Rect.x + CameraX, Rect.y + CameraY, Rect.w, Rect.h};
    SDL_RenderDrawRect(Renderer, &OffsetRect);
    SDL_SetRenderDrawColor(Renderer, Color[0], Color[1], Color[2], Alpha);
    SDL_RenderFillRect(Renderer, &OffsetRect);
}


void render_RenderPlayer(SDL_Renderer* Renderer, player_Player* Player)
{
    int PlayerHitboxColor[3] = {255, 255, 255}; // Just for debugging. The player should never see the hitbox.
    if (Player->Render) {
        render_DrawRect(Renderer, Player->Hitbox, PlayerHitboxColor, PLAYER_HITBOX_ALPHA);
    }

    for (player_Bullet* BulletPtr = Player->BulletsHead; BulletPtr != NULL; BulletPtr = BulletPtr->next) {
        if (BulletPtr->Active) {
            render_DrawRect(Renderer, BulletPtr->Hitbox, PlayerHitboxColor, PLAYER_HITBOX_ALPHA);
        }
    }
}


void render_DrawBarrier(SDL_Renderer* Renderer, obj_Barrier* Barrier)
{
    int WallBarrierColor[3] = {150, 75, 0}; // Placeholder until texture implementation.
    int VoidBarrierColor[3] = {128, 128, 128};
    int PlatformBarrierColor[3] = {255, 255, 255};
    switch (Barrier->Type) {
        case OBJ_BARRIER_TYPE_WALL: {
            render_DrawRect(Renderer, Barrier->Rect, WallBarrierColor, SDL_ALPHA_OPAQUE);
            break;
        }
        case OBJ_BARRIER_TYPE_VOID: {
            render_DrawRect(Renderer, Barrier->Rect, VoidBarrierColor, SDL_ALPHA_OPAQUE);
            break;
        }
        case OBJ_BARRIER_TYPE_PLATFORM: {
            render_DrawRect(Renderer, Barrier->Rect, PlatformBarrierColor, SDL_ALPHA_OPAQUE);
            break;
        }
        default: {
            if (DEBUG_MODE)
                printf("render_DrawBarrier: Unknown barrier type.\n");
            front_Quit();
        }
    }
}


void render_RenderBarriers(SDL_Renderer* Renderer, obj_Barrier* Barriers)
{
    for (obj_Barrier* BarrierPtr = Barriers; BarrierPtr != NULL; BarrierPtr = BarrierPtr->next) {
        render_DrawBarrier(Renderer, BarrierPtr);
    }
}


void render_DrawEntity(SDL_Renderer* Renderer, obj_Entity* Entity)
{
    int EnemyEntityColor[3] = {255, 0, 0}; // Again, placeholders.
    int FriendlyEntityColor[3] = {0, 255, 0};

    if (Entity->Alive) {
        if (Entity->IsEnemy) {
            render_DrawRect(Renderer, Entity->Hitbox, EnemyEntityColor, SDL_ALPHA_OPAQUE);
        } else {
            render_DrawRect(Renderer, Entity->Hitbox, FriendlyEntityColor, SDL_ALPHA_OPAQUE);
        }
    }
}


void render_RenderEntities(SDL_Renderer* Renderer, obj_Entity* Entities)
{
    for (obj_Entity* EntityPtr = Entities; EntityPtr != NULL; EntityPtr = EntityPtr->next) {
        render_DrawEntity(Renderer, EntityPtr);
    }
}


void render_UpdateCamera(player_Player* Player)
{
    CameraX = (front_SCREENX / 2) - Player->Hitbox.x;
    CameraY = 0; // No horizontal scrolling
}


int* render_GetGlobalMousePosition(int MouseX, int MouseY)
{
    // Find the global position of a point relative to the camera.
    int* GlobalPosition = malloc(sizeof(int) * 2);
    GlobalPosition[0] = MouseX + CameraX;
    GlobalPosition[1] = MouseY + CameraY;
    return GlobalPosition;
}