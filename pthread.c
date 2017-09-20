#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#include <time.h>

sem_t sem;
pthread_mutex_t mutex[50];
struct to_exchange{
	int isValid;
	double value;
};

struct thread_args {
    int id;
    long long int size;
    double x[];
    double T;
    double tau;
    double cth;
};

struct to_exchange array[50];

double function_g(double x){
	return x * (2 - x);
}

void  some_function(void * args){
	struct thread_args arguments = (struct thread_args) &args;
	int id = arguments.id;
	int i, k;
	double add_value;
	long long int size = arguments.size;
	double u[2][size];
	double T = arguments.T;
	double tau = arguments.tau;
	double cth = arguments.cth;
	double x[size];// = arguments.x;
	for (i = 0; i < size; i++){
		x[i] = arguments.x[i];
		if (x[i] > 0 && x[i] < 2)
			u[0][i] = function_g(x[i]);
		else
			u[0][i] = 0;
	}
	int amount_steps = T / tau;
	for (k = 0; k < amount_steps; k++){
		if (id % 2 == 0){
			while (1){
				pthread_mutex_lock(&mutex[id]);
				if (array[id].isValid == 0){
					array[id].value = u[k % 2][size - 1];
					array[id].isValid = 1;
					break;
				}
				pthread_mutex_unlock(&mutex[id]);
			}
			if (id != 0)
				while (1){
					pthread_mutex_lock(&mutex[id - 1]);
					if (array[id - 1].isValid == 1){
						add_value = array[id - 1].value;
						array[id - 1].isValid = 0;
						break;
					}
					pthread_mutex_unlock(&mutex[id - 1]);
				}
		}
		else{
			while (1){
				pthread_mutex_lock(&mutex[id - 1]);
				if (array[id - 1].isValid == 1){
					add_value = array[id - 1].value;
					array[id - 1].isValid = 0;
					break;
				}
				pthread_mutex_unlock(&mutex[id - 1]);
			}
			
			while (1){
				pthread_mutex_lock(&mutex[id]);
				if (array[id].isValid == 0){
					array[id].value = u[k % 2][size - 1];
					array[id].isValid = 1;
					break;
				}
				pthread_mutex_unlock(&mutex[id]);
			}	
		}
		
		if (id == 0)
			u[(k + 1) % 2][0] = - cth * (u[k % 2][0]) + u[k % 2][0];
		else 
			u[(k + 1) % 2][0] = - cth * (u[k % 2][0] - add_value) + u[k % 2][0];
			
		for (i = 1; i < size; i++)
			u[(k + 1) % 2][i] = - cth * (u[k % 2][i] - u[k % 2][i - 1]) + u[k % 2][i];
				
	}
	pthread_exit((void*) u[(size -1) % 2]);
}


int main(int argc, char ** argv){
	if (argc < 8)
		return 0;
	sem_init(&sem, 0, 1);
	double T = atod(argv[1]);
	double c = atod(argv[2]);
	double h = atod(argv[3]);
	double tau = atod(argv[4]);
	double left_bound = atod(argv[5]);
	double right_bound = atod(argv[6]); 
	int NUM_OF_THREADS = atoi(argv[7]);
	struct timespec begin, end;
	double elapsed;
	int i, j;
	pthread_t threads[NUM_OF_THREADS];
	long long int amount_points = (right_bound - left_bound) / h;
	long long int size = amount_points / NUM_OF_THREADS;
	clock_gettime(CLOCK_REALTIME, &begin);
	for (i = 0; i < NUM_OF_THREADS; i++){
		array[i].isValid = 0;
		array[i].value = 0;
	}
	
	for (i = 0; i < NUM_OF_THREADS; i++){
		struct thread_args args;
		//if (i == 0)
		args.size = size;
		//else 
			//args.size = size + 1;
		args.id = i;
		double x[size];
		if (i != 0)
			x[0] = left_bound + i* size * h;
		else 
			x[0] = left_bound;
		for (j = 1; j < size; j++)
			x[j] = x[j - 1] + h;
		args.x = x;
		args.T = T;
		args.tau = tau;
		args.cth = c * tau / h; 
		pthread_create(&threads[i], NULL, (void*)(some_function), (void *) args);
	}
	double retval[size];
	
	for (i = 0; i < NUM_OF_THREADS; i++){
		pthread_join(threads[i], (double) &retval);
		printf("%d", i);
	}
	clock_gettime(CLOCK_REALTIME, &end);
	elapsed = end.tv_sec - begin.tv_sec;
	elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
	return 0;
}
