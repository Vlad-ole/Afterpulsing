#include <iostream>
#include <vector>
#include <omp.h> 

#include "TRandom.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "Math/MinimizerOptions.h"

#include "MyClass.h"

using namespace std;

//class MyClass_test
//{
//public:
//	MyClass_test();
//	~MyClass_test();
//
//	int GetA()
//	{
//		return a;
//	}
//
//	void SetA(int a)
//	{
//		this->a = a;
//	}	
//
//private:
//	int a;
//};
//
//MyClass_test::MyClass_test()
//{
//}
//
//MyClass_test::~MyClass_test()
//{
//}

int main()
{
	int i;
	#pragma omp parallel private(i)
	{
		#pragma omp for schedule (static) 
		for (int i = 0; i < 2; i++)
		{
			//printf("before thread is: %d ;itteration is %d \n", omp_get_thread_num(), i);
			//
			//MyClass_test *obj_test = new MyClass_test();
			//obj_test->SetA(i);

			//printf("after thread is: %d ;itteration is %d \n \n", omp_get_thread_num(), obj_test->GetA());

			printf("thread is: %d; itteration is %d \n", omp_get_thread_num(), i);

			MyClass *obj = new MyClass();
			obj->SetParameters();

			#pragma omp critical
			{
				obj->DoFit();
			}
		}
	}

	system("pause");
	return 0;
}