void hist_2d(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	Double_t x, y;
	FILE *f = fopen(name,"r");
	
	TH2F *h2 = new TH2F("hist", "hist_", 7000, -300, 7300, 35, -5, 60);
		
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf\n", &x, &y);
		//xv.push_back(x);
		//yv.push_back(y);
		h2->Fill(x, y);
	}
		
	h2->Draw();
	
}