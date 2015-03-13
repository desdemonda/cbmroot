/**
 * \file CbmMuchHitProducerIdeal.cxx
 * \author  A.Kiseleva
 * \date 13.04.06
 */

#include "CbmMuchHitProducerIdeal.h"
#include "FairRootManager.h"
#include "CbmMuchPoint.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchGeoScheme.h"

#include "TClonesArray.h"
#include "TRandom.h"
#include "TVector3.h"
#include "TFile.h"

#include <cmath>

using std::fabs;

CbmMuchHitProducerIdeal::CbmMuchHitProducerIdeal(const char* digiFileName)
: FairTask("CbmMuchHitProducerIdeal"),
  fDigiFile(digiFileName),
  fGeoScheme(NULL),
  fMuchPoints(NULL),
  fMuchPixelHits(NULL),
  fSigmaX(0.01),
  fSigmaY(0.01),
  fSigmaZ(0.)
{
}

CbmMuchHitProducerIdeal::~CbmMuchHitProducerIdeal()
{
}

InitStatus CbmMuchHitProducerIdeal::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   fMuchPoints = static_cast<TClonesArray*>(ioman->GetObject("MuchPoint"));
   fMuchPixelHits = new TClonesArray("CbmMuchPixelHit", 100);
   ioman->Register("MuchPixelHit", "MUCH", fMuchPixelHits, kTRUE);

   // Initialize GeoScheme
   fGeoScheme = CbmMuchGeoScheme::Instance();
   TFile* oldfile = gFile;
   TFile* file = new TFile(fDigiFile);
   TObjArray* stations = (TObjArray*) file->Get("stations");
   file->Close();
   file->Delete();
   gFile = oldfile;
   fGeoScheme->Init(stations);
   return kSUCCESS;
}

void CbmMuchHitProducerIdeal::Exec(Option_t * option)
{
   static Int_t eventNo = 0;
   fMuchPixelHits->Clear();
   Int_t iHit = 0;
   Int_t nofMuchPoints = fMuchPoints->GetEntriesFast();
   for (Int_t iPoint = 0; iPoint < nofMuchPoints; iPoint++) {
      const CbmMuchPoint* point = static_cast<const CbmMuchPoint*>(fMuchPoints->At(iPoint));

      // Smear position
      Double_t dX = gRandom->Gaus(0, fSigmaX);
      Double_t dY = gRandom->Gaus(0, fSigmaY);
      Double_t dZ = gRandom->Gaus(0, fSigmaZ);

      dX = (fabs(dX) < 3 * fSigmaX) ? dX : (dX > 0) ? 3 * fSigmaX : -3 * fSigmaX;
      dY = (fabs(dY) < 3 * fSigmaY) ? dY : (dY > 0) ? 3 * fSigmaY : -3 * fSigmaY;
      dZ = (fabs(dZ) < 3 * fSigmaZ) ? dZ : (dZ > 0) ? 3 * fSigmaZ : -3 * fSigmaZ;

      TVector3 hitPos(point->GetXIn() + dX, point->GetYIn() + dY, point->GetZIn() + dZ);
      TVector3 hitPosErr(fSigmaX, fSigmaY, fSigmaZ);

      Int_t address = point->GetDetectorID();
      Int_t planeId = fGeoScheme->GetLayerSideNr(address);

      new ((*fMuchPixelHits)[iHit++]) CbmMuchPixelHit(address, hitPos, hitPosErr, 0, iPoint, planeId);
   }
   eventNo++;
   LOG(INFO) << "CbmMuchHitProducerIdeal::Exec: eventNo=" << eventNo << " nofPoints="
         << fMuchPoints->GetEntries() << " nofHits=" << fMuchPixelHits->GetEntries()
         << FairLogger::endl;
}

void CbmMuchHitProducerIdeal::Finish()
{
}

ClassImp(CbmMuchHitProducerIdeal)
