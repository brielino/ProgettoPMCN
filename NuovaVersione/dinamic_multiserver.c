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
#define START_SERVER 4
#define MAX_SERVER 10
#define LIM_MAX_QUEUE 10
#define LIM_MIN_QUEUE 5
/*DICHIARAZIONE DELLE VARIABILI*/
int n_server = START_SERVER;//numero di server di partenza
bool busy[START_SERVER];//Array per verificare se i server sono occupari (false/liberi true/occupati)
double server[START_SERVER][2];//Array dei tempi di arrivo e servizio dei singoli server
int busy_dinamic[MAX_SERVER - START_SERVER];//Array per verificare se i server dinamici sono attivi o meno (0/liberi 1/occupati 2/non disponibili
double dinamic_server[MAX_SERVER - START_SERVER][2];//Array dei tempi di arrivo e servizio dei singoli server dinamici
double time_in_queue = 0.0;
double time_in_service = 0.0;
int n_queue = 0;
double next_arrive = 0.0;
int n_arrive = 0;
int max_in_queue=0;
double element_queue[DIM];
double u = 1/5.0;
double lambda = 1/10.0;

/*DICHIARAZIONE DELLE FUNZIONI*/
double Getservice(double x);
void Getarrival(double x);
void server_full();
void verify();
void check();
void print_results();
void up_down();

void inizialize() //inizializziamo le variabili
{
	for(int i=0;i<n_server;i++){
		busy[i] = false;
		server[i][0]= 0.0;
		server[i][1]= 0.0;
	}
	for(int i=0;i<DIM;i++){
		element_queue[i] = 0.0;
	}
	for(int i=0;i<(MAX_SERVER - START_SERVER);i++){
		busy_dinamic[i] = 2;
		dinamic_server[i][0]= 0.0;
		dinamic_server[i][1]= 0.0;
	}
	
	
}

int main(){
	inizialize();
	while(next_arrive < START){
		Getarrival(lambda);//Passo al prossimo Job
		n_arrive++;
		int serv = 0; //indica se il job è stato servito o meno, 0 no 1 si

		for(int i=0;i<n_server;i++){
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
			up_down();
		}else{
			goto no_dinamic;
		}
		for(int j=0;j< (MAX_SERVER - START_SERVER);j++){
			if(busy_dinamic[j] != 2){
				//Controllo per settare a liberi i server che con il nuovo arrivo si sono liberati
				if(busy_dinamic[j] == 1 && (dinamic_server[j][0] + dinamic_server[j][1]) < next_arrive){	
					busy_dinamic[j] = 0;
					if(serv == 0 ){//Per evitare di assegnare uno stesso Job 
						busy_dinamic[j] = 1;
						dinamic_server[j][0] = next_arrive;
						dinamic_server[j][1] = Getservice(u);
						serv = 1;
					}			
				}else if(busy_dinamic[j] == 0){//Per prendere in considerazione i Server liberi
					if(serv == 0 ){
						busy_dinamic[j] = 1;
						dinamic_server[j][0] = next_arrive;
						dinamic_server[j][1] = Getservice(u);
						serv = 1;
					}	
				}
			}

		}
		if(serv != 1){
			//se tutti i server sono pieni
			server_full();
		}
no_dinamic:
		check();//decremento coda
		verify();
		
		
	}
	printf("%d elementi in coda alla fine prima di smaltirla\n",n_queue);
	while(n_queue!=0){
		next_arrive += next_arrive + 500000000.0;
		check();
		printf("%d elementi in coda\n",n_queue);
	}
	print_results();
	printf("%d elementi in coda alla fine\n",n_queue);
	printf("Il numero di elementi massimo in coda è: %d\n",max_in_queue);
	printf("Tempo fine simulazione %f\n",next_arrive);
	return 0;
}

void up_down()
{
	if(n_queue > LIM_MAX_QUEUE ){
		for(int i=0;i< (MAX_SERVER - START_SERVER);i++){
			if(busy_dinamic[i] == 2){
				busy_dinamic[i] = 0;
				break;
			}
		}
	}
	if(n_queue < LIM_MIN_QUEUE ){
		for(int i=0;i< (MAX_SERVER - START_SERVER);i++){
			if(busy_dinamic[i] == 0){
				busy_dinamic[i] = 2;
				break;
			}
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
		for(int i=0;i<n_server;i++){
			if(busy[i] == false){

				printf("#Server = %d\n", n_server);
				printf("ERRORE di progettazione\n");
				printf("Next arrive ==%f\n",next_arrive);
				printf("%f è quando ha preso il job\n",server[i][0]);
				printf("%f è quando si libererà\n",server[i][1]+server[i][0]);
				printf("%d elementi in coda\n\n\n",n_queue);
		
			}
		}
		for(int i=0;i< (MAX_SERVER - START_SERVER);i++){
			if(busy_dinamic[i] == 0 ){
				printf("ERRORE di progettazione\n");
				printf("Next arrive ==%f\n",next_arrive);
				printf("%f è quando ha preso il job\n",dinamic_server[i][0]);
				printf("%f è quando si libererà\n",dinamic_server[i][1]+dinamic_server[i][0]);
				printf("%d elementi in coda\n\n\n",n_queue);
	
			}
		}
	}
}

void server_full(){
	n_queue ++;
	double min_time_s = 99999.0;
	double min_time_d = 99999.0;
	int current_s = 0;
	int current_d = 0;
	for(int y=0;y<n_server;y++){ //selezione del server che si libererà per primo per servire il Job		
 		if((server[y][0] + server[y][1] - next_arrive) < min_time_s){
			current_s = y;
			min_time_s = ((server[y][0] + server[y][1]) - next_arrive);
		}			
	}
	for(int j=0;j< (MAX_SERVER - START_SERVER);j++){ //selezione del server che si libererà per primo per servire il Job		
 		if((dinamic_server[j][0] + dinamic_server[j][1] - next_arrive) < min_time_d && busy_dinamic[j] !=2){
			current_d = j;
			min_time_d = ((dinamic_server[j][0] + dinamic_server[j][1]) - next_arrive);
		}			
	}
	
	if(min_time_d > min_time_s){
		time_in_queue += server[current_s][0] + server[current_s][1] - next_arrive;
		server[current_s][0] = server[current_s][0] + server[current_s][1];
		server[current_s][1] = Getservice(u);
		for(int i=0;i<DIM;i++){
			if(element_queue[i] == 0.0){
				element_queue[i] = server[current_s][0] + server[current_s][1];
				break;
			}
		}
	}else{
		time_in_queue += dinamic_server[current_d][0] + dinamic_server[current_d][1] - next_arrive;
		dinamic_server[current_d][0] = dinamic_server[current_d][0] + dinamic_server[current_d][1];
		dinamic_server[current_d][1] = Getservice(u);
		for(int i=0;i<DIM;i++){
			if(element_queue[i] == 0.0){
				element_queue[i] = dinamic_server[current_d][0] + dinamic_server[current_d][1];
				break;
			}
		}
	}	
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
