#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "front.h"
#include "obj.h"
#include "weather.h"

#define PLAYER_WINDOW_COLLISION_ID -2

#define PLAYER_SPAWN_X (front_SCREENX / 2)
#define PLAYER_SPAWN_Y (front_SCREENY / 2)
#define PLAYER_HEIGHT  40
#define PLAYER_WIDTH   40
#define PLAYER_BASE_VX 10
#define PLAYER_MAX_VX  10 // Not used, because I'm not using accelerated left/right movement.
#define PLAYER_MAX_VY  20
#define PLAYER_MAX_AX  69420 // I haven't found a use for player ax yet, but i'm putting it here just because.
#define PLAYER_MAX_AY  18

#define PLAYER_STATE_IDLE 0
#define PLAYER_STATE_WALK 1
#define PLAYER_STATE_JUMP 2

#define PLAYER_JUMP_START_BOOST 30
#define PLAYER_JUMP_STEP 14
#define PLAYER_ACCEL_FALL_STEP 30

#define PLAYER_BULLET_BASE_SPEED 20.0
#define PLAYER_BULLET_LENGTH 15 // Side length of bullet.
#define PLAYER_BULLET_DELAY 15 // Number of frames between bullets being allowed to be fired.

#define PLAYER_HITBOX_ALPHA 255 // Should be 0 when distributed (nobody needs to see the hitbox)


typedef struct player_Bullet {
    SDL_Rect Hitbox;
    double vx, vy;
    bool Direction;
    bool Active;
    struct player_Bullet* next;
} player_Bullet;

typedef struct player_Player {
    SDL_Rect Hitbox;
    int vx, vy;
    int ax, ay;
    bool Direction; // True = right, false = left.
    int State; // See above definitions
    bool Alive;
    bool Render;
    player_Bullet* BulletsHead;
    int BulletDelay;
} player_Player;

player_Player player_Init();
void player_CreateNewBullet(player_Player* Player);
int player_BulletCheckCollision(player_Bullet* Bullet, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead);
void player_UpdateBullets(player_Player* Player, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead, weather_Weather* CurrentWeather);
bool player_PlayerCheckWindowCollision(SDL_Rect Hitbox);
bool player_PlayerCheckFell(player_Player* Player);
int player_PlayerCheckCollision(obj_Barrier* BarriersHead, SDL_Rect Hitbox);
bool player_PlayerCheckEnemyEntityCollision(obj_Entity* EntitiesHead, SDL_Rect Hitbox);
void player_DoInputs(player_Player* Player, bool InputKeys[286]);
void player_DoPhysics(player_Player* Player, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead);
void player_UpdatePlayer(player_Player* Player, bool InputKeys[286], obj_Barrier* BarriersHead, obj_Entity* EntitiesHead, weather_Weather* CurrentWeather);


#endif