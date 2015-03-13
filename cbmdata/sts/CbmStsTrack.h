// -------------------------------------------------------------------------
// -----                      CbmStsTrack header file                  -----
// -----                  Created 26/01/05  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmStsTrack.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** STS local track. Holds lists of CbmStsHits and the fitted
 ** track parameters. The fit parameters are of type FairTrackParam
 ** and can only be accesssed and modified via this class.
 **/

#ifndef CBMSTSTRACK_H
#define CBMSTSTRACK_H 1

#include "CbmTrack.h"

class CbmStsTrack : public CbmTrack
{
public:

   /** Default constructor **/
   CbmStsTrack();

   /** Destructor **/
   virtual ~CbmStsTrack();

   /** Associate a MvdHit to the track **/
   void AddMvdHit(Int_t hitIndex);

   /** Inherited from CbmTrack **/
   virtual string ToString() const;

   /** Accessors **/
   Int_t GetNofMvdHits() const { return fMvdHitIndex.size(); }
   Int_t GetMvdHitIndex(Int_t iHit) const { return fMvdHitIndex[iHit]; }
   Double_t GetB() const { return fB; }

   /** Modifiers **/
   void SetB(Double_t b){ fB = b; }
  
private:
   /** Array contains the hit indices of the hits attached to the track **/
   std::vector<Int_t> fMvdHitIndex;
   /** Impact parameter of track at target z, in units of its error **/
   Double32_t fB;

   ClassDef(CbmStsTrack, 2);
};



#endif
