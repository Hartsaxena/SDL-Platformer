#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

#include "obj.h"


obj_Barrier* parse_ParseBarrFile(char* FilePath);
obj_Entity* parse_ParseEntFile(char* FilePath);
obj_Map* parse_ParseMapFolder(char* FolderPath);


#endif