/*
This is where map folders are parsed and translated into in-game levels.
The code is really confusing in this file, so I'll try to explain what the whole thing does, as opposed to what each line does.
Format for a map folder:
    In the folder, there must be a barr file. This file specifies the barriers that are in the level.
        This includes void spaces, walls, platforms, and other barriers. The floor does not need to be specified (it is assumed to be a void space).
    In the folder, there must be a ent file. This file specifies the entities that are in the level.
        This includes enemies, items (not implemented), and other entities.
    In the folder, there must be a wthr file. This file specifies the weather that is in the level.
        This includes weather instances, which are stored in a linked list.


In each function that parses a specific file, the documentation contains a format that explains how to write the object specifications in the file. These formats look confusing as hell because they're really just for me. I'm sorry.
Because they're so fucking confusing, I'm going to try to explain them in a more readable way.

How to read file formats:
Each line in these files contain values separated by colons (":")
    For example, the line "1:2:3:4" means that the first value is 1, the second is 2, the third is 3, and the fourth is 4.

Each value has a specified format, which are written under the parse functions for their respective files.

A value format can look like this: 
    Name of value[possibilities]
Here, the Name of value is just a variable name that normally tells you what the value represents (e.g. "Type" for a barrier).
NOTE: YOU DO NOT HAVE TO WRITE THE VARIABLE NAME. IT'S ONLY IN THE FORMAT TO HELP YOU.
The following possibilities tells you all the possibilities for the value.
So, if a value format look ed like this:
    "Direction[0,1]"
You would write either "0" or "1" in the place of the value.
Notice that the possibilities array isn't seperated by any spaces, only commas.

A value format can also look like this:
    (type of value)Name of value
Here, the value doesn't have a certain amount of possibilities (for example, the location of a barrier could be anywhere).
The (type of value) tells you what type of value the value has to be.
For example, "int" means that the value must be any integer. "float" means that the value must be any floating point number.
So, if a value format looked like this:
    (int)x
You would write an integer value for the x variable (probably the x coordinate of the object you're writing)
For example: "1" or "100". "1.0" or "100.0" Would not work for this value (it might work, but it's not guaranteed) because the computer thinks it's a float, even though you probably meant an integer.
Inversely, if a value format looked like this:
    (float)x
"1.0", "100.0", "3.14159", etc. would work for this value, but "1", "100", "3", etc. would not work for this value.

A value format can also look like this:
    Name of value (condition)
Here, the name of value works the same way as the other value formats. The only difference is the condition
The condition is normally written as (if something is something) or (if something is not something).
Basically, it's a boolean expression.
The value will only be read if the condition is true.
For example, let's look at this short example excerpt:
    Speed specifier[NOSPEED, SPEED]:(float)Speed value (if Speed specifier is SPEED)
The speed specifier can either be SPEED or NOSPEED. If it is SPEED, the next value will be the speed value (a floating point number). If it is NOSPEED, there shouldn't be a specified speed value (the speed value is assumed to be 0).
So, you might write this in your line:
    SPEED:1.0:
Where the speed value is 1.0.
You might also write this in your line:
    NOSPEED:
Where the speed value is 0.0.

Let's do a whole line format example (this example format would be for a rectangle)
    (int)x:(int)y:(int)width:(int)height:Color specifier[NOCOLOR, COLOR]:(int)Color R (if Color specifier is COLOR):(int)Color G (if Color specifier is COLOR):(int)Color B (if Color specifier is COLOR)
This is a pretty big example, but it's not too hard to understand.
Basically, the first three 4 values are the x coordinate, y coordinate, width, and height of the rectangle.
The next value is either NOCOLOR or COLOR. If it is COLOR, the next three values are the RGB values of the color.
If it is NOCOLOR, there shouldn't be any RGB values.
So, you might write this line:
    100:100:400:400:COLOR:255:0:0
Where the rectangle's topleft corner is situated at (100, 100) (Coordinates are always the topleft corner of the object. I don't like it, but computers do) and it's width and height are 400 pixels, making it a square. It's color is a nice bright red (255, 0, 0).
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
#include "weather.h"
#include "calc.h"


obj_Barrier* parse_ParseBarrFile(char* FilePath)
{
    /*
    This function parses a barr file and returns a linked list of barriers (obj_Barrier).
    
    Barrier format:
    Type[B, V, P]:(int)X:(int)Y:(int)W:(int)H
    */
    FILE* BarrFile;
    BarrFile = fopen(FilePath, "r");

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
    while (fgets(BarrierString, BarrierStringMaxSize, BarrFile) != NULL) {
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

    Entity format:
    Is Enemy[0, 1]:(int)X:(int)Y:(int)W:(int)H:Speed Specifier[SPEED, NSPEED]:(int)Speed Value (if Speed Specifier is SPEED):Domain Format (see below):Direction[0, 1] (0 = left, 1 = right)

    Domain Format:
    NDOMAIN (no domain) OR:
    XDomain Specifier[XDOMAIN, NXDOMAIN]:(int)Domain X minimum value (if XDomain Specifier is XDOMAIN):(int)Domain X maximum value (if XDOMAIN Specifier is XDOMAIN):YDomain Specifier[YDOMAIN, NYDOMAIN]:(int)Domain Y minimum value (if YDomain Specifier is YDOMAIN):(int)Domain Y maximum value (if YDomain Specifier is YDOMAIN)
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


weather_Weather* parse_ParseWthrFile(char* FilePath)
{
    /*
    Parses the Wthr file and returns a linked list of weather (weather_Weather).

    Weather format:
    (float)Direction in Degrees:(float)Strength:(int)Duration
    */
    FILE* WthrFile;
    WthrFile = fopen(FilePath, "r");
    
    weather_Weather* WeatherDataHead;
    weather_Weather* WeatherDataTemp;
    weather_Weather* WeatherDataCurr;
    
    char* Splitter = ":";
    char* SplitToken;

    double WeatherDirection;
    double WeatherStrength;
    int WeatherTime;

    char WeatherString[64];
    int WeatherStringCounter = 0; // Number of lines parsed.
    int WeatherStringMaxSize = 64;
    
    bool FirstIter = true;
    while (fgets(WeatherString, WeatherStringMaxSize, WthrFile) != NULL) {
        WeatherStringCounter++;
        
        SplitToken = strtok(WeatherString, Splitter);
        WeatherDirection = DEG_TO_RAD(atof(SplitToken));

        SplitToken = strtok(NULL, Splitter);
        WeatherStrength = atof(SplitToken);

        SplitToken = strtok(NULL, Splitter);
        WeatherTime = atoi(SplitToken) * front_FPS;

        WeatherDataCurr = weather_Init(WeatherDirection, WeatherStrength, WeatherTime);

        if (FirstIter) {
            WeatherDataHead = WeatherDataTemp = WeatherDataCurr;
            FirstIter = false;
        } else {
            WeatherDataTemp->next = WeatherDataCurr;
            WeatherDataTemp = WeatherDataCurr;
        }
    }
    WeatherDataTemp->next = NULL;

    return WeatherDataHead;
}


obj_Map* parse_ParseMapFolder(char* FolderPath)
{
    /*
    This function parses the whole map folder. It returns a obj_Map structure (see definition).
    */
    obj_Map* ParseMap = malloc(sizeof(obj_Map));
    ParseMap->BarriersHead = malloc(sizeof(obj_Barrier));
    ParseMap->EntitiesHead = malloc(sizeof(obj_Entity));
    ParseMap->WeatherInstance = malloc(sizeof(weather_Weather));
    if (ParseMap->BarriersHead == NULL ||
        ParseMap->EntitiesHead == NULL ||
        ParseMap->WeatherInstance == NULL) {
        if (DEBUG_MODE) {
            printf("Error: Could not allocate memory for Parse Result.\n");
        }
        abort();
    }

    char BarriersFilePath[256];
    char EntitiesFilePath[256];
    char WeatherFilePath[256];

    snprintf(BarriersFilePath, 256, "%s\\barr", FolderPath);
    snprintf(EntitiesFilePath, 256, "%s\\ent", FolderPath);
    snprintf(WeatherFilePath, 256, "%s\\wthr", FolderPath);

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

    if (DEBUG_MODE)
        printf("\tParsing %s as wthr file... ", WeatherFilePath);
    ParseMap->WeatherInstance = parse_ParseWthrFile(WeatherFilePath);
    if (DEBUG_MODE)
        printf("Success!\n");


    return ParseMap;
}