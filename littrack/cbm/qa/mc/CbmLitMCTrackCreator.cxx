/**
 * \file CbmLitMCTrackCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#include "CbmLitMCTrackCreator.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairMCPoint.h"
#include "CbmMvdPoint.h"
#include "CbmGeoStsPar.h"
#include "legacy/CbmStsSensor_old.h"
#include "CbmStsDigiScheme.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"
#include "CbmMuchGeoScheme.h"
#include "CbmTrdAddress.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmRichPoint.h"
#include "CbmMvdPoint.h"
#include "CbmTrdPoint.h"
#include "CbmMuchPoint.h"
#include "CbmBaseHit.h"
#include "fitter/CbmRichRingFitterEllipseTau.h"
#include "CbmRichHitProducer.h"

#include "TDatabasePDG.h"
#include "TGeoManager.h"
#include "TClonesArray.h"

CbmLitMCTrackCreator::CbmLitMCTrackCreator():
   fMCTracks(NULL),
   fMvdPoints(NULL),
   fStsPoints(NULL),
   fTrdPoints(NULL),
   fMuchPoints(NULL),
   fTofPoints(NULL),
   fRichPoints(NULL),
   fRichHits(NULL),
   fLitMCTracks()
{
   ReadDataBranches();

   FairRuntimeDb* db = FairRunAna::Instance()->GetRuntimeDb();
   fStsGeoPar = (CbmGeoStsPar*) db->getContainer("CbmGeoStsPar");
   fStsDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");
   fStsDigiScheme  = new CbmStsDigiScheme();
   fStsDigiScheme->Init(fStsGeoPar, fStsDigiPar);

   fTauFit = new CbmRichRingFitterEllipseTau();
}

CbmLitMCTrackCreator::~CbmLitMCTrackCreator()
{

}

CbmLitMCTrackCreator* CbmLitMCTrackCreator::Instance()
{
   static CbmLitMCTrackCreator instance;
   return &instance;
}

void CbmLitMCTrackCreator::Create()
{
   FillStationMaps();

   fLitMCTracks.clear();

   AddPoints(kMVD, fMvdPoints);
   AddPoints(kSTS, fStsPoints);
   AddPoints(kTRD, fTrdPoints);
   AddPoints(kMUCH, fMuchPoints);
   AddPoints(kTOF, fTofPoints);
   AddPoints(kRICH, fRichPoints);
   AddRichHits();
   AddRingParameters();

   std::map<Int_t, CbmLitMCTrack>::iterator it;
   for (it = fLitMCTracks.begin(); it != fLitMCTracks.end(); it++)
          it->second.CalculateNofConsecutivePoints();

//   std::cout << "CbmLitMCTrackCreator: nof MC tracks=" << fLitMCTracks.size() << std::endl;
//   std::map<Int_t, CbmLitMCTrack>::iterator it;
//   for (it = fLitMCTracks.begin(); it != fLitMCTracks.end(); it++)
//       std::cout << (*it).first << " => " << (*it).second;
}

void CbmLitMCTrackCreator::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
   fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
}

void CbmLitMCTrackCreator::AddPoints(
      DetectorId detId,
      const TClonesArray* array)
{
   if (!array) return;
   Int_t nofPoints = array->GetEntriesFast();
   for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++) {
      FairMCPoint* fairPoint = static_cast<FairMCPoint*>(array->At(iPoint));
      CbmLitMCPoint litPoint;
      Int_t stationId = -1;
      if (detId == kMVD) {
         stationId = fMvdStationsMap[iPoint];
         MvdPointCoordinatesAndMomentumToLitMCPoint(static_cast<CbmMvdPoint*>(fairPoint), &litPoint);
      } else if (detId == kSTS) {
         stationId = fStsStationsMap[iPoint];
         StsPointCoordinatesAndMomentumToLitMCPoint(static_cast<CbmStsPoint*>(fairPoint), &litPoint);
      } else if (detId == kTRD) {
         stationId = fTrdStationsMap[iPoint];
         TrdPointCoordinatesAndMomentumToLitMCPoint(static_cast<CbmTrdPoint*>(fairPoint), &litPoint);
      } else if (detId == kMUCH) {
         stationId = fMuchStationsMap[iPoint];
         MuchPointCoordinatesAndMomentumToLitMCPoint(static_cast<CbmMuchPoint*>(fairPoint), &litPoint);
      } else if (detId == kTOF) {
         stationId = 0;
         FairMCPointCoordinatesAndMomentumToLitMCPoint(fairPoint, &litPoint);
      } else if (detId == kRICH) {
         stationId = 0;
         FairMCPointCoordinatesAndMomentumToLitMCPoint(fairPoint, &litPoint);
      }
      if (stationId < 0) continue;
      FairMCPointToLitMCPoint(fairPoint, &litPoint, iPoint, stationId);
      if (detId != kRICH) {
         fLitMCTracks[fairPoint->GetTrackID()].AddPoint(detId, litPoint);
      } else {
         const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(fairPoint->GetTrackID()));
         fLitMCTracks[mcTrack->GetMotherId()].AddPoint(detId, litPoint);
      }
   }
}

void CbmLitMCTrackCreator::AddRichHits()
{
	if (!fRichHits) return;
   map<Int_t, Int_t> nofHitsInRing;
   Int_t nofRichHits = fRichHits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofRichHits; iHit++) {
      const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fRichHits->At(iHit));
      Int_t pointIndex = hit->GetRefId();
      if (pointIndex < 0) continue;
      const FairMCPoint* point = static_cast<const FairMCPoint*>(fRichPoints->At(pointIndex));
      Int_t mcTrackIndex = point->GetTrackID();
      if (mcTrackIndex < 0) continue;
      const CbmMCTrack* track = static_cast<const CbmMCTrack*>(fMCTracks->At(mcTrackIndex));
      Int_t mcMotherIndex = track->GetMotherId();
      if (mcMotherIndex == -1) continue;
      nofHitsInRing[mcMotherIndex]++;
   }

   map<Int_t, Int_t>::const_iterator it;
   for (it = nofHitsInRing.begin(); it != nofHitsInRing.end(); it++) {
	   fLitMCTracks[it->first].SetNofRichHits(it->second);
   }
}

void CbmLitMCTrackCreator::AddRingParameters()
{
   if (NULL == fRichPoints || NULL == fMCTracks) return;
   map<Int_t, CbmRichRingLight> mapRings;
   int nofRichPoints = fRichPoints->GetEntriesFast();
   for (int iPoint = 0; iPoint < nofRichPoints; iPoint++){
      CbmRichPoint* richPoint = (CbmRichPoint*) fRichPoints->At(iPoint);
      if (NULL == richPoint) continue;
      Int_t trackId = richPoint->GetTrackID();
      if (trackId < 0) continue;
      CbmMCTrack* mcTrackRich = (CbmMCTrack*)fMCTracks->At(trackId);
      if (NULL == mcTrackRich) continue;
      int motherIdRich = mcTrackRich->GetMotherId();
      if (motherIdRich == -1) continue;
      TVector3 posPoint;
      richPoint->Position(posPoint);
      TVector3 detPoint;
      // FIXME: this parameters are only valid for rich_vo8a.geo geometry
      // in general they can be accessed via Parameter container
      // but here they are just hardcoded
      Double_t phi = -0.0873109;
      Double_t theta = 0.0873109;
      Double_t detZOrig = 179.975;
      CbmRichHitProducer::TiltPoint(&posPoint, &detPoint, phi, theta, detZOrig);
      CbmRichHitLight hit(detPoint.X(), detPoint.Y());
      mapRings[motherIdRich].AddHit(hit);
   }
   map<Int_t, CbmRichRingLight>::const_iterator it;
   int i = 0;
   for (it = mapRings.begin(); it != mapRings.end(); it++) {
      CbmRichRingLight ring(it->second);
      fTauFit->DoFit( &ring ); //fLitMCTracks[it->first].SetNofRichHits(it->second);
      fLitMCTracks[it->first].SetRingAaxis(ring.GetAaxis());
      fLitMCTracks[it->first].SetRingBaxis(ring.GetBaxis());
      fLitMCTracks[it->first].SetRingCenterX(ring.GetCenterX());
      fLitMCTracks[it->first].SetRingCenterY(ring.GetCenterY());
      //std::cout << ++i << " " << ring.GetAaxis() << " " << ring.GetBaxis() << std::endl;
   }

}

void CbmLitMCTrackCreator::FairMCPointToLitMCPoint(
      const FairMCPoint* fairPoint,
      CbmLitMCPoint* litPoint,
      Int_t refId,
      Int_t stationId)
{
   litPoint->SetRefId(refId);
   litPoint->SetStationId(stationId);
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(fairPoint->GetTrackID()));
   TParticlePDG* pdgParticle = TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode());
   double charge = (pdgParticle != NULL) ? pdgParticle->Charge() : 0.;
   Double_t q = (charge > 0) ? 1. : -1.;
   litPoint->SetQ(q);
}

void CbmLitMCTrackCreator::FairMCPointCoordinatesAndMomentumToLitMCPoint(
      const FairMCPoint* fairPoint,
      CbmLitMCPoint* litPoint)
{
   litPoint->SetXIn(fairPoint->GetX());
   litPoint->SetYIn(fairPoint->GetY());
   litPoint->SetZIn(fairPoint->GetZ());
   litPoint->SetPxIn(fairPoint->GetPx());
   litPoint->SetPyIn(fairPoint->GetPy());
   litPoint->SetPzIn(fairPoint->GetPz());
   litPoint->SetXOut(fairPoint->GetX());
   litPoint->SetYOut(fairPoint->GetY());
   litPoint->SetZOut(fairPoint->GetZ());
   litPoint->SetPxOut(fairPoint->GetPx());
   litPoint->SetPyOut(fairPoint->GetPy());
   litPoint->SetPzOut(fairPoint->GetPz());
}

void CbmLitMCTrackCreator::MvdPointCoordinatesAndMomentumToLitMCPoint(
      const CbmMvdPoint* mvdPoint,
      CbmLitMCPoint* litPoint)
{
   litPoint->SetXIn(mvdPoint->GetX());
   litPoint->SetYIn(mvdPoint->GetY());
   litPoint->SetZIn(mvdPoint->GetZ());
   litPoint->SetPxIn(mvdPoint->GetPx());
   litPoint->SetPyIn(mvdPoint->GetPy());
   litPoint->SetPzIn(mvdPoint->GetPz());
   litPoint->SetXOut(mvdPoint->GetXOut());
   litPoint->SetYOut(mvdPoint->GetYOut());
   litPoint->SetZOut(mvdPoint->GetZOut());
   litPoint->SetPxOut(mvdPoint->GetPxOut());
   litPoint->SetPyOut(mvdPoint->GetPyOut());
   litPoint->SetPzOut(mvdPoint->GetPzOut());
}

void CbmLitMCTrackCreator::StsPointCoordinatesAndMomentumToLitMCPoint(
      const CbmStsPoint* stsPoint,
      CbmLitMCPoint* litPoint)
{
   litPoint->SetXIn(stsPoint->GetXIn());
   litPoint->SetYIn(stsPoint->GetYIn());
   litPoint->SetZIn(stsPoint->GetZIn());
   litPoint->SetPxIn(stsPoint->GetPx());
   litPoint->SetPyIn(stsPoint->GetPy());
   litPoint->SetPzIn(stsPoint->GetPz());
   litPoint->SetXOut(stsPoint->GetXOut());
   litPoint->SetYOut(stsPoint->GetYOut());
   litPoint->SetZOut(stsPoint->GetZOut());
   litPoint->SetPxOut(stsPoint->GetPxOut());
   litPoint->SetPyOut(stsPoint->GetPyOut());
   litPoint->SetPzOut(stsPoint->GetPzOut());
}

void CbmLitMCTrackCreator::TrdPointCoordinatesAndMomentumToLitMCPoint(
      const CbmTrdPoint* trdPoint,
      CbmLitMCPoint* litPoint)
{
   litPoint->SetXIn(trdPoint->GetXIn());
   litPoint->SetYIn(trdPoint->GetYIn());
   litPoint->SetZIn(trdPoint->GetZIn());
   litPoint->SetPxIn(trdPoint->GetPxIn());
   litPoint->SetPyIn(trdPoint->GetPyIn());
   litPoint->SetPzIn(trdPoint->GetPzIn());
   litPoint->SetXOut(trdPoint->GetXOut());
   litPoint->SetYOut(trdPoint->GetYOut());
   litPoint->SetZOut(trdPoint->GetZOut());
   litPoint->SetPxOut(trdPoint->GetPxOut());
   litPoint->SetPyOut(trdPoint->GetPyOut());
   litPoint->SetPzOut(trdPoint->GetPzOut());
}

void CbmLitMCTrackCreator::MuchPointCoordinatesAndMomentumToLitMCPoint(
      const CbmMuchPoint* muchPoint,
      CbmLitMCPoint* litPoint)
{
   litPoint->SetXIn(muchPoint->GetXIn());
   litPoint->SetYIn(muchPoint->GetYIn());
   litPoint->SetZIn(muchPoint->GetZIn());
   litPoint->SetPxIn(muchPoint->GetPx());
   litPoint->SetPyIn(muchPoint->GetPy());
   litPoint->SetPzIn(muchPoint->GetPz());
   litPoint->SetXOut(muchPoint->GetXOut());
   litPoint->SetYOut(muchPoint->GetYOut());
   litPoint->SetZOut(muchPoint->GetZOut());
   litPoint->SetPxOut(muchPoint->GetPxOut());
   litPoint->SetPyOut(muchPoint->GetPyOut());
   litPoint->SetPzOut(muchPoint->GetPzOut());
}

void CbmLitMCTrackCreator::FillStationMaps()
{
   fMvdStationsMap.clear();
   fStsStationsMap.clear();
   fTrdStationsMap.clear();
   fMuchStationsMap.clear();

   // MVD
   if (NULL != fMvdPoints) {
      Int_t nofMvdPoints = fMvdPoints->GetEntriesFast();
       for (Int_t iPoint = 0; iPoint < nofMvdPoints; iPoint++) {
          CbmMvdPoint* point = static_cast<CbmMvdPoint*>(fMvdPoints->At(iPoint));
          fMvdStationsMap[iPoint] = point->GetStationNr() - 1;
       }
   }
   // end MVD

   // STS
   if (NULL != fStsPoints) {
//      FairRunAna* run = FairRunAna::Instance();
//      FairRuntimeDb* runDb = run->GetRuntimeDb();
//      CbmGeoStsPar* stsGeoPar = (CbmGeoStsPar*) runDb->getContainer("CbmGeoStsPar");
//      TObjArray* stsNodes = stsGeoPar->GetGeoSensitiveNodes();
//      Int_t nofStsStations = stsNodes->GetEntries();
//      std::map<Int_t, Int_t> stsStationNrFromMcId;
//      std::cout << "nofStations=" << nofStsStations << std::endl;
//      for (Int_t ist = 0; ist < nofStsStations; ist++) {
//        FairGeoNode* stsNode = (FairGeoNode*) stsNodes->At(ist);
//        std::string stsNodeName(stsNode->GetName());
//        std::cout << "stsNode:" <<  stsNode->GetName() << std::endl;
//        std::string stsStationNr = stsNodeName.substr(10, 2);
//        Int_t stationNr = atoi(stsStationNr.c_str());
//        stsStationNrFromMcId[stsNode->getMCid()] = stationNr - 1;
//      }
//
//      CbmStsDetectorId stsDetectorId;
//      Int_t nofStsPoints = fStsPoints->GetEntriesFast();
//      for (Int_t iPoint = 0; iPoint < nofStsPoints; iPoint++) {
//         const FairMCPoint* point = static_cast<const FairMCPoint*>(fStsPoints->At(iPoint));
//         Int_t stationId = stsStationNrFromMcId[point->GetDetectorID()];
//         fStsStationsMap[iPoint] = stationId;
//      }

	   Int_t nofStsPoints = fStsPoints->GetEntriesFast();
	   for (Int_t iPoint = 0; iPoint < nofStsPoints; iPoint++) {
		   const CbmStsPoint* point = static_cast<const CbmStsPoint*>(fStsPoints->At(iPoint));
		   Double_t xin = point->GetXIn();
		   Double_t yin = point->GetYIn();
		   Double_t zin = point->GetZIn();
		   gGeoManager->FindNode(xin, yin, zin);
		   TGeoNode* curNode = gGeoManager->GetCurrentNode();  // only needed for old geometries
		   CbmStsSensor_old* sensor = (fStsDigiScheme->IsNewGeometry()) ?
				   fStsDigiScheme->GetSensorByName(fStsDigiScheme->GetCurrentPath()) :
                       fStsDigiScheme->GetSensorByName(curNode->GetName());
		   if (sensor != NULL) {
		      Int_t stationId = sensor->GetStationNr() - 1;
		      fStsStationsMap[iPoint] = stationId;
		   }
	   }
   }
   // end STS

   // MUCH
   if (NULL != fMuchPoints) {
      Int_t nofMuchPoints = fMuchPoints->GetEntriesFast();
      for (Int_t iPoint = 0; iPoint < nofMuchPoints; iPoint++) {
         const FairMCPoint* point = static_cast<const FairMCPoint*>(fMuchPoints->At(iPoint));
         Int_t stationId = 100 * CbmMuchGeoScheme::GetStationIndex(point->GetDetectorID())
            + 10 * CbmMuchGeoScheme::GetLayerIndex(point->GetDetectorID())
            + CbmMuchGeoScheme::GetLayerSideIndex(point->GetDetectorID());
//         Int_t stationId = CbmMuchGeoScheme::Instance()->GetLayerSideNr(point->GetDetectorID());
         fMuchStationsMap[iPoint] = stationId;
      }
   }
   // end MUCH

   // TRD
   if (NULL != fTrdPoints) {
      Int_t nofTrdPoints = fTrdPoints->GetEntriesFast();
      for (Int_t iPoint = 0; iPoint < nofTrdPoints; iPoint++) {
         const FairMCPoint* point = static_cast<const FairMCPoint*>(fTrdPoints->At(iPoint));
         //Int_t stationId = 10 * CbmTrdAddress::GetStationNr(point->GetDetectorID()) + CbmTrdAddress::GetLayerNr(point->GetDetectorID());
         Int_t stationId = CbmTrdAddress::GetLayerId(point->GetDetectorID());
         fTrdStationsMap[iPoint] = stationId;
      }
   }
   // end TRD
}
