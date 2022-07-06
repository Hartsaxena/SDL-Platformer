#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "player.h"
#include "calc.h"
#include "level.h"
#include "front_tools.h"
#include "front.h"
#include "game_config.h"


struct player_Player player_Init()
{
    SDL_Rect NewPlayerRect = {
        CONFIG_PLAYER_SPAWN_X, CONFIG_PLAYER_SPAWN_Y,
        CONFIG_PLAYER_HEIGHT, CONFIG_PLAYER_WIDTH
    };
    struct player_Player NewPlayer = (struct player_Player) {
        .Hitbox = NewPlayerRect,
        .vx = 0, .vy = 0,
        .ax = 0, .ay = 0,
        .state = CONFIG_PLAYER_STATE_IDLE
    };

    return NewPlayer;
}


void player_DrawPlayer(SDL_Renderer* Renderer, struct player_Player* Player)
{
    int PlayerHitboxColor[3] = {255, 255, 255}; // Just for debugging. The player should never see the hitbox.
    frontTools_DrawRect(Renderer, &Player->Hitbox, PlayerHitboxColor, CONFIG_PLAYER_HITBOX_ALPHA);
}


bool player_PlayerCheckWindowCollision(SDL_Rect Hitbox)
{
    // Check Y coordinate
    // if (Hitbox.y < 0) {
    //     return true;
    // }
    // if (Hitbox.y + Hitbox.h > front_SCREENY) {
    //     return true;
    // }
    // Check X coordinate
    if (Hitbox.x < 0) {
        return true;
    }
    if (Hitbox.x + Hitbox.w > front_SCREENX) {
        return true;
    }

    return false;
}


bool player_PlayerCheckCollision(struct level_BarrierMap* Barriers, SDL_Rect Hitbox)
{
    if (player_PlayerCheckWindowCollision(Hitbox)) {
        return true;
    }

    for (struct level_BarrierMap* Barrier = Barriers; Barrier != NULL; Barrier = Barrier->next) {
        SDL_Rect BarrierRect = Barrier->Rect;
        if (SDL_HasIntersection(&Hitbox, &BarrierRect)) {
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
        bool IsIdle = (Player->state == CONFIG_PLAYER_STATE_IDLE || Player->state == CONFIG_PLAYER_STATE_WALK);
        bool InAir = (Player->vy != 0);
        if (IsIdle && !InAir) {
            Player->state = CONFIG_PLAYER_STATE_JUMP_START;
        }
    }
    if (Down) {
        if (Player->state == CONFIG_PLAYER_STATE_JUMP_ACCELERATE) {
            Player->state = CONFIG_PLAYER_STATE_ACCEL_FALL;
            Player->vy += CONFIG_PLAYER_ACCEL_FALL_STEP;
        }
    }
    if (Left) {
        if (Player->state != CONFIG_PLAYER_STATE_ACCEL_FALL) {
            Player->vx -= CONFIG_PLAYER_BASE_VX;
        }
    }
    if (Right) {
        if (Player->state != CONFIG_PLAYER_STATE_ACCEL_FALL) {
            Player->vx += CONFIG_PLAYER_BASE_VX;
        }
    }
}


void player_DoPhysics(struct player_Player* Player, struct level_BarrierMap* Barriers)
{
    SDL_Rect NewHitbox = {
        Player->Hitbox.x,
        Player->Hitbox.y,
        Player->Hitbox.h,
        Player->Hitbox.w
    };

    if (Player->state == CONFIG_PLAYER_STATE_JUMP_START) {
        Player->vy -= CONFIG_PLAYER_JUMP_START_BOOST;
        Player->state = CONFIG_PLAYER_STATE_JUMP_ACCELERATE;
    }
    if (Player->state == CONFIG_PLAYER_STATE_JUMP_ACCELERATE) {
        Player->state = CONFIG_PLAYER_STATE_IDLE;
        Player->vy -= CONFIG_PLAYER_JUMP_STEP;
    }

    Player->vy += CONFIG_PLAYER_GRAVITY_AY; // Oh look here's gravity.
    // Detect Collision for Y axis
    Player->vy = MIN(Player->vy, CONFIG_PLAYER_MAX_VY);
    NewHitbox.y += Player->vy;
    if (player_PlayerCheckCollision(Barriers, NewHitbox)) {
        Player->vy = 0;
        NewHitbox.y = Player->Hitbox.y;
    }
    // ... X axis
    NewHitbox.x += Player->vx;
    if (player_PlayerCheckCollision(Barriers, NewHitbox)) {
        Player->vx = 0;
        NewHitbox.x = Player->Hitbox.x;
    }

    Player->Hitbox = NewHitbox;
}


void player_Update(struct player_Player* Player, bool InputKeys[322], struct level_BarrierMap* Barriers)
{
    player_DoInputs(Player, InputKeys);
    player_DoPhysics(Player, Barriers);
}