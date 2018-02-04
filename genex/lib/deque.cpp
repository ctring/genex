#include "deque.h"
#include <stdlib.h>

/// Initial the queue at the begining step of envelop calculation
deque::deque(int capacity)
{
    this->capacity = capacity;
    size = 0;
    data = (int *) malloc(sizeof(int)*capacity);
    f = 0;
    r = capacity-1;
}

/// Destroy the queue
deque::~deque(void)
{
    free(data);
}

/// Insert to the queue at the back
void deque::push_back(int v)
{
    data[r] = v;
    r--;
    if (r < 0)
        r = capacity-1;
    size++;
}

/// Delete the current (f) element from queue
void deque::pop_front(void)
{
    f--;
    if (f < 0)
        f = capacity-1;
    size--;
}

/// Delete the last element from queue
void deque::pop_back(void)
{
    r = (r+1)%capacity;
    size--;
}

/// Get the value at the current position of the circular queue
int deque::front(void)
{
    int aux = f - 1;

    if (aux < 0)
        aux = capacity-1;
    return data[aux];
}

/// Get the value at the last position of the circular queueint back(struct deque *d)
int deque::back(void)
{
    int aux = (r+1)%capacity;
    return data[aux];
}

/// Check whether or not the queue is empty
int deque::empty(void)
{
    return size == 0;
}
