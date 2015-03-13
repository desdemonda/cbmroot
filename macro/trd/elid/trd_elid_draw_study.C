/*  Description: This macro train and test different electron/pion identification methods in the TRD detector.

 Author : Semen Lebedev
 E-mail : S.Lebedev@gsi.de
 */
#include <vector>
using namespace std;

#pragma link C++ class vector<TH1*>;
void trd_elid_draw_study()
{

   // ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();
   gSystem->Load("libTMVA");

   string dir = "results/";

   vector<string> detectors;
   vector<string> leg;
  // detectors.push_back("Spectra_Be_run2210010_Rad_G10_Sus02.root"); leg.push_back("02/G10");
  // detectors.push_back("Spectra_Be_run2310008_Rad_A_Sus02.root"); leg.push_back("02/A");
  // detectors.push_back("Spectra_Be_run2310003_Rad_B_Sus02.root"); leg.push_back("02/B");
   detectors.push_back("Spectra_Be_run2410003_Rad_H++_Sus02.root"); leg.push_back("H++/02");

   detectors.push_back("Spectra_Be_run2310013_Rad_G30_Sus06.root"); leg.push_back("G30/06");
  // detectors.push_back("Spectra_Be_run2210010_Rad_G10_Sus06.root");leg.push_back("06/G10");
  // detectors.push_back("Spectra_Be_run2310010_Rad_A_Sus06.root"); leg.push_back("06/A");

  // detectors.push_back("Spectra_Be_run2210010_Rad_G10_Sus12.root"); leg.push_back("12/G10");
  // detectors.push_back("Spectra_Be_run2310007_Rad_G20_Sus12.root"); leg.push_back("12/G20");

   detectors.push_back("Spectra_Be_run2310004_Rad_F_Sus18.root"); leg.push_back("F/18");
  // detectors.push_back("Spectra_Be_run2210010_Rad_G10_Sus18.root"); leg.push_back("18/G10");
   detectors.push_back("Spectra_Be_run2310014_Rad_B_Sus18.root"); leg.push_back("B/18");
  // detectors.push_back("Spectra_Be_run2310010_Rad_G20_Sus18.root"); leg.push_back("18/G20");

   detectors.push_back("4mm_foam"); leg.push_back("4mm_foam");
   detectors.push_back("5mm_fibre"); leg.push_back("5mm_fibre");
   detectors.push_back("4mm_f350"); leg.push_back("4mm_f350");

   vector<vector<vector<double> > > piSupp;
   piSupp.resize(detectors.size());
   for (int iF = 0; iF < detectors.size(); iF++){
      piSupp[iF].resize(5);
      for (int iM = 0; iM < 5; iM++){
         piSupp[iF][iM].resize(13);
         for (int iH = 6; iH < 13; iH++){
            stringstream ss;
            ss << dir << detectors[iF] <<"/" << iM << "/" << iH << "/trd_elid_hist.root";
            TFile* f = new TFile(ss.str().c_str(), "READ");
            TH1D* h = (TH1D*)f->Get("fhResults")->Clone();
           // cout << h->GetBinContent(1) << " " << h->GetBinContent(2) << endl;
            piSupp[iF][iM][iH] = h->GetBinContent(1);
            f->Close();
         }
      }
   }

   int nofTrdLayers = 10;
   vector<TH1*> result_hist;
   result_hist.resize(5);
   for (int iM = 0; iM < 5; iM++){
      stringstream ss;
      ss << "pisupp_method_" << iM << ";Radiator;Pion suppression";
      result_hist[iM] = new TH1D(ss.str().c_str(), ss.str().c_str(), detectors.size(), 0, detectors.size());
      for (int iF = 0; iF < detectors.size(); iF++){
         result_hist[iM]->SetBinContent(iF+1, piSupp[iF][iM][nofTrdLayers]);
         result_hist[iM]->SetBinError(iF+1, 0.00001);
         result_hist[iM]->GetXaxis()->SetBinLabel(iF+1, leg[iF].c_str());
      }
      result_hist[iM]->GetXaxis()->SetLabelSize(0.01);
      result_hist[iM]->SetTitle("");
   }

 /*  int radiatorInd = 4;
   vector<TH1*> result_hist;
   result_hist.resize(5);
   for (int iM = 0; iM < 5; iM++){
      stringstream ss;
      ss << "pisupp_method_" << iM << ";Number of TRD layers;Pion suppression";
      result_hist[iM] = new TH1D(ss.str().c_str(), ss.str().c_str(), 7, 5.5, 12.5);
      for (int iH = 6; iH < 13; iH++){
         result_hist[iM]->SetBinContent(iH - 5, piSupp[radiatorInd][iM][iH]);
         result_hist[iM]->SetBinError(iH - 5, 0.00001);
      }
      result_hist[iM]->GetXaxis()->SetLabelSize(0.01);
      result_hist[iM]->SetTitle("");
   }*/


   vector<string> legM;
   legM.push_back("ANN");
   legM.push_back("BDT");
   legM.push_back("Med");
   legM.push_back("Like");
   legM.push_back("Mean");
  // result_hist[0]->Draw();
   stringstream ss;
  // ss << detectors[radiatorInd] << "_results";
   ss << "trd_elid_radiators_results";
   TCanvas* c = new TCanvas(ss.str().c_str(), ss.str().c_str(), 1200, 1000);
   DrawH1(result_hist, legM, 1, 0, true, 0.91, 0.70, 1.0, 1.0);
   result_hist[0]->GetXaxis()->SetLabelSize(0.04);

   for (int iM = 0; iM < 5; iM++){
      result_hist[iM]->SetLabelSize(0.05);
      result_hist[iM]->SetMinimum(9);
      result_hist[iM]->SetMaximum(3e3);
      result_hist[iM]->SetLineWidth(4);
      result_hist[iM]->SetMarkerSize(2);
   }

}
