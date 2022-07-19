/*
Normally, I don't like writing documentation for header files, but I'm going to do it here because it's a very special file.
This file contains macros and structures that are used to identify objects in the game (Platforms, Barriers, Entities, etc.)

Note that the player and player bullet objects are not in this file. It is in player.h.
This is because the player is a special object, and should be separated from other objects (in my opinion).
The weather_Weather struct is also not in this file. It is in weather.h. My reasoning is that weather isn't a physical object, but rather a mechanic that affects the objects inside it.

Most object structs are formatted as a Singly-Linked List. This is because the game needs to be able to handle an unspecified number of objects.
*/
#ifndef OBJ_H
#define OBJ_H

#include <stdbool.h>

#include "weather.h"

// Different variants of objects.
#define OBJ_NONE -1
#define OBJ_BARRIER_TYPE_WALL 0
#define OBJ_BARRIER_TYPE_VOID 1
#define OBJ_BARRIER_TYPE_PLATFORM 2
#define OBJ_ENTITY_TYPE_BASE 100

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
    int Type;
    int State;
    bool Direction;
    bool IsEnemy;
    bool Alive;
    struct obj_Entity* next;
} obj_Entity;

typedef struct obj_Map {
    // Not an individual object, but an entire map of objects. This is mainly used in parse.c (at the moment).
    // Contains pointers to heads of singly-linked lists of objects.
    obj_Barrier* BarriersHead;
    obj_Entity* EntitiesHead;
    weather_Weather* WeatherInstance;
} obj_Map;


#endif