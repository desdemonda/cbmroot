// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidLike source file          -----
// -----                  Created 25/02/07 by F.Uhlig                  -----
// -------------------------------------------------------------------------
#include "CbmTrdSetTracksPidLike.h"

#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrdGas.h"

#include "CbmGlobalTrack.h"
#include "FairRootManager.h"
#include "CbmStsTrack.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TString.h"
#include "TMath.h"
#include "TROOT.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrdSetTracksPidLike::CbmTrdSetTracksPidLike()
  : FairTask(),
    fTrackArray(NULL), 
    fTrdHitArray(0), 
    fglobalTrackArray(NULL),
    fstsTrackArray(NULL),
    fHistdEdx(NULL), 
    fNofTracks(0), 
    fTrackMomentum(NULL), 
    fNBins(0), 
    fBinSize(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdSetTracksPidLike::CbmTrdSetTracksPidLike(const char* name,
				 const char* title)
  : FairTask(name), 
    fTrackArray(NULL), 
    fTrdHitArray(0), 
    fglobalTrackArray(NULL),
    fstsTrackArray(NULL),
    fHistdEdx(NULL), 
    fNofTracks(0), 
    fTrackMomentum(NULL), 
    fNBins(0), 
    fBinSize(0)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdSetTracksPidLike::~CbmTrdSetTracksPidLike() { }
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmTrdSetTracksPidLike::SetParContainers()
{

}
// -------------------------------------------------------------------------


// -----  RaedData -------------------------------------------------
Bool_t CbmTrdSetTracksPidLike::ReadData()
{
  //
  // Read the TRD dEdx histograms.
  //

  // Get the name of the input file from CbmTrdGas
  // This file stores all information about the gas layer of the TRD
  // and can construct the required file name

  CbmTrdGas *fTrdGas = CbmTrdGas::Instance();
  if (fTrdGas==0) {
     fTrdGas = new CbmTrdGas();
     fTrdGas->Init();
  }
  TString inputFile = fTrdGas->GetFileName("Like");

  // Open ROOT file with the histograms
  TFile *histFile = new TFile(inputFile, "READ");
  if (!histFile || !histFile->IsOpen()) {
    cout << " -E- CbmTrdSetTracksPidWkn::ReadData "<<endl;
    cout << " -E- Could not open input file." <<inputFile<< endl;
    return kFALSE;
  } else {
    cout <<" -I- CbmTrdSetTracksPidLike::ReadData " << endl;
    cout <<" -I- inputFile: " << inputFile << endl;
  }

  gROOT->cd();

  // Read the histograms and store them at the right position in
  // the TObjectArray
  Char_t text[200];
  for (Int_t particle = 0; particle < fgkNParts; ++particle)
  {
    const Char_t* particleKey = "";
    switch (particle)
    {
      case CbmTrdSetTracksPidLike::kElectron: particleKey = "EL"; break;
      case CbmTrdSetTracksPidLike::kPion:     particleKey = "PI"; break;
      case CbmTrdSetTracksPidLike::kKaon:     particleKey = "KA"; break;
      case CbmTrdSetTracksPidLike::kProton:   particleKey = "PR"; break;
      case CbmTrdSetTracksPidLike::kMuon:     particleKey = "MU"; break;
    }

    for (Int_t imom = 0; imom < fgkNMom; imom++)
    {
      if (imom<9){
        sprintf(text, "h1dEdx%s%01d", particleKey, imom+1);
      }else {
        sprintf(text, "h1dEdx%s%02d", particleKey, imom+1);
      }
      //      cout <<"Text: "<<text<<endl;
      TH1F* hist = (TH1F*)histFile->Get(text)->Clone();
      if (hist->Integral() > 0.) hist->Scale(1.0/hist->Integral());
      fHistdEdx->AddAt(hist, GetHistID(particle, imom));

    }
  }

  histFile->Close();
  delete histFile;

  // Calculate number of bins and bin size
  TH1F* hist = (TH1F*) fHistdEdx->At(GetHistID(CbmTrdSetTracksPidLike::kPion, 1));
  fNBins   = hist->GetNbinsX();
  fBinSize = hist->GetBinWidth(1);

  return kTRUE;

}

//_________________________________________________________________________
// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmTrdSetTracksPidLike::Init() {

  //
  // Initalize data members
  //

  fTrackMomentum = new Double_t[fgkNMom];
  Double_t trackMomentum[fgkNMom] = {0.25, 0.5, 1.0, 1.5, 2.0, 3.0, 4.0,
                                   5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  for (Int_t imom = 0; imom < fgkNMom; imom++) {
    fTrackMomentum[imom] = trackMomentum[imom];
  }

  fHistdEdx = new TObjArray(fgkNParts * fgkNMom);
  fHistdEdx->SetOwner();                        // ?????

  fNBins = 0;
  fBinSize = 0;

  // Read the data from ROOT file. In case of problems return kFATAL;

  /*
  if(!inputFile) {
    cout << "-E- CbmTrdSetTracksPidLike::Init: "
	 << "No input file for histos set." << endl;
    return kFATAL;
  }
  */

  if (!ReadData()) return kFATAL;


  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdSetTracksPidWkn::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }




 // Get GlobalTack array
  fglobalTrackArray  = (TClonesArray*) ioman->GetObject("GlobalTrack");
  if ( ! fglobalTrackArray) {
    cout << "-E- CbmTrdSetTracksPidLike::Init:! No GlobalTack array!"
         << endl;
    return kFATAL;
  }

  // Get StsTrack array
  fstsTrackArray  = (TClonesArray*) ioman->GetObject("StsTrack");
  if ( ! fstsTrackArray) {
    cout << "-E-  CbmTrdSetTracksPidLike::Init:! No StsTrack array!"
         << endl;
    return kFATAL;
  }


  // Get TrdTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("TrdTrack"); //=>SG
  if ( ! fTrackArray) {
    cout << "-E- CbmTrdSetTracksPidWkn::Init: No TrdTrack array!"
	 << endl;
    return kFATAL;
  }

  // Get TrdTrack array
  fTrdHitArray  = (TClonesArray*) ioman->GetObject("TrdHit"); //=>SG
  if ( ! fTrdHitArray) {
    cout << "-E- CbmTrdSetTracksPidWkn::Init: No TrdHit array!"
	 << endl;
    return kFATAL;
  }

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdSetTracksPidLike::Exec(Option_t* opt) {

  Double_t momentum;
  Double_t prob[fgkNParts];
  Double_t probTotal;


  if ( !fTrackArray ) return;

  Int_t nTracks = fglobalTrackArray->GetEntriesFast();
  //Int_t nTracks = fTrackArray->GetEntriesFast();
  cout << nTracks <<" global tracks" << endl;

  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {

    CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fglobalTrackArray->At(iTrack);


    Int_t stsTrackIndex = gTrack->GetStsTrackIndex();
    if (stsTrackIndex == -1) {
      //      cout <<" -W- CbmTrdSetTracksPidLike::Exec : no Sts track"<<endl;
      continue;
    }

    Int_t trdTrackIndex = gTrack->GetTrdTrackIndex();
    if (trdTrackIndex == -1) {
      //  cout <<" -W- CbmTrdSetTracksPidLike::Exec : no Trd track"<<endl;
      continue;
    }

    CbmTrdTrack* pTrack = (CbmTrdTrack*)fTrackArray->At(trdTrackIndex);
    if (!pTrack){
      cout <<" -W- CbmTrdSetTracksPidLike::Exec : no Trd track pointer"<<endl;
      continue;
    }



    CbmStsTrack* stsTrack = (CbmStsTrack*)fstsTrackArray->At(stsTrackIndex);
    if (!stsTrack){
      cout <<" -W- CbmTrdSetTracksPidLike::Exec : no Sts track pointer"<<endl;
      continue;
    }


    /*
    CbmTrdTrack* pTrack = (CbmTrdTrack*)fTrackArray->At(iTrack);
    if (!pTrack) continue;
    */

    //    cout <<"Trd Hits: "<< pTrack->GetNofTrdHits() << endl;

    // Up to now only for tracks with twelve hits the Like can be calculated
    if (pTrack->GetNofHits() < 12 ) {
      fNofTracks++;
      continue;
    }



    probTotal = 0.0;
    for (Int_t iSpecies = 0; iSpecies < fgkNParts; iSpecies++) {
      prob[iSpecies] = 1.0;
    }


    //    Get the momentum from the first sts station
    if (TMath::Abs(stsTrack->GetParamFirst()->GetQp())>0.) {
      momentum = TMath::Abs(1./(stsTrack->GetParamFirst()->GetQp()));
    }
    else if (TMath::Abs(pTrack->GetParamFirst()->GetQp())>0.){
      momentum = TMath::Abs(1./(pTrack->GetParamFirst()->GetQp()));
    }
    else if (TMath::Abs(pTrack->GetParamLast()->GetQp())>0.){
      momentum = TMath::Abs(1./(pTrack->GetParamLast()->GetQp()));
    }
    else {
      cout <<" -W- CbmTrdSetTracksPidLike::Exec : Could not assign any momentum to the track"<<endl;
      momentum = 0;
    }

    //    cout <<"Momentum: "<<momentum<<endl;

    Double_t dEdx;

    for (Int_t iTRD=0; iTRD < pTrack->GetNofHits(); iTRD++){
      Int_t index = pTrack->GetHitIndex(iTRD);
      CbmTrdHit* trdHit = (CbmTrdHit*) fTrdHitArray->At(index);

      dEdx = trdHit->GetELoss()*1000000;

      for (Int_t iSpecies = 0; iSpecies < fgkNParts; iSpecies++) {

        prob[iSpecies] *= GetProbability(iSpecies, momentum, dEdx);

      }

    } // loop TRD hits

    for (Int_t iSpecies = 0; iSpecies < fgkNParts; iSpecies++) {
      probTotal +=  prob[iSpecies];
    }

    for (Int_t iSpecies = 0; iSpecies < fgkNParts; iSpecies++) {
      if (probTotal >0) {
        prob[iSpecies] /= probTotal;
      }
      else {
        prob[iSpecies] = -1.5;
      }
    }

    pTrack->SetPidLikeEL(prob[CbmTrdSetTracksPidLike::kElectron]);
    pTrack->SetPidLikePI(prob[CbmTrdSetTracksPidLike::kPion]);
    pTrack->SetPidLikeKA(prob[CbmTrdSetTracksPidLike::kKaon]);
    pTrack->SetPidLikePR(prob[CbmTrdSetTracksPidLike::kProton]);
    pTrack->SetPidLikeMU(prob[CbmTrdSetTracksPidLike::kMuon]);

  }
}
// -------------------------------------------------------------------------

Double_t CbmTrdSetTracksPidLike::GetProbability(Int_t k, Double_t mom, Double_t dedx) const
{
  //
  // Gets the Probability of having dedx at a given momentum (mom)
  // and particle type k from the precalculated de/dx distributions
  //

  Int_t    iEnBin = ((Int_t) (dedx/fBinSize+1));
  if(iEnBin > fNBins) iEnBin = fNBins;

  if (k < 0 || k > fgkNParts) {
    return 1;
  }

  TH1F* hist1 = 0;
  TH1F* hist2 = 0;
  Double_t mom1 = 0;
  Double_t mom2 = 0;

  // Lower limit
  if (mom<=fTrackMomentum[0])  {
    hist1 = (TH1F*) fHistdEdx->At(GetHistID(k,1));
    hist2 = (TH1F*) fHistdEdx->At(GetHistID(k,0));
    mom1 = fTrackMomentum[1];
    mom2 = fTrackMomentum[0];
  }

  // Upper Limit
  if(mom>=fTrackMomentum[fgkNMom-1]) {
    hist2 = (TH1F*) fHistdEdx->At(GetHistID(k,fgkNMom-1));
    hist1 = (TH1F*) fHistdEdx->At(GetHistID(k,fgkNMom-2));
    mom2 = fTrackMomentum[fgkNMom-1];
    mom1 = fTrackMomentum[fgkNMom-2];
  }

  // In the range
  for (Int_t ip=1; ip<fgkNMom; ip++) {
    if ((fTrackMomentum[ip-1]<= mom) && (mom<fTrackMomentum[ip])) {
      hist1 = (TH1F*) fHistdEdx->At(GetHistID(k,ip));
      hist2 = (TH1F*) fHistdEdx->At(GetHistID(k,ip-1));
      mom1 = fTrackMomentum[ip];
      mom2 = fTrackMomentum[ip-1];
    }
  }

  Double_t slop = (hist1->GetBinContent(iEnBin) - hist2->GetBinContent(iEnBin))
                / (mom1 - mom2);
  return hist2->GetBinContent(iEnBin) + slop * (mom - mom2);

}

// -----   Public method Finish   ------------------------------------------
void CbmTrdSetTracksPidLike::Finish() { }
// -------------------------------------------------------------------------


ClassImp(CbmTrdSetTracksPidLike)
