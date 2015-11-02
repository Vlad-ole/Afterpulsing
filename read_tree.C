void ReadTree()
{
	string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\results\\";
	//string dir_name = "D:\\Data_work\\";
	string tree_name = dir_name + "tree.root";
	string graphs_name_fit1 = dir_name + "graphs_fit1.root";
	string graphs_name_fit2 = dir_name + "graphs_fit2.root";
	
	TObjArray Hlist_fit1(0);
	TObjArray Hlist_fit2(0);
	
	Hlist_fit1.SetOwner(kTRUE);
	Hlist_fit2.SetOwner(kTRUE);

	TFile f(tree_name.c_str(), "UPDATE");
	TTree* t;
	f.GetObject("t1", t);

	TMultiGraph* graph_fit1 = 0;
	TMultiGraph* graph_fit2 = 0;
	
	//TBranch *branch = t->GetBranch("gr_fit1");
	//branch->SetAddress(&graph);
	//branch->SetAutoDelete(kTRUE);
	
	t->SetBranchAddress("gr_fit1", &graph_fit1);
	t->SetBranchAddress("gr_fit2", &graph_fit2);
	
	Double_t a_1, chi_1, a_2, b_2, chi_2, dt_2_ab ;
	t->SetBranchAddress("a_1", &a_1);
	t->SetBranchAddress("chi_1", &chi_1);
	t->SetBranchAddress("a_2", &a_2);
	t->SetBranchAddress("b_2", &b_2);
	t->SetBranchAddress("chi_2", &chi_2);
	t->SetBranchAddress("dt_2_ab", &dt_2_ab);
	
	TCut fit_1 = "chi_1 < 4";
	TCut A1 = "a_1 > 0.1 && a_1 < 0.3";
	TCut B1 = "a_1 > 0.3 && a_1 < 0.5";
	TCut C1 = "a_1 > 0.5 && a_1 < 0.7";
	
	TCut fit_2 = "chi_1 > 4 && chi_2 < 4";
	TCut A2 = "(a_2 + b_2) > 0.15 && (a_2 + b_2) < 0.25";
	//TCut B2_tail = "(a_2 + b_2) > 0.25 && (a_2 + b_2) < 0.38";
	//TCut B2_circle = "(a_2 + b_2) > 0.38 && (a_2 + b_2) < 0.46";
	TCut B2 = "(a_2 + b_2) > 0.2 && (a_2 + b_2) < 0.47";
	TCut C2 = "(a_2 + b_2) > 0.47 && (a_2 + b_2) < 0.68";
	
	TCut temp_cut = "dt_2_ab < 1";
	
	TCut total_cut = fit_2 && C2;
	
	t->SetMarkerStyle(4);
	
	//t->Draw("chi_1:a_1", total_cut);
	//t->Draw("chi_2:(a_2 + b_2)", total_cut);
	//t->Draw("a_2:b_2", total_cut);
	//t->Draw("(a_2 + b_2):dt_2_ab", total_cut);
	t->Draw("dt_2_ab", total_cut);
	
	bool condition = false;	
	if(condition)
	{
		for (int i = 0; i < 1000/*t->GetEntries()*/; ++i)
		{
			t->GetEntry(i);	

			condition = /*fit_2*/(chi_1 > 4 && chi_2 < 4) && /*C2*/ ( true ) && /*temp*/ ( dt_2_ab < 40 && dt_2_ab > 1 && (a_2 + b_2) > 0.58 );
			
			if(condition)
			{
				Hlist_fit1.Add(graph_fit1->Clone());
				Hlist_fit2.Add(graph_fit2->Clone());
			}
		}
	}
	
	
	

	TFile ofile_Hlist_fit1(graphs_name_fit1.c_str(), "RECREATE");
	Hlist_fit1.Write();
	ofile_Hlist_fit1.Close();
	
	TFile ofile_Hlist_fit2(graphs_name_fit2.c_str(), "RECREATE");
	Hlist_fit2.Write();
	ofile_Hlist_fit2.Close();
}

void CutTree()
{
	//TFile *f = new TFile("D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\results\\ntuple_01_10.root");
	//TNtuple *my_ntuple = (TNtuple*)f->GetObjectChecked("ntuple", "TNtuple");
	
	TChain *chain = new TChain("ntuple"); // name of the tree is the argument
	chain->Add("D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\results\\ntuple_01_10.root");
	chain->Add("D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\results\\ntuple_11_20.root");
	
	// a_1:chi_1:a_2:b_2:chi_2:dt_2_ab - ntuple format
	chain->SetMarkerStyle(4);
	
	TCut fit_1 = "chi_1 < 4";
	TCut A1 = "a_1 > 0.1 && a_1 < 0.3";
	TCut B1 = "a_1 > 0.3 && a_1 < 0.5";
	TCut C1 = "a_1 > 0.5 && a_1 < 0.7";
	
	TCut fit_2 = "chi_1 > 4 && chi_2 < 4";
	TCut A2 = "(a_2 + b_2) > 0.15 && (a_2 + b_2) < 0.25";
	TCut B2_tail = "(a_2 + b_2) > 0.25 && (a_2 + b_2) < 0.38";
	TCut B2_circle = "(a_2 + b_2) > 0.38 && (a_2 + b_2) < 0.46";
	TCut C2 = "(a_2 + b_2) > 0.46 && (a_2 + b_2) < 0.7";
	
	TCut fit_3 = "chi_1 > 4 && chi_2 > 4";
	
	TCut temp = "dt_2_ab < 1E-7";
	TCut total_cut = fit_2 && A2;
		
	//chain->Draw("(a_2 + b_2):dt_2_ab", total_cut);
	//chain->Draw("chi_2:(a_2 + b_2)", total_cut);
	//chain->Draw("a_2:b_2", total_cut);
	
	chain->Draw("chi_1:a_1", total_cut);
	
	//my_ntuple->Draw("chi_2:(a_2 + b_2)", fit_1 && A2);
	
	//f->Close();
	
	//TChain chain("T");   // name of the tree is the argument
	//chain.Add("file1.root");
	
	Float_t a_1, chi_1, a_2, b_2, chi_2, dt_2_ab ;
	chain->SetBranchAddress("a_1", &a_1);
	chain->SetBranchAddress("chi_1", &chi_1);
	chain->SetBranchAddress("a_2", &a_2);
	chain->SetBranchAddress("b_2", &b_2);
	chain->SetBranchAddress("chi_2", &chi_2);
	chain->SetBranchAddress("dt_2_ab", &dt_2_ab);
	
	ofstream file_out("D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\results\\out.dat");
	for(int i = 0; i< chain->GetEntries() ;i++)
	{
		chain->GetEntry(i);
		if(chi_1 > 4)
		{
			file_out << a_2 + b_2 << "\t" << chi_2 << endl;
		}
	}
}















void treew() 
{
   TFile f("test.root","recreate");
   TNtuple *ntuple = new TNtuple("ntuple","Demo","px:py:pz:random:i");
   Float_t px, py, pz;
   for ( Int_t i=0; i<10000000; i++) {
      gRandom->Rannor(px,py);
      pz = px*px + py*py;
      Float_t random = gRandom->Rndm(1);
      ntuple->Fill(px,py,pz,random,i);
      if (i%1000 == 1) ntuple->AutoSave("SaveSelf");
   }
}

void treer() 
{
   TFile f("test.root");
   TTree *ntuple = (TTree*)f.Get("ntuple");
   TCanvas c1;
   Int_t first = 0;
   while(1) {
      if (first == 0) ntuple->Draw("px>>hpx", "","",10000000,first);
      else            ntuple->Draw("px>>+hpx","","",10000000,first);
      first = (Int_t)ntuple->GetEntries();
      c1.Update();
      gSystem->Sleep(3000); //sleep 1 second
      ntuple->Refresh();
   }
}