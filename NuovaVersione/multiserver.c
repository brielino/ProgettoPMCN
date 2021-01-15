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
bool busy[N_SERVER];//Array per verificare se i server sono occupari (false/liberi true/occupati)
double server[N_SERVER][2];//Array dei tempi di arrivo e servizio dei singoli server
double time_in_queue = 0.0;
double time_in_service = 0.0;
int n_queue = 0;
double next_arrive = 0.0;
double Getservice(double x);
void server_full();
void Getarrival(double x);
int n_arrive = 0;
void verify();
void check();
void print_results();
int max_in_queue=0;
double element_queue[DIM];
double u = 1/5.0;
double lambda = 1/49.0;
void inizialize()
{
	for(int i=0;i<N_SERVER;i++){
		busy[i] = false;
		server[i][0]= 0.0;
		server[i][1]= 0.0;
	}
	for(int i=0;i<DIM;i++){
		element_queue[i] = 0.0;
	}
}



int main(){
	
	inizialize();
	while(next_arrive < START){
		Getarrival(1/49.0);//Passo al prossimo Job
		n_arrive++;
		int serv = 0; //indica se il job è stato servito o meno, 0 no 1 si
		for(int i=0;i<N_SERVER;i++){
			//Controllo per settare a liberi i server che con il nuovo arrivo si sono liberati
			if(busy[i] == true && (server[i][0] + server[i][1]) < next_arrive){	
				busy[i] = false;
				if(serv == 0 ){//Per evitare di assegnare uno stesso Job 
					busy[i] = true;
					server[i][0] = next_arrive;
					server[i][1] = Getservice(u);
					serv = 1;
				}			
			}else if(busy[i] == false){//Per prendere in considerazione i Server liberi
				if(serv == 0 ){
					busy[i] = true;
					server[i][0] = next_arrive;
					server[i][1] = Getservice(u);
					serv = 1;
				}	
			}
		}
		if(serv != 1){
			//se tutti i server sono pieni
			server_full();
		}
		check();//decremento coda
		verify();
	
	}
	printf("%d elementi in coda alla fine prima di smaltirla\n",n_queue);
	while(n_queue!=0){
		Getarrival(1/2.0);
		check();
	}
	print_results();
	printf("%d elementi in coda alla fine\n",n_queue);
	printf("Il numero di elementi massimo in coda è: %d\n",max_in_queue);
	printf("Tempo fine simulazione %f\n",next_arrive);
	return 0;
}

void print_results()
{
	printf("\n\n************************\n");
	printf("FINE SIMULAZIONE - Dati:\n");
	printf("************************\n");

	
	double mean_queue = time_in_queue / n_arrive;
	printf("Media tempo in coda dei Job = %f\n\n",mean_queue);

	
}

void check(){
	if(n_queue > max_in_queue){
		max_in_queue = n_queue;
	}
	for(int i=0;i<DIM;i++){
			if(element_queue[i] <= next_arrive && n_queue !=0){
				n_queue--;
				element_queue[i] = 0.0;	
			}
		}
}

void verify(){
	if(n_queue != 0){
		for(int i=0;i<N_SERVER;i++){
			if(busy[i] == false){
				printf("ERRORE di progettazione\n");
				printf("Next arrive ==%f\n",next_arrive);
				printf("%f è quando ha preso il job\n",server[i][0]);
				printf("%f è quando si libererà\n",server[i][1]+server[i][0]);
				printf("%d elementi in coda\n",n_queue);	
			}
		}
	}
}

void server_full(){
	n_queue ++;
	double min_time = 99999.0;
	int current_s;
	for(int y=0;y<N_SERVER;y++){		
 		if((server[y][0] + server[y][1] - next_arrive) < min_time){
			current_s = y;
			min_time = ((server[y][0] + server[y][1]) - next_arrive);
		}			
	}
	for(int i=0;i<DIM;i++){
		if(element_queue[i] == 0.0){
			element_queue[i] = server[current_s][0] + server[current_s][1];
		}
	}
	time_in_queue += server[current_s][0] + server[current_s][1] - next_arrive;
	server[current_s][0] = server[current_s][0] + server[current_s][1];
	server[current_s][1] = Getservice(u);
	
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
