#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
double func(double x){
	return 1. / (1 + x * x);
}

int main(int argc, char* argv[]){
	if (argc < 3)
		return 0;
	#ifdef _OPENMP
	printf("OpenMP is supported! %d \n", _OPENMP);
	#endif
	double h = atof(argv[2]);
	int num_threads = atoi(argv[1]);
	int a = 0;
	int b = 1;
	int i = 0;
	omp_set_num_threads(num_threads);
	double result_for_thread = 0, result = 0, result_reduction = 0;
	long long int size = (b - a) / h;
	double *x = malloc(sizeof(double)* size);
	int amount = 0;
	for (i = 0; i < size; i++)
		x[i] = i * h;
	double begin = omp_get_wtime();
	#pragma omp parallel shared(x, result) private(result_for_thread) 
	{	
		#pragma omp for
		for (i = 0; i < size - 1; i++){
			result_for_thread += h / 2 * (func(x[i + 1]) + func(x[i]));
		}
		#pragma omp critical
		{
			result += result_for_thread;
		}
	}
	//#pragma omp barrier
	double end = omp_get_wtime();
	//#pragma omp barrier
	double begin2 = omp_get_wtime();
	#pragma omp parallel reduction (+ : result_reduction)
	{
		#pragma omp for
		for (i = 0; i < size - 1; i++){
			result_reduction += h / 2 * (func(x[i + 1]) + func(x[i]));
		}
		//printf("end for: %f\n", result_reduction);
	}
	//#pragma omp barrier
	double end2 = omp_get_wtime();
	//printf("final: %f\n", result_reduction);
	double mistake = (M_PI - 4 * result) / M_PI;
	printf("%d\t%lf\t%f\t", num_threads, 4 * result, end - begin);
	printf("%lf\t%f\t%f\t", 4 * result_reduction, end2 - begin2, M_PI);
	printf("%f\n", mistake * 100);
	return 0;
}
