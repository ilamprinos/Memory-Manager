#include "deque.h"

#include <stdio.h>
// #include "allocator.h"
//  HERE CHECK MALLOC

// extern "C"{
	__attribute__((constructor)) static void initializer(void) {
	
	// original_free = NULL;
	// write(1,"pre\n",4);
	original_free =dlsym(RTLD_NEXT,"free");
}


void original(void* ptr){

	// static void (*original_free)(void* pointer);
	
	// dlsym(RTLD_NEXT,"free");
	// original_free =dlsym(RTLD_NEXT,"free");
	// if(ptr!=NULL);
	// return (*original_free)(ptr);
	if(ptr==NULL)return;
	
	// if(original_free==NULL)
		// original_free =dlsym(RTLD_NEXT,"free");

	return (*original_free)(ptr);

}



void dequeInit(t_deque* deque)
{
	// t_deque *deque;
	// deque = malloc(sizeof(t_deque));
	// spin_lock_init(&(deque->lock));
	deque->first = NULL;
	deque->last = NULL;
	// deque->size = 0;
	// return deque;
}

int isEmpty(t_deque *deque)
{	
	// int ret=0;
	// ret = !deque->first || !deque->last;
	// return ret;
	return (!deque->first || !deque->last);
}

void pushFront(t_deque *deque,t_deque_node* node)
{	
	// spin_lock(&(deque->lock));
	// t_deque_node *node = malloc(sizeof(t_deque_node));
	// node->content = content;

	// if(!deque->first){}

	node->prev = NULL;
	node->next = deque->first;
	if (isEmpty(deque))
		deque->last = node;
	else
		deque->first->prev = node;
	deque->first = node;
	// deque->size++;
	// spin_unlock(&(deque->lock));
}

void atomic_push(t_stack *stack, t_stack_node *s_node)
{
	//t_stack temp;
	do{

		// temp_head = stack->head;

	
		// t_stack_node* temp_head = ((t_stack_node* ) CLEAN_FIRST_16_BYTES_OF_POINTER((unsigned long)(stack->head)));
		
		
		s_node = (t_stack_node*) (CLEAN_FIRST_16_BYTES_OF_POINTER((unsigned long)s_node));
		// if(temp_head==NULL){
		// 	s_node->next=NULL;

		// 	// s_node =(t_stack_node*) (CLEAN_FIRST_16_BYTES_OF_POINTER((unsigned long)s_node)| ((1ULL)<<48));
		// }else{
			
		// 	s_node->next = temp_head->next;
		// }

		s_node->next = stack->head;

		s_node =(t_stack_node*) (((unsigned long)s_node)| ((1ULL)<<48));

		// s_node->next = ((t_stack_node* ) CLEAN_FIRST_16_BYTES_OF_POINTER((unsigned long)stack->head))->next;
		// t_stack_node* temp_node = ((t_stack_node* ) (CLEAN_FIRST_16_BYTES_OF_POINTER((unsigned long)s_node)|CLEAN_LAST_48_BYTES_OF_POINTER((unsigned long)stack->head)));

		// s_node->next = temp_head->next;

	}while (!compare_and_swap_ptr(&(stack->head),stack->head,s_node));
	
}

void *atomic_pop(t_stack *stack)
{
	t_stack_node *temp;
	if(stack->head->next == NULL){return NULL;}
	do
	{
		temp = stack->head->next;

		
	} while (!compare_and_swap_ptr(stack->head->next,stack->head->next,temp->next));
	


}



void pushBack(t_deque *deque,t_deque_node* node)
{
	// spin_lock(&(deque->lock));
	// t_deque_node *node = malloc(sizeof(t_deque_node));
	// node->content = content;
	node->prev = deque->last;
	node->next = NULL;
	if (isEmpty(deque))
		deque->first = node;
	else
		deque->last->next = node;
	deque->last = node;
	// deque->size++;
	// spin_unlock(&(deque->lock));
}

void *popFront(t_deque *deque)
{
	// spin_lock(&(deque->lock));
	t_deque_node *node;
	// void *content;
	if (isEmpty(deque)){

		// spin_unlock(&(deque->lock));
		return NULL;
	}
	node = deque->first;
	deque->first = node->next;
	if (!deque->first)
		deque->last = NULL;
	else
		deque->first->prev = NULL;
	// content = node->content;
	// deque->size--;
	// free(node);
	// original(node);
	// spin_unlock(&(deque->lock));
	return node;

}

void *popBack(t_deque *deque)
{
	// spin_lock(&(deque->lock));
	t_deque_node *node;
	// void *content;
	if (isEmpty(deque)){
		// spin_unlock(&(deque->lock));
		return NULL;
	}
	node = deque->last;
	deque->last = node->prev;
	if (!deque->last)
		deque->first = NULL;
	else
		deque->last->next = NULL;
	// content = node->content;
	// deque->size--;
	// free(node);
	// spin_unlock(&(deque->lock));
	return node ;
}

void *peekFront(t_deque *deque)
{
	if(isEmpty(deque))
		return NULL;
	return deque->first;
}

void *peekBack(t_deque *deque)
{
	if(isEmpty(deque))
		return NULL;
	return deque->last;
}


// int get_queue_size(t_deque *deque)
// {
// 	return(deque->size);
// }
// }