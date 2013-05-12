#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pqueue.h"
#include "astar.h"
#include "map.h"

int main()
{
    map_t* map = map_create("./map.txt");
    if (!map)
    {
        printf("create map error!\n");
        return 1;
    }

    coord_t path1[1024];
    coord_t path2[1024];

    int path_count1 = 0;
    int path_count2 = 0;

    clock_t time1, time2, time3;
    int i1, i2;
    i1 = i2 = 1000;
    time1 = clock();
    while(i1--)
        path_count1 = astar_find_path(map->astar, (coord_t){5, 11}, (coord_t){11,4}, path1, 1024, true);
    time2 = clock();
    while(i2--)
        path_count2 = astar_find_path(map->astar, (coord_t){5, 11}, (coord_t){11,4}, path2, 1024, false);
    time3 = clock();

    printf("jps astar find time: %f\n", (double)(time2 - time1) / CLOCKS_PER_SEC);
    for (i1 = path_count1 - 1; i1 >= 0 ; i1--)
    {
        printf("x, y: %d, %d\n", path1[i1].x, path1[i1].y);
    }

    printf("normal astar find time: %f\n", (double)(time3 - time2) / CLOCKS_PER_SEC);
    for (i2 = path_count2 - 1; i2 >= 0 ; i2--)
    {
        printf("x, y: %d, %d\n", path2[i2].x, path2[i2].y);
    }

    map_release(map);
	return 0;
}
