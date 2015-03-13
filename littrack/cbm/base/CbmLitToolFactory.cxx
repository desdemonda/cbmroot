/**
 * \file CbmLitToolFactory.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

#include "base/CbmLitToolFactory.h"
#include "base/CbmLitMapField.h"
#include "base/CbmLitTrackingGeometryConstructor.h"
#include "base/CbmLitPtrTypes.h"
#include "finder/CbmLitTrackFinderNN.h"
#include "finder/CbmLitTrackFinderBranch.h"
#include "finder/CbmLitNearestHitTofMerger.h"
#include "finder/CbmLitAllHitsTofMerger.h"
#include "fitter/CbmLitKalmanFilter.h"
#include "fitter/CbmLitKalmanSmoother.h"
#include "fitter/CbmLitTrackFitterImp.h"
#include "fitter/CbmLitTrackFitterIter.h"
#include "interface/CbmLitField.h"
#include "propagation/CbmLitRK4TrackExtrapolator.h"
#include "propagation/CbmLitLineTrackExtrapolator.h"
#include "propagation/CbmLitCleverTrackExtrapolator.h"
#include "propagation/CbmLitTGeoTrackPropagator.h"
#include "selection/CbmLitTrackSelectionEmpty.h"
#include "selection/CbmLitTrackSelectionCuts.h"
#include "selection/CbmLitTrackSelectionMuch.h"
#include "selection/CbmLitTrackSelectionTrd.h"

#include "FairRunAna.h"

#include <cassert>

#include <boost/assign/list_of.hpp>

using boost::assign::list_of;

CbmLitToolFactory::CbmLitToolFactory()
{
}

CbmLitToolFactory::~CbmLitToolFactory()
{
}

CbmLitToolFactory* CbmLitToolFactory::Instance()
{
   static CbmLitToolFactory instance;
   return &instance;
}

TrackExtrapolatorPtr CbmLitToolFactory::CreateTrackExtrapolator(
   const std::string& name)
{
   assert(name == "line" || name == "rk4" || name == "lit");
   if (name == "line") {
      return TrackExtrapolatorPtr(new CbmLitLineTrackExtrapolator());
   } else if (name == "rk4") {
      CbmLitField* field = new CbmLitMapField(FairRunAna::Instance()->GetField());
      return TrackExtrapolatorPtr(new CbmLitRK4TrackExtrapolator(field));
   } else if (name == "lit") {
      return TrackExtrapolatorPtr(new CbmLitCleverTrackExtrapolator());
   }
   return TrackExtrapolatorPtr();
}

TrackPropagatorPtr CbmLitToolFactory::CreateTrackPropagator(
   const std::string& name)
{
   assert(name == "lit" || name == "rk4" || name == "line");
   if (name == "lit") {
      return TrackPropagatorPtr(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("lit")));
   } else if(name == "rk4") {
      return TrackPropagatorPtr(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("rk4")));
   } else if(name == "line") {
      return TrackPropagatorPtr(new CbmLitTGeoTrackPropagator(CreateTrackExtrapolator("line")));
   }
   return TrackPropagatorPtr();
}

TrackUpdatePtr CbmLitToolFactory::CreateTrackUpdate(
   const std::string& name)
{
   assert(name == "kalman");
   if (name == "kalman") {
      return TrackUpdatePtr(new CbmLitKalmanFilter());
   }
   return TrackUpdatePtr();
}

TrackFitterPtr CbmLitToolFactory::CreateTrackFitter(
   const std::string& name)
{
   assert(name == "lit_kalman" || name == "kalman_smoother");
   if (name == "lit_kalman") {
      return TrackFitterPtr(new CbmLitTrackFitterImp(CreateTrackPropagator("lit"), CreateTrackUpdate("kalman")));
   } else if (name == "kalman_smoother") {
      return TrackFitterPtr(new CbmLitKalmanSmoother());
   }
   return TrackFitterPtr();
}

TrackSelectionPtr CbmLitToolFactory::CreateTrackSelection(
   const std::string& name)
{
   assert(name == "empty" || name == "momentum" || name == "momentum_seed" || name == "chi_square" || name == "much_final" || name == "trd_final");
   if (name == "empty") {
      return TrackSelectionPtr(new CbmLitTrackSelectionEmpty());
   } else if (name == "momentum") {
      CbmLitTrackSelectionCuts* momSelection = new CbmLitTrackSelectionCuts();
      momSelection->SetMinMomentum(0.1);
      return TrackSelectionPtr(momSelection);
   } else if (name == "momentum_seed") {
      CbmLitTrackSelectionCuts* momSelection = new CbmLitTrackSelectionCuts();
      momSelection->SetMinMomentum(0.1);
      return TrackSelectionPtr(momSelection);
   } else if (name == "chi_square") {
      CbmLitTrackSelectionCuts* chiSqSelection = new CbmLitTrackSelectionCuts();
      chiSqSelection->SetMaxChiSq(30.);
      return TrackSelectionPtr(chiSqSelection);
   } else if (name == "much_final") {
      CbmLitTrackSelectionMuch* muchSelection = new CbmLitTrackSelectionMuch();
      muchSelection->SetNofSharedHits(3);
      return TrackSelectionPtr(muchSelection);
   } else if (name == "trd_final") {
      CbmLitTrackSelectionTrd* trdSelection = new CbmLitTrackSelectionTrd();
      trdSelection->SetNofSharedHits(3);
      return TrackSelectionPtr(trdSelection);
   }
   return TrackSelectionPtr();
}

TrackFinderPtr CbmLitToolFactory::CreateTrackFinder(
   const std::string& name)
{
   assert(name == "e_nn" || name == "e_branch" || name == "mu_nn" || name == "mu_branch" || name == "mvd_nn");
   if (name == "e_nn") {
      CbmLitTrackFinderNN* trdFinderNN = new CbmLitTrackFinderNN();
      trdFinderNN->SetSeedSelection(CreateTrackSelection("momentum"));
      trdFinderNN->SetFinalSelection(CreateTrackSelection("trd_final"));
      trdFinderNN->SetPropagator(CreateTrackPropagator("lit"));
      trdFinderNN->SetFilter(CreateTrackUpdate("kalman"));
      trdFinderNN->SetNofStations(CbmLitTrackingGeometryConstructor::Instance()->GetNofMuchTrdStations());
      trdFinderNN->SetNofIterations(1);
      trdFinderNN->SetMaxNofMissingHits(list_of(4));
      trdFinderNN->SetPDG(list_of(211));
      trdFinderNN->SetChiSqStripHitCut(list_of(9.));
      trdFinderNN->SetChiSqPixelHitCut(list_of(25.));
      trdFinderNN->SetSigmaCoef(list_of(5.));
      return TrackFinderPtr(trdFinderNN);
   } else if (name == "e_branch") {
      CbmLitTrackFinderBranch* trdFinderBranch = new CbmLitTrackFinderBranch();
      trdFinderBranch->SetSeedSelection(CreateTrackSelection("momentum"));
      trdFinderBranch->SetFinalSelection(CreateTrackSelection("trd_final"));
      trdFinderBranch->SetPropagator(CreateTrackPropagator("lit"));
      trdFinderBranch->SetFilter(CreateTrackUpdate("kalman"));
      trdFinderBranch->SetNofStations(CbmLitTrackingGeometryConstructor::Instance()->GetNofMuchTrdStations());
      trdFinderBranch->SetNofIterations(1);
      trdFinderBranch->SetMaxNofMissingHits(list_of(4));
      trdFinderBranch->SetPDG(list_of(211));
      trdFinderBranch->SetChiSqStripHitCut(list_of(9.));
      trdFinderBranch->SetChiSqPixelHitCut(list_of(25.));
      trdFinderBranch->SetSigmaCoef(list_of(5.));
      return TrackFinderPtr(trdFinderBranch);
   } else if (name == "mu_nn") {
      CbmLitTrackFinderNN* muchFinderNN = new CbmLitTrackFinderNN();
      muchFinderNN->SetSeedSelection(CreateTrackSelection("momentum_seed"));
      muchFinderNN->SetFinalSelection(CreateTrackSelection("much_final"));
      muchFinderNN->SetPropagator(CreateTrackPropagator("lit"));
      muchFinderNN->SetFilter(CreateTrackUpdate("kalman"));
      muchFinderNN->SetNofStations(CbmLitTrackingGeometryConstructor::Instance()->GetNofMuchTrdStations());
      muchFinderNN->SetNofIterations(1);
      muchFinderNN->SetMaxNofMissingHits(list_of(4));
      muchFinderNN->SetPDG(list_of(13));
      muchFinderNN->SetChiSqStripHitCut(list_of(9.));
      muchFinderNN->SetChiSqPixelHitCut(list_of(25.));
      muchFinderNN->SetSigmaCoef(list_of(5.));
      return TrackFinderPtr(muchFinderNN);
   } else if (name == "mu_branch") {
      CbmLitTrackFinderBranch* muchFinderBranch = new CbmLitTrackFinderBranch();
      muchFinderBranch->SetSeedSelection(CreateTrackSelection("momentum_seed"));
      muchFinderBranch->SetFinalSelection(CreateTrackSelection("much_final"));
      muchFinderBranch->SetPropagator(CreateTrackPropagator("lit"));
      muchFinderBranch->SetFilter(CreateTrackUpdate("kalman"));
      muchFinderBranch->SetNofStations(CbmLitTrackingGeometryConstructor::Instance()->GetNofMuchTrdStations());
      muchFinderBranch->SetNofIterations(1);
      muchFinderBranch->SetMaxNofMissingHits(list_of(4));
      muchFinderBranch->SetPDG(list_of(13));
      muchFinderBranch->SetChiSqStripHitCut(list_of(9.));
      muchFinderBranch->SetChiSqPixelHitCut(list_of(25.));
      muchFinderBranch->SetSigmaCoef(list_of(5.));
      return TrackFinderPtr(muchFinderBranch);
   } else if (name == "mvd_nn") {
      CbmLitTrackFinderNN* mvdFinderNN = new CbmLitTrackFinderNN();
      mvdFinderNN->SetSeedSelection(CreateTrackSelection("empty"));
      mvdFinderNN->SetFinalSelection(CreateTrackSelection("empty"));
      mvdFinderNN->SetPropagator(CreateTrackPropagator("rk4"));
      mvdFinderNN->SetFilter(CreateTrackUpdate("kalman"));
      mvdFinderNN->SetNofStations(CbmLitTrackingGeometryConstructor::Instance()->GetNofMvdStations());
      mvdFinderNN->SetNofIterations(1);
      mvdFinderNN->SetMaxNofMissingHits(list_of(1));
      mvdFinderNN->SetPDG(list_of(211));
      mvdFinderNN->SetChiSqStripHitCut(list_of(9.));
      mvdFinderNN->SetChiSqPixelHitCut(list_of(15.));
      mvdFinderNN->SetSigmaCoef(list_of(5.));
      return TrackFinderPtr(mvdFinderNN);
   }
   return TrackFinderPtr();
}

HitToTrackMergerPtr CbmLitToolFactory::CreateHitToTrackMerger(
   const std::string& name)
{
   assert(name == "tof_nearest_hit" || name == "tof_all_hits");
   if (name == "tof_nearest_hit") {
      CbmLitNearestHitTofMerger* nhMerger = new CbmLitNearestHitTofMerger();
      nhMerger->SetFieldPropagator(CreateTrackPropagator("lit"));
      nhMerger->SetLinePropagator(CreateTrackPropagator("line"));
      nhMerger->SetFilter(CreateTrackUpdate("kalman"));
      nhMerger->SetPDG(211);
      nhMerger->SetChiSqCut(50.);//13.86);
      return HitToTrackMergerPtr(nhMerger);
   } else if (name == "tof_all_hits") {
      CbmLitAllHitsTofMerger* nhMerger = new CbmLitAllHitsTofMerger();
      nhMerger->SetFieldPropagator(CreateTrackPropagator("lit"));
      nhMerger->SetLinePropagator(CreateTrackPropagator("line"));
      nhMerger->SetFilter(CreateTrackUpdate("kalman"));
      nhMerger->SetPDG(211);
      nhMerger->SetChiSqCut(50.);//13.86);
      return HitToTrackMergerPtr(nhMerger);
   }
   return HitToTrackMergerPtr();
}
