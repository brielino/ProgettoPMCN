#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<stdio.h>
#include<stdbool.h>

double p_0;
double PQ;
double Enq;
double Ens;
double Etq;
double Ets;
int fattoriale(int n);
void main(){
	
	int lambda ;
	int u;
	int n_server;
	printf("Inserisci il valore di lambda -- task/s\n");
	scanf("%d",&lambda);
	printf("Inserisci il valore di u -- task/s\n");
	scanf("%d",&u);
	printf("Inserisci il numero di server -- \n");
	scanf("%d",&n_server);
	double ro = (double) lambda/(u*n_server);
	printf("%f RO\n",ro);
	double pot2 = pow(n_server*ro,n_server);
	for(int i= 0;i<n_server;i++){
		double pot = pow(n_server*ro,i);	
		pot = pot/fattoriale(i);
		p_0 += pot;	
	}
	pot2 = pot2/(fattoriale(n_server)*(1-ro));
	p_0 += pot2;
	p_0 = pow(p_0,-1);
	printf("%f p_0\n",p_0);
	PQ = pot2*p_0;
	printf("%f PQ\n",PQ);
	
	Enq = PQ*(ro/(1-ro));
	printf("%f E(Nq)\n",Enq);
	Ens = Enq + n_server*ro;
	printf("%f E(Ns)\n",Ens);
	//Con Little
	Etq = Enq/(double) lambda;
	printf("%f E(Tq)\n",Etq);
	Ets = Ens/(double)lambda;
	printf("%f E(Ts)\n",Ets);
}

int fattoriale(int n)
{
  if (n < 0) return -1; /* Fattoriale non e' definito per interi negativi! */

  if (n == 0) return 1;
  else return n*fattoriale(n-1);
}
