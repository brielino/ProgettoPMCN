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
#include<stdbool.h>
#include<sys/time.h>
#include<signal.h>

typedef struct
{
	pthread_t id;
	int attive;
	//attive indica se il thread è attivo o no (1/si 0/no)

}dinamic_threads;

void *print_message_function( void *ptr );
void *print_message_function2();
float calculate_dif_time(struct timeval time1,struct timeval time2);
void *inserisci( void *ptr );
pthread_mutex_t lock;
element list_elements[50];
double Getarrival(double x);
static double arrival = 0.0;
void print_results( void);
int inseriti=0;
int processati=0;
time_t START;
float Tot_queue = 0.0;
float Tot_service = 0.0;

dinamic_threads list_threads[3];
pthread_t thread4, thread5, thread6;
void signal_handler()
{
	print_results();
}

void print_results()
{
	printf("\n\nSono stati inseriti %d Job\n\n",inseriti);
	printf("Sono stati processati %d Job\n\n",processati);
	printf("Tempo trascorso dall'inizio della simulazione: %d\n\n",(int)difftime(time(NULL),START));
	float mean_queue = 0.0;
	float mean_system = 0.0;
	mean_queue = Tot_queue / processati;
	mean_system = Tot_service / processati;
	printf("Media tempo in coda dei Job = %f\n",mean_queue);
	printf("Media tempo di servizio dei Job = %f\n",mean_system);

	exit(1);

}


void *dinamic_create(){
	while(1){
		if(element_in_queue(list_elements)>25){
			for(int i=0;i<3;i++){
				if(list_threads[i].attive == 0){
					pthread_create( &(list_threads[i].id), NULL, print_message_function2,NULL);
					break;
				}
			}
			sleep(10);
		}
	}
}

void start_t(){
	list_threads[0].id= thread4;
	list_threads[0].attive= 0;
	list_threads[1].id= thread5;
	list_threads[1].attive= 0;
	list_threads[2].id= thread6;
	list_threads[2].attive= 0;
}

int main()
{
	pthread_t thread1, thread2, thread3;
	

	char *message1 = "Thread 1";
	char *message2 = "Thread 2";
	char *message3 = "Thread 3";

	initializate(list_elements);

	signal(SIGINT,signal_handler);	
	/* Create independent threads each of which will execute function */

	pthread_create( &thread1, NULL, print_message_function, (void*) message1);
	pthread_create( &thread2, NULL, inserisci, (void*) message2);
	pthread_create( &thread3, NULL, dinamic_create, (void*) message3);
	start_t();

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
	
	START = time(NULL);
	
	pthread_join( thread3, NULL);
	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL);
	
	print_results();
	exit(0);
}

void *inserisci( void *ptr) //thread #3
{	
	time_t difference_time;
	double lambda = Random()*9;
	double time_next_arrival;
	time_t start_time;
	start_time = time(NULL);
	while((double) difftime(time(NULL),START) < 20.0)
	{
		difference_time = difftime(time(NULL),start_time); 
		if((double) difference_time < 5.0)
		{
			time_next_arrival = Getarrival(lambda);
			printf("     %f\n",time_next_arrival);
		}
		else
		{
			lambda = Random()*9;
			start_time = time(NULL);
			time_next_arrival = Getarrival(lambda);
			printf("    %f\n",time_next_arrival);
		}
		pthread_mutex_lock(&lock);
		push(list_elements);
		printf("Ho inserito un elemento %d\n",inseriti+1);
		pthread_mutex_unlock(&lock);
		inseriti++;
		sleep((double)time_next_arrival);
		//printf("sono sveglio\n");
	}
}

void *print_message_function(void *ptr)
{
	char *message;
	message = (char *) ptr;
	double service_time;
	
	while((double) difftime(time(NULL),START) < 20.0  || isEmpty(list_elements)==0)
	{	

		pthread_mutex_lock(&lock);
		element current_element = pull(list_elements);
		if(current_element.id == 0)
		{
			printf("Coda vuota, sono il %s\n",message);
			pthread_mutex_unlock(&lock);
		}
		else
		{
			printf("Sono il %s e ho preso l'elemento %d\n", message,current_element.id);
			gettimeofday(&current_element.time_exit_queue,NULL);
			float time_in_queue = calculate_dif_time(current_element.time_arrive,current_element.time_exit_queue);
			printf("L'elemento %d è stato in coda %f\n",current_element.id,time_in_queue );
			processati++;
			service_time = Exponential(1.5);
			Tot_service += service_time;
			Tot_queue += time_in_queue;
			pthread_mutex_unlock(&lock);
			sleep(service_time);
		}
	}
	
}

void *print_message_function2()
{
	double service_time;
	
	while((double) difftime(time(NULL),START) < 20.0  || isEmpty(list_elements)==0)
	{	
		if(element_in_queue(list_elements)<15){
			break;
			//aggiornare il valore attive del thread corrente
		}
		pthread_mutex_lock(&lock);
		element current_element = pull(list_elements);
		if(current_element.id == 0)
		{
			//printf("Coda vuota, sono il %s\n",message);
			pthread_mutex_unlock(&lock);
		}
		else
		{
			printf("Ho preso l'elemento %d\n",current_element.id);
			gettimeofday(&current_element.time_exit_queue,NULL);
			float time_in_queue = calculate_dif_time(current_element.time_arrive,current_element.time_exit_queue);
			printf("L'elemento %d è stato in coda %f\n",current_element.id,time_in_queue );
			processati++;
			service_time = Exponential(1.5);
			Tot_service += service_time;
			Tot_queue += time_in_queue;
			pthread_mutex_unlock(&lock);
			sleep(service_time);
		}
	}
	
}


double Getarrival(double x)
{
	SelectStream(1);
	arrival = Exponential(x);
	return arrival;
}

float calculate_dif_time(struct timeval before, struct timeval after)
{
	struct timeval time;
	timersub(&after,&before,&time);
	float sec = (float) time.tv_sec;
        float micro = (float) time.tv_usec;
        float micro_ = micro/1000000;
	return sec + micro_;	
}
