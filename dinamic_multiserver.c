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
#include<time.h>

#define DIM 250

double Time1 = 0;
double Time2 = 0;
int T_id1 = 0;
int T_id2 = 0;

typedef struct
{
	pthread_t id;
	int attive;
	//attive indica se il thread è attivo o no (1/si 0/no)

}dinamic_threads;

void *print_message_function();
void *print_message_function2();
float calculate_dif_time(struct timeval time1,struct timeval time2);
void *inserisci();
pthread_mutex_t lock;
pthread_mutex_t lock2;

element list_elements[DIM];
double Getarrival(double x);
static double arrival = 0.0;
void print_results( void);
int inseriti=0;
int processati=0;
struct timeval START;
float Tot_queue = 0.0;
float Tot_service = 0.0;

dinamic_threads list_threads[3];
pthread_t thread4, thread5, thread6;

/*Funzione chiamata quando si tenta di uscire dal programma premendo CTRL+C*/
void signal_handler()
{
	print_results();
}

/*Funzione che stampa tutti i risultati ottenuti*/
void print_results()
{
	printf("\n\n************************\n");
	printf("FINE SIMULAZIONE - Dati:\n");
	printf("************************\n");

	printf("\n\nSono stati inseriti %d Job\n\n",inseriti);
	printf("Sono stati processati %d Job\n\n",processati);
	
	struct timeval now;
	gettimeofday(&now, NULL);

	printf("Tempo trascorso dall'inizio della simulazione: %f\n\n",calculate_dif_time(START, now));
	float mean_queue = 0.0;
	float mean_system = 0.0;
	mean_queue = Tot_queue / processati;
	mean_system = Tot_service / processati;
	printf("Media tempo in coda dei Job = %f\n\n",mean_queue);
	printf("Media tempo di servizio dei Job = %f\n\n",mean_system);

	double mean_time2 = 0;

	mean_time2 = Time2 / T_id2;

	printf("Tempo di servizio thread1 = %f\n\n",Time1);
	printf("Media tempo di servizio del DINAMICO = %f\n\n",mean_time2);

	int euro = 10;
	float Totale = Time1*euro;
	float Totale2 = (euro*mean_time2)*T_id2;
	float Tot = Totale + Totale2; 

	printf("Server dinamici attivati = %d\n\n", T_id2);
	printf("NOTA: il Prezzo per tenere ogni secondo un server DINAMICO Up è = 10€.\n");
	printf("Di conseguenza abbiamo speso circa %f€ in Server Dinamici\n", Totale2);
	printf("\nAbbiamo speso per thread1 %f€\n\n", Totale);
	printf("TOTALE SPESO = %f€\n", Tot);
	exit(1);
}

/*Funzione per creazione dinamica dei nuovi threads*/
void *dinamic_create()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	while((double) calculate_dif_time(START, now) < 60.0)
	{
		if(element_in_queue(list_elements,DIM)>10)
		{
			for(int i=0;i<3;i++)
			{
				if(list_threads[i].attive == 0)
				{
					list_threads[i].attive = 1;
					pthread_create( &(list_threads[i].id), NULL, print_message_function2,NULL);
					break;
				}
			}
		}
		gettimeofday(&now,NULL);	
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

/*Funzione princilale*/
int main() 
{
	pthread_t thread1, thread2, thread3;
	
	initializate(list_elements,DIM);

	signal(SIGINT,signal_handler);	
	/* Create independent threads each of which will execute function */

	pthread_create( &thread1, NULL, print_message_function, NULL);
	pthread_create( &thread2, NULL, inserisci,NULL);
	pthread_create( &thread3, NULL, dinamic_create,NULL);
	start_t();

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
	
	gettimeofday(&START, NULL);
	
	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL);
	pthread_join( thread3, NULL);
	
	print_results();
	exit(0);
}

/*Thread per inserimento dei Job, Thread 3*/
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

/*Thread STATICO*/
void *print_message_function()
{
	struct timeval start1;
	
	struct timeval end1;

	double service_time;
	float tempo1 = 0;
	gettimeofday(&start1, NULL);

queue:	while((double) calculate_dif_time(START, start1) < 60.0  || isEmpty(list_elements)==0)
	{	
		
		if(isEmpty(list_elements)!=0){
			gettimeofday(&start1,NULL);			
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
	gettimeofday(&end1, NULL);
	tempo1 = calculate_dif_time(START, end1);
	printf("\n\n TEMPO %f\n\n", tempo1);
	
	pthread_mutex_lock(&lock2); 
	Time1 += tempo1;
	pthread_mutex_unlock(&lock2);
	
}

/*Threads DINAMICI*/
void *print_message_function2()
{
	struct timeval start2;
	struct timeval end2;

	double service_time;
	float tempo2 = 0;

	gettimeofday(&start2, NULL);

	pthread_t current_thread = pthread_self();
	while((double) calculate_dif_time(START, start2) < 60.0  || isEmpty(list_elements)==0)
	{	
		
		if(isEmpty(list_elements)!=0)
		{
			goto off;
		}
		else
		{
			pthread_mutex_lock(&lock);
			element current_element = pull(list_elements,DIM);
			printf("DINAMIC Ho preso l'elemento %d %ld\n",current_element.id,current_thread);
			gettimeofday(&current_element.time_exit_queue,NULL);
			float time_in_queue = calculate_dif_time(current_element.time_arrive,current_element.time_exit_queue);
			printf("L'elemento %d è stato in coda %f\n",current_element.id,time_in_queue );
			processati++;
			service_time = Exponential(1.5);
			Tot_service += service_time;
			Tot_queue += time_in_queue;
			pthread_mutex_unlock(&lock);
			sleep(service_time);
			if(element_in_queue(list_elements,DIM)<20){
				for(int i=0;i<3;i++)
				{
					if(list_threads[i].attive == 1)
					{
						if(pthread_equal(current_thread,list_threads[i].id)!=0){
							list_threads[i].attive = 0;
							goto off;
						}
					}
				}
			}
		}
	}
	off: printf("sono uscito porca di quella puttana %ld\n",current_thread);
	gettimeofday(&end2, NULL);
	tempo2 = calculate_dif_time(start2, end2);
	printf("\n\n TEMPO2 %f\n\n", tempo2);
	
	pthread_mutex_lock(&lock2); 
	Time2 += tempo2;
	T_id2++;
	pthread_mutex_unlock(&lock2);	
}

/*Funzione per calcolare il Tasso di Arrivo*/
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
