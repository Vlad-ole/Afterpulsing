#include <iostream>
#include <vector>

using namespace std;
vector<double> xv;

int main()
{
	
	//for (int j = 1; j < 20*1000*1000; j+= 100000)
	{
		int j = 200*1000*1000;
		
		int yv_size_new = j;
		xv.reserve(yv_size_new + 1);
		xv.resize(yv_size_new);

		for (int i = 0; i < yv_size_new; i++)
		{
			xv[i] = 0.2 * i;
		}

		cout << "xv.capacity() = " << xv.capacity() << "\t xv.size() = " << xv.size() << endl;

		yv_size_new = j + 1;
		xv.clear();
		xv.reserve(yv_size_new);
		xv.resize(yv_size_new);

		for (int i = 0; i < yv_size_new; i++)
		{
			xv[i] = 0.2 * i;
		}

		cout << "xv.capacity() = " << xv.capacity() << "\t xv.size() = " << xv.size() << endl << endl;
	}
	
	
	system("pause");

	return 0;
}