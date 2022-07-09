#ifndef OBJ_H
#define OBJ_H

#include <stdbool.h>

#define OBJ_BARRIER_TYPE_WALL 0
#define OBJ_BARRIER_TYPE_VOID 1

typedef struct obj_Barrier {
    SDL_Rect Rect;
    int Type;
    struct obj_Barrier* next;
} obj_Barrier;

typedef struct obj_Entity {
    SDL_Rect Hitbox;
    int Domain_left, Domain_right;
    int Domain_top, Domain_bottom;
    int vx, vy;
    int ax, ay;
    int Speed;
    int State;
    bool Direction;
    bool IsEnemy;
    bool Alive;
    struct obj_Entity* next;
} obj_Entity;


#endif