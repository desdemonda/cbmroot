#include <sstream>
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

TH1* hFileSizeMC = new TH1F("h_file_size_mc", "h_file_size_mc", 1000, 0, 0);
TH1* hTreeSizeMC = new TH1F("h_tree_size_mc", "h_tree_size_mc", 1000, 0, 0);
TH1* hFileSizeReco = new TH1F("h_file_size_reco", "h_file_size_reco", 1000, 0, 0);
TH1* hTreeSizeReco = new TH1F("h_tree_size_reco", "h_tree_size_reco", 1000, 0, 0);

void check_files() {
      std::string dir = "/hera/cbm/users/vassilie/mc/sts_rich_tof/urqmd/auau/6gev/mbias/";
      std::string fileNameMC = dir + "mc.auau.6gev.mbias.";
      std::string fileNameReco = dir + "reco.auau.6gev.mbias.";

      for (int i = 1; i <= 5000; i++) {
         stringstream ss;
         ss << fileNameMC ;
         ss.fill('0');
         ss.width(5);
         ss  << i << ".root";
         TFile* file = TFile::Open(ss.str().c_str());
         process_file(file, i, "mc");
	 file->Close();
	 if ((i % 100) == 0) std::cout << "-I- Procss file: " << i << std::endl;
      }

      for (int i = 1; i <= 5000; i++) {
         stringstream ss;
         ss << fileNameReco;
         ss.fill('0');
         ss.width(5);
         ss  << i << ".root";
         TFile* file = TFile::Open(ss.str().c_str());
         process_file(file, i, "reco");
	 file->Close();
         if ((i % 100) == 0) std::cout << "-I- Procss file: " << i << std::endl;
      }

      TCanvas* canvas = new TCanvas("files", "files", 1200, 1200);
      canvas->Divide(2, 2);
      canvas->cd(1);
      hFileSizeMC->Draw();
      canvas->cd(2);
      hTreeSizeMC->Draw();
      canvas->cd(3);
      hFileSizeReco->Draw();
      canvas->cd(4);
      hTreeSizeReco->Draw();
}

void process_file(TFile* file, Int_t index, const string& opt) {
    if (file == NULL) {
       std::cout << "-E- NULL file with index " << index << std::endl;
       return;
    }
    TTree* tree = (TTree*)file->Get("cbmsim");
    TFolder* folder = (TFolder*)file->Get("cbmroot");
    if (folder == NULL) folder = (TFolder*) file->Get("cbmout");
    if (folder == NULL) {
	std::cout << "-E- No cbmsim or cbmout TTree found in file " << file->GetName() << std::endl;
	return;
    }

    Long64_t fileSize = file->GetSize();
    Long64_t treeSize = tree->GetEntriesFast();

    if (opt == "mc") {
	hFileSizeMC->Fill(fileSize);
	hTreeSizeMC->Fill(treeSize);
    } else if (opt == "reco") {
        hFileSizeReco->Fill(fileSize);
	hTreeSizeReco->Fill(treeSize);
    }
}
