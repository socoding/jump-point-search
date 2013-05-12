#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "astar.h"
#include "map.h"

map_t* map_create(const char* file_name)
{
    FILE* file = fopen(file_name, "r");
    if (!file) return NULL;

    int size_x, size_y;
    int count = fscanf(file, "%d,%d\n", &size_x, &size_y);
    if (EOF == count || count < 2 || size_x <= 0 || size_y <= 0)
    {
        fclose(file);
        return NULL;
    }

    map_t* map = (map_t*)malloc(sizeof(map_t));
    map->terrain = (cell_t*)malloc(sizeof(cell_t)*size_x*size_y);
    map->size_x = size_x;
    map->size_y = size_y;
    char buffer[1024];
    int x, y;
    for (y = 0; y < size_y; ++y)
    {
        for (x = 0; x < size_x; ++x)
        {
            char flag = fgetc(file);
            if (EOF == flag || '\n' == flag || '\r' == flag)
            {
                free(map->terrain);
                free(map);
                fclose(file);
                return NULL;
            }
            int index = XY_TO_CELL_INDEX(map, x, y);
            cell_t* cell = INDEX_TO_CELL(map, index);
            CELL_INDEX(map, cell) = index;
            CELL_X(map, cell) = x;
            CELL_Y(map, cell) = y;
            CELL_FLAG(cell) = (unsigned)(flag - '0');
        }
        fgets(buffer, sizeof(buffer), file);
    }
    map->astar = astar_create(map);
	return map;
}

void map_release(map_t* map)
{
    astar_release(map->astar);
    free(map->terrain);
    free(map);
}
