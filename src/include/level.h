#ifndef LEVEL_H
#define LEVEL_H


struct level_BarrierMap {
    SDL_Rect Rect;
    struct level_BarrierMap* next;
};

struct level_BarrierMap* level_ParseMapFile(char* filepath);


#endif