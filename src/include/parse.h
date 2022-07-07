#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

#include "obj.h"


typedef struct parse_ParseResult {
    obj_Barrier* BarriersParseResult;
    obj_Entity* EntitiesParseResult;
} parse_ParseResult;


obj_Barrier* parse_ParseBarrFile(char* FilePath);
obj_Entity* parse_ParseEntFile(char* FilePath);
parse_ParseResult parse_ParseMapFolder(char* FolderPath);


#endif