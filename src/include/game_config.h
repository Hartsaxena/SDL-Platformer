#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H


#define CONFIG_PLAYER_SPAWN_X (front_SCREENX / 2)
#define CONFIG_PLAYER_SPAWN_Y (front_SCREENY / 2)
#define CONFIG_PLAYER_HEIGHT  40
#define CONFIG_PLAYER_WIDTH   40
#define CONFIG_PLAYER_BASE_VX 10
#define CONFIG_PLAYER_MAX_VX  10 // Not Needed, because I'm not using accelerated left/right movement.
#define CONFIG_PLAYER_MAX_VY  13
#define CONFIG_PLAYER_MAX_AX  69420 // I haven't found a use for player ax yet, but i'm putting it here just because.
#define CONFIG_PLAYER_MAX_AY  18

#define CONFIG_PLAYER_STATE_IDLE 0
#define CONFIG_PLAYER_STATE_WALK 1
#define CONFIG_PLAYER_STATE_JUMP_START 2
#define CONFIG_PLAYER_STATE_JUMP_ACCELERATE 3
#define CONFIG_PLAYER_STATE_ACCEL_FALL 4

#define CONFIG_PLAYER_JUMP_START_BOOST 30
#define CONFIG_PLAYER_JUMP_STEP 14
#define CONFIG_PLAYER_GRAVITY_AY 3
#define CONFIG_PLAYER_ACCEL_FALL_STEP 30

#define CONFIG_PLAYER_HITBOX_ALPHA 255 // Should be 0 when distributed (nobody needs to see the hitbox)

#endif