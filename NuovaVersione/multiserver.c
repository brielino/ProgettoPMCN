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
bool busy[4] = {false,false,false,false};
double server[4][2] = {{0.0,0.0},{0.0,0.0},{0.0,0.0},{0.0,0.0}};
double time_in_queue = 0.0;
double time_in_service = 0.0;
int n_coda = 0;
double next_arrive = 0.0;


void main(){
	double prec_arrive = 0.0 indica se precedentemente ci sono stati server occupati
next:	Getarrival(1/10.0);
	while(next_arrive < START){
		for(int i=0;i<4;i++){
			int n_server_free = 0;
			if(busy[i] == true){
				if( (server[i][0] + server[i][1]) < next_arrive) ){	
					busy[i] = false;
					n_server_free ++;
				}			
			}else{
				busy[i] = true;
				server[i][0] = next_arrive;
				server[i][1] = Getservice(1/2.0);
				goto next;
			}
		}
		if(
		double max_time = 0.0;
		double min_time = -99999.0;
		int current_s;
		int current_sl;
		int n_server_later = 0;
		for(int y=0;y<4;y++){
			if((next_arrive -(server[y][0] + server[y][1])) > 0){
 				if(next_arrive -(server[y][0] + server[y][1])) > max_time){
					current_s = y;
					max_time = (next_arrive -(server[y][0] + server[y][1]));
				}
			}else if( (next_arrive -(server[y][0] + server[y][1])) > min_time){
				current_sl = y;
				min_time = next_arrive -(server[y][0] + server[y][1]));				
				n_server_later++;				
			}			
		}
		if(n_server_later = 0){
			server[current_s][0] = next_arrive;
			server[current_s][1] = Getservice(1/2.0);
			Getarrival(1/10.0);
			for(int j=0;j<4;j++){
				if(current_s != j && (next_arrive -(server[j][0] + server[j][1])) > 0 ){
					busy[j] = false;
				}
			}			
		}else{
			time_in_queue -= (next_arrive -(server[current_sl][0] + server[current_sl][1])));
			server[current_sl][0] = next_arrive - (next_arrive -(server[current_sl][0] + server[current_sl][1])));
			server[current_sl][1] = Getservice(1/2.0);
			Getarrival(1/10.0);
			for(int j=0;j<4;j++){
				if(current_sl != j && (next_arrive -(server[j][0] + server[j][1])) > 0 ){
					busy[j] = false;
				}
			}
		}
		goto next;
	}

}


double Getservice(double x)
{
	SelectStream(1);
	return Exponential(x);
}
double Getarrival(double x)
{
	SelectStream(1);
	next_arrive += Exponential(x);
	return arrival;
}
