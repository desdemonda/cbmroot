
// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidWkn source file          -----
// -----                  Created 13/02/07 by F.Uhlig                  -----
// -------------------------------------------------------------------------
#include "CbmTrdSetTracksPidWkn.h"

#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmTrdSetTracksPidWkn::CbmTrdSetTracksPidWkn() 
  : FairTask(),
    fTrackArray(NULL),
    fTrdHitArray(NULL),
    fNofTracks(0),
    WknPar1(1.279),
    WknPar2(0.61)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdSetTracksPidWkn::CbmTrdSetTracksPidWkn(const char* name,
				 const char* title)
  : FairTask(name),
    fTrackArray(NULL),
    fTrdHitArray(NULL),
    fNofTracks(0),
    WknPar1(1.279),
    WknPar2(0.61)
 
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdSetTracksPidWkn::~CbmTrdSetTracksPidWkn() { }
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void CbmTrdSetTracksPidWkn::SetParContainers()
{

}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmTrdSetTracksPidWkn::Init() {

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdSetTracksPidWkn::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TrdTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("TrdTrack");
  if ( ! fTrackArray) {
    cout << "-E- CbmTrdSetTracksPidWkn::Init: No TrdTrack array!"
	 << endl;
    return kERROR;
  }

  // Get TrdHit array
  fTrdHitArray  = (TClonesArray*) ioman->GetObject("TrdHit");
  if ( ! fTrdHitArray) {
    cout << "-E- CbmTrdSetTracksPidWkn::Init: No TrdHit array!"
	 << endl;
    return kERROR;
  }

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdSetTracksPidWkn::Exec(Option_t* opt) {

  if ( !fTrackArray ) return;

  Int_t nTracks = fTrackArray->GetEntriesFast();

  Double_t result_wkn;

  Int_t i,j,k,n;
  Int_t Ndec = 12;


  fNofTracks = 0;

  Double_t Ye_tmp[Ndec];     // temporary array for calculations
  Double_t tmp[Ndec];
  Int_t index_sorted[Ndec];
  Double_t Ye_tmp_new[Ndec];     // temporary array for calculations
  Double_t eloss[Ndec];

  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {

    CbmTrdTrack* pTrack = (CbmTrdTrack*)fTrackArray->At(iTrack);

    // Up to now only for tracks with twelve hits the Wkn can be calculated
    // This should be changed in the future.
    if (pTrack->GetNofHits() < 12 ) {
      fNofTracks++;
      pTrack->SetPidWkn(-2.);
      continue;
    }

    for (Int_t iTRD=0; iTRD < pTrack->GetNofHits(); iTRD++){
      Int_t TRDindex = pTrack->GetHitIndex(iTRD);
      CbmTrdHit* trdHit = (CbmTrdHit*) fTrdHitArray->At(TRDindex);
      eloss[iTRD] = trdHit->GetELoss();
    }

    // calculate the lambda parameter for each TRD layer
    for (i=0;i<Ndec;i++)
    {
      tmp [i] = eloss[i]*1.e6;
      tmp [i] = (tmp[i]-WknPar1)/WknPar2-0.225;
    }

    // sort array tmp in increasing order
    TMath::Sort(Ndec, (const Double_t *)tmp, index_sorted , kFALSE);

    for (i=0;i<Ndec;i++) {
      Ye_tmp [i] =  TMath::LandauI (tmp [index_sorted[i]]);
    }


    i = 1;            //  first index used in the ordered sample
    k = 8;            //  statistics degree
    n = Ndec - i + 1; //  sample size

    for (j=0;j<n;j++) {
      Ye_tmp_new[j] = Ye_tmp [i+j-1];
    }

    // calculate the Wkn and add the information to the TrdTrack
    result_wkn  = CbmWknStat (Ye_tmp_new, k, n);
    pTrack->SetPidWkn(result_wkn);

  }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmTrdSetTracksPidWkn::Finish() { }
// -------------------------------------------------------------------------

Double_t CbmTrdSetTracksPidWkn::CbmWknStat (Double_t *Y,Int_t k, Int_t n)
{
  Double_t
    wkn0,
    S = 0,
    ty,ti,
    k1=k+1;
  wkn0 = pow (n,0.5*k)/k1;
  for (Int_t i=0;i<n;i++)
    {
      ty = Y[i];  ti = i;
      S += pow((ti-1)/n-ty,k1)-pow(ti/n-ty,k1);
    }
  return (-wkn0*S);
}


ClassImp(CbmTrdSetTracksPidWkn)
