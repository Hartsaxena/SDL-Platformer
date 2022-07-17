/*
This is where map folders are parsed and translated into in-game levels.
Admittedly, this is probably the most badly-written code in the game, but it's okay because it only runs once per game (at the time of writing this).
Format for a map folder:
    In the folder, there must be a barr file. This file specifies the barriers that are in the level.
        This includes void spaces, walls, platforms, and other barriers. The floor does not need to be specified (it is assumed to be a void space).
    In the folder, there must be a ent file. This file specifies the entities that are in the level.
        This includes enemies, items (not implemented), and other entities.
*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "obj.h"
#include "update.h"
#include "parse.h"
#include "debug.h"
#include "front.h"


obj_Barrier* parse_ParseBarrFile(char* FilePath)
{
    /*
    This function parses a barr file and returns a linked list of barriers (obj_Barrier).
    */
    // Accessing the Map File
    FILE* MapFile;
    MapFile = fopen(FilePath, "r");

    // Initializing Variables for the Linked List
    obj_Barrier* BarrierDataHead;
    obj_Barrier* BarrierDataTemp;
    obj_Barrier* BarrierDataCurr;

    // Initializing Variables for splitting the string (see `char StringBarrier[64]`)
    char* Splitter = ":";
    char* SplitToken;

    // Declaring Temporary Variables
    int Barrier_x, Barrier_y, Barrier_h, Barrier_w;
    SDL_Rect Barrier_Rect;
    int Barrier_Type;
    char BarrierString[64]; // Temporary String: Represents a single line of characters in the Map File
    int BarrierStringMaxSize = 64;

    bool FirstIter = true;
    while (fgets(BarrierString, BarrierStringMaxSize, MapFile) != NULL) {
        BarrierDataCurr = malloc(sizeof(obj_Barrier));
        if (BarrierDataCurr == NULL) {
            if (DEBUG_MODE) {
                printf("Error: Could not allocate memory for a new Barrier.\n");
            }
            abort();
        }
        SplitToken = strtok(BarrierString, Splitter);
        if (strcmp(SplitToken, "B") == 0) {
            Barrier_Type = OBJ_BARRIER_TYPE_WALL;
        } else if (strcmp(SplitToken, "V") == 0) {
            Barrier_Type = OBJ_BARRIER_TYPE_VOID;
        } else if (strcmp(SplitToken, "P") == 0) {
            Barrier_Type = OBJ_BARRIER_TYPE_PLATFORM;
        }

        SplitToken = strtok(NULL, Splitter);
        Barrier_x = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_y = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_w = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_h = atoi(SplitToken);

        Barrier_Rect = (SDL_Rect) {Barrier_x, Barrier_y, Barrier_w, Barrier_h};

        BarrierDataCurr->Rect = Barrier_Rect;
        BarrierDataCurr->Type = Barrier_Type;
    
        // Linked List Iteration
        if (FirstIter) {
            BarrierDataHead = BarrierDataTemp = BarrierDataCurr;
            FirstIter = false;
        } else {
            BarrierDataTemp->next = BarrierDataCurr;
            BarrierDataTemp = BarrierDataCurr;
        }
    }
    BarrierDataTemp->next = NULL;

    return BarrierDataHead;
}


obj_Entity* parse_ParseEntFile(char* FilePath)
{
    /*
    Parses the Ent file and returns a linked list of entities (obj_Entity).
    */
    FILE* EntFile;
    EntFile = fopen(FilePath, "r");

    obj_Entity* EntityDataHead;
    obj_Entity* EntityDataTemp;
    obj_Entity* EntityDataCurr;

    char* Splitter = ":";
    char* SplitToken;

    bool EntityIsEnemy;
    int Entity_x, Entity_y, Entity_h, Entity_w;
    SDL_Rect NewEntityRect;
    int Entity_Domain_left, Entity_Domain_right, Entity_Domain_top, Entity_Domain_bottom;
    bool EntityDirection;
    char EntityString[64];
    int EntityStringCounter = 0; // Number of lines parsed.
    int EntityStringMaxSize = 64;

    bool FirstIter = true;
    while (fgets(EntityString, EntityStringMaxSize, EntFile) != NULL) {
        EntityStringCounter++;
        EntityDataCurr = malloc(sizeof(obj_Entity));
        if (EntityDataCurr == NULL) {
            if (DEBUG_MODE) {
                printf("Error: Could not allocate memoory for a new Entity.\n");
            }
            abort();
        }

        SplitToken = strtok(EntityString, Splitter);
        EntityIsEnemy = (strcmp(SplitToken, "1") == 0);

        SplitToken = strtok(NULL, Splitter);
        Entity_x = atoi(SplitToken);

        SplitToken = strtok(NULL, Splitter);
        Entity_y = atoi(SplitToken);

        SplitToken = strtok(NULL, Splitter);
        Entity_w = atoi(SplitToken);

        SplitToken = strtok(NULL, Splitter);
        Entity_h = atoi(SplitToken);

        SplitToken = strtok(NULL, Splitter);
        if (strcmp(SplitToken, "SPEED") == 0) {
            SplitToken = strtok(NULL, Splitter);
            EntityDataCurr->Speed = atoi(SplitToken);
        } else if (strcmp(SplitToken, "NSPEED") == 0) {
            EntityDataCurr->Speed = 0;
        } else {
            if (DEBUG_MODE)
                printf("ENT FILE ERROR: Line %d: Invalid Speed.\n", EntityStringCounter);
            front_Quit();
        }

        SplitToken = strtok(NULL, Splitter);
        if (strcmp(SplitToken, "NDOMAIN") == 0) {
            Entity_Domain_left = -1;
            Entity_Domain_right = -1;
            Entity_Domain_top = -1;
            Entity_Domain_bottom = -1;
        } else {
            if (strcmp(SplitToken, "XDOMAIN") == 0) {
                SplitToken = strtok(NULL, Splitter);
                Entity_Domain_left = atoi(SplitToken);

                SplitToken = strtok(NULL, Splitter);
                Entity_Domain_right = atoi(SplitToken);
            } else if (strcmp(SplitToken, "NXDOMAIN") == 0) {
                Entity_Domain_left = -1;
                Entity_Domain_right = -1;
            } else {
                if (DEBUG_MODE)
                    printf("ENT FILE ERROR: Line %d: Invalid/Unspecified X Domain.\n", EntityStringCounter);
                front_Quit();
            }

            SplitToken = strtok(NULL, Splitter);
            if (strcmp(SplitToken, "YDOMAIN") == 0) {
                SplitToken = strtok(NULL, Splitter);
                Entity_Domain_top = atoi(SplitToken);

                SplitToken = strtok(NULL, Splitter);
                Entity_Domain_bottom = atoi(SplitToken);
            } else if (strcmp(SplitToken, "NYDOMAIN") == 0) {
                Entity_Domain_top = -1;
                Entity_Domain_bottom = -1;
            } else {
                if (DEBUG_MODE)
                    printf("ENT FILE ERROR: Line %d: Invalid/Unspecified Y Domain.\n", EntityStringCounter);
                front_Quit();
            }
        }

        SplitToken = strtok(NULL, Splitter);
        if (strcmp(SplitToken, "DIR") == 0) {
            SplitToken = strtok(NULL, Splitter);
            if (strcmp(SplitToken, "1") == 0) {
                EntityDirection = true;
            } else {
                EntityDirection = false;
            }
        } else if (strcmp(SplitToken, "NDIR") == 0) {
            EntityDirection = true;
        } else {
            if (DEBUG_MODE)
                printf("ENT FILE ERROR: Line %d: Invalid/Unspecified Direction.\n", EntityStringCounter);
        }

        NewEntityRect = (SDL_Rect) {Entity_x, Entity_y, Entity_w, Entity_h};

        EntityDataCurr->Hitbox = NewEntityRect;
        EntityDataCurr->Domain_left = Entity_Domain_left;
        EntityDataCurr->Domain_right = Entity_Domain_right;
        EntityDataCurr->Domain_top = Entity_Domain_top;
        EntityDataCurr->Domain_bottom = Entity_Domain_bottom;
        EntityDataCurr->vx = 0, EntityDataCurr->vy = 0;
        EntityDataCurr->ax = 0, EntityDataCurr->ay = 0;
        EntityDataCurr->Type = OBJ_ENTITY_TYPE_BASE; // Different types of entities aren't implemented yet.
        EntityDataCurr->State = ENTITY_STATE_IDLE;
        EntityDataCurr->Direction = EntityDirection;
        EntityDataCurr->IsEnemy = EntityIsEnemy;
        EntityDataCurr->Alive = true;

        if (FirstIter) {
            EntityDataHead = EntityDataTemp = EntityDataCurr;
            FirstIter = false;
        } else {
            EntityDataTemp->next = EntityDataCurr;
            EntityDataTemp = EntityDataCurr;
        }
    }
    EntityDataTemp->next = NULL;
    // free(EntityDataCurr);
    
    return EntityDataHead;
}


obj_Map* parse_ParseMapFolder(char* FolderPath)
{
    /*
    This function parses the whole map folder. It returns a obj_Map structure (see definition).
    */
    obj_Map* ParseMap = malloc(sizeof(obj_Map));
    ParseMap->BarriersHead = malloc(sizeof(obj_Barrier));
    ParseMap->EntitiesHead = malloc(sizeof(obj_Entity));
    if (ParseMap->BarriersHead == NULL ||
        ParseMap->EntitiesHead == NULL) {
        if (DEBUG_MODE) {
            printf("Error: Could not allocate memory for the Parse Result.\n");
        }
        abort();
    }

    char BarriersFilePath[256];
    char EntitiesFilePath[256];

    snprintf(BarriersFilePath, 256, "%s\\barr", FolderPath);
    snprintf(EntitiesFilePath, 256, "%s\\ent", FolderPath);

    if (DEBUG_MODE)
        printf("\tParsing %s as barr file... ", BarriersFilePath);
    ParseMap->BarriersHead = parse_ParseBarrFile(BarriersFilePath);
    if (DEBUG_MODE)
        printf("Success!\n");

    if (DEBUG_MODE)
        printf("\tParsing %s as ent file... ", EntitiesFilePath);
    ParseMap->EntitiesHead = parse_ParseEntFile(EntitiesFilePath);
    if (DEBUG_MODE)
        printf("Success!\n");

    return ParseMap;
}