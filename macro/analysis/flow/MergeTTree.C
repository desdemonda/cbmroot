// -------------------------------------------------------------------------
// ----- This macro merges several files containing TTrees into one; from different input files:
// ----- mode=0: files with reconstructed event planes (non-corrected if fQcorr = fBarcorr = 0, corrected otherwise) - produced by eventPlane.C
// ----- mode=1  files with particle azimuthal distirbution (w.r.t. true reaction plane or reconstructed event plane) - model-generated files (UNIGEN) - produced by anaFlow.C in mode=0
// ----- mode=2  files with particle azimuthal distirbution (w.r.t. true reaction plane or reconstructed event plane) - reconstructed events           - produced by anaFlow.C in mode=1
// ----- mode=3  files with particle azimuthal distirbution (w.r.t. true reaction plane or reconstructed event plane) - kparticle finder output files  - produced by anaFlow.C in mode=2
// -------------------------------------------------------------------------

void MergeTTree(Int_t mode = 2, Double_t En = 10., Int_t fQcorr = 0, Int_t fBarcorr = 0)
{
    //gROOT->Reset();

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    TString sServ = "/hera/cbm/users/sseddiki/";

    Int_t gen = 0;
    TString sGen = "";
    if (gen == 0) sGen = "urqmd";
    if (gen == 1) sGen = "shield";
    
    TString sEn = "";
    sEn += En;

    Double_t bMIN = 5.;
    Double_t bMAX = 10.;

    TString sbMIN = "";
    TString sbMAX = "";
    sbMIN += bMIN;
    sbMAX += bMAX;
    TString Bcut = "_Bsel_" + sbMIN + "fm_" + sbMAX + "fm";

    if (mode == 0)
    {
        TString smcreco = "_RECOtracks";
        //TString smcreco = "_MCtracks_ge4hitsSTS";
        TString scutY_sub = "_STScutY_lt0.8";

        TString sFlatBin = "";
        sFlatBin = "_FlatBin_FixStep";
        //sFlatBin = "_FlatBin_FixStat";

	TString sQcorr = "";
	if (fQcorr == 1) sQcorr = "_Qcorr";

	TString sBarcorr = "";
	if (fBarcorr == 1) sBarcorr = "_Barcorr_n8";     
	
	TString dir = sServ + "test/ana/";
	
	TString outName = dir + "ana_1Mevt" + smcreco + scutY_sub + sQcorr + sBarcorr + sFlatBin + ".root";
	cout << "output: " << outName << endl;
	TFile *outFile = new TFile(outName, "RECREATE");

	TChain *bTuple = new TChain("cbmsim");
	bTuple->SetDirectory(outFile); 
	
	TString inFiles = dir + "ana_0200evt_*" + smcreco + scutY_sub + sQcorr + sBarcorr + sFlatBin + ".root";
	cout << "inFiles: "  << inFiles << endl;

	bTuple->Add(inFiles);
	bTuple->Merge(outName,"RECREATE");  
    }
    
    if(mode == 1)
    {
	TString dir = sServ + "test/ana/";
	
	TString outName = dir + "anaFlow_gen_100Kevt" + Bcut + ".root";
	cout << "output: " << outName << endl;
	TFile *outFile = new TFile(outName, "RECREATE");

	TChain *bTuple = new TChain("cbmana");
	bTuple->SetDirectory(outFile);   

	TString inFiles = dir + "anaFlow_gen_0200evt" + Bcut + "*.root";
	bTuple->Add(inFiles);    
	bTuple->Merge(outName,"RECREATE"); 
    }
    
    if(mode == 2)
    {
	TString dir = sServ + "test/ana/";
	
	TString outName = dir + "anaFlow_reco_track_100Kevt" + Bcut + ".root";
	cout << "output: " << outName << endl;
	TFile *outFile = new TFile(outName, "RECREATE");

	TChain *bTuple = new TChain("cbmana");
	bTuple->SetDirectory(outFile);   

	TString inFiles = dir + "anaFlow_reco_track_0200evt" + Bcut + "*.root";
	cout << "input files merged: " << inFiles << endl;
	bTuple->Add(inFiles);
	bTuple->Merge(outName,"RECREATE"); 
    }
    
    if(mode == 3)
    {
	TString dir = sServ + "test/ana/";
	
	TString outName = dir + "anaFlow_kfpart_10000evt" + Bcut + ".root";
	cout << "output: " << outName << endl;
	TFile *outFile = new TFile(outName, "RECREATE");

	TChain *bTuple = new TChain("cbmana");
	bTuple->SetDirectory(outFile);   

	TString inFiles = dir + "anaFlow_kfpart_0100evt" + Bcut + "_*.root";
	bTuple->Add(inFiles);    
	bTuple->Merge(outName,"RECREATE"); 
    }

    outFile->Close();
    gApplication->Terminate();
}