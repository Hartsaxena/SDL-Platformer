#ifndef OBJ_H
#define OBJ_H

#include <stdbool.h>


typedef struct obj_Barrier {
    SDL_Rect Rect;
    struct obj_Barrier* next;
} obj_Barrier;

typedef struct obj_Entity {
    SDL_Rect Hitbox;
    int vx, vy;
    int ax, ay;
    int State;
    bool IsEnemy;
    bool Alive;
    struct obj_Entity* next;
} obj_Entity;


#endif