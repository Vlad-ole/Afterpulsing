#include <iostream>
#include <vector>
#include <omp.h> 

#include "TRandom.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "Math/MinimizerOptions.h"
#include "TVirtualFitter.h"

#include "MyClass.h"

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

int main()
{
//	TVirtualFitter::SetDefaultFitter("Minuit2");
//	
//	int i;
//#pragma omp parallel private(i)
//	{
//#pragma omp for schedule (static) 
//		for (int i = 0; i < 1; i++)
//		{
//			MyClass *obj = new MyClass();
//			obj->SetParameters();
//			obj->DoFit();
//		}
//	}
	int i;
	#pragma omp parallel private(i)
		{
	#pragma omp for schedule (static) 
			for (int i = 0; i < 10; i++)
			{
				MyClass2 *obj = new MyClass2();
				obj->SetParameter(i);
				printf("value is %d\n", obj->GetParameter());
			}
		}


	system("pause");
	return 0;
}