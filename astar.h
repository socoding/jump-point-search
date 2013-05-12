#ifndef ASTAR_H_
#define ASTAR_H_
#include "common.h"
#include "pqueue.h"

extern const dir_vec_t g_dir_vec;

typedef struct {
    pqueue_size_t pqueue_index; /* index in priority queue. 0 represents it's not in */
    priority_t h_score;         /* estimating cost from current cell to target */
    priority_t g_score;         /* cost from current cell to start */
    cell_t* prev_cell;          /* previous cell */
    direction_t prev_direction; /* direction from pre_cell to currect cell */
    bool closed;                /* Whether this cell has been put to be closed */
}path_info_t;

typedef struct astar_t{
	struct map_t* map;
	cell_t* beg_cell;       /* start cell of current pathfinding. */
	cell_t* end_cell;       /* target cell of current pathfinding. */
	struct pqueue_t* open_list;
    path_info_t* path_info;
    direction_t marked_direction;
    bool marked;             /* marked as finished */
}astar_t;

astar_t* astar_create(struct map_t* map_inst);

void astar_release(astar_t* astar);

int astar_find_path(astar_t* astar, coord_t beg, coord_t end, coord_t* path, int size, bool use_jps);

#endif
