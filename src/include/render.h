#ifndef RENDER_H
#define RENDER_H

#include <SDL2\\SDL.h>

#include "player.h"
#include "parse.h"


void render_FillScreenColor(SDL_Renderer* Renderer, int Color[3], int Alpha);
void render_DrawRect(SDL_Renderer* Renderer, SDL_Rect Rect, int Color[3], int Alpha);

void render_RenderPlayer(SDL_Renderer* Renderer, player_Player* Player);

void render_DrawBarrier(SDL_Renderer* Renderer, obj_Barrier* Barrier);
void render_RenderBarriers(SDL_Renderer* Renderer, obj_Barrier* Barriers);

void render_DrawEntity(SDL_Renderer* Renderer, obj_Entity* Entity);
void render_RenderEntities(SDL_Renderer* Renderer, obj_Entity* Entities);

void render_UpdateCamera(player_Player* Player);

#endif