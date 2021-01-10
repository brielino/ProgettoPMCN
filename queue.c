#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct
{
	int id;
	clock_t time_arrive;
	clock_t time_exit_queue;
}element;


int k=1;

int push(element *list_elements){
	element p;
	p.time_arrive=clock();
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
	current_element.time_exit_queue = clock();
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
