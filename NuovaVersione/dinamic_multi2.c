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
double lambda = 1/20.0;
int processati = 0;

int ciclo_w; //Per definire quante volte entra nel while (prove-su-prove)

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
}

int main(){
	
	inizialize();
	while(next_arrive < START){
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
			ciclo_w += 1;
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
						s--;
						/*printf("\nnext_arrive = %f\n", next_arrive);
						printf("CODA VUOTA, SERVER LIBERI - ARRIVO NUMERO %d\n", n_arrive);
						printf("Server%d = %d\n", i, busy[i]);	//Per verificare lo stato dei server
						printf("Tempo in cui si libererà il Server = %f\n\n", T);
						//sleep(2);*/
						goto exit_while;					
					}
				}
				else{
					double element = pull();
					if(not_insert_again){ //per evitare che next_arrive sia inserito in coda + di una volta
						insert_in_queue(next_arrive);
						not_insert_again = false;
					}			
					/******************************************
					printf("\n\n\nARRIVO NUMERO %d, valore = %f\n", n_arrive,  next_arrive);
					printf("Elementi in coda:\n");
					for(int q=0;q<DIM;q++){
						if(element_queue[q] != 0.0)
						printf(" %f\n", element_queue[q]);
					}
					printf("#elementi in coda = %d\n", n_queue);
					printf("Elemento appena estratto dalla coda = %f\n", element);

					for(int z=0; z<N_SERVER; z++){
						printf("Server%d = %d\n", z, busy[z]);	//Per verificare lo stato dei server
					}
					******************************************/

					double min_time= 99999.0;
					int current_s;
					for(int i=0;i<N_SERVER;i++){
						if((server[i][0] + server[i][1]) - element < min_time && busy[i] == 0){
							min_time = (server[i][0] + server[i][1]) - element;
							current_s = i;
							if(min_time <0){	
								/*printf("ERRORE\n");
								printf("i = %d\n", i);
								printf("meantime = %f\n", min_time);
								printf(" %f\n", element);
								printf("%d\n", n_arrive);								
								return 0;*/
							}
						}
					}
					if(min_time <0){
						server[current_s][0] = element;
						server[current_s][1] = Getservice(u, current_s);
						s--;
						busy[current_s] = 1;
						/*printf(" min_time<0 - PRESO DA server%d\n", current_s);
						printf("Aggiornamento stato: Server%d=%d\n", current_s, busy[current_s]);
						//sleep(3);*/
						break;
					}
					else{
						time_in_queue += min_time;
						server[current_s][0] = element+min_time;
						server[current_s][1] = Getservice(u, current_s);
						s--;
						busy[current_s] = 1;
						/*printf("PRESO DA server%d\n", current_s);
						printf("Aggiornamento stato: Server%d=%d\n", current_s, busy[current_s]);
						//sleep(3);*/
						break;
					}
				}
			}
		}
		if(in_while==0){
			insert_in_queue(next_arrive);
			//printf("\n\nSERVER PIENI - ARRIVO NUMERO %d, valore = %f\n", n_arrive, next_arrive);	
			//sleep(2);
		}		
	exit_while:	verify();
    }
	printf("%d elementi in coda alla fine prima di smaltirla\n",n_queue);
	while(n_queue != 0){
		Getarrival(1/2.0);
		up_down();
		n_arrive--;	
		free_server();
		double element = pull();			
		double min_time= 99999.0;
		int current_s = -1;	

		for(int i=0;i<N_SERVER;i++){
			if((server[i][0] + server[i][1]) - element < min_time && busy[i] == 0){
				min_time = (server[i][0] + server[i][1]) - element;
				current_s = i;
			}
		}
		if(min_time <0 ){
			server[current_s][0] = element;
			server[current_s][1] = Getservice(u, current_s);
			busy[current_s] = 1;			
		}
		else if(current_s != -1){
			time_in_queue += min_time;
			server[current_s][0] = element+min_time;
			server[current_s][1] = Getservice(u, current_s);
			busy[current_s] = 1;			
		}
	}
	printf("NO-WHILE");	
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
	printf("INSERITI : %d\n", n_arrive);
	printf("PROCESSATI : %d\n", processati);
	printf("Tempo di servizio TOTALE = %f\n", time_in_service + 4*next_arrive);
	printf("Tempo di servizio DINAMICO = %f\n", time_in_service);	
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

double Getservice(double x, int server)
{
	SelectStream(1+server);
    double service_time = Exponential(x);
	return service_time;
}

void Getarrival(double x)
{
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
	
void up_down()
{
	if(n_queue > LIM_MAX_QUEUE ){
		for(int i=STATIC_SERVER;i < N_SERVER;i++){
			if(busy[i] == 2){
				busy[i] = 0;
				server[i][0] = next_arrive;
				server[i][2] = next_arrive; 
				break;
			}
		}
	}
	else if(n_queue < LIM_MIN_QUEUE ){ 
		for(int j=STATIC_SERVER;j < N_SERVER;j++){
			if(busy[j] == 0){
				busy[j] = 2;
				server[j][1] = 0.0;
				time_in_service += next_arrive - server[j][2]; 
			}
		}		
	}
}
