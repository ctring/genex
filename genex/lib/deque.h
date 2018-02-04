#ifndef DEQUE_H
#define DEQUE_H

/// Data structure (circular array) for finding minimum and maximum for LB_Keogh envolop
class deque
{
 protected:
    int *data;
    int size, capacity;
    int f, r;

 public:
    deque(int capacity);

    ~deque();

    /// Insert to the queue at the back
    void push_back(int v);

    /// Delete the current (front) element from queue
    void pop_front(void);

    /// Delete the last element from queue
    void pop_back(void);

    /// Get the value at the current position of the circular queue
    int front(void);

    /// Get the value at the last position of the circular queueint back(struct deque *d)
    int back(void);

    /// Check whether or not the queue is empty
    int empty(void);
};

#endif // DEQUE_H
