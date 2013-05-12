#include <stdlib.h>
#include "pqueue.h"

pqueue_t* pqueue_create(pqueue_size_t capacity)
{
    if (capacity <= 0)
        capacity = DEFAULT_PQUEUE_SIZE;

	pqueue_t* pqueue = (pqueue_t*)malloc(sizeof(*pqueue));
	pqueue->container = (element_t*)malloc(sizeof(element_t)*(capacity + 1));
	pqueue->capacity = capacity;
	pqueue->size = 0;
	return pqueue;
}

void pqueue_release(pqueue_t* pqueue)
{
    free(pqueue->container);
    free(pqueue);
}

#define PQUEUE_ASSIGN_ELEMENT_INDEX(element, index)					\
		if ((element).index_ptr)									\
		{															\
			*((element).index_ptr) = (index);						\
		}

#define PQUEUE_COPY_ELEMENT(dest_element, src_element, dest_index)	\
		(dest_element) = (src_element);								\
		PQUEUE_ASSIGN_ELEMENT_INDEX((dest_element), (dest_index))

static void pqueue_up_shift(pqueue_t* pqueue, pqueue_size_t index)
{
	if (index <= 1) return;

	pqueue_size_t i, j;
	element_t* container = pqueue->container;
	element_t element = container[index];
	for (i = index, j = i >> 1; j > 0 && compare_element(element, container[j]); i = j, j >>= 1)
	{
		PQUEUE_COPY_ELEMENT(container[i], container[j], i);
	}
	if (i == index) return;

	PQUEUE_COPY_ELEMENT(container[i], element, i);
}

static int pqueue_realloc(pqueue_t* pqueue)
{
    if (pqueue->capacity >= MAX_PQUEUE_SIZE)
        return 0;
    pqueue_size_t new_capacity = pqueue->capacity + pqueue->capacity > 2 + 1;
    if (new_capacity > MAX_PQUEUE_SIZE)
        new_capacity = MAX_PQUEUE_SIZE;
    element_t* new_container = (element_t*)realloc(pqueue->container, new_capacity);
    if (!new_container)
        return 0;
    pqueue->container = new_container;
    pqueue->capacity = new_capacity;
    return 1;
}

int pqueue_push(pqueue_t* pqueue, element_t element)
{
	if (pqueue->size >= pqueue->capacity && !pqueue_realloc(pqueue))
        return 0;

	pqueue->container[++pqueue->size] = element;
	PQUEUE_ASSIGN_ELEMENT_INDEX(element, pqueue->size);

	pqueue_up_shift(pqueue, pqueue->size);

	return 1;
}

static void pqueue_down_shift(pqueue_t* pqueue, pqueue_size_t index)
{
	if (index >= pqueue->size) return;

	element_t* container = pqueue->container;
	element_t element = container[index];
	element_t* last_element_ptr = &container[pqueue->size];
	pqueue_size_t i, j;
	for (i = index, j = i << 1; j <= pqueue->size; i = j, j <<= 1)
	{
		if (j != pqueue->size && compare_element(container[j + 1], container[j]))
			j++;
		if (compare_element(container[j], element))
		{
			if (compare_element(container[j], *last_element_ptr))
			{
				PQUEUE_COPY_ELEMENT(container[i], container[j], i);
			}
			else
			{
				PQUEUE_COPY_ELEMENT(container[i], *last_element_ptr, i);
				i = pqueue->size;
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (i == index) return;

	PQUEUE_COPY_ELEMENT(container[i], element, i);
}

element_t pqueue_pop(pqueue_t* pqueue)
{
	if (pqueue->size == 0)
		return (element_t){ NULL, (priority_t)0, NULL };

	element_t* container = pqueue->container;
	element_t first_element = container[1];
	element_t* last_element_ptr = &container[pqueue->size--];
	pqueue_size_t i, j;
	for (i = 1, j = i << 1; j <= pqueue->size; i = j, j <<= 1)
	{
		if (j != pqueue->size && compare_element(container[j + 1], container[j]))
			j++;
		if (compare_element(container[j], *last_element_ptr))
		{
			PQUEUE_COPY_ELEMENT(container[i], container[j], i);
		}
		else
		{
			break;
		}
	}
	if (pqueue->size > 0)
	{
		PQUEUE_COPY_ELEMENT(container[i], *last_element_ptr, i);
	}

	PQUEUE_ASSIGN_ELEMENT_INDEX(first_element, 0);

	return first_element;
}

int pqueue_change_priority(pqueue_t* pqueue, pqueue_size_t index, priority_t new_priority)
{
	if (index < 1 || index > pqueue->size)
		return 0;

	element_t* element_ptr = &pqueue->container[index];
	priority_t old_priority = element_ptr->priority;
	element_ptr->priority = new_priority;
	if (compare_priority(new_priority, old_priority))
		pqueue_up_shift(pqueue, index);
	else
		pqueue_down_shift(pqueue, index);
	return 1;
}

