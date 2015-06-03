/** CbmLitFindGlobalTracks.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/

#include "CbmLitFindGlobalTracks.h"
#include "base/CbmLitToolFactory.h"
#include "base/CbmLitTrackingGeometryConstructor.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitTrack.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"
#include "propagation/CbmLitTGeoTrackPropagator.h"

#include "CbmHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmTofTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmStsHit.h"

#include "TClonesArray.h"

#include <iostream>
#include <cmath>

CbmLitFindGlobalTracks::CbmLitFindGlobalTracks():
   fDet(),

   fStsTracks(NULL),
   fMvdHits(NULL),
   fStsHits(NULL),
   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fMuchTracks(NULL),
   fTrdHits(NULL),
   fTrdTracks(NULL),
   fTofHits(NULL),
   fTofTracks(NULL),
   fGlobalTracks(NULL),

   fLitStsTracks(),
   fLitHits(),
   fLitTofHits(),
   fLitOutputTracks(),
   fLitOutputTofTracks(),

   fFinder(),
   fMerger(),
   fFitter(),
   fPropagator(),

   fTrackingType("branch"),
   fMergerType("nearest_hit"),
   fFitterType("lit_kalman"),

   fTrackingWatch(),
   fMergerWatch(),

   fEventNo(0)
{

}

CbmLitFindGlobalTracks::~CbmLitFindGlobalTracks()
{

}

InitStatus CbmLitFindGlobalTracks::Init()
{
   fDet.DetermineSetup();
   std::cout << fDet.ToString();

   ReadAndCreateDataBranches();

   InitTrackReconstruction();

   fTrackingWatch.Reset();
   fMergerWatch.Reset();

   return kSUCCESS;
}

void CbmLitFindGlobalTracks::Exec(
   Option_t* opt)
{
   if (fTrdTracks != NULL) fTrdTracks->Delete();
   if (fMuchTracks != NULL) fMuchTracks->Delete();
   if (fTofTracks != NULL) fTofTracks->Delete();
   fGlobalTracks->Clear();

   ConvertInputData();

   RunTrackReconstruction();

   ConvertOutputData();

   CalculateLength();

   ClearArrays();

   std::cout << "CbmLitFindGlobalTracks::Exec event: " << fEventNo++ << std::endl;
}

void CbmLitFindGlobalTracks::SetParContainers()
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();

   rtdb->getContainer("FairBaseParSet");
   rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitFindGlobalTracks::Finish()
{
   PrintStopwatchStatistics();
}

void CbmLitFindGlobalTracks::ReadAndCreateDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("Init","CbmRootManager is not instantiated"); }

   if (fDet.GetDet(kMVD)) {
	   fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
	   if (NULL == fMvdHits) { Fatal("Init","No MvdHit array!"); }
   }

   //STS data
   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   if (NULL == fStsTracks) { Fatal("Init","No StsTrack array!"); }
   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
   if (NULL == fStsHits) { Fatal("Init","No StsHit array!"); }

   //MUCH data
   if (fDet.GetDet(kMUCH)) {
      fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
      fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
      if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) { Fatal("Init", "No MuchPixelHit or MuchStrawHit array!"); }
      if (fMuchPixelHits) { std::cout << "-I- MuchPixelHit branch found in tree" << std::endl; }
      if (fMuchStrawHits) { std::cout << "-I- MuchStrawHit branch found in tree" << std::endl; }
   }

   //TRD data
   if (fDet.GetDet(kTRD)) {
      fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
      if (NULL == fTrdHits) { Fatal("Init", "No TRDHit array!"); }
      std::cout << "-I- TRDHit branch found in tree" << std::endl;
   }

   //TOF data
   if (fDet.GetDet(kTOF)) {
      fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
      if (NULL == fTofHits) { Fatal("Init", "No TofHit array!"); }
      std::cout << "-I- TofHit branch found in tree" << std::endl;
   }

   // Create and register track arrays
   fGlobalTracks = new TClonesArray("CbmGlobalTrack",100);
   ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);

   if (fDet.GetDet(kMUCH)) {
      fMuchTracks = new TClonesArray("CbmMuchTrack", 100);
      ioman->Register("MuchTrack", "Much", fMuchTracks, kTRUE);
   }

   if (fDet.GetDet(kTRD)) {
      fTrdTracks = new TClonesArray("CbmTrdTrack", 100);
      ioman->Register("TrdTrack", "Trd", fTrdTracks, kTRUE);
   }

   if (fDet.GetDet(kTOF)) {
      fTofTracks = new TClonesArray("CbmTofTrack", 100);
      ioman->Register("TofTrack", "Tof", fTofTracks, kTRUE);
   }
}

void CbmLitFindGlobalTracks::InitTrackReconstruction()
{
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   if (fDet.GetElectronSetup()) {
      if (fTrackingType == "branch" || fTrackingType == "nn" || fTrackingType == "nn_parallel") {
         std::string st("e_");
         st += fTrackingType;
         fFinder = factory->CreateTrackFinder(st);
      } else {
         TObject::Fatal("CbmLitFindGlobalTracks","Tracking type not found");
      }
   } else {
      if (fTrackingType == "branch" || fTrackingType == "nn" || fTrackingType == "nn_parallel") {
         std::string st("mu_");
         st += fTrackingType;
         fFinder = factory->CreateTrackFinder(st);
      } else {
         TObject::Fatal("CbmLitFindGlobalTracks","Tracking type not found");
      }
   }

   if (fDet.GetDet(kTOF)) {
      if (fMergerType == "nearest_hit" || fMergerType == "all_hits") {
         fMerger = factory->CreateHitToTrackMerger("tof_" + fMergerType);
      } else {
         TObject::Fatal("CbmLitFindGlobalTracks","Merger type not found");
      }
   }

   if (fFitterType == "lit_kalman") {
      fFitter = factory->CreateTrackFitter("lit_kalman");
   } else {
      TObject::Fatal("CbmLitFindGlobalTracks","Fitter type not found");
   }

   fPropagator = factory->CreateTrackPropagator("lit");
}

void CbmLitFindGlobalTracks::ConvertInputData()
{
   CbmLitConverter::StsTrackArrayToTrackVector(fStsTracks, fLitStsTracks);
   std::cout << "-I- CbmLitFindGlobalTracks: Number of STS tracks: " << fLitStsTracks.size() << std::endl;

   if (fMuchPixelHits) { CbmLitConverter::HitArrayToHitVector(fMuchPixelHits, fLitHits); }
   if (fMuchStrawHits) { CbmLitConverter::HitArrayToHitVector(fMuchStrawHits, fLitHits); }
   if (fTrdHits) {
      CbmLitConverter::HitArrayToHitVector(fTrdHits, fLitHits);
      //If MUCH-TRD setup, than shift plane id for the TRD hits
      if (fDet.GetDet(kMUCH) && fDet.GetDet(kTRD)) {
         Int_t nofStations = CbmLitTrackingGeometryConstructor::Instance()->GetNofMuchStations();
         for (Int_t i = 0; i < fLitHits.size(); i++) {
            CbmLitHit* hit = fLitHits[i];
            if (hit->GetSystem() == kLITTRD) { hit->SetDetectorId(kLITTRD, hit->GetStation() + nofStations); }
         }
      }
   }
   std::cout << "-I- CbmLitFindGlobalTracks: Number of hits: " << fLitHits.size() << std::endl;

   if (fTofHits) {
      CbmLitConverter::HitArrayToHitVector(fTofHits, fLitTofHits);
      std::cout << "-I- CbmLitFindGlobalTracks: Number of TOF hits: " << fLitTofHits.size() << std::endl;
   }
}

void CbmLitFindGlobalTracks::ConvertOutputData()
{
   CbmLitConverter::LitTrackVectorToGlobalTrackArray(fLitOutputTracks, fLitOutputTofTracks, fGlobalTracks, fStsTracks, fTrdTracks, fMuchTracks, fTofTracks);
}

void CbmLitFindGlobalTracks::CalculateLength()
{
   if (fTofTracks == NULL || fGlobalTracks == NULL) return;

   /* Calculate the length of the global track
    * starting with (0, 0, 0) and adding all
    * distances between hits
    */
   Int_t nofTofTracks = fTofTracks->GetEntriesFast();
   for (Int_t itt = 0; itt < nofTofTracks; itt++) {
      CbmTofTrack* tofTrack = static_cast<CbmTofTrack*>(fTofTracks->At(itt));
      CbmGlobalTrack* globalTrack = static_cast<CbmGlobalTrack*>(fGlobalTracks->At(tofTrack->GetTrackIndex()));
      if (globalTrack == NULL) { continue; }

      std::vector<Double_t> X, Y, Z;
      X.push_back(0.);
      Y.push_back(0.);
      Z.push_back(0.);

      // get track segments indices
      Int_t stsId = globalTrack->GetStsTrackIndex();
      Int_t trdId = globalTrack->GetTrdTrackIndex();
      Int_t muchId = globalTrack->GetMuchTrackIndex();
      Int_t tofId = tofTrack->GetTofHitIndex();//globalTrack->GetTofHitIndex();

      if (stsId > -1) {
         const CbmStsTrack* stsTrack = static_cast<const CbmStsTrack*>(fStsTracks->At(stsId));
         Int_t nofStsHits = stsTrack->GetNofHits();
         for(Int_t ih = 0; ih < nofStsHits; ih++) {
            CbmStsHit* hit = (CbmStsHit*) fStsHits->At(stsTrack->GetHitIndex(ih));
            X.push_back(hit->GetX());
            Y.push_back(hit->GetY());
            Z.push_back(hit->GetZ());
         }
      }

      if (muchId > -1) {
         const CbmTrack* muchTrack = static_cast<const CbmTrack*>(fMuchTracks->At(muchId));
         Int_t nofMuchHits = muchTrack->GetNofHits();
         for(Int_t ih = 0; ih < nofMuchHits; ih++) {
            HitType hitType = muchTrack->GetHitType(ih);
            if (hitType == kMUCHPIXELHIT) {
               CbmPixelHit* hit = (CbmPixelHit*) fMuchPixelHits->At(muchTrack->GetHitIndex(ih));
               X.push_back(hit->GetX());
               Y.push_back(hit->GetY());
               Z.push_back(hit->GetZ());
            } else if (hitType == kMUCHSTRAWHIT) {

            }
         }
      }

      if (trdId > -1) {
         const CbmTrack* trdTrack = static_cast<const CbmTrack*>(fTrdTracks->At(trdId));
         Int_t nofTrdHits = trdTrack->GetNofHits();
         for(Int_t ih = 0; ih < nofTrdHits; ih++) {
            CbmPixelHit* hit = (CbmPixelHit*) fTrdHits->At(trdTrack->GetHitIndex(ih));
            X.push_back(hit->GetX());
            Y.push_back(hit->GetY());
            Z.push_back(hit->GetZ());
         }
      }

      if (tofId > -1) {
         const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(fTofHits->At(tofId));
         X.push_back(hit->GetX());
         Y.push_back(hit->GetY());
         Z.push_back(hit->GetZ());
      }

      // Calculate distances between hits
      Double_t length = 0.;
      for (Int_t i = 0; i < X.size() - 1; i++) {
         Double_t dX = X[i] - X[i+1];
         Double_t dY = Y[i] - Y[i+1];
         Double_t dZ = Z[i] - Z[i+1];
         length += std::sqrt(dX*dX + dY*dY + dZ*dZ);
      }

      if (globalTrack->GetTofHitIndex() == tofTrack->GetTofHitIndex()) globalTrack->SetLength(length);
      tofTrack->SetTrackLength(length);
   }
}

//void CbmLitFindGlobalTracks::CalculateLength()
//{
//   /* Calculate the length of the global track
//    * starting with (0, 0, 0) and adding all
//    * distances between hits
//    */
//
//	// Reduce step to calculate track length more accurately
//	CbmLitTGeoTrackPropagator::MAXIMUM_PROPAGATION_STEP_SIZE = 1.0;
//
//	TrackPtrVector litTracks;
//	CbmLitConverter::GlobalTrackArrayToLitTrackVector(fGlobalTracks, fStsTracks, fTrdTracks, fMuchTracks, fMvdHits, fStsHits, fTrdHits, fMuchStrawHits, fMuchPixelHits, fTofHits, litTracks);
//	Int_t nofTracks = litTracks.size();
//	for (UInt_t iTrack = 0; iTrack < nofTracks; iTrack++) {
//		CbmGlobalTrack* globalTrack = static_cast<CbmGlobalTrack*>(fGlobalTracks->At(iTrack));
//		if (globalTrack->GetStsTrackIndex() >= 0 && globalTrack->GetTofHitIndex() >= 0) {
//			CbmLitTrack* track = litTracks[iTrack];
//			CbmLitTrackParam par = *track->GetParamFirst();
//			LitStatus propStatus = fPropagator->Propagate(&par, 0.0, 211, NULL);
//			track->SetParamFirst(&par);
//			fFitter->Fit(track);
//			globalTrack->SetLength(track->GetLength());
//		} else {
//			globalTrack->SetLength(-1.);
//		}
//	}
//
//	CbmLitTGeoTrackPropagator::MAXIMUM_PROPAGATION_STEP_SIZE = 10.;
//
//	// free memory
//	for (UInt_t iTrack = 0; iTrack < nofTracks; iTrack++) {
//		CbmLitTrack* track = litTracks[iTrack];
//		Int_t nofHits = track->GetNofHits();
//		for (Int_t iHit = 0; iHit < nofHits; iHit++) {
//			delete track->GetHit(iHit);
//		}
//		track->ClearHits();
//	}
//	for_each(litTracks.begin(), litTracks.end(), DeleteObject());
//}

void CbmLitFindGlobalTracks::ClearArrays()
{
   // Free memory
   for_each(fLitStsTracks.begin(), fLitStsTracks.end(), DeleteObject());
   for_each(fLitOutputTracks.begin(), fLitOutputTracks.end(), DeleteObject());
   for_each(fLitHits.begin(), fLitHits.end(), DeleteObject());
   for_each(fLitTofHits.begin(), fLitTofHits.end(), DeleteObject());
   for_each(fLitOutputTofTracks.begin(), fLitOutputTofTracks.end(), DeleteObject());
   fLitStsTracks.clear();
   fLitOutputTracks.clear();
   fLitHits.clear();
   fLitTofHits.clear();
   fLitOutputTofTracks.clear();
}

void CbmLitFindGlobalTracks::RunTrackReconstruction()
{
   // Track finding in TRD or MUCH
   if (fDet.GetDet(kMUCH) || fDet.GetDet(kTRD)) {
      fTrackingWatch.Start(kFALSE);
      fFinder->DoFind(fLitHits, fLitStsTracks, fLitOutputTracks);
      fTrackingWatch.Stop();
   }
   // Merging of TOF hits to global tracks
   if (fDet.GetDet(kTOF)) {
      // If there are no TRD or MUCH than merge STS tracks with TOF
      if (!(fDet.GetDet(kMUCH) || fDet.GetDet(kTRD))) {
         for(TrackPtrIterator it = fLitStsTracks.begin(); it != fLitStsTracks.end(); it++) {
            CbmLitTrack* track = new CbmLitTrack(*(*it));
            fLitOutputTracks.push_back(track);
         }
      }

      // Selection of tracks to be merged with TOF
      if (fDet.GetDet(kMUCH) || fDet.GetDet(kTRD)) {
         SelectTracksForTofMerging();
      } else {
         for (TrackPtrIterator it = fLitOutputTracks.begin(); it != fLitOutputTracks.end(); it++) {
            (*it)->SetQuality(kLITGOODMERGE);
         }
      }

      fMergerWatch.Start(kFALSE);
      fMerger->DoMerge(fLitTofHits, fLitOutputTracks, fLitOutputTofTracks);
      fMergerWatch.Stop();
   }

   // Refit found tracks
   for(TrackPtrIterator it = fLitOutputTracks.begin(); it != fLitOutputTracks.end(); it++) {
      CbmLitTrack* track = *it;
      fFitter->Fit(track);
   }
}

void CbmLitFindGlobalTracks::SelectTracksForTofMerging()
{
   // The aim of this procedure is to select only those tracks
   // which have at least one hit in the last station group.
   // Only those tracks will be propagated further and merged
   // with TOF hits.

   Int_t nofStations = CbmLitTrackingGeometryConstructor::Instance()->GetNofMuchTrdStations();
   //   Int_t stationCut = nofStations - 4;
   // TODO: Fix this issue in a better way. This is done only as an ugly fix
   // FU 19.09.13 
   Int_t stationCut = nofStations - 3;

   for(TrackPtrIterator it = fLitOutputTracks.begin(); it != fLitOutputTracks.end(); it++) {
      CbmLitTrack* track = *it;
      if (track->GetQuality() == kLITBAD) { continue; }
      const CbmLitHit* hit = track->GetHit(track->GetNofHits() - 1);
      if (hit->GetStation() >= stationCut) {
         // OK select this track for further merging with TOF
         track->SetQuality(kLITGOODMERGE);
      }
   }
}

void CbmLitFindGlobalTracks::PrintStopwatchStatistics()
{
   std::cout << "Stopwatch: " << std::endl;
   std::cout << "tracking: counts=" << fTrackingWatch.Counter()
             << ", real=" << fTrackingWatch.RealTime()/fTrackingWatch.Counter()
             << "/" << fTrackingWatch.RealTime()
             << " s, cpu=" << fTrackingWatch.CpuTime()/fTrackingWatch.Counter()
             << "/" << fTrackingWatch.CpuTime() << std::endl;
   std::cout << "fitter: real=" << fMergerWatch.Counter()
             << ", real=" << fMergerWatch.RealTime()/fMergerWatch.Counter()
             << "/" << fMergerWatch.RealTime()
             << " s, cpu=" << fMergerWatch.CpuTime()/fMergerWatch.Counter()
             << "/" << fMergerWatch.CpuTime() << std::endl;
}

ClassImp(CbmLitFindGlobalTracks);
