#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv){
	if (argc < 2)
		return 0;
	double tau = atof(argv[1]);
	int U_l = 80, U_d = 0, U_u = 100, U_r = 30, U_0 = 5, i,j,k;
	double L_x = 0.5, L_y = 0.5, h = 0.01, kappa = 1.1e-4; 
	int size_x = L_x / h;
	struct timespec begin, end;
	double elapsed;
	double mult = kappa * tau / h / h;
	int amount_steps = 60 / tau;
	int myrank, size;
	MPI_Status Status;
	MPI_Init (&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	int step = (size_x - 2) / (size - 1);
	int my_step, his_step;
	int rest = (size_x - 2) % (size - 1);
	if (myrank == 0){
		double begin = MPI_Wtime();
		//clock_gettime(CLOCK_REALTIME, &begin);
		double u[2][2][size_x];
		double recieve[size_x];
		for (j = 0; j < 2; j++)
			for (i = 0; i < size_x; ++i){
				u[j][0][i] = U_d;
				u[j][1][i] = U_u;
			}
			
		for (k = 0; k < amount_steps; k++){
				MPI_Send(&u[0][0], size_x, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
				MPI_Send(&u[0][1], size_x, MPI_DOUBLE, size - 1, 1, MPI_COMM_WORLD);
		}
		/*for (j = 0; j < size_x; j++)
			printf("%f\t", u[0][0][j]);
		printf("\n");*/
		for (k = 1; k < size; k++){
			if (k <= rest)
				his_step = step + 1;
			else
				his_step = step;
			for (j = 0; j < his_step; j++){
				MPI_Recv((void*)&recieve, size_x,  MPI_DOUBLE, k, 0, MPI_COMM_WORLD, &Status);
				/*for (i = 0; i < size_x; i++)
					printf("%f\t", recieve[i]);
				printf("\n");*/
			}
		}
		/*for (j = 0; j < size_x; j++)
			printf("%f\t", u[0][1][j]);
		printf("\n");*/
		double end = MPI_Wtime();
		printf("%d\t%f\t%f\n", size, tau, end - begin);
		MPI_Finalize();
		return 0;
	}
	else {
		if (myrank <= rest)
			my_step = step + 1;
		else
			my_step = step;
		double u[2][my_step][size_x];
		double u_for_recv[2][size_x];
		for (i = 0; i < my_step; i++){
			u[0][i][0] = U_l;
			u[0][i][size_x - 1] = U_r;
		}
		for (i = 0; i < my_step; ++i)
			for (j = 1; j < size_x - 1; ++j)
				u[0][i][j] = U_0;
		
		for (k = 0; k < amount_steps; k++){
			if (myrank != size - 1)
				MPI_Sendrecv(&u[0][my_step - 1], size_x, MPI_DOUBLE, (myrank + 1), 1,
							(void*)&u_for_recv[0], size_x, MPI_DOUBLE, myrank - 1, 1, MPI_COMM_WORLD, &Status);
			//MPI_Recv((void*)&u_for_recv[0], size_x, MPI_DOUBLE, myrank - 1, 1, MPI_COMM_WORLD, &Status);
			else 	
				MPI_Recv((void*)&u_for_recv[0], size_x, MPI_DOUBLE, myrank - 1, 1, MPI_COMM_WORLD, &Status);
			if (myrank != 1) 
				MPI_Sendrecv(&u[0][0], size_x, MPI_DOUBLE, myrank - 1, 1, 
							(void*)&u_for_recv[1], size_x, MPI_DOUBLE, (myrank + 1) % size, 1, MPI_COMM_WORLD, &Status);
			//MPI_Recv((void*)&u_for_recv[1], size_x, MPI_DOUBLE, (myrank + 1) % size, 1, MPI_COMM_WORLD, &Status);
			else
				MPI_Recv((void*)&u_for_recv[1], size_x, MPI_DOUBLE, (myrank + 1) % size, 1, MPI_COMM_WORLD, &Status);
			
			for (i = 0; i < my_step; i++){
				for (j = 0; j < size_x; j++){ 
					if (i == 0){
							u[1][i][j] = u[0][i][j] + mult * (u_for_recv[0][j] + u[0][i+1][j] - 2*u[0][i][j]);	
						}
					else if (i == my_step - 1){
							u[1][i][j] = u[0][i][j] + mult * (u_for_recv[1][j] - 2*u[0][i][j] + u[0][i-1][j]);
						}
						else {
							u[1][i][j] = u[0][i][j] + mult * (u[0][i+1][j] - 2*u[0][i][j] + u[0][i-1][j]);
						}
					}
				}
				
			for (i = 0; i < my_step; i++)
				for (j = 1; j < size_x - 1; j++)
						u[0][i][j] = u[1][i][j] + mult * (u[1][i][j + 1] - 2*u[1][i][j] + u[1][i][j - 1]);
		}
		
		for (i = 0; i < my_step; i++){
			MPI_Send(&u[0][i], size_x, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		}
		MPI_Finalize();
	}	
}
