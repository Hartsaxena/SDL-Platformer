/*
This file contains methods that render all objects in the game. Calculations for updating positions of objects, and the like, are done in the update.c file. Game updates and rendering should be done separately.
*/
#include <stdio.h>
#include <SDL2\\SDL.h>
#include <math.h>
#include <stdlib.h>

#include "debug.h"
#include "front.h"
#include "game_config.h"
#include "obj.h"
#include "calc.h"
#include "parse.h"
#include "player.h"
#include "weather.h"
#include "update.h"
#include "render.h"


static int Camera_x = 0, Camera_y = 0;

void render_FillScreenColor(SDL_Renderer* Renderer, int Color[3], int Alpha)
{
    SDL_SetRenderDrawColor(Renderer, Color[0], Color[1], Color[2], Alpha);
    SDL_RenderClear(Renderer);
}


void render_DrawRect(SDL_Renderer* Renderer, SDL_Rect Rect, int Color[3], int Alpha)
{
    SDL_Rect OffsetRect = {Rect.x + Camera_x, Rect.y + Camera_y, Rect.w, Rect.h};
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
    Camera_x = (front_SCREENX / 2) - Player->Hitbox.x;
    Camera_y = 0; // Not horizontal scrolling
}


render_PointedRect render_GetRectPoints(int centerx, int centery, int width, int height, double Angle)
{
    double Centerpoint[2] = {centerx, centery};
    double DistanceToPoints = sqrt((double)(pow(width, 2) + pow(height, 2))) / 2.0;

    double ReciprocalAngle = calc_NormalizeAngle(PI + Angle);
    double TriangleAngle = atan2(height, width);
    double TopLeftAngle = ReciprocalAngle + TriangleAngle;
    double TopRightAngle = ReciprocalAngle - TriangleAngle;

    double* Changes;
    render_PointedRect Points;

    Changes = calc_AngledPointsDifference(Centerpoint, TopLeftAngle, DistanceToPoints);
    Points.TopLeft[0] = centerx - Changes[0];
    Points.TopLeft[1] = centery - Changes[1];

    Changes = calc_AngledPointsDifference(Centerpoint, PI + TopLeftAngle, DistanceToPoints);
    Points.BottomRight[0] = centerx - Changes[0];
    Points.BottomRight[1] = centery - Changes[1];

    Changes = calc_AngledPointsDifference(Centerpoint, TopRightAngle, DistanceToPoints);
    Points.TopRight[0] = centerx - Changes[0];
    Points.TopRight[1] = centery - Changes[1];

    Changes = calc_AngledPointsDifference(Centerpoint, PI + TopRightAngle, DistanceToPoints);
    Points.BottomLeft[0] = centerx - Changes[0];
    Points.BottomLeft[1] = centery - Changes[1];

    return Points;
}


void render_RenderPointedRect(SDL_Renderer* Renderer, render_PointedRect* Rect, int Color[3])
{
    SDL_SetRenderDrawColor(Renderer, Color[0], Color[1], Color[2], SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(Renderer, Rect->TopLeft[0], Rect->TopLeft[1], Rect->TopRight[0], Rect->TopRight[1]);
    SDL_RenderDrawLine(Renderer, Rect->TopRight[0], Rect->TopRight[1], Rect->BottomRight[0], Rect->BottomRight[1]);
    SDL_RenderDrawLine(Renderer, Rect->BottomRight[0], Rect->BottomRight[1], Rect->BottomLeft[0], Rect->BottomLeft[1]);
    SDL_RenderDrawLine(Renderer, Rect->BottomLeft[0], Rect->BottomLeft[1], Rect->TopLeft[0], Rect->TopLeft[1]);
}


render_WeatherRenderData* render_CreateWeatherRenderData(weather_Weather* WeatherInstance)
{
    render_WeatherRenderData* NewRenderDataHead;
    render_WeatherRenderData* NewRenderDataTemp;
    render_WeatherRenderData* NewRenderDataCurr;

    double RandomX;
    double RandomY;

    bool FirstIter = true;
    for (int i = 0; i < RENDER_MAX_WEATHER_PARTICLES; i++) {
        NewRenderDataCurr = malloc(sizeof(render_WeatherRenderData));
        if (NewRenderDataCurr == NULL) {
            if (DEBUG_MODE)
                printf("FATAL ERROR: Failed to allocate memory for render data.\n");
            front_Quit();
        }

        RandomX = (double)rand() / (double)RAND_MAX * (double)front_SCREENX;
        RandomY = (double)rand() / (double)RAND_MAX * (double)front_SCREENY;
        NewRenderDataCurr->Rect = render_GetRectPoints(RandomX, RandomY, RENDER_WEATHER_PARTICLE_WIDTH, RENDER_WEATHER_PARTICLE_HEIGHT, WeatherInstance->Direction);
        double TopLeftFloat[2] = {(double)NewRenderDataCurr->Rect.TopLeft[0], (double)NewRenderDataCurr->Rect.TopLeft[1]};
        double* Velocities = calc_AngledPointsDifference(TopLeftFloat, WeatherInstance->Direction, WeatherInstance->Strength * 10);
        NewRenderDataCurr->vx = -Velocities[0];
        NewRenderDataCurr->vy = -Velocities[1];
        NewRenderDataCurr->AttachedWeatherInstance = WeatherInstance;

        if (FirstIter) {
            NewRenderDataHead = NewRenderDataTemp = NewRenderDataCurr;
            FirstIter = false;
        } else {
            NewRenderDataTemp->next = NewRenderDataCurr;
            NewRenderDataTemp = NewRenderDataTemp->next;
        }
    }
    NewRenderDataTemp->next = NULL;

    return NewRenderDataHead;
}


void render_RenderWeatherData(SDL_Renderer* Renderer, render_WeatherRenderData* RenderData)
{
    int WeatherDataColor[3] = {128, 128, 128};
    for (render_WeatherRenderData* RenderDataPtr = RenderData; RenderDataPtr != NULL; RenderDataPtr = RenderDataPtr->next) {
        render_RenderPointedRect(Renderer, &RenderDataPtr->Rect, WeatherDataColor);
    }
}


static render_WeatherRenderData* RenderingWeatherData;
void render_RenderWeather(SDL_Renderer* Renderer, weather_Weather* WeatherInstance)
{
    if (RenderingWeatherData == NULL || RenderingWeatherData->AttachedWeatherInstance != WeatherInstance) {
        RenderingWeatherData = render_CreateWeatherRenderData(WeatherInstance);
    }

    update_UpdateWeatherRenderData(RenderingWeatherData);

    render_RenderWeatherData(Renderer, RenderingWeatherData);
}