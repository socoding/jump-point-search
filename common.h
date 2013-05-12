#ifndef COMMON_H_
#define COMMON_H_

/*
Config the following macro to fit your own requirement.
#define MAP_CELL_ARRAY_X_Y      //map cell array[x][y], default is array[y][x]
#define JPS_POINT_ONLY          //stores only jps point into result path.
*/

typedef char bool;
#define true  1
#define false 0

#define COORD_VALUE	int x; int y

typedef struct {
	COORD_VALUE;
}coord_t;

typedef struct {
	COORD_VALUE;
	int index;
	unsigned flag;
}cell_t;

#define COORD_X(coord)		((coord)->x)
#define COORD_Y(coord)		((coord)->y)

/* We may calculate the index use x and y. Or we may have flag in struct cell_t
   when we should calculate index, x and y with map address and cell_t address.
   e.g CELL_INDEX(map, cell) : ((int)((cell) - map->terrain)) */
#define CELL_INDEX(map, cell)	((cell)->index)
#define CELL_X(map, cell)		((cell)->x)
#define CELL_Y(map, cell)		((cell)->y)
#define CELL_FLAG(cell)         ((cell)->flag)
#define IS_BLOCK(cell)		    ((CELL_FLAG(cell)) > 0)

#define IS_X_VALID(map, x)			((x) >= 0 && (x) < (map)->size_x)
#define IS_Y_VALID(map, y)			((y) >= 0 && (y) < (map)->size_y)
#define IS_XY_VALID(map, x, y)		((IS_X_VALID(map, x)) && (IS_Y_VALID(map, y)))
#define IS_COORD_VALID(map, coord)	(IS_XY_VALID(map, COORD_X(coord), COORD_Y(coord)))

#define MAX_CELL_INDEX(map)         ((map)->size_x*(map)->size_y)
#define MAP_SIZE_X(map)             ((map)->size_x)
#define MAP_SIZE_Y(map)             ((map)->size_y)

#ifdef MAP_CELL_ARRAY_X_Y	/* map cell array[x][y] */
#define INDEX_TO_COORD_X(map, index)	((index) % (map)->size_y)
#define INDEX_TO_COORD_Y(map, index)	((index) / (map)->size_y)
#define XY_TO_CELL_INDEX(map, x, y)		((y) + (map)->size_y * (x))
#else						/* map cell array[y][x] */
#define INDEX_TO_COORD_X(map, index)	((index) % (map)->size_x)
#define INDEX_TO_COORD_Y(map, index)	((index) / (map)->size_x)
#define XY_TO_CELL_INDEX(map, x, y)		((x) + (map)->size_x * (y))
#endif

#define INDEX_TO_CELL(map, index)		(&((map)->terrain[index]))
#define XY_TO_CELL(map, x, y)			(INDEX_TO_CELL(map, XY_TO_CELL_INDEX(map, x, y)))
#define COORD_TO_CELL(map, coord)		(XY_TO_CELL(map, COORD_X(coord), COORD_Y(coord)))


/**********************************************
                    Y > 0
				 3    2    1
				   *  *  *
					* * *
      X < 0<--   4 ******* 0    -->X > 0
					* * *
				   *  *  *
				 5    6    7
                    Y < 0
**********************************************/
#define DIRECTION_MAX 7

typedef char direction_t;

typedef coord_t vector_t;

typedef vector_t dir_vec_t[DIRECTION_MAX + 1];

extern const dir_vec_t g_dir_vec;

#define DIRECTION(direction)              ((direction)&(DIRECTION_MAX))
#define DIRECTION_VECTOR(direction) 	  (g_dir_vec[(DIRECTION(direction))])
#define DIRECTION_IS_DIAGONAL(direction)  ((direction)&0x01)    /* DIAGONAL direction */
#define DIRECTION_IS_VERTICAL(direction)  (!((direction)&0x03)) /* VERTICAL direction */


#define abs(a) ((a) < 0 ? (-(a)) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define X_DISTANCE(map, beg_cell, end_cell) (abs(CELL_X(map, beg_cell) - CELL_X(map, end_cell)))
#define Y_DISTANCE(map, beg_cell, end_cell) (abs(CELL_Y(map, beg_cell) - CELL_Y(map, end_cell)))

#define H_DISTANCE(map, beg_cell, end_cell, direction) (max((X_DISTANCE(map, beg_cell, end_cell)), (Y_DISTANCE(map, beg_cell, end_cell))))
#define G_DISTANCE(map, beg_cell, end_cell, direction) ((DIRECTION_IS_DIAGONAL(direction)) ? (14 * (X_DISTANCE(map, beg_cell, end_cell))) : ((DIRECTION_IS_VERTICAL(direction)) ? 10 * (X_DISTANCE(map, beg_cell, end_cell)) : 10 * (Y_DISTANCE(map, beg_cell, end_cell))))

#if defined __GNUC__
#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#endif
