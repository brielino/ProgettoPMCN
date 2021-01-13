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

#define DIM 250
void *print_message_function();
float calculate_dif_time(struct timeval time1,struct timeval time2);
void *inserisci();
pthread_mutex_t lock;
element list_elements[DIM];
double Getarrival(double x);
static double arrival = 0.0;
void print_results( void);
int inseriti=0;
int processati=0;
struct timeval START;
float Tot_queue = 0.0;
float Tot_service = 0.0;


/*Funzione chiamata quando si tenta di uscire dal programma premendo CTRL+C*/
void signal_handler()
{
	print_results();
}

/*Funzione che stampa tutti i risultati ottenuti*/
void print_results()
{	struct timeval current_t;
	gettimeofday(&current_t,NULL);
	float time_simulation = calculate_dif_time(START,current_t);
	printf("\n\n************************\n");
	printf("FINE SIMULAZIONE - Dati:\n");
	printf("************************\n");
	printf("\n\nSono stati inseriti %d Job\n\n",inseriti);
	printf("Sono stati processati %d Job\n\n",processati);
	printf("Tempo trascorso dall'inizio della simulazione: %f\n\n",time_simulation);
	float mean_queue = 0.0;
	float mean_system = 0.0;
	int euro = 10;
	mean_queue = Tot_queue / processati;
	mean_system = Tot_service / processati;
	float Tot = time_simulation*euro*4;
	printf("Media tempo in coda dei Job = %f\n\n",mean_queue);
	printf("Media tempo di servizio dei Job = %f\n\n",mean_system);
	printf("NOTA: il Prezzo per tenere ogni secondo un server Up è = 10€.\n");
	printf("TOTALE SPESO = %f€.\n", Tot);
	exit(1);

}

/*Funzione princilale*/
int main()
{
	pthread_t thread1, thread2, thread3, thread4, thread5;


	initializate(list_elements,DIM);
	signal(SIGINT,signal_handler);	
	/* Create independent threads each of which will execute function */

	pthread_create( &thread1, NULL, print_message_function, NULL);
	pthread_create( &thread2, NULL, print_message_function, NULL);
	pthread_create( &thread4, NULL, print_message_function, NULL);
	pthread_create( &thread5, NULL, print_message_function, NULL);
	pthread_create( &thread3, NULL, inserisci, NULL);

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
	
	gettimeofday(&START,NULL);
	
	pthread_join( thread3, NULL);
	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL);
	
	print_results();
	exit(0);
}
/*Thread per inserimento dei Job, Thread3*/
void *inserisci()
{	
	float difference_time;
	double lambda = Random()*8;
	double time_next_arrival;
	struct timeval start_time;
	struct timeval internal_t;
	struct timeval internal_t1;
	gettimeofday(&internal_t1,NULL);
	gettimeofday(&start_time,NULL);
	while( calculate_dif_time(START,start_time) < 60.0)
	{	
		gettimeofday(&internal_t,NULL);
		difference_time = calculate_dif_time(internal_t1,internal_t);	
		if(difference_time < 5.0)
		{
			time_next_arrival = Getarrival(lambda);
			printf("     %f\n",time_next_arrival);
		}
		else
		{
			lambda = Random()*8;
			internal_t1 = internal_t;
			time_next_arrival = Getarrival(lambda);
			printf("    RESET %f\n",time_next_arrival);
		}
		printf("Sono qui\n");
		pthread_mutex_lock(&lock);
		push(list_elements,DIM);
		printf("Ho inserito un elemento %d\n",inseriti+1);
		pthread_mutex_unlock(&lock);
		inseriti++;
		sleep((double)time_next_arrival);
		gettimeofday(&start_time,NULL);
		//printf("sono sveglio\n");
	}
}

/*Thread STATICI*/
void *print_message_function()
{
	float service_time;
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
queue:	while(calculate_dif_time(START,start_time) < 60.0  || isEmpty(list_elements)==0)
	{	

		if(isEmpty(list_elements)!=0){
			//printf("Coda vuota, sono il %s\n",message);
			gettimeofday(&start_time,NULL);
			goto queue;
		}
		else
		{	
			pthread_mutex_lock(&lock);
			element current_element = pull(list_elements,DIM);
			printf("Sono il Thread e ho preso l'elemento %d\n",current_element.id);
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
