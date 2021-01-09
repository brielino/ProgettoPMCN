#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"queue.h"
#include<unistd.h>
#include"rngs.h"
#include"rvgs.h"
#include"rvms.h"
#include<math.h>
#include<time.h>
#include<stdio.h>
void *print_message_function( void *ptr );
void *inserisci(void *ptr);
pthread_mutex_t lock;
element list_elements[50];
double Getarrival(double x);
static double arrival = 0.0;
int main()
{
	pthread_t thread1, thread2, thread3;
	char *message1 = "Thread 1";
	char *message2 = "Thread 2";
	char *message3 = "Thread 3";
	int  iret1, iret2, iret3;

	initializate(list_elements);
	/* Create independent threads each of which will execute function */

	iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
	iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);
	iret3 = pthread_create( &thread3, NULL, inserisci, (void*) message3);

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
	
	pthread_join( thread3, NULL);
	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL);

	printf("Thread 1 returns: %d\n",iret1);
	printf("Thread 2 returns: %d\n",iret2);
	printf("Thread 3 returns: %d\n",iret3);
	exit(0);
}

void *inserisci( void *ptr)
{	time_t difference_time;
	double lambda = 1/10.0;
	double time_next_arrival;
	time_t start_time = time(NULL);
	while(1){
		difference_time = difftime(time(NULL),start_time);
		if((double) difference_time < 5.0){
			time_next_arrival = Getarrival(lambda);
			printf("1/10     %f",time_next_arrival);

		}else{
			start_time = time(NULL);
			time_next_arrival = Getarrival(1/8.0);
			printf("1/8    %f\n",time_next_arrival);
		}
		pthread_mutex_lock(&lock);
		push(list_elements);
		printf("Ho inserito un elemento\n");
		pthread_mutex_unlock(&lock);
		sleep((double)time_next_arrival);
		printf("sono sveglio\n");
	}	
}

void *print_message_function( void *ptr )
{
	char *message;
	message = (char *) ptr;
	printf("%s \n", message);
	while(1){
		pthread_mutex_lock(&lock);
		element current_element = pull(list_elements);
		if(current_element.id == 0){
			printf("Coda vuota, sono il %s\n",message);
		}else{
			printf("Sono il %s e ho preso l'elemento %d\n", message,current_element.id);
		}
		pthread_mutex_unlock(&lock);
		sleep(1);
		//printf("SONO SVEGLIO %s\n", message);

	}

}
double Getarrival(double x){
	SelectStream(0);
	arrival += Exponential(x);
	return arrival;
}
