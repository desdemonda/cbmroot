/**
 * \file CbmTrackMatchNew.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 *
 * Base data class for storing RECO-to-MCTrack matching information.
 **/

#ifndef CBMTRACKMATCHNEW_H_
#define CBMTRACKMATCHNEW_H_

#include "CbmMatch.h"
#include <string>

using std::string;

class CbmTrackMatchNew : public CbmMatch
{
public:
   /**
    * \brief Default constructor.
    */
   CbmTrackMatchNew();

   /**
    * \brief Destructor.
    */
   virtual ~CbmTrackMatchNew();

   /* Accessors */
   Int_t GetNofTrueHits() const { return fNofTrueHits; }
   Int_t GetNofWrongHits() const { return fNofWrongHits; }
   Int_t GetNofHits() const { return fNofTrueHits + fNofWrongHits; }
   Double_t GetTrueOverAllHitsRatio() const {
      Double_t all = GetNofHits();
      return (all == 0) ? 0. : (fNofTrueHits / all);
   }
   Double_t GetWrongOverAllHitsRatio() const {
      Double_t all = GetNofHits();
      return (all == 0) ? 0. : (fNofWrongHits / all);
   }

   /* Modifiers */
   void SetNofTrueHits(Int_t nofTrueHits) { fNofTrueHits = nofTrueHits; }
   void SetNofWrongHits(Int_t nofWrongHits) { fNofWrongHits = nofWrongHits; }

   /**
    * \brief Return string representation of the object.
    * \return String representation of the object.
    **/
   virtual string ToString() const;

private:
   Int_t fNofTrueHits; // Number of true hits in reconstructed track
   Int_t fNofWrongHits; // Number of wrong hits in reconstructed track

   ClassDef(CbmTrackMatchNew, 1);
};

#endif /* CBMTRACKMATCHNEW_H_ */
