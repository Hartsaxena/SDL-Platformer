#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "obj.h"
#include "front.h"

#define PLAYER_WINDOW_COLLISION_ID -2

#define PLAYER_SPAWN_X (front_SCREENX / 2)
#define PLAYER_SPAWN_Y (front_SCREENY / 2)
#define PLAYER_HEIGHT  40
#define PLAYER_WIDTH   40
#define PLAYER_BASE_VX 10
#define PLAYER_MAX_VX  10 // Not used, because I'm not using accelerated left/right movement.
#define PLAYER_MAX_AX  69420 // I haven't found a use for player ax yet, but i'm putting it here just because.
#define PLAYER_MAX_AY  18

#define PLAYER_STATE_IDLE 0
#define PLAYER_STATE_WALK 1
#define PLAYER_STATE_JUMP_START 2
#define PLAYER_STATE_JUMP_ACCELERATE 3
#define PLAYER_STATE_ACCEL_FALL 4

#define PLAYER_JUMP_START_BOOST 30
#define PLAYER_JUMP_STEP 14
#define PLAYER_ACCEL_FALL_STEP 30

#define PLAYER_HITBOX_ALPHA 255 // Should be 0 when distributed (nobody needs to see the hitbox)


typedef struct player_Player {
    SDL_Rect Hitbox;
    int vx, vy;
    int ax, ay;
    int State; // See above definitions
    bool Alive;
    bool Render;
} player_Player;


struct player_Player player_Init();
void render_RenderPlayer(SDL_Renderer* Renderer, struct player_Player* Player);
bool player_PlayerCheckWindowCollision(SDL_Rect Hitbox);
bool player_PlayerCheckFell(player_Player* Player);
int player_PlayerCheckCollision(obj_Barrier* BarriersHead, SDL_Rect Hitbox);
bool player_PlayerCheckEnemyEntityCollision(obj_Entity* EntitiesHead, SDL_Rect Hitbox);
void player_DoInputs(struct player_Player* Player, bool InputKeys[322]);
void player_DoPhysics(struct player_Player* Player, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead);
void player_UpdatePlayer(struct player_Player* Player, bool InputKeys[322], obj_Barrier* Barriers, obj_Entity* EntitiesHead);


#endif