#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2\\SDL.h>

#include "level.h"

/* This only has BarrierMap implemented. TODO: Implement level entities and such */

struct level_BarrierMap* level_ParseMapFile(char* filepath)
{
    // Accessing the Map File
    FILE* MapFile;
    MapFile = fopen(filepath, "r");

    // Initializing Variables for the Linked List
    struct level_BarrierMap* BarrierDataHead;
    struct level_BarrierMap* BarrierDataTemp;
    struct level_BarrierMap* BarrierDataPtr;

    // Initializing Variables for splitting the string (see `char StringBarrier[64]`)
    char* Splitter = ":";
    char* SplitToken;

    // Declaring Temporary Variables
    int Barrier_x, Barrier_y, Barrier_h, Barrier_w;
    char StringBarrier[64]; // Temporary String: Represents a single line of characters in the Map File
    int StringBarrierMaxSize = 64;

    bool FirstIter = true;
    while (fgets(StringBarrier, StringBarrierMaxSize, MapFile) != NULL) {
        BarrierDataPtr = malloc(sizeof(struct level_BarrierMap));
        if (BarrierDataPtr == NULL) {
            fprintf(stderr, "FATAL ERROR: Could not allocate memory to BarrierDataPtr while parsing map file.\n");
            abort();
        }

        SplitToken = strtok(StringBarrier, Splitter);
        Barrier_x = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_y = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_h = atoi(SplitToken);
        
        SplitToken = strtok(NULL, Splitter);
        Barrier_w = atoi(SplitToken);


        SDL_Rect NewBarrierRect = {Barrier_x, Barrier_y, Barrier_h, Barrier_w};
        BarrierDataPtr->Rect = NewBarrierRect;
    
        // Linked List Iteration
        if (FirstIter) {
            BarrierDataHead = BarrierDataTemp = BarrierDataPtr;
            FirstIter = false;
        } else {
            BarrierDataTemp->next = BarrierDataPtr;
            BarrierDataTemp = BarrierDataPtr;
        }
    }

    BarrierDataTemp->next = NULL;

    return BarrierDataHead;
}