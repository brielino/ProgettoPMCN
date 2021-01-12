#ifndef queue_h
#define queue_h

typedef struct
{
	int id;
	struct timeval time_arrive;
	struct timeval time_exit_queue;
}element;

int push(element *list_elements,int size);
element pull(element *list_elements,int size);
void initializate(element *list_elements,int size);
void print_elements(element *list_elements,int size);
int isEmpty(element *list_elements);
int element_in_queue(element *list_elements,int size);
#endif
