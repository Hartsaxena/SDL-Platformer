#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "obj.h"
#include "update.h"
#include "parse.h"
#include "debug.h"


obj_Barrier* parse_ParseBarrFile(char* FilePath)
{
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
    SDL_Rect NewBarrierRect;
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
        Barrier_x = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_y = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_h = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_w = atoi(SplitToken);

        NewBarrierRect = (SDL_Rect) {Barrier_x, Barrier_y, Barrier_h, Barrier_w};

        BarrierDataCurr->Rect = NewBarrierRect;
    
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
    char EntityString[64];
    int EntityStringMaxSize = 64;

    bool FirstIter = true;
    while (fgets(EntityString, EntityStringMaxSize, EntFile) != NULL) {
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
        Entity_h = atoi(SplitToken);

        SplitToken = strtok(NULL, Splitter);
        Entity_w = atoi(SplitToken);

        NewEntityRect = (SDL_Rect) {Entity_x, Entity_y, Entity_h, Entity_w};

        EntityDataCurr->Hitbox = NewEntityRect;
        EntityDataCurr->vx = 0, EntityDataCurr->vy = 0;
        EntityDataCurr->ax = 0, EntityDataCurr->ay = 0;
        EntityDataCurr->State = ENTITY_STATE_IDLE; // Entities don't move for now.
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
    
    return EntityDataHead;
}


parse_ParseResult parse_ParseMapFolder(char* FolderPath)
{
    parse_ParseResult ParseResult;
    ParseResult.BarriersParseResult = malloc(sizeof(obj_Barrier));
    ParseResult.EntitiesParseResult = malloc(sizeof(obj_Entity));
    if (ParseResult.BarriersParseResult == NULL ||
        ParseResult.EntitiesParseResult == NULL) {
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
    ParseResult.BarriersParseResult = parse_ParseBarrFile(BarriersFilePath);
    if (DEBUG_MODE)
        printf("Success!\n");

    if (DEBUG_MODE)
        printf("\tParsing %s as ent file... ", EntitiesFilePath);
    ParseResult.EntitiesParseResult = parse_ParseEntFile(EntitiesFilePath);
    if (DEBUG_MODE)
        printf("Success!\n");

    return ParseResult;
}