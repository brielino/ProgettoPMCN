#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "rvms.h"

double calculate_deviation(double *array,int batch);
double calculate_mean(double *array,int batch);
double calculate_interval_values(double *array,int batch);

double calculate_deviation(double *array,int batch){
	double sum=0.0;
    	double sum_2= 0.0;
    	for(int i = 0; i <batch; i++){
        	sum+=array[i];
	}
	sum = sum/batch;
	double dev = 0.0;
	for(int j = 0; j < batch; j++){
		dev = dev + (array[j] - sum)*(array[j] - sum);
	}
	return sqrt(dev/(batch-1));

}

double calculate_mean(double *array,int batch){
	double mean=0.0;
    	for(int i = 0; i <batch; i++){
        	mean+=array[i];
	}
	return (double)mean/(double)batch;

}

double calculate_interval_values(double *array,int batch){
	double alfa = 0.05;
	double t = idfStudent(batch-1, 1-(alfa/2));
	double mean = calculate_mean(array,batch);
	double deviation = calculate_deviation(array,batch);
	double delta = (t*deviation)/sqrt(batch-1);
	printf("%f t %f deviazione %f radice\n",t,deviation,sqrt(batch-1)); 
	printf("%f -/+ %f ---> Intervallo di Confidenza ( %f , %f )\n",mean, delta,mean+delta,mean-delta);
	
}
