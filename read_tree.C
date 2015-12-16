void ReadTree()
{
	string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\275K\\69_10V\\";
	//string dir_name = "D:\\Data_work\\";
	//string tree_name = dir_name + "tree.root";
	string graphs_name_fit1 = dir_name + "graphs_fit1.root";
	string graphs_name_fit2 = dir_name + "graphs_fit2.root";
	string graphs_name_fit3 = dir_name + "graphs_fit3.root";
	
	TObjArray Hlist_fit1(0);
	TObjArray Hlist_fit2(0);
	TObjArray Hlist_fit3(0);
	
	Hlist_fit1.SetOwner(kTRUE);
	Hlist_fit2.SetOwner(kTRUE);
	Hlist_fit3.SetOwner(kTRUE);
	
	//TFile f(tree_name.c_str(), "UPDATE");
	//TTree* t;
	//f.GetObject("t1", t);

	TMultiGraph* graph_fit1 = 0;
	TMultiGraph* graph_fit2 = 0;
	TMultiGraph* graph_fit3 = 0;
	
	//TBranch *branch = chain.GetBranch("gr_fit1");
	//branch->SetAddress(&graph);
	//branch->SetAutoDelete(kTRUE);
	
	TChain chain("t1");   // name of the tree is the argument
	for(int i = 1; i <= 1000; i++)
	{
		ostringstream file_tree_oss;
		file_tree_oss << dir_name << "trees_2\\run_" << i << ".root";
		chain.Add(file_tree_oss.str().c_str());
	}
	
	chain.SetBranchAddress("gr_fit1", &graph_fit1);
	chain.SetBranchAddress("gr_fit2", &graph_fit2);
	chain.SetBranchAddress("gr_fit3", &graph_fit3);
	
	Double_t a_1, chi_1, time_1;
	Double_t a_2, b_2, chi_2, time_2_a, time_2_b;
	Double_t a_3, b_3, c_3, chi_3, time_3_a, time_3_b, time_3_c;
	
	chain.SetBranchAddress("a_1", &a_1);
	chain.SetBranchAddress("chi_1", &chi_1);
	chain.SetBranchAddress("time_1", &time_1);
	
	chain.SetBranchAddress("a_2", &a_2);
	chain.SetBranchAddress("b_2", &b_2);
	chain.SetBranchAddress("chi_2", &chi_2);
	chain.SetBranchAddress("time_2_a", &time_2_a);
	chain.SetBranchAddress("time_2_b", &time_2_b);
	
	chain.SetBranchAddress("a_3", &a_3);
	chain.SetBranchAddress("b_3", &b_3);
	chain.SetBranchAddress("c_3", &c_3);
	chain.SetBranchAddress("chi_3", &chi_3);
	chain.SetBranchAddress("time_3_a", &time_3_a);
	chain.SetBranchAddress("time_3_b", &time_3_b);
	chain.SetBranchAddress("time_3_c", &time_3_c);

	
	chain.SetBranchStatus("*", 0); //disable all branches
	
	chain.SetBranchStatus("a_1", 1);
	chain.SetBranchStatus("chi_1", 1);
	chain.SetBranchStatus("time_1", 1);
	
	chain.SetBranchStatus("a_2", 1);
	chain.SetBranchStatus("b_2", 1);
	chain.SetBranchStatus("chi_2", 1);
	chain.SetBranchStatus("time_2_a", 1);
	chain.SetBranchStatus("time_2_b", 1);
	
	chain.SetBranchStatus("a_3", 1);
	chain.SetBranchStatus("b_3", 1);
	chain.SetBranchStatus("c_3", 1);
	chain.SetBranchStatus("chi_3", 1);
	chain.SetBranchStatus("time_3_a", 1);
	chain.SetBranchStatus("time_3_b", 1);
	chain.SetBranchStatus("time_3_c", 1);
	
	chain.SetBranchStatus("gr_fit1", 0);
	chain.SetBranchStatus("gr_fit2", 0);
	chain.SetBranchStatus("gr_fit3", 0);
	
	TCut fit_1 = "chi_1 < 3.5";
	TCut A1 = "a_1 > 0.12 && a_1 < 0.22 && chi_1 < 2.5";
	TCut B1 = "a_1 > 0.28 && a_1 < 0.4 && chi_1 < 3.0";
	TCut C1 = "a_1 > 0.44 && a_1 < 0.6 && chi_1 < 4.5";
	//TCut D1 = "a_1 > 0.7 && a_1 < 0.9";
	TCut noise = "a_1 < 0.06 && chi_1 < 5.4";

	
	TCut fit_2 = "chi_1 > 4 && chi_2 < 4";
	
	//TCut A2 = "(a_2 + b_2) > 0.2 && (a_2 + b_2) < 0.59 && chi_2 < 3.5 && chi_1 > 3";
	//TCut A2 = "(a_2 + b_2) > 0.2 && (a_2 + b_2) < 0.31 && chi_2 < 3.5 && chi_1 > 3";
	TCut A2 = "(a_2 + b_2) > 0.18 && (a_2 + b_2) < 0.4 && chi_2 < 2 && a_2 < 0.21 && b_2 < 0.21";
	
	TCut B2 = "(a_2 + b_2) > 0.6 && (a_2 + b_2) < 0.88 && chi_2 < 4";
	TCut C2 = "(a_2 + b_2) > 0.68 && (a_2 + b_2) < 0.88";
	
	TCut A3 = "(a_3 + b_3 + c_3) > 0.1 && (a_3 + b_3 + c_3) < 0.2";

	//TCut total_cut = "";
	//TCut total_cut = "chi_1 > 10 && (a_2 + b_2) > 0.13 && (a_2 + b_2) < 0.2 && chi_2 < 3";
	TCut total_cut = !A1 && !B1 && !C1 && "(a_2 + b_2) > 0" && A2;
	
	chain.SetMarkerStyle(4);
	
	//chain.Draw("a_1", total_cut);
	//chain.Draw("chi_1:a_1", total_cut);
	
	//chain.Draw("chi_2:(a_2 + b_2)", total_cut);
	//chain.Draw("a_2:b_2", total_cut);
	chain.Draw("(a_2 + b_2):(time_2_b - time_2_a)", total_cut);
	//chain.Draw("dt_2_ab", total_cut);
	
	//chain.Draw("chi_3:(a_3 + b_3 + c_3)", total_cut);
	
	bool condition = true;
	if(condition)
	{
		string file_out_s = dir_name + "out.dat";
		string file_out_dt_s = dir_name + "dt_spectrum.dat";
		
		ofstream file_out( file_out_s.c_str() );
		ofstream file_out_dt( file_out_dt_s.c_str() );
		
		bool PreviousIs1e = false;
		double last_time = 0;
		
		for (int i = 0; i < chain.GetEntries() ; ++i)
		{
			chain.GetEntry(i);
			
			//cout << time_1 << endl;

			bool condition_1 = /*A1*/ a_1 > 0.12 && a_1 < 0.22 && chi_1 < 2.5;
			bool condition_2 = /* !A1 && !B1 && !C1 && A2 */ !(condition_1) && !(a_1 > 0.28 && a_1 < 0.4 && chi_1 < 3.0) && !(a_1 > 0.44 && a_1 < 0.6 && chi_1 < 4.5) && ( (a_2 + b_2) > 0.18 && (a_2 + b_2) < 0.4 && chi_2 < 2 && a_2 < 0.21 && b_2 < 0.21 );
			
			//bool condition_1 = a_1 > 0.15 && a_1 < 0.27 && chi_1 < 2.2 ;
			//bool condition_2 = false;
			
			bool A1_B1_C1 = !(a_1 > 0.2 && a_1 < 0.35 && chi_1 < 3.2) && !(a_1 > 0.42 && a_1 < 0.65 && chi_1 < 4.7) && !(a_1 > 0.7 && a_1 < 0.9 && chi_1 < 8.3);
			//bool condition_3 = A1_B1_C1 && ( chi_2 > 7 && (a_3 + b_3 + c_3) > 0.26 && (a_3 + b_3 + c_3) < 0.34 && chi_3 < 3.3 );
			bool condition_3 = false;
			if(condition_3)
			{
				Hlist_fit1.Add(graph_fit1->Clone());
				Hlist_fit2.Add(graph_fit2->Clone());
				Hlist_fit3.Add(graph_fit3->Clone());
			}
			
			if(condition_1)
			{
				if(PreviousIs1e)
					file_out_dt << time_1 - last_time << endl;
				
				last_time = time_1;
				PreviousIs1e = true;
			}
			
			if(condition_2)
			{
				if(PreviousIs1e)
				{
					file_out_dt << time_2_a - last_time << endl;
					file_out_dt << time_2_b - time_2_a << endl;					
				}
				else
				{
					file_out_dt << time_2_b - time_2_a << endl;	
				}
				
				last_time = time_2_b;
				PreviousIs1e = true;
				
			}
			
			if( !(condition_1 || condition_2))
				PreviousIs1e = false;			
			
		}
	}
	
	
	

	TFile ofile_Hlist_fit1(graphs_name_fit1.c_str(), "RECREATE");
	Hlist_fit1.Write();
	ofile_Hlist_fit1.Close();
	
	TFile ofile_Hlist_fit2(graphs_name_fit2.c_str(), "RECREATE");
	Hlist_fit2.Write();
	ofile_Hlist_fit2.Close();
	
	TFile ofile_Hlist_fit3(graphs_name_fit3.c_str(), "RECREATE");
	Hlist_fit3.Write();
	ofile_Hlist_fit3.Close();
}
