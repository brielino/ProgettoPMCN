#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
typedef struct
{
	int id;
	struct timeval time_arrive;
	struct timeval time_exit_queue;
}element;


int k=1;

int push(element *list_elements){
	element p;
	gettimeofday(&p.time_arrive,NULL);
	p.id=k;
	k++;
	for(int i=0;i<50;i++){
		if(list_elements[i].id==0){
			list_elements[i]=p;
			return 1;
		}
	}
	return 0;
	
}

int element_in_queue(element *list_elements){
	int n =0;	
	for(int i=0;i<50;i++){
		if(list_elements[i].id!=0){
			n++;
		}
	}
	return n;
}

int isEmpty(element *list_elements){
	if(list_elements[0].id != 0){
		return 0;
	}
	return 1;
}


element pull(element *list_elements){
	int i;
	if(list_elements[0].id==0){
		/*printf("WAIT!!! Queue full\n");*/
		element p;
		p.id=0;
		return p;
	}
	element current_element = list_elements[0];
	gettimeofday(&current_element.time_exit_queue,NULL);
	for(i=1;i<50;i++){
		if(list_elements[i-1].id!=0){
			list_elements[i-1].id=list_elements[i].id;
			list_elements[i-1].time_arrive=list_elements[i].time_arrive;
		}
		if(i==50-1){
			element p;
			p.id=0;
			list_elements[i]=p;
		}
	}
	return current_element;
}

void initializate(element *list_elements){
	for(int i=0;i<50;i++){
		element p;
		p.id=0;
		list_elements[i]=p;
	}
	return;
}

void print_elements(element *list_elements){
	for(int i=0;i<50;i++){
		printf("%d valore elemento\n",list_elements[i].id);
	}
}
