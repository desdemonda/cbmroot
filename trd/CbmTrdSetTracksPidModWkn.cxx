// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidModWkn source file          -----
// -----                                  -----
// -------------------------------------------------------------------------
#include "CbmTrdSetTracksPidModWkn.h"

#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"
#include "CbmTrdPoint.h"
#include "CbmTrackMatch.h"
#include "CbmMCTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

#ifdef HAVE_SSE
  #include "P4_F32vec4.h"
#else
  #include "PSEUDO_F32vec4.h"
  #error NoSseFound
#endif // HAVE_SSE

#include <vector>
#include <cmath>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrdSetTracksPidModWkn::CbmTrdSetTracksPidModWkn() 
  : FairTask(),
    fTrackArray(NULL),
    fTrdHitArray(NULL),
    fnSet(0),
    fdegWkn(0),
    fk1(0),
    fwkn0(0),
    fEmp(0),
    fXi(0),
    fWmin(0),
    fWmax(0),
    fDiff(0),
    fSISType("sis300")
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmTrdSetTracksPidModWkn::CbmTrdSetTracksPidModWkn(const char* name,
				 const char* title)
  : FairTask(name),
    fTrackArray(NULL),
    fTrdHitArray(NULL),
    fnSet(0),
    fdegWkn(0),
    fk1(0),
    fwkn0(0),
    fEmp(0),
    fXi(0),
    fWmin(0),
    fWmax(0),
    fDiff(0),
    fSISType("sis300")
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdSetTracksPidModWkn::~CbmTrdSetTracksPidModWkn() { }
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmTrdSetTracksPidModWkn::Init() {

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdSetTracksPidModWkn::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TrdTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("TrdTrack");
  if ( ! fTrackArray) {
    cout << "-E- CbmTrdSetTracksPidModWkn::Init: No TrdTrack array!" << endl;
    return kERROR;
  }

  // Get TrdHit array
  fTrdHitArray  = (TClonesArray*) ioman->GetObject("TrdHit");
  if ( ! fTrdHitArray) {
    cout << "-E- CbmTrdSetTracksPidModWkn::Init: No TrdHit array!" << endl;
    return kERROR;
  }

  SetParameters();  
//  fmom = new TH1D("(p_{rec}-p_{mc})/p_{mc}","",400, -1, 1);
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdSetTracksPidModWkn::Exec(Option_t* opt) {

  TStopwatch timer;
  timer.Start();

  if ( !fTrackArray ) return;
  
  float ti=0, 
        mom=0;

  std::vector<float> eLossVector; // vector for energy losses

  fvec* mWkn = new fvec[fnSet];
  fvec resWkn=0; 
  fvec numTr=0;
 
  int nTracks_SIMD = fvecLen,
      NHits=0,
      iV = 0;

  unsigned short nTracks = fTrackArray->GetEntriesFast();
  unsigned short nTrue = 0;
  
  for(unsigned short itr = 0; itr < nTracks; itr++) {
    CbmTrdTrack* pTr = (CbmTrdTrack*)fTrackArray->At(itr); 
    if ((pTr->GetNofHits()) > fnSet-1) nTrue ++;
	else pTr->SetPidWkn(-2.);
	}   

   for(unsigned short itrack = 0; itrack < nTrue; itrack++) {
    eLossVector.clear();

    CbmTrdTrack* pTrack = (CbmTrdTrack*)fTrackArray->At(itrack); 
    NHits = pTrack->GetNofHits();
    numTr[iV] = itrack;
    mom = fabs(1.f/pTrack->GetParamFirst()->GetQp());

	fEmp = 0.0002598*mom*mom*mom-0.008862*mom*mom+0.1176*mom+0.9129;
    fXi = 0.00008938*mom*mom*mom-0.003022*mom*mom+0.03999*mom+0.5292;

    for (Int_t iHit=0; iHit < NHits; iHit++){
        Int_t TRDindex = pTrack->GetHitIndex(iHit);
        CbmTrdHit* trdHit = (CbmTrdHit*) fTrdHitArray->At(TRDindex);
        eLossVector.push_back((trdHit->GetELoss())*1000000);
    }

    for (int jVec = 0; jVec<eLossVector.size(); jVec++) 
        eLossVector[jVec]=(eLossVector[jVec]-fEmp)/fXi-0.225;

    sort(eLossVector.begin(), eLossVector.end());

    for (int jVec = 0; jVec<eLossVector.size(); jVec++) 
        eLossVector[jVec]=TMath::LandauI(eLossVector[jVec]);

    for (Int_t iHit=0; iHit < fnSet; iHit++)
        mWkn[iHit][iV] = eLossVector[NHits-fnSet+iHit];

    iV++;

    if(!((iV==fvecLen) || ((itrack==(nTrue-1)) && (iV!=0)) )) continue;

    fvec sumWkn=0;
    for (Int_t iHit=0; iHit < fnSet; iHit++) {
        ti = iHit + 1;
        fvec g1 = (ti-1)/fnSet-mWkn[iHit];
        fvec g2 = ti/fnSet-mWkn[iHit];
        fvec s1=1, s2=1;
        for(int iPow=0; iPow <5; iPow++){
            s1*=g1;
            s2*=g2;
        }
        sumWkn += s1-s2;
    }
	
 //     resWkn = -fwkn0*sumWkn;
 //    resWkn = 2*(-fwkn0*sumWkn-fWmin)/fDiff-1;  //[-1;1]
    resWkn = (-fwkn0*sumWkn-fWmin)/fDiff;  //[0;1]



    for (Int_t iHit=0; iHit < iV; iHit++) {
        CbmTrdTrack* pTrack2 = (CbmTrdTrack*)fTrackArray->At(numTr[iHit]); 
	    pTrack2->SetPidWkn(resWkn[iHit]);
    }
	
    iV=0;
  }
  
  delete[] mWkn;

  timer.Stop();
  static int nEv = 0;
  nEv++;
  static Double_t rtime = 0;
  rtime += timer.RealTime();
  static Double_t ctime = 0;
  ctime += timer.CpuTime();
 cout << endl << endl;
  cout <<"--CbmTrdSetTracksPidModWkn--"<< endl;
  cout << "Real time " << (rtime/Double_t(nEv))<< " s, CPU time " << (ctime/Double_t(nEv))
       << "s" << endl << endl;
}


void CbmTrdSetTracksPidModWkn::SetParameters() {
    if (fSISType == "sis300") {
	    fnSet = 5; //  number of the layers with TR
        fdegWkn = 4; //  statistics degree
	}
	
	if (fSISType == "sis100") {
	    fnSet = 2; //  number of the layers with TR
        fdegWkn = 2; //  statistics degree
	}
							   
    fk1=fdegWkn+1;
    fwkn0 = pow (fnSet,0.5*fdegWkn)/fk1;
	fWmin = 1/(pow(2,fdegWkn)*pow(fnSet,fdegWkn/2)*(fdegWkn+1)),
	fWmax = pow(fnSet,fdegWkn/2)/(fdegWkn+1),
	fDiff = fWmax - fWmin;
 }


// -----   Public method Finish   ------------------------------------------
void CbmTrdSetTracksPidModWkn::Finish() {
//fmom->Write();
 }
// -------------------------------------------------------------------------

ClassImp(CbmTrdSetTracksPidModWkn)
