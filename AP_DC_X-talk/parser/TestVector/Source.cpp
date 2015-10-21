#include <iostream>
#include <vector>

using namespace std;
vector<double> xv;

int main()
{
	
	//for (int j = 0; j < 10; j++)
	{
		//xv.clear();
		
		int yv_size_new = 200000000;
		xv.resize(yv_size_new);

		for (int i = 0; i < yv_size_new; i++)
		{
			xv[i] = 0.2 * i;
		}

		xv.clear();

		cout << "xv.capacity() = " << xv.capacity() << "\t xv.size() = " << xv.size() << endl;
		system("pause");
	}
	
	return 0;
}