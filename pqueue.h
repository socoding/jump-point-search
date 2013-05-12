#ifndef PQUUE_H_
#define PQUUE_H_

typedef int priority_t;
typedef int pqueue_size_t;

#define DEFAULT_PQUEUE_SIZE 32
#define MAX_PQUEUE_SIZE (1 << (sizeof(pqueue_size_t) << 3 - 1) / sizeof(element_t))

typedef struct
{
	void* user_data;			/* user data. */
	priority_t priority;		/* priority of the element. */
	pqueue_size_t* index_ptr;	/* address to hold the index where the element is in the priority queue.
								   set it null if you don't care about it. */
}element_t;

typedef struct pqueue_t
{
	element_t* container;
	pqueue_size_t capacity;
	pqueue_size_t size;
}pqueue_t;

/*you can change it to be a max queue or to match the type of priority_t.*/
#define compare_priority(priority1, priority2) ((priority1) < (priority2))

#define compare_element(element1, element2) compare_priority(((element1).priority), ((element2).priority))

#define pqueue_is_empty(pqueue) ((pqueue)->size == (priority_t)(0))

#define pqueue_size(pqueue) ((pqueue)->size)

#define pqueue_clear(pqueue) ((pqueue)->size = (priority_t)(0))

pqueue_t* pqueue_create(pqueue_size_t capacity);

void pqueue_release(pqueue_t* pqueue);

int pqueue_push(pqueue_t* pqueue, element_t element);

element_t pqueue_pop(pqueue_t* pqueue);

#define pqueue_top(pqueue) ((pqueue)->container[1])

int pqueue_change_priority(pqueue_t* pqueue, pqueue_size_t index, priority_t new_priority);

#define pqueue_exist(pqueue, index) ((index) > (priority_t)(0) && (index) <= (pqueue)->size)

#define pqueue_get_priority(pqueue, index) ((pqueue)->container[(index)].priority)

#endif
