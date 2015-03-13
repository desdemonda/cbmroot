// -------------------------------------------------------------------------
// -----                     CbmMvdDigiMatch header file               -----
// -----                    Created 02/04/08  by C.Dritsa              -----
// -----           Update to new CbmMatch Class by P. Sitzmann         -----
// -------------------------------------------------------------------------



#ifndef CBMMVDDIGIMATCH_H
#define CBMMVDDIGIMATCH_H 1

#include "CbmDetectorList.h"
#include "CbmDigi.h"
#include "CbmMvdDetectorId.h"
#include "CbmMatch.h"

#include "TObject.h"
#include "TMath.h"
#include "TVector3.h"


class CbmMvdDigiMatch : public CbmMatch
{

public:

    /** Default constructor **/
    CbmMvdDigiMatch();

    /** Constructor with all variables **/

    CbmMvdDigiMatch(Double_t weight, Int_t index, Int_t entry = -1, Int_t file = -1);

    

    /** Destructor **/
    virtual ~CbmMvdDigiMatch();

 /** Accessors **/
  Int_t  GetFileNumber()  const { return fFileNumber;  }
  Int_t  GetIndexNumber() const { return fIndex; }
  Int_t  GetEntryNumber() const { return fEntry; }
  Float_t GetWeight()     const { return fWeight; }

  Int_t GetNContributors() { return GetNofLinks();};

  Int_t GetDominatorX() { return 0; }
  Int_t GetDominatorY() { return 0; }
  Int_t GetDominatorPointID() { return 0; }
  Int_t GetDominatorTrackID() { return 0; }



  /** Reset data members **/
//  void Clear();


 private:

  Int_t fFileNumber;
  Int_t fIndex; 
  Double_t fWeight;
  Int_t fEntry;
  

protected:

    ClassDef(CbmMvdDigiMatch,1);

};


#endif

