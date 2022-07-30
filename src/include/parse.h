#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

#include "obj.h"


obj_Barrier* parse_ParseBarrFile(char* FilePath);
obj_Entity* parse_ParseEntFile(char* FilePath);
obj_Map* parse_ParseMapFolder(char* FolderPath);

void parse_SaveBarrFile(obj_Barrier* BarriersHead, char* BarrFilePath);
void parse_SaveEntFile(obj_Entity* EntitiesHead, char* EntFilePath);
void parse_SaveMap(obj_Barrier* BarriersHead, obj_Entity* EntitiesHead, const char* NewMapDir);


#endif