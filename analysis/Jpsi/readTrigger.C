#include "iostream.h"
void readTrigger()
{
  TLorentzVector eplus[50], eminus[50];
  TLorentzVector momentum,  momentum_pair;
  TVector3 mom;
  Float_t px, py, pz;
  Int_t pdg;
  Int_t nplus=0, nminus=0, njpsi=0; 

  TH1F *hEl = new TH1F("hEl","number of electrons", 10,0,10);
  TH1F *hPtel = new TH1F("hPtel"," dN/dPt ",100,0,4);
  TH1F *hPel = new TH1F("hPel"," dN/dP ",100,0,30);
   
 
  // Load basic libraries


  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libPhysics");
  
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libJpsi.so");

  Char_t fileName[100];
  Int_t fNevents = 0;
  Int_t fElectronEvents=0; 
  for (Int_t ifile=0; ifile<1; ifile++)
    {
      TFile *f1 = new TFile(Form("jpsitrigger.25gev.pure.0000.root",ifile));
      TTree* t1 =(TTree*) f1->Get("cbmsim");
      TFolder *fd1 =(TFolder*)  f1->Get("cbmout");

      TClonesArray* fJpsi = (TClonesArray*)fd1->FindObjectAny("CbmJpsiTriggerElectron");
      t1->SetBranchAddress("CbmJpsiTriggerElectron",&fJpsi);
      
      
      Int_t nevent = t1->GetEntries();
      cout<<nevent<<endl;
      nplus=0; nminus=0;
      for(Int_t i=0; i<nevent; i++)
	{
	  nplus = 0;  
	  nminus = 0;
	  fNevents++;
	  t1->GetEntry(i);
	  Int_t nel = fJpsi->GetEntries();
	  for ( Int_t iel=0; iel<nel; iel++)
	    { 
	      CbmJpsiTriggerElectron *jel = (CbmJpsiTriggerElectron*)fJpsi->At(iel);
	      mom = jel->GetMomentum();
	      hPel->Fill(mom.Mag());
	      hPtel->Fill(mom.Pt());
	      if (mom.Pt() < 1.2) continue;
	      momentum.SetVectM(mom,0.000511);
	      pdg = jel -> GetPdg();
	      Double_t ann = jel -> GetAnn();
	      Double_t like = jel -> GetLike();
	      Double_t wkn = jel -> GetWkn();
	      Double_t ran = gRandom->Rndm();
	        if(ann > 0.8 ) 
		{
		  eplus[nplus]=momentum; 
		  nplus++;
		  eplus[nplus].SetXYZT(0,0,0,0);

		}	
	    }
	  hEl->Fill(nplus);
	
	  if ( nplus>1 )   fElectronEvents++;  
    } // event loop
 
      f1->Close();
    }

  cout<< " ElectronEvents "<< fElectronEvents<<endl;
 TFile *fmass = new TFile("jpsiTrigger.hist.root","RECREATE");
 
  hEl->Scale(1./Float_t (fNevents));
  hEl->Write();
  hPel->Scale(1./Float_t (fNevents ));
  hPel->Write();
  hPtel->Scale(1./Float_t (fNevents));
  hPtel->Write();
}







	
