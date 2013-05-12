#ifndef MAP_H_
#define MAP_H_
#include "common.h"

typedef struct map_t{
	cell_t* terrain;
	int size_x;
	int size_y;
	struct astar_t* astar;
}map_t;


map_t* map_create(const char* file_name);

void map_release(map_t* map);

#endif
