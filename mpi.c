#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char** argv){
	if (argc < 2)
		return 0;
	int i;
	long long int N = atoll(argv[1]);
	int myrank, size;
	
	long long int result = 0, cur_res = 0, S_0 = 0;
	MPI_Status Status;
	MPI_Init (&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	int step = N / (size - 1);
	int rest = N % (size - 1);
	
	if (myrank == 0){
		int array[N];
		for (i = 0; i < N; i++){
			array[i] = i + 1;
			S_0 += array[i];
		}
		for (i = 1; i < size; i++){
			if (rest >= i)
				MPI_Send(&array[(i - 1)*step + (i - 1)], step + 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			else
				MPI_Send(&array[(i - 1)*step + rest], step, MPI_INT, i, 1, MPI_COMM_WORLD);
			}
		for (i = 1; i < size; ++i){
			MPI_Recv((void*)&cur_res, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &Status);
			printf("rank = %d, sum = %lld\n", i, cur_res);
			result += cur_res;
		}
		
		printf("the final result = %lld\n", result);
		printf("real result = %lld\n", S_0);;
		MPI_Finalize();
		return 0;
	}
	
	else if (myrank != 0){
		int array[step + 1];
		if (rest >= myrank){
			MPI_Recv((void*)&array, step + 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &Status);
			result += array[step];
		}
		else 
			MPI_Recv((void*)&array, step, MPI_INT, 0, 1, MPI_COMM_WORLD, &Status);
		for (i = 0; i < step; ++i)
			result += array[i];
		MPI_Send(&result, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Finalize();
	}
}
