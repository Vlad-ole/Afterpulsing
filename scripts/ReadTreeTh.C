void ReadTreeTh()
{
	string dir_name = "D:\\Data_work\\tektronix_signal\\KETEK PM1125NS-SB0\\275K\\29_76V\\";
	//string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\290K\\70_33V\\";
	
	//string dir_name = "D:\\Data_work\\";
	string tree_name = dir_name + "tree_th_der.root";
	
	TFile f(tree_name.c_str(), "UPDATE");
	TTree* t;
	f.GetObject("t1", t);

	Double_t th_amp, th_der, tau_amp, tau_der;
	Int_t counter;
	
	t->SetBranchAddress("th_amp", &th_amp);
	t->SetBranchAddress("th_der", &th_der);
	t->SetBranchAddress("tau_amp", &tau_amp);
	t->SetBranchAddress("tau_der", &tau_der);
	t->SetBranchAddress("counter", &counter);
	
	TCut total_cut = " counter > 0.1 ";

	
	t->SetMarkerStyle(4);
	
	t->Draw("counter:(-th_amp)", total_cut);
	
	string file_out_s = dir_name + "tree_th_der.dat";
	ofstream file_out( file_out_s.c_str() );

	if(false)
	{
		for (int i = 0; i < t->GetEntries(); ++i)
		{
			t->GetEntry(i);	
			if(tau_amp == 41)
			{
				file_out << -th_amp << "\t" << counter << endl;
			}
		}
	}
}