#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "level.h"


struct player_Player {
    SDL_Rect Hitbox;
    int vx, vy;
    int ax, ay;
    int state; // See above definitions
};


struct player_Player player_Init();
void player_DrawPlayer(SDL_Renderer* Renderer, struct player_Player* Player);
bool player_PlayerCheckWindowCollision(SDL_Rect Hitbox);
bool player_PlayerCheckCollision(struct level_BarrierMap* Barriers, SDL_Rect Hitbox);
void player_DoInputs(struct player_Player* Player, bool InputKeys[322]);
void player_DoPhysics(struct player_Player* Player, struct level_BarrierMap* Barriers);
void player_Update(struct player_Player* Player, bool InputKeys[322], struct level_BarrierMap* Barriers);


#endif