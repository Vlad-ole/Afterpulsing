#include <iostream>
#include <vector>


#include "TRandom.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "Math/MinimizerOptions.h"
#include "TVirtualFitter.h"

#include "MyClass.h"

#include <chrono>

#include <thread>
#include <omp.h> 
#include <mpi.h>

using namespace std;

class MyClass2
{
public:
	MyClass2()
	{

	}

	~MyClass2()
	{

	}

	static int variable;

	static void SetParameter(int a)
	{
		variable = a;
	}

	static int GetParameter()
	{
		return variable;
	}
};

int MyClass2::variable;

int main(int argc, char *argv[])
{
	//	TVirtualFitter::SetDefaultFitter("Minuit2");
	//	

	int n, myid, numprocs, i;
	MPI_Init(&argc, &argv);

	MPI_Finalize();
	
//#pragma omp parallel num_threads(1)
//	{
//#pragma omp for   
//		for (int i = 0; i < 2; i++)
//		{
//			printf(" \n thread is %d\n", omp_get_thread_num());
//			MyClass *obj = new MyClass();
//			obj->SetParameters();
//			obj->DoFit();
//		}
//	}

	//static int a;
	//threadprivate(MyClass2::variable)

//
//#pragma omp parallel num_threads(3) 
//	{
//		int a;
//#pragma omp for threadprivate(a)  
//		for (int i = 0; i < 10; i++)
//		{
//			//MyClass2 *obj = new MyClass2();
//			//MyClass2::SetParameter(i);
//			a = i;
//			std::this_thread::sleep_for(std::chrono::milliseconds(1));
//			printf("value is %d thread is %d\n", a, omp_get_thread_num());
//		}
//	}

	//int commsize, rank, len;
	//char procname[MPI_MAX_PROCESSOR_NAME];
	//MPI_Init(&argc, &argv);
	//MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	//MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//MPI_Get_processor_name(procname, &len);
	//printf("Hello, MPI World! Process %d of %d on node %s.\n",
	//	rank, commsize, procname);
	//MPI_Finalize();

	//int commsize, rank, len;
	//MPI_Comm_size(MPI_COMM_WORLD, &commsize);

	system("pause");
	return 0;
}