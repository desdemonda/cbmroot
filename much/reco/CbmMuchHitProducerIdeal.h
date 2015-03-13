/**
 * \file CbmMuchHitProducerIdeal.h
 * \author  A.Kiseleva
 * \date 13.04.06
 * \brief Ideal hit producer for MUon CHambers detector.
 */

#ifndef CBMMUCHHITPRODUCERIDEAL_H_
#define CBMMUCHHITPRODUCERIDEAL_H_

#include "FairTask.h"
#include "TString.h"

class TClonesArray;
class CbmMuchGeoScheme;

class CbmMuchHitProducerIdeal : public FairTask
{
public:

   CbmMuchHitProducerIdeal(const char* digiFileName);

   virtual ~CbmMuchHitProducerIdeal();

   InitStatus Init();
   void Exec(Option_t * option);
   void Finish();

   void SetSigmaX(Double_t sigma) { fSigmaX = sigma; }
   void SetSigmaY(Double_t sigma) { fSigmaY = sigma; }
   void SetSigmaZ(Double_t sigma) { fSigmaZ = sigma; }

private:
   TClonesArray* fMuchPoints; // Much MC points
   TClonesArray* fMuchPixelHits; // Much hits

   Double_t fSigmaX;
   Double_t fSigmaY;
   Double_t fSigmaZ;

   TString fDigiFile; // Digitization file
   CbmMuchGeoScheme* fGeoScheme; // Geometry scheme

   CbmMuchHitProducerIdeal(const CbmMuchHitProducerIdeal&);
   CbmMuchHitProducerIdeal& operator=(const CbmMuchHitProducerIdeal&);

   ClassDef(CbmMuchHitProducerIdeal, 1);
};

#endif

