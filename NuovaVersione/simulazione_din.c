#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"rngs.h"
#include"rvgs.h"
#include"rvms.h"
#include"batch.h"
#include<math.h>
#include<stdio.h>
#include<stdbool.h>

#define START 10800
#define DIM 5000
#define N_SERVER 10
#define STATIC_SERVER 4
#define LIM_MAX_QUEUE 15
#define LIM_MIN_QUEUE 5
#define START_JOB 2000
#define N_SIMULATION 100
//DICHIARAZIONE DI FUNZIONI
double Getservice(double x, int server);
double pull();
void Getarrival(double x);
void verify();
void check();
void print_results();
void up_down();
void insert_in_queue(double time_arrive);
void free_server();
//DICHIARAZIONE DI VARIABILI
int busy[N_SERVER];//Array per verificare se i server sono occupari (0/liberi - 1/occupato - 2/ non attivo)
double server[N_SERVER][3];//Array dei tempi di arrivo e servizio dei singoli server
double time_in_queue = 0.0;
double time_in_service = 0.0;
int n_queue = 0; //# elementi in coda
double next_arrive = 0.0;
int n_arrive = 0; //# elementi inseriti
int max_in_queue=0;
double element_queue[DIM]; //Coda
double u = 1/5.0;
double lambda = 1/45.0;
int processati = 0;
double response_time = 0.0;

int simulazione = 0;
int seme = 89;

//array per memorizzare valori tempo medio risposta coda ogni x job
double m_coda[N_SIMULATION];
double m_risposta[N_SIMULATION];
void inizialize()
{
	for(int i=0;i<N_SERVER;i++){
		if(i < STATIC_SERVER){ //Numero server statici (4)
		   	busy[i] = 0;
		   	server[i][0]= 0.0;
		   	server[i][1]= 0.0;
			server[i][2]= 0.0;	
		}
       	else{ 
           	busy[i] = 2;
			server[i][0]= 0.0;
			server[i][1]= 0.0;
			server[i][2]= 0.0; 
		}    
   	}
	for(int i=0;i<DIM;i++){
		element_queue[i] = 0.0;
	}
    processati = 0;
	time_in_queue = 0.0;
	response_time = 0.0; //Tempo medio di risposta
	n_queue = 0; //# elementi in coda
	next_arrive = 0.0;
	n_arrive = 0; //# elementi inseriti
	max_in_queue=0;
}

int main(){
    for(int i=0;i<N_SIMULATION;i++){
		m_coda[i] = 0.0;  
   	}
	for(int i=0;i<N_SIMULATION;i++){
		m_risposta[i] = 0.0;  
   	}
inizio: seme+=11;
	PlantSeeds(seme);
	inizialize();
	while(simulazione < N_SIMULATION){
		Getarrival(lambda);//Passo al prossimo Job      		
		up_down(); //Controllo per istanziare nuovi Server
		free_server();//Libero i Server. 
		int s = 0;//numero server liberi
		for(int i=0;i<N_SERVER;i++){
			if(busy[i] == 0){
				s+=1;
			}
		}		
		int in_while = 0;
		bool not_insert_again = true;		
		int s_before = s;
		while(s > 0){
			in_while = 1;
			if(s_before != s && n_queue == 0){
				break;
			}			
			for(int i=0;i<N_SERVER;i++){
				if(n_queue == 0){
					if(busy[i] == 0){
						busy[i] = 1;
						server[i][0] = next_arrive;
						server[i][1] = Getservice(u, i);
						processati++;
                        if(processati%START_JOB == 0){
							//printf("%d; %f; %f\n\n", seme, time_in_queue/processati, (response_time+time_in_queue)/processati);
							m_coda[simulazione] = time_in_queue/processati;
							m_risposta[simulazione] = (response_time+time_in_queue)/processati; 
							simulazione++;
							goto inizio;
                        }
						s--;
						goto exit_while;					
					}
				}
				else{
					double element = pull();
					if(not_insert_again){ //per evitare che next_arrive sia inserito in coda + di una volta
						insert_in_queue(next_arrive);
						not_insert_again = false;
					}			
					double min_time= 99999.0;
					int current_s;
					for(int i=0;i<N_SERVER;i++){
						if((server[i][0] + server[i][1]) - element < min_time && busy[i] == 0){
							min_time = (server[i][0] + server[i][1]) - element;
							current_s = i;
						}
					}
					if(min_time <0){
						server[current_s][0] = element;
						server[current_s][1] = Getservice(u, current_s);
						s--;
						busy[current_s] = 1;
                        if(processati%START_JOB == 0){
							//printf("%d; %f; %f\n\n", seme, time_in_queue/processati, (response_time+time_in_queue)/processati);
							m_coda[simulazione] = time_in_queue/processati;
							m_risposta[simulazione] = (response_time+time_in_queue)/processati; 
							simulazione++;
							goto inizio;
                        }
						break;
					}
					else{
						time_in_queue += min_time;
						server[current_s][0] = element+min_time;
						server[current_s][1] = Getservice(u, current_s);
						s--;
						busy[current_s] = 1;
                        if(processati%START_JOB == 0){
							//printf("%d; %f; %f\n\n", seme, time_in_queue/processati, (response_time+time_in_queue)/processati);
							m_coda[simulazione] = time_in_queue/processati;
							m_risposta[simulazione] = (response_time+time_in_queue)/processati; 
							simulazione++;
							goto inizio;
                        }
						break;
					}
				}
			}
		}
		if(in_while==0){
			insert_in_queue(next_arrive);
		}		
exit_while:	verify();
	}
    calculate_interval_values(m_coda, N_SIMULATION);
	calculate_interval_values(m_risposta, N_SIMULATION);
	return 0;
}

void free_server(){ //Probabilmente da modificare 
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
	double mean_queue = time_in_queue / n_arrive;
	printf("Media tempo in coda dei Job = %f\n\n",mean_queue);
	printf("Media tempo di servizio = %f\n", (response_time + time_in_queue) /n_arrive);	
	printf("INSERITI : %d\n", n_arrive);
	printf("PROCESSATI : %d\n", processati);
	printf("Tempo di servizio TOTALE = %f\n", time_in_service + 4*next_arrive);
	printf("Tempo di servizio DINAMICO = %f\n", time_in_service);	
}

void verify(){
	if(n_queue != 0){
		for(int i=0;i<N_SERVER;i++){
			if(busy[i] == 0){
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

double Getservice(double x, int server){
	SelectStream(1+server);
	double service_time = Exponential(x);
	response_time += service_time;
	return service_time;
}

void Getarrival(double x){
	n_arrive++;
	SelectStream(0);
	next_arrive += Exponential(x);
}

void insert_in_queue(double time_arrive){
	for(int i=0;i<DIM;i++){
		if(element_queue[i] == 0.0){
			element_queue[i] = time_arrive;
            break;
		}
	}
	n_queue++;
	if(n_queue > max_in_queue){
		max_in_queue = n_queue;
	}
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
	n_queue--; //se scriviamo n_queue-1 dice l'espressione risulta non usata 
	processati++;
	return elem;
}
	
void up_down(){
	if(n_queue >= LIM_MAX_QUEUE ){
		for(int i=STATIC_SERVER;i < N_SERVER;i++){
			if(busy[i] == 2){
				busy[i] = 0;
				server[i][0] = next_arrive;
				server[i][2] = next_arrive; 
				break;
			}
		}
	}
	else if(n_queue <= LIM_MIN_QUEUE ){ 
		for(int j=STATIC_SERVER;j < N_SERVER;j++){
			if(busy[j] == 0){
				busy[j] = 2;
				server[j][1] = 0.0;
				time_in_service += next_arrive - server[j][2];
			}
		}		
	}
}
