void ReadTree()
{
	string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\";
	string tree_name = dir_name + "tree.root";
	string graphs_name = dir_name + "graphs.root";
	
	TObjArray Hlist(0);
	Hlist.SetOwner(kTRUE);

	TFile f(tree_name.c_str());
	TTree* t;
	f.GetObject("t1", t);

	TMultiGraph* graph = 0;

	TBranch *branch = t->GetBranch("gr");
	branch->SetAddress(&graph);
	//branch->SetAutoDelete(kTRUE);
	//t->SetBranchAddress("gr", &graph);

	for (int i = 0; i < t->GetEntries(); ++i)
	{
		t->GetEntry(i);		
		//graph->Dump();
		//Hlist.Add(new TMultiGraph(*graph)); // error
		
		//TMultiGraph* graph_copy = graph->Clone();
		//graph = new TMultiGraph();
		Hlist.Add(graph->Clone());
	}

	

	TFile ofile_Hlist(graphs_name.c_str(), "RECREATE");
	Hlist.Write();
	ofile_Hlist.Close();
}

