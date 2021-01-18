#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"rngs.h"
#include"rvgs.h"
#include"rvms.h"
#include<math.h>
#include<stdio.h>
#include<stdbool.h>

#define START 10800
#define DIM 5000
#define N_SERVER 10
#define STATIC_SERVER 4
#define LIM_MAX_QUEUE 10
#define LIM_MIN_QUEUE 5

int busy[N_SERVER];//Array per verificare se i server sono occupari (0/liberi - 1/occupato - 2/ non attivo)
double server[N_SERVER][2];//Array dei tempi di arrivo e servizio dei singoli server
double time_in_queue = 0.0;
double time_in_service = 0.0;
int n_queue = 0;
double next_arrive = 0.0;
double Getservice(double x);
void Getarrival(double x);
int n_arrive = 0;
void verify();
void check();
void print_results();
int max_in_queue=0;
double element_queue[DIM];
double u = 1/10.0;
double lambda = 1/20.0;
int processati = 0;
int bohhh();
void up_down();
double pull();
void insert_in_queue(double time_arrive);
void free_server();

void inizialize()
{
	for(int i=0;i<N_SERVER;i++){
        if(i < STATIC_SERVER){ //Numero server statici (4)
		    busy[i] = 0;
		    server[i][0]= 0.0;
		    server[i][1]= 0.0;
        }
        else{ 
            busy[i] = 2;
		    server[i][0]= 0.0;
		    server[i][1]= 0.0; 
        }    
    }
	for(int i=0;i<DIM;i++){
		element_queue[i] = 0.0;
	}
}

int main(){
	
	inizialize();
	while(next_arrive < START){
continua:	Getarrival(lambda);//Passo al prossimo Job
		n_arrive++;      	
		up_down();
		double element_in_q;
		int s = 0;//numero server liberi
		free_server();
		for(int i=0;i<N_SERVER;i++){
			if(busy[i] == 0){
				s=s+1;
			}
		}		
		int in_while = 0;		
		while(s > 0){
			insert_in_queue(next_arrive);
			double min_time= 99999.0;
			element_in_q=pull();
			int current_s;
			in_while = 1;
			int delay = 0;
			if(element_in_q == 0.0){
				goto continua;
			}
			for(int i=0;i<N_SERVER;i++){
				if(element_in_q == next_arrive){
					if(busy[i] == 0){
						busy[i] = 1;
						server[i][0] = element_in_q;
						server[i][1] = Getservice(u);
						processati++;
						goto decremento;					
					}
				}else if((server[i][0] + server[i][1])- element_in_q < min_time && busy[i] == 0){
					min_time = (server[i][0] + server[i][1]) - element_in_q;
					delay = 1;
					current_s = i;
				}
			}
			if(delay ==1){
				printf("%f next_arrive\n",next_arrive);
				time_in_queue += min_time;
				busy[current_s] = 1;
				server[current_s][0] = element_in_q + min_time;
				server[current_s][1] = Getservice(u);
				processati++;
			}
decremento:			s--;
			
		}
		if(in_while==0){
			insert_in_queue(next_arrive);
		}		
		verify();
    	}
	printf("%d elementi in coda alla fine prima di smaltirla\n",n_queue);
	double final_time = 0.0;
    	for(int u=0; u<N_SERVER; u++){
        	if((server[u][0] + server[u][1]) > final_time){
            		final_time = server[u][0] + server[u][1]; 
        	}
	}
    	next_arrive = final_time;
	print_results();
	printf("%d elementi in coda alla fine\n",n_queue);
	printf("Il numero di elementi massimo in coda è: %d\n",max_in_queue);
	printf("Tempo fine simulazione %f\n",next_arrive);
	return 0;
}
void free_server(){
	for(int i=0;i<N_SERVER;i++){
			//Controllo per settare a liberi i server che con il nuovo arrivo si sono liberati
			if(busy[i] == 1 && (server[i][0] + server[i][1]) < next_arrive){	
				busy[i] = 0;

			}
	}	
}
void print_results()
{
	printf("\n\n************************\n");
	printf("FINE SIMULAZIONE - Dati:\n");
	printf("************************\n");

	
	double mean_queue = time_in_queue / processati;
	printf("Media tempo in coda dei Job = %f\n\n",mean_queue);
	printf("PROCESSATI : %d\n", processati);
	printf("\nINSERITI : %d\n", n_arrive);

	
}

void check(){
	if(n_queue > max_in_queue){
		max_in_queue = n_queue;
	}
	for(int i=0;i<DIM;i++){
		if(element_queue[i] <= next_arrive && n_queue !=0 && element_queue[i] != 0.0){
			n_queue--;
			element_queue[i] = 0.0;	
		}
	}
}

void verify(){
	if(n_queue != 0){
		for(int i=0;i<N_SERVER;i++){
			if(busy[i] == 0){
                		//sleep(5);
				printf("ERRORE di progettazione\n");
				printf("Next arrive ==%f\n",next_arrive);
				printf("%f è quando ha preso il job\n",server[i][0]);
				printf("%f è quando si libererà\n",server[i][1]+server[i][0]);
				printf("%d elementi in coda\n",n_queue);
                		for(int i=0;i<N_SERVER;i++){
                    			printf("busy[%d] = %d\n\n", i, busy[i]);
                		}	
			}
		}
	}
}

int bohhh(){
	int num_server_no_busy = 0;
	for(int i=0;i<N_SERVER;i++){
		if(busy[i] == 0){
			num_server_no_busy++;
		}
	}
	return num_server_no_busy;	
}


double Getservice(double x)
{
	SelectStream(1);
	return Exponential(x);
}

void Getarrival(double x)
{
	SelectStream(1);
	next_arrive += Exponential(x);
}

void insert_in_queue(double time_arrive){
	for(int i=0;i<DIM;i++){
		if(element_queue[i] == 0.0){
			element_queue[i] = time_arrive;
            		break;
		}
	}
	n_queue ++;
}

double pull(){
	double elem = element_queue[0];
	for(int i=1;i<DIM;i++){
		if(element_queue[i-1]!=0.0){
			element_queue[i-1]=element_queue[i];
		}
		if(i==DIM-1){
			element_queue[i] = 0.0;
		}
	}
	n_queue--;
	return elem;
}
	
void up_down()
{
	if(n_queue > LIM_MAX_QUEUE ){
		for(int i=STATIC_SERVER;i < N_SERVER;i++){
			if(busy[i] == 2){
				busy[i] = 0;
				server[i][0] = next_arrive;
				break;
			}
		}
	}
	else if(n_queue < LIM_MIN_QUEUE ){ 
		for(int j=STATIC_SERVER;j < N_SERVER;j++){
			if(busy[j] == 0){
				busy[j] = 2;
				server[j][1] = 0.0;
				break;
			}
		}		
	}
}
