#include <assert.h>
#include <stdlib.h>

#include "queue.h"
#include <iostream>


/// A generic node of the queue
typedef struct queue_node
{
	/// Pointer to the user data
	int data;
	/// Next node in the queue
	queue_node* next;
} queue_node_t;

/// A queue of user data elements
typedef struct queue
{
	/// Number of elements in the queue
	size_t count;
	/// Pointer to the first element in the queue
	queue_node_t* head;
	/// Pointer to the last element in the queue
	queue_node_t* tail;
} queue_t;


queue_t* queue_create()
{
	// Create a record in dynamic memory with all its bits in zero
	queue_t* queue = new queue_t() ;
	assert(queue);
	return queue;
}

int queue_append(queue_t* queue, int data)
{
	assert(queue);

	// Create the node and copy the data
	queue_node_t* node = new queue_node_t();
	if ( node == nullptr ) return 0;
	node->data = data;

	// Append the new node to the queue
	if ( queue_is_empty(queue) )
		queue->head = queue->tail = node;
	else
		queue->tail = queue->tail->next = node;

	// Return a pointer to the data
	++queue->count;
	return node->data;
}

int queue_peek(queue_t* queue)
{
	assert(queue);
	assert(queue_is_empty(queue) == false);

	return queue->head->data;
}

size_t queue_count(const queue_t* queue)
{
	assert(queue);

	return queue->count;
}

bool queue_is_empty(const queue_t* queue)
{
	assert(queue);

	return queue->head == NULL;
}

void queue_destroy(queue_t* queue, bool remove_data)
{
	assert(queue);

	queue_clear(queue, remove_data);
	free(queue);
}

void queue_clear(queue_t* queue, bool remove_data)
{
	assert(queue);

	while ( ! queue_is_empty(queue) )
	{
		if ( remove_data )
			/*delete */( queue_pop(queue) );
		else
			queue_pop(queue);
	}

	queue->head = queue->tail = NULL;
}

int queue_pop(queue_t* queue)
{
	assert(queue);
	assert(queue_is_empty(queue) == false);

	// Get a pointer to the next node and its data
	queue_node_t* node = queue->head;
	int data = node->data;

	// Move the head to the next node
	queue->head = queue->head->next;
	--queue->count;

	// If queue bacame empty, update the tail
	if ( queue_is_empty(queue) )
		queue->tail = NULL;

	// Release the memory of the node, not its data
	delete (node);
	return data;
}


queue_iterator_t queue_begin(queue_t* queue)
{
	assert(queue);
	return queue->head;
}

queue_iterator_t queue_end(queue_t* queue)
{
	(void)queue;
	return NULL;
}

queue_iterator_t queue_next(queue_iterator_t iterator)
{
	assert(iterator);
	return iterator->next;
}

int queue_data(queue_iterator_t iterator)
{
	assert(iterator);
	return iterator->data;
}
