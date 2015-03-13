#include <iostream>

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "StsCosyTrack.h"
#include "CbmStsHit.h"

#include "TMath.h"
#include "TF1.h"
#include "TGraphErrors.h"

using std::cout;
using std::endl;



StsCosyTrack::StsCosyTrack() :
  FairTask("StsCosyTrack",1),
  fHits(NULL),
  fDigiArray(NULL),
  fNDigis(0),
  fAlignment(kFALSE),
  fChi2X(1000),
  fChi2Y(1000)
{ 
  fChain = new TChain("cbmsim");
}

StsCosyTrack::~StsCosyTrack() 
{
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
  if ( fDigiArray ) {
    fDigiArray->Delete();
    delete fDigiArray;
  }

}


InitStatus StsCosyTrack::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
    {
      LOG(FATAL) << "StsCosyTrack::Init: noRootManager !" << FairLogger::endl; 
      return kFATAL;
    }
  // Get input arrays
  
  fHits =(TClonesArray *)  ioman->GetObject("StsHit");
  if ( ! fHits ) {
    LOG(ERROR) << "No StsHits array!" << FairLogger::endl;
    LOG(ERROR) << "Task will be inactive" << FairLogger::endl;
    return kERROR;
  }

  for(int i=0;i<5;i++)
    fMapPts.insert(make_pair(i,vector<CbmStsHit*>()));   
	
   for (Int_t i=0;i<5;i++)
     {
       Char_t buf[25]; 
       sprintf(buf,"ResX%i",i); 
       h_resX[i]      = new TH1F(buf,buf,4000, -1,1);
       sprintf(buf,"ResY%i",i); 
       h_resY[i]      = new TH1F(buf,buf,4000, -1,1);
       sprintf(buf,"PullX%i",i); 
       h_pullX[i]      = new TH1F(buf,buf,400, -100.,100.);
       sprintf(buf,"PullY%i",i); 
       h_pullY[i]      = new TH1F(buf,buf,400, -100.,100.);       
       sprintf(buf,"XY_sts_%i",i);
       XY[i] = new TH2D(buf,buf,400, -1, 1,400, -1, 1);
     }

   for (Int_t i=0;i<3;i++)
     {
       Char_t buf[25]; 
       sprintf(buf,"chi2change_sts_%i",i); 
       chi_change[i]= new TH1F(buf,buf,1000, -1,1000);
     }

   h_chi2       = new TH1F("chi2","chi2",400, 0.,20.);
   h_pchi2      = new TH1F("pchi2","pchi2",400, 0.,1.);
   y_chi2       = new TH1F("Ychi2","Ychi2",400, 0.,20.);
   y_pchi2      = new TH1F("Ypchi2","Ypchi2",400, 0.,1.);

   h_xx01 = new TH2F("XX01","XX01",400, -2.,2.,400, -2.,2.);
   h_xx21 = (TH2F*)h_xx01->Clone("XX21");
   h_xx02 = (TH2F*)h_xx01->Clone("XX02");
   h_yy01 = (TH2F*)h_xx01->Clone("YY01");
   h_yy21 = (TH2F*)h_xx01->Clone("YY21");
   h_yy02 = (TH2F*)h_xx01->Clone("YY02");
   
   cout << "-I- StsCosyTrack: Intialisation successfull " << kSUCCESS<< endl;
   return kSUCCESS;

}

// -----   Public method Exec   --------------------------------------------
void StsCosyTrack::Exec(Option_t* opt) {
    
  Reset(); 
  CbmStsHit* hit = NULL;
  Int_t nofHits = fHits->GetEntries();
 
  int hit_is[5]={-1,-1,-1,-1,-1};

  if(nofHits<5)return;
  
  // Loop over hits
  for (Int_t iHit=0; iHit<nofHits; iHit++) 
    {
      hit = (CbmStsHit*) fHits->At(iHit);
      
      // Determine sector type and channel numbers
      
      Int_t detId  = hit->GetSectorNr();
      vector<CbmStsHit*>& vlist =  fMapPts[detId];
      vlist.push_back((CbmStsHit*) hit); 
    } 
  
  // Check the content per detector   
  for (int idet=0; idet<5;idet++)
    {
      // get the vector     
      vector<CbmStsHit*>& v =  fMapPts[idet];
      // select at least 1 points per detector
      if (v.size()==0) return ;
      // select multiplicity per detector
      //if (v.size()>1) return ;
     
      //     cout << "-I- StsCosyTrack idet# " << idet <<  " has " << v.size() << " hits " << endl;    
    }
    
  // Find the tracks 
   FindTracks();
     
}


Bool_t StsCosyTrack::FindTracks(){

  Double_t align[2][3];
  for(int i=0; i<2; i++)for(int j=0; j<3; j++)align[i][j]=0;

  if(fAlignment)
  {
    
    align[0][0] = 0.00722846;
    align[0][1] = -0.104797;
    align[0][2] = 0.00385107;
  
    align[1][0] = -0.00437047;
    align[1][1] = 0.0421997;
    align[1][2] = -0.00237545;
  }

  
  // Check the content per detector   
  for (int idet=0; idet<5;idet++)
    {
      // get the vector     
      vector<CbmStsHit*>& v =  fMapPts[idet];
      // select at least 1 points per detector
      if (v.size()==0) return kFALSE;
    }

  
//  vector<CbmStsHit*>& v_hodo0 =  fMapPts[0];
//  vector<CbmStsHit*>& v_hodo1 =  fMapPts[4];
  vector<CbmStsHit*>& v_sts0 =  fMapPts[1];
  vector<CbmStsHit*>& v_sts2 =  fMapPts[2];
  vector<CbmStsHit*>& v_sts1 =  fMapPts[3];
    
  CbmStsHit* pt0=NULL; 
  CbmStsHit* pt1=NULL; 
  CbmStsHit* pt2=NULL; 
  
  TVector3 sts0_pos, sts1_pos, sts2_pos;
  int Npoints = 3;
  
  double x[Npoints];
  double ex[Npoints];
  double z[Npoints];
  double ez[Npoints];
  double y[Npoints];
  double ey[Npoints];
  
  for(Int_t i=0; i< v_sts0.size() ; i++) 
    {
      pt0 = (CbmStsHit*) v_sts0[i];
      pt0->Position(sts0_pos); 
      x[0]= sts0_pos.X()-align[0][0];
      y[0]= sts0_pos.Y()-align[1][0];
      z[0]= sts0_pos.Z();
      ex[0]= pt0->GetDx();
      ey[0]= pt0->GetDy();
      ez[0]= 0.5;
      for(Int_t j=0; j< v_sts2.size() ; j++) 
	{
	  pt1 = (CbmStsHit*) v_sts2[j];
	  pt1->Position(sts1_pos); 
	  x[1]= sts1_pos.X()-align[0][1];
	  y[1]= sts1_pos.Y()-align[1][1];	    
	  z[1]= sts1_pos.Z();
	  ex[1]= pt1->GetDx();
	  ey[1]= pt1->GetDy();
	  ez[1]= 0.5;
	  for(Int_t k=0; k< v_sts1.size() ; k++) 
	    {
	      pt2 = (CbmStsHit*) v_sts1[k];
	      pt2->Position(sts2_pos); 
	      x[2]= sts2_pos.X()-align[0][2];
	      y[2]= sts2_pos.Y()-align[1][2];	    
	      z[2]= sts2_pos.Z();
	      ex[2]= pt2->GetDx();
	      ey[2]= pt2->GetDy();
	      ez[2]= 0.5;
	      
	      h_xx01->Fill(x[0],x[1]);
	      h_xx21->Fill(x[2],x[1]);
	      h_xx02->Fill(x[0],x[2]);
	      h_yy01->Fill(y[0],y[1]);
	      h_yy21->Fill(y[2],y[1]);
	      h_yy02->Fill(y[0],y[2]);
	      
	      // Fit data numerically:
	      // ---------------------
	      TGraphErrors* graph = new TGraphErrors(Npoints,z,x,ez,ex);
	      TF1 *fit = new TF1("fit", "[0] + [1]*x");
	      
	      graph->Fit("fit","Q");
	      
	      double Chi2 = fit->GetChisquare();
	      double Ndof = fit->GetNDF();
	      double p0 = fit->GetParameter(0);
	      double p1 = fit->GetParameter(1);
	      double Prob = fit->GetProb();
	      
	      h_chi2->Fill(Chi2/Ndof);
	      h_pchi2->Fill(Prob);

	      TGraphErrors* graph1 = new TGraphErrors(Npoints,z,y,ez,ey);
	      TF1 *fit1 = new TF1("fit1", "[0] + [1]*x");
	      graph1->Fit("fit1","Q");
	      
	      double yChi2 = fit1->GetChisquare();
	      double yNdof = fit1->GetNDF();
	      double yProb = fit1->GetProb();
	      double p2 = fit1->GetParameter(0);
	      double p3 = fit1->GetParameter(1);
	      
	      y_chi2->Fill(yChi2/yNdof);
	      y_pchi2->Fill(yProb);
	      
	      graph1->Delete();
	      graph->Delete();
	      fit->Delete();
 	      fit1->Delete();

	      if(Chi2/Ndof > fChi2X || yChi2/yNdof > fChi2Y)continue;
	      
	      h_resX[0]->Fill(x[0]-p0-p1*z[0]);
	      h_resX[1]->Fill(x[1]-p0-p1*z[1]);
	      h_resX[2]->Fill(x[2]-p0-p1*z[2]);	      

	      h_resY[0]->Fill(y[0]-p2-p3*z[0]);
	      h_resY[1]->Fill(y[1]-p2-p3*z[1]);
	      h_resY[2]->Fill(y[2]-p2-p3*z[2]);

	      XY[0]->Fill(x[0],y[0]);
	      XY[1]->Fill(x[1],y[1]);
	      XY[2]->Fill(x[2],y[2]);

     	    }//!sts2
	}//!sts1
    }//!sts0
  
  return kTRUE;
}


void StsCosyTrack::Reset() {
  fNDigis = 0;
  if ( fDigiArray ) fDigiArray->Delete();
  fMapPts.clear();
}


void StsCosyTrack::Finish(){
 
  h_chi2->Write();
  h_pchi2->Write();

  y_chi2->Write();
  y_pchi2->Write();
  
  for(Int_t i=0;i<3;i++){h_resX[i]->Write(); h_resY[i]->Write();}
  

  h_xx01->Write();
  h_xx21->Write();
  h_xx02->Write();
  
  h_yy01->Write();
  h_yy21->Write();
  h_yy02->Write();
  
  for(Int_t i=0;i<3;i++)XY[i]->Write();
   
  // TFile *fout = FairRunAna::Instance()->GetOutputFile();
  // if (fout) fout->Write();
}




ClassImp(StsCosyTrack)
