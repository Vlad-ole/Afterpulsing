#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char *argv[])
{
	int n, myid, numprocs, i;
	double mypi, pi, h, sum, x, t1, t2, PI25DT = 3.141592653589793238462643;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	while (1)
	{
		if (myid == 0)
		{
			printf("Enter the number of intervals: (0 quits) ");
			scanf("%d", &n);
			t1 = MPI_Wtime();
		}
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if (n == 0) break;
		else
		{
			h = 1.0 / (double)n;
			sum = 0.0;
			for (i = myid + 1; i <= n; i += numprocs)
			{
				x = h * ((double)i - 0.5);
				sum += (4.0 / (1.0 + x*x));
			}
			mypi = h * sum;
			MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0,
				MPI_COMM_WORLD);
			if (myid == 0)
			{
				t2 = MPI_Wtime();
				printf("pi is approximately %.16f. Error is %.16f\n", pi, fabs(pi - PI25DT));
				printf("'time is %f seconds \n", t2 - t1);
			}
		}
	}
	MPI_Finalize();

	
	
	//system("pause");
	return 0;
}
