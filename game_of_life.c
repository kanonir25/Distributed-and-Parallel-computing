#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>

int check_neighbours(short int ** life, int i, int j, int x_size, int y_size){
        int k, t;
//printf("OK!\n");
        int neigh = 0;
        int varx[3] = {(i - 1) % x_size, i, (i + 1) % x_size};
        int vary[3] = {(j - 1) % y_size, j, (j + 1) % y_size};
        for (k = 0; k < 3; k++)
                for (t = 0; t < 3; t++)
                        if ((vary[t] != j || varx[k] != i) && life[varx[k]][vary[t]] == 1)
                                ++neigh;

        return neigh;
}
int main(int argc, char** argv){
	if (argc < 4){
		printf("Lalki, malo argumentov!!!9!9!\n");
		return 1;
	}
        int x_size, y_size, i, j,k,t,s, amount_steps;
	x_size = atoi(argv[2]);
	y_size = atoi(argv[3]);
	int x, y;
	amount_steps = atoi(argv[1]);
        short int ** life = (short int**)malloc(sizeof(short int *) * x_size);
        short int ** trans = (short int**)malloc(sizeof(short int *) * x_size);
        for (i = 0; i < x_size; i++){
                life[i] = (short int*)malloc(sizeof(short int ) * y_size);
                trans[i] = (short int*)malloc(sizeof(short int ) * y_size);
        }
	
        FILE *fp;
	if((fp = fopen("/home/mia/Downloads/state.txt", "r+")) == NULL) {
		printf("No such file\n");
		exit(1);
	}
	#pragma omp parallel shared(life)
	{
		#pragma omp for collapse(2)
		for (i = 0; i < x_size; i++)
			for (j = 0; j < y_size; j++)
				life[i][j] = 0;
	}
	while (1) {
        int ret = fscanf(fp, "%d %d", &x, &y);
        if(ret == 2)
        	life[x % x_size][y % y_size] = 1;
    	else if(ret == EOF)
            break;
	else 
	    printf("No match.\n");
	
    	}
	printf("\n");
	if(!feof(fp)) {
		puts("NOT EOF!");
		exit(1);
	}
	for (i = 0; i < x_size; i++){
		for (j = 0; j < y_size; j++)
			if (life[i][j] == 1)
				printf("*");
			else
				printf(".");
		printf("\n");
	}
	printf("\n");
        for (i = 0; i < amount_steps; i++){
		#pragma omp parallel shared(trans, life)
		{
			#pragma omp for collapse(2)
		        for (j = 0 ; j < x_size; j++)
		                for (k = 0; k < y_size; k++)
                             	   trans[j][k] = life[j][k];
			#pragma omp for collapse(2)
			for (j = 0 ; j < x_size; j++)
			        for (k = 0; k < y_size; k++){
					int amount = 0;
					int varx[3] = {(j - 1 + x_size) % x_size, j, (j + 1) % x_size};
					int vary[3] = {(k - 1 + y_size) % y_size, k, (k + 1) % y_size};
					for (s = 0; s < 3; s++)
						for (t = 0; t < 3; t++)
							if ((vary[t] != k || varx[s] != j) && trans[varx[s]][vary[t]] == 1)
								++amount;		
			                if (trans[j][k] == 0)
			                        if (amount == 3)
			                                life[j][k] = 1;
			                        else
			                                life[j][k] = 0;
			                else {
			                        if (amount < 2 ||  amount > 3)
			                                life[j][k] = 0;
			                        else
			                                life[j][k] = 1;
			                        }
				}	
	printf("Number of iteration = %d\n", i + 1);
	
	for (k = 0; k < x_size; k++){
		#pragma omp for
		for (j = 0; j < y_size; j++)
			if (life[k][j] == 1)
				printf("*");
			else
				printf(".");
		printf("\n");
	}
	printf("\n");

	}
       }
	
        return 0;
}

