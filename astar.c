#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "astar.h"

/*Definition of global vector array from direction */
const dir_vec_t g_dir_vec = {
	{ 1,	0 },
	{ 1,	1 },
	{ 0,	1 },
	{ -1,	1 },
	{ -1,	0 },
	{ -1,	-1 },
	{ 0,	-1 },
	{ 1,	-1 }
};

astar_t* astar_create(map_t* map)
{
    astar_t* astar = (astar_t*)malloc(sizeof(astar_t));
    astar->map = map;
    astar->open_list = pqueue_create((MAP_SIZE_X(map))+(MAP_SIZE_Y(map)));
    astar->path_info = (path_info_t*)malloc(sizeof(path_info_t)*MAX_CELL_INDEX(map));
    return astar;
}

void astar_release(astar_t* astar)
{
    pqueue_release(astar->open_list);
    free(astar->path_info);
    free(astar);
}

static cell_t* next_walkable_cell (astar_t *astar, cell_t* cell, direction_t direction);

static cell_t* jump(astar_t* astar, cell_t* next_cell, direction_t direction);

static bool traversal_jps_neighbours(astar_t *astar, cell_t* curr_cell);

static bool traversal_all_neighbours(astar_t *astar, cell_t* curr_cell);

static int add_to_open_list(astar_t* astar, cell_t* curr_cell, cell_t* next_cell, direction_t direction);

static int link_cell(astar_t* astar, cell_t* curr_cell, cell_t* next_cell, direction_t direction);

static int resolve_path(astar_t* astar, coord_t* path, int size);

// is this coordinate within the map bounds, and also walkable?
static cell_t* next_walkable_cell (astar_t *astar, cell_t* cell, direction_t direction)
{
    vector_t vector = DIRECTION_VECTOR(direction);
    vector.x = CELL_X(astar->map, cell) + vector.x;
    vector.y = CELL_Y(astar->map, cell) + vector.y;
    if (!IS_COORD_VALID(astar->map, &vector))
        return NULL;

    cell = COORD_TO_CELL(astar->map, &vector);
    if (IS_BLOCK(cell))
        return NULL;

    return cell;
}

/**
* Returns true if jps+ has been triggered where we have searched the path
* and should return directly.
*/
static bool traversal_jps_neighbours(astar_t *astar, cell_t* curr_cell)
{
#define ADD_NEIGHBOUR(next_cell, direction)                                              \
    if ((next_cell))                                                                     \
    {                                                                                    \
        next_next_cell = jump(astar, next_cell, (direction));                            \
        if (astar->marked)                                                               \
        {                                                                                \
            link_cell(astar, curr_cell, next_next_cell, (direction));                    \
            link_cell(astar, next_next_cell, astar->end_cell, astar->marked_direction);  \
            return true;                                                                 \
        }                                                                                \
        if (next_next_cell)                                                              \
        {                                                                                \
            add_to_open_list(astar, curr_cell, next_next_cell, (direction));             \
        }                                                                                \
    }

    cell_t* next_next_cell;
    path_info_t* path_info = &astar->path_info[CELL_INDEX(astar->map, curr_cell)];
    if (unlikely(!path_info->prev_cell))
    {
        bool nonblock[9];
        direction_t direction;
        for(direction = 0; direction <= DIRECTION_MAX; direction += 2)
        {
            cell_t* next_cell = next_walkable_cell(astar, curr_cell, direction);
            ADD_NEIGHBOUR(next_cell, direction);
            nonblock[direction] = next_cell ? 1 : 0;
        }
        nonblock[8] = nonblock[0];
        for(direction = 1; direction <= DIRECTION_MAX && nonblock[direction - 1] && nonblock[direction + 1]; direction += 2)
        {
            cell_t* next_cell = next_walkable_cell(astar, curr_cell, direction);
            ADD_NEIGHBOUR(next_cell, direction);
        }
    }
    else
    {
        if (DIRECTION_IS_DIAGONAL(path_info->prev_direction)) /*diagnal*/
        {
            /* natural neighbour */
            cell_t* next_cell1 = next_walkable_cell(astar, curr_cell, path_info->prev_direction);
            ADD_NEIGHBOUR(next_cell1, path_info->prev_direction);
            cell_t* next_cell2 = next_walkable_cell(astar, curr_cell, path_info->prev_direction + 1);
            ADD_NEIGHBOUR(next_cell2, path_info->prev_direction + 1);
            cell_t* next_cell3 = next_walkable_cell(astar, curr_cell, path_info->prev_direction - 1);
            ADD_NEIGHBOUR(next_cell3, path_info->prev_direction - 1);
        }
        else/*straight*/
        {
            /* natural neighbour */
            cell_t* next_cell1 = next_walkable_cell(astar, curr_cell, path_info->prev_direction);
            ADD_NEIGHBOUR(next_cell1, path_info->prev_direction);
            /* forced neighbour */
            cell_t* next_cell2 = next_walkable_cell(astar, curr_cell, path_info->prev_direction + 3);
            cell_t* next_cell3 = next_cell2 ? NULL : next_walkable_cell(astar, curr_cell, path_info->prev_direction + 2);
            ADD_NEIGHBOUR(next_cell3, path_info->prev_direction + 2);
            cell_t* next_cell4 = !(next_cell3 && next_cell1) ? NULL : next_walkable_cell(astar, curr_cell, path_info->prev_direction + 1);
            ADD_NEIGHBOUR(next_cell4, path_info->prev_direction + 1);
            /* forced neighbour */
            cell_t* next_cell5 = next_walkable_cell(astar, curr_cell, path_info->prev_direction + 5);
            cell_t* next_cell6 = next_cell5 ? NULL : next_walkable_cell(astar, curr_cell, path_info->prev_direction + 6);
            ADD_NEIGHBOUR(next_cell6, path_info->prev_direction + 6);
            cell_t* next_cell7 = !(next_cell6 && next_cell1) ? NULL : next_walkable_cell(astar, curr_cell, path_info->prev_direction + 7);
            ADD_NEIGHBOUR(next_cell7, path_info->prev_direction + 7);
        }
    }
    return false;
}

static bool traversal_all_neighbours(astar_t *astar, cell_t* curr_cell)
{
    bool nonblock[9];
    direction_t direction;
    for(direction = 0; direction <= DIRECTION_MAX; direction += 2)
    {
        cell_t* next_cell = next_walkable_cell(astar, curr_cell, direction);
        if (next_cell)
        {
            add_to_open_list(astar, curr_cell, next_cell, direction);
            nonblock[direction] = 1;
        }
        else
        {
            nonblock[direction] = 0;
        }
    }
    nonblock[8] = nonblock[0];
    for(direction = 1; direction <= DIRECTION_MAX && nonblock[direction - 1] && nonblock[direction + 1]; direction += 2)
    {
        cell_t* next_cell = next_walkable_cell(astar, curr_cell, direction);
        if (next_cell)
        {
            add_to_open_list(astar, curr_cell, next_cell, direction);
        }
    }
    return false;
}

//(!(DIRECTION_IS_DIAGONAL(direction)) &&
#define HAS_FORCED_NEIGHBOUR(astar, cell, direction)    \
        ((!next_walkable_cell(astar, cell, (direction) + 3) && next_walkable_cell(astar, cell, (direction) + 2)) ||  \
         (!next_walkable_cell(astar, cell, (direction) + 5) && next_walkable_cell(astar, cell, (direction) + 6)))

static cell_t* jump(astar_t* astar, cell_t* next_cell, direction_t direction)
{
    /* assert(next_cell != NULL); */
    cell_t *next_next_cell1, *next_next_cell2, *next_next_cell3;

    if (unlikely(next_cell == astar->end_cell))
    {
        astar->marked = 1;
        astar->marked_direction = direction;
        return next_cell;
    }

    if (unlikely(!DIRECTION_IS_DIAGONAL(direction)))
    {
        if (HAS_FORCED_NEIGHBOUR(astar, next_cell, direction))
            return next_cell;
    }
    else
    {
        next_next_cell1 = next_walkable_cell(astar, next_cell, direction + 1);
        if (next_next_cell1 && jump(astar, next_next_cell1, direction + 1))
            return next_cell;

        next_next_cell2 = next_walkable_cell(astar, next_cell, direction - 1);
        if (next_next_cell2 && jump(astar, next_next_cell2, direction - 1))
            return next_cell;
        /* can't walk through DIAGONAL block */
        if (!next_next_cell1 || !next_next_cell2)
            return NULL;
    }

    next_next_cell3 = next_walkable_cell(astar, next_cell, direction);
    return next_next_cell3 ? jump(astar, next_next_cell3, direction) : NULL;
}

static int link_cell(astar_t* astar, cell_t* curr_cell, cell_t* next_cell, direction_t direction)
{
    if (unlikely(curr_cell == next_cell)) return 0;
    path_info_t* next_path_info = &astar->path_info[CELL_INDEX(astar->map, next_cell)];
    //if (next_path_info->closed) return 0;
    next_path_info->prev_cell = curr_cell;
    next_path_info->prev_direction = direction;
    return 1;
 }

static int add_to_open_list(astar_t* astar, cell_t* curr_cell, cell_t* next_cell, direction_t direction)
{
    path_info_t* next_path_info = &astar->path_info[CELL_INDEX(astar->map, next_cell)];
    if (unlikely(next_path_info->closed)) return 0;

    path_info_t* curr_path_info = &astar->path_info[CELL_INDEX(astar->map, curr_cell)];
    if (!pqueue_exist(astar->open_list, next_path_info->pqueue_index))
    {
	    next_path_info->prev_cell = curr_cell;
	    next_path_info->prev_direction = direction;
	    next_path_info->g_score = curr_path_info->g_score + G_DISTANCE(astar->map, curr_cell, next_cell, direction);
        next_path_info->h_score = H_DISTANCE(astar->map, next_cell, astar->end_cell, direction);
        priority_t priority = next_path_info->g_score + next_path_info->h_score;
        pqueue_push(astar->open_list, (element_t){(void*)next_cell, priority, &(next_path_info->pqueue_index)});
        return 1;
    }

    priority_t new_g_score = curr_path_info->g_score + G_DISTANCE(astar->map, curr_cell, next_cell, direction);
    if (compare_priority(new_g_score, next_path_info->g_score))
    {
        next_path_info->prev_cell = curr_cell;
        next_path_info->prev_direction = direction;
        next_path_info->g_score = new_g_score;
        priority_t priority = new_g_score + next_path_info->h_score;
        pqueue_change_priority(astar->open_list, next_path_info->pqueue_index, priority);
        return 2;
    }

    return 0;
}

/*
* Stores all path points on the road into `path`. If you need jps point only,
  define JPS_POINT_ONLY in file common.h.
*/
static int resolve_path(astar_t* astar, coord_t* path, int size)
{
#define ADD_TO_PATH_COORD(cell)                      \
    if (unlikely(path_count + 1 > size))              \
        return 0;                                      \
    path[path_count].x = CELL_X(astar->map, (cell));  \
    path[path_count].y = CELL_Y(astar->map, (cell));  \
    ++path_count;

    int path_count = 0;
    path_info_t* path_info;
    cell_t* curr_cell = astar->end_cell;
    while (curr_cell != astar->beg_cell)
    {
        path_info = &astar->path_info[CELL_INDEX(map, curr_cell)];
        if (unlikely(!path_info->prev_cell)) break;
#ifdef JPS_POINT_ONLY
        ADD_TO_PATH_COORD(curr_cell);
        curr_cell = path_info->prev_cell;
#else
        while (curr_cell != path_info->prev_cell)
        {
            ADD_TO_PATH_COORD(curr_cell);
            vector_t vector = DIRECTION_VECTOR(path_info->prev_direction + 4); //anti direction
            vector.x = CELL_X(astar->map, curr_cell) + vector.x;
            vector.y = CELL_Y(astar->map, curr_cell) + vector.y;
            curr_cell = COORD_TO_CELL(astar->map, &vector);
        }
#endif
    }
    return path_count;
}

int astar_find_path(astar_t* astar, coord_t beg, coord_t end, coord_t* path, int size, bool use_jps)
{
	map_t* map = astar->map;

	if (!IS_COORD_VALID(map, &beg) || !IS_COORD_VALID(map, &end))
		return 0;

    cell_t* beg_cell = COORD_TO_CELL(map, &beg);
    cell_t* end_cell = COORD_TO_CELL(map, &end);

	if (IS_BLOCK(beg_cell) || IS_BLOCK(end_cell))
		return 0;

	astar->beg_cell = beg_cell;
	astar->end_cell = end_cell;
    astar->marked = 0;
    pqueue_clear(astar->open_list);
    memset(astar->path_info, 0, sizeof(path_info_t)*MAX_CELL_INDEX(map));

    path_info_t* path_info = &astar->path_info[CELL_INDEX(map, beg_cell)];
    path_info->h_score = H_DISTANCE(map, beg_cell, end_cell, 0);
    element_t to_push = { (void*)(beg_cell), path_info->h_score, &(path_info->pqueue_index) };
    pqueue_push(astar->open_list, to_push);
	while (!pqueue_is_empty(astar->open_list)) {
		cell_t* curr_cell =(cell_t*)pqueue_top(astar->open_list).user_data;
        if(curr_cell == end_cell) break;

        pqueue_pop(astar->open_list);
        astar->path_info[CELL_INDEX(map, curr_cell)].closed = 1;

        if (use_jps)
        {
            if (traversal_jps_neighbours(astar, curr_cell))
                return resolve_path(astar, path, size);
        }
        else
        {
            traversal_all_neighbours(astar, curr_cell);
        }
	}

    return resolve_path(astar, path, size);
}

