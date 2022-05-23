#define _GNU_SOURCE

#ifndef DEQUE_H
#define DEQUE_H
#include <dlfcn.h>
#include"atomic.h"
// #include<stdlib.h>
// #include "atomic.h"

// delete content, fix the queue to hold thread_t as node
#define CLEAN_FIRST_16_BYTES_OF_POINTER(X)(X & (((~(0ULL))<<16)>>16) )
#define CLEAN_LAST_48_BYTES_OF_POINTER(X)(X & ((~(0ULL))<<48))

#ifdef __cplusplus
    #define _BEGIN_DECLS_ extern "C" {
    #define _END_DECLS_         }
#else
    #define _BEGIN_DECLS_
    #define _END_DECLS_
#endif   

_BEGIN_DECLS_

typedef struct s_deque_node {
	// void *content;
	struct s_deque_node *next;
	struct s_deque_node *prev;
} t_deque_node;

typedef struct s_deque {
	struct s_deque_node *first;
	struct s_deque_node *last;
	// spin_t lock;
	// int size;
} t_deque;

typedef struct stack_node {
	struct stack_node *next;
} t_stack_node;

typedef struct stack
{
	struct stack_node *head;
}t_stack;

void original(void*ptr);
void dequeInit(t_deque* deque);
int isEmpty(t_deque *deque);
void pushFront(t_deque *deque,t_deque_node* node);
void pushBack(t_deque *deque,t_deque_node* node);
void *popFront(t_deque *deque);
void *popBack(t_deque *deque);
void *peekFront(t_deque *deque);
void *peekBack(t_deque *deque);
void atomic_push(t_stack *stack, t_stack_node *s_node);
void *atomic_pop(t_stack *stack);
// static thread_local (void (*original_free)(void* pointer));
static void (*original_free)(void* ptr);
// static int push_back_atomic(t_deque *deque,t_deque_node* node){
// 	pushBack	
// }
// int get_queue_size(t_deque *deque);
_END_DECLS_
#endif