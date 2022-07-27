#ifndef RENDER_H
#define RENDER_H

#include <SDL2\\SDL.h>

#include "player.h"
#include "parse.h"

#define RENDER_MAX_WEATHER_PARTICLES 30
#define RENDER_WEATHER_PARTICLE_HEIGHT 10
#define RENDER_WEATHER_PARTICLE_WIDTH 20


typedef struct {
    int TopLeft[2];
    int TopRight[2];
    int BottomLeft[2];
    int BottomRight[2];
} render_PointedRect;

typedef struct WeatherRenderData {
    render_PointedRect StartRect;
    render_PointedRect Rect;
    int vx, vy;
    weather_Weather* AttachedWeatherInstance;
    struct WeatherRenderData* next;
} render_WeatherRenderData;


void render_FillScreenColor(SDL_Renderer* Renderer, int Color[3], int Alpha);
void render_DrawRect(SDL_Renderer* Renderer, SDL_Rect Rect, int Color[3], int Alpha);

void render_RenderPlayer(SDL_Renderer* Renderer, player_Player* Player);

void render_DrawBarrier(SDL_Renderer* Renderer, obj_Barrier* Barrier);
void render_RenderBarriers(SDL_Renderer* Renderer, obj_Barrier* Barriers);

void render_DrawEntity(SDL_Renderer* Renderer, obj_Entity* Entity);
void render_RenderEntities(SDL_Renderer* Renderer, obj_Entity* Entities);

void render_UpdateCamera(player_Player* Player);

render_PointedRect render_GetRectPoints(int centerx, int centery, int width, int height, double Angle);
void render_RenderPointedRect(SDL_Renderer* Renderer, render_PointedRect* Rect, int Color[3]);

render_WeatherRenderData* render_CreateWeatherRenderData(weather_Weather* WeatherInstance);
void render_RenderWeatherData(SDL_Renderer* Renderer, render_WeatherRenderData* RenderData);
void render_RenderWeather(SDL_Renderer* Renderer, weather_Weather* WeatherInstance);

#endif