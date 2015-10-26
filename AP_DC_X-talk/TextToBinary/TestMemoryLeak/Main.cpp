#include <iostream>
#include "MyClass.h"

using namespace std;

int main()
{
	while (true)
	{
		MyClass *obj = new MyClass();
		delete obj;
	}

	system("pause");
	return 0;
}