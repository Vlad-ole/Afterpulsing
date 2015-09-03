#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
	struct sort_pred 
	{
		bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right) 
		{
			return left.first < right.first;
		}
	};
	
	vector<double> Amp;
	vector<double> Time;

	pair<double, double> p;
	
	Amp.push_back(4);
	Amp.push_back(7);
	Amp.push_back(2);

	Time.push_back(0);
	Time.push_back(1);
	Time.push_back(2);

	vector<pair<double, double>> v;
	p.first = 4;
	p.second = 0;
	v.push_back(p);

	p.first = 7;
	p.second = 1;
	v.push_back(p);

	p.first = 2;
	p.second = 2;
	v.push_back(p);
	
	for (int i = 0; i < Amp.size(); i++)
	{
		cout << Amp[i] << "\t" << Time[i] << "\t" << v[i].first << "\t" << v[i].second << endl;
	}

	sort(Amp.begin(), Amp.end());
	sort(v.begin(), v.end(), sort_pred());
	
	cout << endl;

	for (int i = 0; i < Amp.size(); i++)
	{
		cout << Amp[i] << "\t" << Time[i] << "\t" << v[i].first << "\t" << v[i].second << endl;
	}


	//vector<std::pair<Amp, Time>> items;
	
	system("pause");
	return 0;
}