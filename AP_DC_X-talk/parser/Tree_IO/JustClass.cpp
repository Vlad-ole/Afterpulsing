#include "JustClass.h"


JustClass::JustClass()
{
	gr_der = new TGraph();
	
	//fill graphs
	for (int i = 0; i < 10; ++i)
	{
		gr_der->SetPoint(i, i + gRandom->Uniform(-.5, .5), gRandom->Uniform(-.5, .5));
	}
}


JustClass::~JustClass()
{
}

void JustClass::FillData()
{

}