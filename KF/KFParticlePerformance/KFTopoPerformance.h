//----------------------------------------------------------------------------
// Implementation of the KFParticle class
// .
// @author  I.Kisel, I.Kulakov, M.Zyzak
// @version 1.0
// @since   20.08.13
// 
// 
//  -= Copyright &copy ALICE HLT and CBM L1 Groups =-
//____________________________________________________________________________

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef KFTOPOPERFORMANCE_H
#define KFTOPOPERFORMANCE_H


#include "KFParticlePerformanceBase.h"

#include "KFMCVertex.h"
#include "KFMCTrack.h"
#include <fstream>
#include <cstdio>
#include <map>

#include "KFPartMatch.h"
#include "KFMCParticle.h"

class AliHLTTPCCAGBTracker;


class TObject;
class TParticle;
class KFParticleTopoReconstructor;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

class TFile;

/**
 * @class KFTopoPerformance. Don't use w\o GlobalPerformance
 */
class KFTopoPerformance: public KFParticlePerformanceBase
{
 public:
  
  KFTopoPerformance();
  virtual ~KFTopoPerformance();
#ifdef KFPWITHTRACKER
  virtual void SetNewEvent(
    const AliHLTTPCCAGBTracker * const Tracker,
    AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
    AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
    AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints);
#endif  
  void SetTopoReconstructor( const KFParticleTopoReconstructor * const TopoReconstructor ); // use together with SetNewEvent !!!
  const KFParticleTopoReconstructor * GetTopoReconstructor() const { return fTopoReconstructor; }
    
    /// Efficiency
    // Check if MC track is reconstructable. Calculate set of MC track. Etc.
  virtual void CheckMCTracks(); // fill mcData.
    // Find reco-MCTracks correspondence
  virtual void MatchTracks();   // fill recoData.
    // Calculate efficiencies
  virtual void EfficiencyPerformance(){}; // current don't use eff

  virtual void PrintEfficiencyStatistic(){}; // current don't use eff
  virtual void PrintEfficiency()         {};
  
    /// Histograms
    //     virtual void CreateHistos(string histoDir);
  virtual void FillHistos();

  void AddV0Histos();
  
  void SetTrackMatch(const vector<int>& trackMatch) { fTrackMatch = trackMatch;}
  void SetMCTracks(const vector<KFMCTrack>& mcTracks)
  { 
    
    vMCTracks = mcTracks;
  }
  
  const KFPartEfficiencies GetEfficiency() const { return fParteff; }
  void SetPrintEffFrequency(int n) { fPrintEffFrequency = n;}

  const vector<KFMCParticle>& MCParticles() { return vMCParticles; }
  const vector<KFPartMatch>& ParticlesMatch() { return RtoMCParticleId; }
  
 private:

  const KFTopoPerformance& operator = (const KFTopoPerformance&);
  KFTopoPerformance(const KFTopoPerformance&);
   
  void GetMCParticles();
  void MatchParticles();
  void MatchPV();
  void CalculateEfficiency();
  void CalculatePVEfficiency();
  void FindReconstructableMCParticles();
  void CheckMCParticleIsReconstructable(KFMCParticle &part);
  void FindReconstructableMCVertices();
  
  const KFParticleTopoReconstructor *fTopoReconstructor;

  vector<KFMCVertex> fPrimVertices; // primary vertex positions (currently only one vertex is implemented)
  vector<int> fMCTrackToMCPVMatch; // match between MC tracks and PV 
  vector<double> fPVPurity;
  vector<double> fPVTracksRate[4]; //0 - ghost tracks, 1 - from trigger PV, 2 - from pileup, 3 - from physics bg
  vector<int> fNCorrectPVTracks;

  vector<int> fTrackMatch;
  vector<KFMCTrack> vMCTracks;  // MC particles
  vector<KFMCParticle> vMCParticles;  // MC particles

  vector<KFPartMatch> MCtoRParticleId; // array for match MC and reco particles
  vector<KFPartMatch> RtoMCParticleId;

  vector<KFPartMatch> MCtoRPVId; // array for match MC and reco PV
  vector<KFPartMatch> RtoMCPVId;
  
  int fPrintEffFrequency;
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
