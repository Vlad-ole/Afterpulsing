#include <iostream>
#include <cuda.h>

using namespace std;

__global__ void kernel(void){}

int main()
{
	printf("Hello, word! \n");
	system("pause");
	return 0;
}