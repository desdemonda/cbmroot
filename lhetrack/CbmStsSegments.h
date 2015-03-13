//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// CbmStsSegments class -                                                       //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_SEGMENTS_H
#define CBM_STS_SEGMENTS_H

#include "Riostream.h"

#include "TClonesArray.h"
#include "CbmStsCMPoint.h"
#include "CbmStsVertex.h"

class CbmStsSegments {

protected:

  // Volume segments
  TObjArray *fSegments;       // array of volume (station, beta, alpha) elements

  Int_t  fNumOfStation;       //  the number of station
  Int_t  fNumBetaSegment;     // number of beta segments
  Int_t  fNumAlphaSegment;    // number of alpha segments
  Int_t  fBounds;             // Number of cells (segments)
  Int_t  fMaxSTStation;       // max number of silicon stations

  // max. min. alpha values
  Double_t fAlphaMin;
  Double_t fAlphaMax;

  // max. min. beta values
  Double_t fBetaMin;
  Double_t fBetaMax;

  CbmStsVertex *fVertex;
  
public:

  CbmStsSegments();                   //
  CbmStsSegments(Int_t nStation,
		 Int_t nBeta,
		 Int_t nAlpha);
  virtual ~CbmStsSegments();          //

  void  CalcAlphaBetaLimits(CbmStsVertex *vtx);      // calculates the limit for alpha

  void  FillSegments(TObjArray *hits);     //
  void  PrintSegmentContents();

  // getters

  Int_t  GetNumAlphaSegments() { return fNumAlphaSegment; }  // returns # of beta segments
  Int_t  GetNumBetaSegments()  { return fNumBetaSegment; }  // returns # of beta segments

  Int_t  GetBetaSegm(CbmStsCMPoint *hit);    // returns # of beta segment of a  hit
  Int_t  GetAlphaSegm(CbmStsCMPoint *hit);    // returns # of alpha segment of a hit
  Int_t  GetStationSegm(CbmStsCMPoint *hit);  // returns # of station segment of a hit

  Int_t  GetStationSegm(Int_t segm);        //  number of station segment of a segment
  Int_t     GetBetaSegm(Int_t segm);        //  number of beta segment of a  segment
  Int_t    GetAlphaSegm(Int_t segm);        //  number of alpha segment of a  segment

  Int_t   GetStation(Int_t segm);          // returns the station of a given station segment
  Double_t   GetBeta(Int_t segm);          // returns beta of a given beta segment
  Double_t  GetAlpha(Int_t segm);          // returns alpha of a given alpha segment

  TObjArray *GetSegments()    { return fSegments; }  //
  Int_t  GetSegm(Int_t stn, Int_t beta_s, Int_t alpha_s);  // returns number of segment

  ClassDef(CbmStsSegments, 1)  //  
};


//_________________________________________________________________
inline Int_t CbmStsSegments::GetStationSegm(CbmStsCMPoint *hit) {
  // Returns number of station segment of a specific hit.
  //  cout << "-I :GetStationSegm: " << hit->GetStation() << endl;
  return hit->GetStation() - 1;  // 
}


//_________________________________________________________________
inline Int_t CbmStsSegments::GetStation(Int_t segm) {
  // Returns the row number of a given row segment.

  return segm + 1;
}


//_________________________________________________________________
inline Int_t CbmStsSegments::GetBetaSegm(CbmStsCMPoint *hit) {
  // Returns number of beta segment of a specific hit.
  
  Double_t beta = hit->GetBeta();
  Int_t beta_segm;
  
  beta_segm = (Int_t)((beta - fBetaMin) * fNumBetaSegment/(fBetaMax - fBetaMin) );

 // cout << "-I GetBetaSegm " <<  hit->GetBeta() << " calc: "
 //       <<  beta_segm  << endl;
  return beta_segm;
}

//_________________________________________________________________
inline Double_t CbmStsSegments::GetBeta(Int_t segm) {
  // Returns the angle beta of a given segment.
  return +segm * (fBetaMax - fBetaMin)/ ((float) fNumBetaSegment) + fBetaMin;
}


//_________________________________________________________________
inline Int_t CbmStsSegments::GetAlphaSegm(CbmStsCMPoint *hit) {
  // Returns number of alpha segment of a specific hit.

  Double_t alpha = hit->GetAlpha();
  Int_t alpha_segm;
  
  alpha_segm = (Int_t)((alpha - fAlphaMin) * fNumAlphaSegment/(fAlphaMax - fAlphaMin) );
 // cout << "-I GetAlphaSegm " <<  hit->GetAlpha() << " calc: "
 //       <<  alpha_segm  << endl;

  return alpha_segm;
}

//_________________________________________________________________
inline Int_t CbmStsSegments::GetAlphaSegm(Int_t segm) {
  // Returns number of alpha segment of a specifiv segment.

  return (segm % (fNumBetaSegment * fNumAlphaSegment)) % (fNumAlphaSegment);
}


//_________________________________________________________________
inline Double_t CbmStsSegments::GetAlpha(Int_t segm) {
  // Returns the  alpha of the given segment.

  return +segm * (fAlphaMax - fAlphaMin)/ (fNumAlphaSegment) + fAlphaMin;
}


//_________________________________________________________________
inline Int_t CbmStsSegments::GetSegm(Int_t row_segm,
				     Int_t beta_segm,
				     Int_t alpha_segm) {
  // Calculates the volume segment number from the segmented volumes
  //   (segm = segm(station,beta,alpha)).

  Int_t segm = row_segm * (fNumBetaSegment * fNumAlphaSegment) +
    beta_segm * (fNumAlphaSegment) + alpha_segm;

  if (segm >= fBounds) {
      cout << " Error in segment " << row_segm << " "
          << GetStation(row_segm) << endl;
 //      cout << " segm " << segm << " Bounds " << fBounds << endl;
   return fBounds-1;
  }

  else return segm;
}


//_________________________________________________________________
inline Int_t CbmStsSegments::GetStationSegm(Int_t segm)
{
  // Returns number of station segment of a specifiv segment.

  return (segm - GetAlphaSegm(segm) - GetBetaSegm(segm)) /
    (fNumBetaSegment * fNumAlphaSegment);
}


//_________________________________________________________________
inline Int_t CbmStsSegments::GetBetaSegm(Int_t segm)
{
  // Returns number of beta segment of a specifiv segment.

  return (segm - GetAlphaSegm(segm)) % (fNumBetaSegment * fNumAlphaSegment) /
    (fNumAlphaSegment);
}

#endif
