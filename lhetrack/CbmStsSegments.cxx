///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// CbmStsSegments class - class                                                  //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#include "CbmStsSegments.h"
#include "CbmStsHit.h"
#include "CbmStsCMTrack.h"
#include "CbmStsCMPoint.h"
#include "CbmStsVertex.h"

#include "TObjArray.h"

#include "iomanip.h"

ClassImp(CbmStsSegments)

//________________________________________________________________
CbmStsSegments::CbmStsSegments() {
// Default constructor.

  fSegments = NULL;

  fNumOfStation = 7;                   // The number of stations
  fNumBetaSegment = 50; // 20
  fNumAlphaSegment = 200; //24;  // 10;
  fBounds = fNumOfStation * fNumBetaSegment * fNumAlphaSegment;

}

//________________________________________________________________
CbmStsSegments::CbmStsSegments(Int_t nStation,
			       Int_t nBeta,
			       Int_t nAlpha) {
//
  fSegments = NULL;

  fNumOfStation = nStation;   // The number of stations
  fNumBetaSegment = nBeta;    //
  fNumAlphaSegment = nAlpha;  //
  fBounds = fNumOfStation * fNumBetaSegment * fNumAlphaSegment;

}

//_________________________________________________________________
CbmStsSegments::~CbmStsSegments() {

}

//________________________________________________________________
void CbmStsSegments::CalcAlphaBetaLimits(CbmStsVertex *vtx) {

// Calculates the min. and max. value of beta  and alpha with the
// given main vertex.

  fVertex = vtx;

  fAlphaMax = TMath::ATan2(50., (100. - fVertex->GetZ()));
  fAlphaMin = -fAlphaMax;

  fBetaMax = TMath::ATan2(50., (100. - fVertex->GetZ()));
  fBetaMin = -fBetaMax;

}

//________________________________________________________________
void  CbmStsSegments::FillSegments(TObjArray *fCMHits) {


  if (fSegments) fSegments->Delete();

  fSegments = new TObjArray(fBounds);     //

  for (Int_t iv = 0; iv < fBounds; iv++) {
    fSegments->AddAt(new TObjArray(0), iv);
  }

  CbmStsCMPoint *h = NULL;

  for (Int_t i = 0; i < fCMHits->GetEntriesFast(); i++) {
    h = (CbmStsCMPoint *)fCMHits->At(i);
    h->Setup(fVertex);
    h->SetUsage(kFALSE);
    Int_t iseg = GetSegm(GetStationSegm(h), GetBetaSegm(h), GetAlphaSegm(h));
    cout<<"Hit: "<<i<<endl;
    ((TObjArray *)fSegments->At(iseg))->AddLast(h);

  }
}

//________________________________________________________________
void CbmStsSegments::PrintSegmentContents() {

// This function loops over all hits in segment

  TObjArray *segment;
  CbmStsCMPoint *hit;

  // loop over the  stations

  for (Int_t st_num = fNumOfStation-1; st_num > 1; st_num--) {

    // loop over beta segments
    for (Int_t beta_segm_counter = 0;
	 beta_segm_counter < fNumBetaSegment;
	 beta_segm_counter++) {

      // go over beta in two directions, one segment in each direction alternately
      Int_t beta_segm;

        beta_segm = beta_segm_counter;

      // loop over alpha segments
      for(Int_t alpha_segm = 0; alpha_segm < fNumAlphaSegment; alpha_segm++) {
	
	// loop over entries in one segment
        segment = (TObjArray *)fSegments->At(GetSegm(st_num, beta_segm, alpha_segm));
        Int_t entries = segment->GetEntriesFast();

	    cout << "\n segment " <<
	      GetSegm(st_num, beta_segm, alpha_segm) << ": ";
        if (entries) {

          for (Int_t hit_num = 0; hit_num < entries; hit_num++) {
            hit = (CbmStsCMPoint *)segment->At(hit_num);

	    cout << "\n hit ";
	    cout << " geant # " << hit->GetTrackID();
	    cout << " station " << hit->GetStation();
	    cout << " beta =  " << beta_segm;
	    cout << " alpha = " << alpha_segm;
          }
        }    // if (entries)

      }
    }
  }

}
