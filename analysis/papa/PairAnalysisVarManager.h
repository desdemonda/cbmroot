#ifndef PAIRANALYSISVARMANAGER_H
#define PAIRANALYSISVARMANAGER_H
/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * based on the ALICE-papa package                                  */

//#############################################################
//#                                                           #
//#         Class PairAnalysisVarManager                        #
//#         Class for management of available variables       #
//#                                                           #
//#  Authors:                                                 #
//#   Julian    Book,     Uni Ffm / Julian.Book@cern.ch       #
//#                                                           #
//#############################################################

#include <TNamed.h>
#include <TDatabasePDG.h>

#include <TBits.h>
#include <TRandom3.h>
#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>

#include <FairRootManager.h>
#include <CbmRichElectronIdAnn.h>
#include <CbmPixelHit.h>
#include <CbmRichHit.h>
#include <CbmTrdHit.h>
#include <CbmStsHit.h>

#include <FairTrackParam.h>
#include <FairMCPoint.h>

#include <CbmVertex.h>
#include <CbmKFVertex.h>
#include <CbmGlobalTrack.h>
#include <CbmStsTrack.h>
#include <CbmTrdTrack.h>
#include <CbmRichRing.h>
#include <CbmMCTrack.h>
#include <CbmDetectorList.h>
#include <CbmTrackMatchNew.h>

//#include <CbmL1PFFitter.h>
#include <CbmStsKFTrackFitter.h>

#include "PairAnalysisEvent.h"
#include "PairAnalysisPair.h"
#include "PairAnalysisPairLV.h"
#include "PairAnalysisTrack.h"
#include "PairAnalysisMC.h"
#include "PairAnalysisHelper.h"

#include "assert.h"
#include "vector"
using std::vector;

//________________________________________________________________
class PairAnalysisVarManager : public TNamed {
  
public:

  enum ValueTypes {
// Hit specific variables
    kPosX =1,                // X position [cm]
    kPosY,                   // Y position [cm]
    kPosZ,                   // Z position [cm]
    kEloss,                  // TRD energy loss dEdx+TR
    kElossTR,                // TRD energy loss TR only
    kNPhotons,               // RICH number of photons in this hit
    kPmtId,                  // RICH photomultiplier number
    kHitMax,
// Particle specific variables
    kPx = kHitMax,           // px
    kPy,                     // py
    kPz,                     // pz
    kPt,                     // transverse momentum
    kPtSq,                   // transverse momentum squared
    kP,                      // momentum
    kXv,                     // vertex position in x
    kYv,                     // vertex position in y
    kZv,                     // vertex position in z
    kOneOverPt,              // 1/pt
    kPhi,                    // phi angle
    kTheta,                  // theta angle
    kEta,                    // pseudo-rapidity
    kY,                      // rapidity
    kYlab,                   // rapidity lab
    kE,                      // energy
    kM,                      // mass
    kCharge,                 // charge
    kImpactParXY,            // Impact parameter in XY plane
    //kImpactParZ,             // Impact parameter in Z
    kInclAngle,              // inclination angle
    kParticleMax,
// Track specific variables
    // global track
    kTrackLength=kParticleMax, // Track length
    kTrackChi2NDF,           // chi2/ndf
    kPin,                    // first point momentum (GeV/c)
    kPtin,                   // first point transverse momentum (GeV/c)
    kPout,                   // last point momentum (GeV/c)
    kPtout,                  // last point transverse momentum (GeV/c)
    // trd track information
    kTRDSignal,
    kTRDPidWkn,              // PID value Wkn method
    kTRDPidANN,              // PID value Artificial Neural Network (ANN-method)
    kTRDPidLikeEL,           // PID value Likelihood method: electron
    kTRDPidLikePI,           // PID value Likelihood method: pion
    kTRDPidLikeKA,           // PID value Likelihood method: kaon
    kTRDPidLikePR,           // PID value Likelihood method: proton
    kTRDPidLikeMU,           // PID value Likelihood method: muon
    kTRDHits,                // number of TRD hits
    kTRDChi2NDF,             // chi2/ndf TRD
    kTRDPin,                 // first point TRD momentum (GeV/c)
    kTRDPtin,                // first point TRD transverse momentum (GeV/c)
    kTRDPout,                // last point TRD momentum (GeV/c)
    kTRDPtout,               // last point TRD transverse momentum (GeV/c)
    // sts track information
    kMVDHits,                // number of MVD hits
    kImpactParZ,             // Impact parameter of track at target z, in units of its error  
    kSTSHits,                // number of STS hits
    kSTSChi2NDF,             // chi2/ndf STS
    kSTSPin,                 // first point STS momentum (GeV/c)
    kSTSPtin,                // first point STS transverse momentum (GeV/c)
    kSTSPout,                // last point STS momentum (GeV/c)
    kSTSPtout,               // last point STS transverse momentum (GeV/c)
    kSTSXv,                  // STS point: x-coordinate
    kSTSYv,                  // STS point: y-coordinate
    kSTSZv,                  // STS point: z-coordinate
    kSTSChi2NDFtoVtx,        // chi2/ndf impact parameter STS track to primary vertex in (sigmas)
    kSTSFirstHitPosZ,        // position of the first hit in the STS (cm)
    // rich ring information
    kRICHPidANN,             // PID value Artificial Neural Network (ANN-method)
    kRICHHitsOnRing,         // number of RICH hits on the ring
    kRICHHits,               // number of RICH hits
    kRICHChi2NDF,            // chi2/ndf RICH
    kRICHRadius,             // RICH ring radius
    kTrackMax,

// Pair specific variables
    kChi2NDF = kTrackMax,    // Chi^2/NDF
    kDecayLength,            // decay length
    kR,                      // distance to the origin
    kOpeningAngle,           // opening angle
    kCosPointingAngle,       // cosine of the pointing angle
    kArmAlpha,               // Armenteros-Podolanski alpha
    kArmPt,                  // Armenteros-Podolanski pt
    // helicity picture: Z-axis is considered the direction of the mother's 3-momentum vector
    kThetaHE,                // theta in mother's rest frame in the helicity picture 
    kPhiHE,                  // phi in mother's rest frame in the helicity picture
    kThetaSqHE,              // squared value of kThetaHE
    kCos2PhiHE,              // Cosine of 2*phi in mother's rest frame in the helicity picture
    kCosTilPhiHE,            // Shifted phi depending on kThetaHE
    // Collins-Soper picture: Z-axis is considered the direction of the vectorial difference between 
    // the 3-mom vectors of target and projectile beams
    kThetaCS,                // theta in mother's rest frame in Collins-Soper picture
    kPhiCS,                  // phi in mother's rest frame in Collins-Soper picture
    kThetaSqCS,              // squared value of kThetaCS
    kCos2PhiCS,              // Cosine of 2*phi in mother's rest frame in the Collins-Soper picture
    kCosTilPhiCS,            // Shifted phi depending on kThetaCS
    kPsiPair,                // phi in mother's rest frame in Collins-Soper picture
    kPhivPair,               // angle between ee plane and the magnetic field (can be useful for conversion rejection)
	
    kLegDist,                // distance of the legs
    kLegDistXY,              // distance of the legs in XY
    kDeltaEta,         // Absolute value of Delta Eta for the legs
    kDeltaPhi,           // Absolute value of Delta Phi for the legs
    kMerr,                   // error of mass calculation
    kDCA,                    // distance of closest approach TODO: not implemented yet
    kPairType,               // type of the pair, like like sign ++ unlikesign ...
    kPseudoProperTime,       // pseudo proper time
    kPseudoProperTimeErr,    // pseudo proper time error
    kPseudoProperTimeResolution,     // resolution for pseudo proper decay time (reconstructed - MC truth)
    kPseudoProperTimePull,   // normalizd resolution for pseudo proper time = (reco - MC truth)/dReco
    kTRDpidEffPair,          // TRD pid efficieny from conversion electrons
    kMomAsymDau1,            // momentum fraction of daughter1
    kMomAsymDau2,            // momentum fraction of daughter2
    kPairEff,                // pair efficiency
    kOneOverPairEff,         // 1 / pair efficiency (correction factor)
    kOneOverPairEffSq,        // 1 / pair efficiency squared (correction factor)
    kRndmPair,               // radomly created number (used to apply special signal reduction cuts)
    kPairs,                  // number of Ev1PM pair candidates after all cuts
    kPairMax,                 //

  // Event specific variables
    kXvPrim=kPairMax,        /// prim vertex [cm]
    kYvPrim,                 /// prim vertex [cm]
    kZvPrim,                 /// prim vertex [cm]
    kVtxChi,                 /// chi2
    kVtxNDF,                 /// nof degrees of freedom 
    kXRes,                   // primary vertex x-resolution
    kYRes,                   // primary vertex y-resolution
    kZRes,                   // primary vertex z-resolution
    kPhiMaxPt,               // phi angle of the track with maximum pt
    kMaxPt,                  // track with maximum pt

    kNTrk,                   // number of tracks (or tracklets) TODO: ambiguous
    kTracks,                 // track after all cuts
    kNVtxContrib,            /// number of primary vertex contibutors

    kRefMult,                // reference multiplicity (only in AODs) should be Ntrk w/o double counts
    kRefMultTPConly,         // TPC only Reference Multiplicty (AliESDtrackCuts::GetReferenceMultiplicity(&esd, kTRUE))

    kNch,                    // MC true number of charged particles in |eta|<1.6
    kNch05,                  // MC true number of charged particles in |eta|<0.5
    kNch10,                  // MC true number of charged particles in |eta|<1.0

    kCentrality,             // event centrality fraction V0M
    kTriggerInclONL,         // online trigger bits fired (inclusive)
    kTriggerInclOFF,         // offline trigger bits fired (inclusive)
    kTriggerExclOFF,         // offline only this trigger bit fired (exclusive)
    kNevents,                // event counter
    kRunNumber,              // run number
    kYbeam,                  // beam rapdity
    kEbeam,                  // beam energy
    kMixingBin,
    kNMaxValues,              //

// MC information
    // Hit specific variables
    kPosXMC=kNMaxValues,       // X position [cm]
    kPosYMC,                   // Y position [cm]
    kPosZMC,                   // Z position [cm]
    kElossMC,                  // energy loss dEdx+TR
    kHitMaxMC,
    // Particle specific MC variables
    kPxMC=kHitMaxMC,           // px
    kPyMC,                     // py
    kPzMC,                     // pz
    kPtMC,                     // transverse momentum
    kPtSqMC,                   // transverse momentum squared
    kPMC,                      // momentum
    kXvMC,                     // vertex position in x
    kYvMC,                     // vertex position in y
    kZvMC,                     // vertex position in z
    kOneOverPtMC,              // 1/pt
    kPhiMC,                    // phi angle
    kThetaMC,                  // theta angle
    kEtaMC,                    // pseudo-rapidity
    kYMC,                      // rapidity
    kYlabMC,                   // rapidity lab
    kEMC,                      // energy
    kMMC,                      // mass
    kChargeMC,                 // charge
    kPdgCode,                  // PDG code
    kPdgCodeMother,            // PDG code of the mother
    kPdgCodeGrandMother,       // PDG code of the grand mother
    kGeantId,                  // geant process id (see TMCProcess)
    kWeight,                   // weight NxBR
    kParticleMaxMC,

    // Track specific MC variables
    kTRDHitsMC=kParticleMaxMC, // number of TRD hits
    kMVDHitsMC,                // number of MVD hits
    kSTSHitsMC,                // number of STS hits
    kRICHHitsMC,               // number of RICH hits
    kTRDMCPoints,              // number of TRD MC Points in reconstructed track
    kTRDTrueHits,              // number of true TRD hits in reconstructed track
    kTRDFakeHits,              // number of fake TRD hits in reconstructed track
    kSTSTrueHits,              // number of true STS hits in reconstructed track
    kSTSFakeHits,              // number of fake STS hits in reconstructed track
    kTRDisMC,                  // status bit for matching btw. glbl. and local MC track
    kMVDisMC,                  // status bit for matching btw. glbl. and local MC track
    kSTSisMC,                  // status bit for matching btw. glbl. and local MC track
    kRICHisMC,                 // status bit for matching btw. glbl. and local MC track
    kTrackMaxMC,

    // Pair specific MC variables
    kPairMaxMC=kTrackMaxMC,

    // Event specific MCvariables
    kNTrkMC=kPairMaxMC,      // number of MC tracks
    kSTSMatches,             // number of matched STS tracks
    kTRDMatches,             // number of matched TRD tracks
    kVageMatches,            // number of MC tracks (STS) matched to multiple reconstr. track
    kNMaxValuesMC

  };


  PairAnalysisVarManager();
  PairAnalysisVarManager(const char* name, const char* title);
  virtual ~PairAnalysisVarManager();

  static void InitFormulas();
  static void InitKFFitter();

  static void Fill(             const TObject* particle,                    Double_t * const values);
  static void FillVarMCParticle(const CbmMCTrack *p1, const CbmMCTrack *p2, Double_t * const values);

  // Setter
  static void SetFillMap(        TBits   *map)                   { fgFillMap=map;     }
  static void SetEvent(          PairAnalysisEvent * const ev);
  static void SetEventData(const Double_t data[PairAnalysisVarManager::kNMaxValuesMC]);
  static void SetValue(          ValueTypes   var, Double_t val) { fgData[var]  =val; }
  static void SetRichPidResponse(CbmRichElectronIdAnn  *pid)     { fgRichElIdAnn=pid; }

  // Getter
  static PairAnalysisEvent* GetCurrentEvent()  { return fgEvent;       }
  static const CbmKFVertex* GetKFVertex()       { return fgKFVertex;    }
  static const char* GetValueName(Int_t i)      { return (i>=0&&i<kNMaxValuesMC)?fgkParticleNames[i][0]:""; }
  static const char* GetValueLabel(Int_t i)     { return (i>=0&&i<kNMaxValuesMC)?fgkParticleNames[i][1]:""; }
  static const char* GetValueUnit(Int_t i)      { return (i>=0&&i<kNMaxValuesMC)?fgkParticleNames[i][2]:""; }
  static Double_t* GetData()              { return fgData;        }
  static Double_t GetValue(ValueTypes val)      { return fgData[val];   }
  static UInt_t GetValueType(const char* valname);
  static UInt_t GetValueTypeMC(UInt_t var);

  static UInt_t* GetArray(ValueTypes var0,
			  ValueTypes var1=kNMaxValuesMC, ValueTypes var2=kNMaxValuesMC, ValueTypes var3=kNMaxValuesMC,
			  ValueTypes var4=kNMaxValuesMC, ValueTypes var5=kNMaxValuesMC, ValueTypes var6=kNMaxValuesMC,
			  ValueTypes var7=kNMaxValuesMC, ValueTypes var8=kNMaxValuesMC, ValueTypes var9=kNMaxValuesMC);

  // data member
  static TFormula *fgFormula[kNMaxValuesMC];        // variable formulas

private:

  // data member
  static Double_t fgData[kNMaxValuesMC];                   //! data
  static const char* fgkParticleNames[kNMaxValuesMC][3];   // variable names
  //static const char* fgkParticleNamesMC[kNMaxValuesMC]; // MC variable names
  static PairAnalysisEvent *fgEvent;                      // current event pointer
  static CbmKFVertex        *fgKFVertex;                   // kf vertex
  static CbmStsKFTrackFitter*fgKFFitter;                   // kf fitter
  static TBits              *fgFillMap;                    // map for filling variables
  static Int_t               fgCurrentRun;                 // current run number
  static CbmRichElectronIdAnn *fgRichElIdAnn;              // NN electron pid for Rich

  // fill functions
  static Bool_t Req(ValueTypes var) { return (fgFillMap ? fgFillMap->TestBitNumber(var) : kTRUE); }

  static void FillVarPairAnalysisEvent(const PairAnalysisEvent *event, Double_t * const values);
  static void FillVarVertex(            const CbmVertex *vertex,         Double_t * const values);
  static void FillVarPairAnalysisTrack(   const PairAnalysisTrack *track,    Double_t * const values);
  static void FillVarGlobalTrack(       const CbmGlobalTrack *track,     Double_t * const values);
  static void FillVarStsTrack(          const CbmStsTrack *track,        Double_t * const values);
  static void FillVarTrdTrack(          const CbmTrdTrack *track,        Double_t * const values);
  static void FillVarRichRing(          const CbmRichRing *track,        Double_t * const values);
  static void FillVarMCTrack(           const CbmMCTrack *particle,      Double_t * const values);
  static void FillVarPairAnalysisPair(    const PairAnalysisPair *pair,      Double_t * const values);
  static void FillVarStsHit(            const CbmStsHit *hit,            Double_t * const values);
  static void FillVarTrdHit(            const CbmTrdHit *hit,            Double_t * const values);
  static void FillVarRichHit(           const CbmRichHit *hit,           Double_t * const values);
  static void FillVarPixelHit(          const CbmPixelHit *hit,          Double_t * const values);
  static void FillVarMCPoint(           const FairMCPoint *hit,          Double_t * const values);

  //  static void FillVarKFParticle(const AliKFParticle *pair,   Double_t * const values);

  // setter
  static void ResetArrayData(Int_t to,   Double_t * const values);
  static void ResetArrayDataMC(Int_t to, Double_t * const values);

  PairAnalysisVarManager(const PairAnalysisVarManager &c);
  PairAnalysisVarManager &operator=(const PairAnalysisVarManager &c);

  ClassDef(PairAnalysisVarManager,1);
};


//Inline functions
inline void PairAnalysisVarManager::Fill(const TObject* object, Double_t * const values)
{
  //
  // Main function to fill all available variables according to the type
  //

  //Protect
  if (!object) return;

  if      (object->IsA() == PairAnalysisEvent::Class()) FillVarPairAnalysisEvent(static_cast<const PairAnalysisEvent*>(object), values);
  else if (object->IsA() == CbmVertex::Class())       FillVarVertex(         static_cast<const CbmVertex*>(object),      values);
  else if (object->IsA() == PairAnalysisTrack::Class()) FillVarPairAnalysisTrack(static_cast<const PairAnalysisTrack*>(object),values);
  else if (object->IsA() == CbmGlobalTrack::Class())  FillVarGlobalTrack(    static_cast<const CbmGlobalTrack*>(object), values);
  else if (object->IsA() == CbmStsTrack::Class())     FillVarStsTrack(       static_cast<const CbmStsTrack*>(object),    values);
  else if (object->IsA() == CbmTrdTrack::Class())     FillVarTrdTrack(       static_cast<const CbmTrdTrack*>(object),    values);
  else if (object->IsA() == CbmRichRing::Class())     FillVarRichRing(       static_cast<const CbmRichRing*>(object),    values);
  else if (object->IsA() == CbmMCTrack::Class())      FillVarMCTrack(        static_cast<const CbmMCTrack*>(object),     values);
  else if (object->InheritsFrom(PairAnalysisPair::Class()))  FillVarPairAnalysisPair( static_cast<const PairAnalysisPair*>(object), values);
  else if (object->IsA() == CbmStsHit::Class())       FillVarStsHit(         static_cast<const CbmStsHit*>(object),    values);
  else if (object->IsA() == CbmTrdHit::Class())       FillVarTrdHit(         static_cast<const CbmTrdHit*>(object),    values);
  else if (object->IsA() == CbmRichHit::Class())      FillVarRichHit(        static_cast<const CbmRichHit*>(object),   values);
  else if (object->InheritsFrom(FairMCPoint::Class()))     FillVarMCPoint(   static_cast<const FairMCPoint*>(object),  values);
  else printf(Form("PairAnalysisVarManager::Fill: Type %s is not supported by PairAnalysisVarManager! \n", object->ClassName()));
}


inline void PairAnalysisVarManager::ResetArrayData(Int_t to, Double_t * const values)
{
  // Protect
  if (to >= kNMaxValues) return;
  // Reset
  for (Int_t i=0; i<to; ++i) {
    if(i!=kEbeam)  values[i] = 0.;
  }
  // reset values different from zero
  if(to>=kTrackMax && to>kParticleMax) {
    values[kTRDPidANN]  = -999.;
    values[kRICHPidANN] = -999.;
  }
}


inline void PairAnalysisVarManager::ResetArrayDataMC(Int_t to, Double_t * const values)
{
  // Protect
  if (to >= kNMaxValuesMC) return;
  // Reset
  for (Int_t i=kNMaxValues; i<to; ++i) values[i] = 0.;
  // reset values different from zero
  //  /*
  values[kPdgCode]            = -99999.;
  values[kPdgCodeMother]      = -99999.;
  values[kPdgCodeGrandMother] = -99999.;
  //  */
  //valuesMC[kNumberOfDaughters]  = -999.;
}

inline void PairAnalysisVarManager::FillVarPairAnalysisEvent(const PairAnalysisEvent *event, Double_t * const values)
{
  //
  // Fill event information available into an array
  //

  // Protect
  if(!event) return;

  // Reset array
  ResetArrayData(  kNMaxValues,   values);
  ResetArrayDataMC(kNMaxValuesMC, values);

  // Set
  values[kNTrk]         = event->GetNumberOfTracks();
  values[kSTSMatches]   = event->GetNumberOfMatches(kSTS);
  values[kTRDMatches]   = event->GetNumberOfMatches(kTRD);
  values[kVageMatches]  = event->GetNumberOfVageMatches();
  values[kNTrkMC]       = event->GetNumberOfMCTracks();
  const Double_t proMass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
  values[kYbeam]        = TMath::ATanH( TMath::Sqrt(1-1/TMath::Power(values[kEbeam] / proMass,2)) );
  //  Printf("beam rapidity new: %f",values[kYbeam]);

  // Set vertex
  FillVarVertex(event->GetPrimaryVertex(),values);

}

inline void PairAnalysisVarManager::FillVarVertex(const CbmVertex *vertex, Double_t * const values)
{
  //
  // Fill vertex information available into an array
  //

  // Protect
  if (!vertex) return;

  // Reset
  // ResetArrayData(kNMaxValues, values);

  // Set
  values[kXvPrim]       = vertex->GetX();
  values[kYvPrim]       = vertex->GetY();
  values[kZvPrim]       = vertex->GetZ();
  values[kNVtxContrib]  = vertex->GetNTracks();
  values[kVtxChi]       = vertex->GetChi2();
  values[kVtxNDF]       = vertex->GetNDF();

}


inline void PairAnalysisVarManager::FillVarPairAnalysisTrack(const PairAnalysisTrack *track, Double_t * const values)
{
  //
  // Fill track information for the all track and its sub tracks
  //

  // Protect
  if(!track) return;

  // Reset
  ResetArrayData(  kTrackMax,   values);
  ResetArrayDataMC(kTrackMaxMC, values);

  // Set track specific variables
  Fill(track->GetGlobalTrack(), values);
  Fill(track->GetStsTrack(),    values);
  Fill(track->GetTrdTrack(),    values);
  Fill(track->GetRichRing(),    values);

  // mc
  Fill(track->GetMCTrack(),     values); // this contains particle infos as well
  if(track->GetTrackMatch(kTRD)) {       // track match specific (accessors via CbmTrackMatchNew)
    values[kTRDMCPoints]    = track->GetTrackMatch(kTRD)->GetNofLinks();
    values[kTRDTrueHits]    = track->GetTrackMatch(kTRD)->GetNofTrueHits();
    values[kTRDFakeHits]    = track->GetTrackMatch(kTRD)->GetNofWrongHits();
  }
  if(track->GetTrackMatch(kSTS)) {
    values[kSTSTrueHits]    = track->GetTrackMatch(kSTS)->GetNofTrueHits();
    values[kSTSFakeHits]    = track->GetTrackMatch(kSTS)->GetNofWrongHits();
  }
  values[kSTSisMC]   = track->TestBit( BIT(14+kSTS) );
  values[kTRDisMC]   = track->TestBit( BIT(14+kTRD) );
  values[kRICHisMC]  = track->TestBit( BIT(14+kRICH));
  values[kMVDisMC]   = track->TestBit( BIT(14+kMVD) );
  values[kWeight]    = track->GetWeight();

  // Reset
  ResetArrayData(  kParticleMax,   values);

  // Set DATA default (currently Sts track parmams first)
  values[kPx]        = track->Px();
  values[kPy]        = track->Py();
  values[kPz]        = track->Pz();
  values[kPt]        = track->Pt();
  values[kPtSq]      = track->Pt()*track->Pt();
  values[kP]         = track->P();

  values[kXv]        = track->Xv();
  values[kYv]        = track->Yv();
  values[kZv]        = track->Zv();

  values[kOneOverPt] = (track->Pt()>1.0e-3 ? track->OneOverPt() : 0.0);
  values[kPhi]       = (TMath::IsNaN(track->Phi()) ? -999. : TVector2::Phi_0_2pi(track->Phi()));
  values[kTheta]     = track->Theta();
  //  values[kEta]       = track->Eta();
  values[kY]         = track->Y() - values[kYbeam];
  values[kYlab]      = track->Y();
  values[kE]         = track->E();
  values[kM]         = track->M();
  values[kCharge]    = track->Charge();
  values[kPdgCode]   = track->PdgCode();

  values[kInclAngle] = TMath::ASin(track->Pt()/track->P());

}

inline void PairAnalysisVarManager::FillVarGlobalTrack(const CbmGlobalTrack *track, Double_t * const values)
{
  //
  // Fill track information for the global track into array
  //

  // Protect
  if(!track) return;

  // Set
  values[kTrackChi2NDF]= (track->GetNDF()>0. ? track->GetChi2()/track->GetNDF() : -999.);
  values[kTrackLength] = track->GetLength();
  // accessors via first FairTrackParam
  TVector3 mom;
  track->GetParamFirst()->Momentum(mom);
  values[kPin]         = mom.Mag();
  values[kPtin]        = mom.Pt();
  track->GetParamLast()->Momentum(mom);
  values[kPout]        = mom.Mag();
  values[kPtout]       = mom.Pt();
  values[kCharge]      = (track->GetParamFirst()->GetQp()>0. ? +1. : -1. );
 
}

inline void PairAnalysisVarManager::FillVarRichRing(const CbmRichRing *track, Double_t * const values)
{
  //
  // Fill track information for the trd track into array
  //

  // Protect
  if(!track) return;

  // Set
  values[kRICHPidANN]      = fgRichElIdAnn->DoSelect(const_cast<CbmRichRing*>(track), values[kP]); // PID value ANN method
  values[kRICHHitsOnRing]  = track->GetNofHitsOnRing();
  values[kRICHHits]        = track->GetNofHits();
  values[kRICHChi2NDF]     = (track->GetNDF()>0. ? track->GetChi2()/track->GetNDF() : -999.);
  values[kRICHRadius]      = track->GetRadius();

}

inline void PairAnalysisVarManager::FillVarTrdTrack(const CbmTrdTrack *track, Double_t * const values)
{
  //
  // Fill track information for the trd track into array
  //

  // Protect
  if(!track) return;

  // Calculate eloss
  TClonesArray *hits   = fgEvent->GetHits(kTRD);
  if(hits &&  track->GetELoss()<1.e-8 /*&& Req(kTRDSignal)*/ ) {
    Double_t eloss = 0;
    for (Int_t ihit=0; ihit < track->GetNofHits(); ihit++){
      Int_t idx = track->GetHitIndex(ihit);
      CbmTrdHit* hit = (CbmTrdHit*) hits->At(idx);
      if(hit) eloss += hit->GetELoss(); // dEdx + TR
    }
    //   printf("track %p \t eloss %.3e \n",track,eloss);
    const_cast<CbmTrdTrack*>(track)->SetELoss(eloss);
  }

  // Set
  values[kTRDSignal]      = track->GetELoss();
  values[kTRDPidWkn]      = track->GetPidWkn(); // PID value Wkn method
  values[kTRDPidANN]      = track->GetPidANN(); // PID value ANN method
  // PID value Likelihood method
  values[kTRDPidLikeEL]   = track->GetPidLikeEL();
  values[kTRDPidLikePI]   = track->GetPidLikePI();
  values[kTRDPidLikeKA]   = track->GetPidLikeKA();
  values[kTRDPidLikePR]   = track->GetPidLikePR();
  values[kTRDPidLikeMU]   = track->GetPidLikeMU();
  // accessors via CbmTrack
  values[kTRDHits]        = track->GetNofHits();
  values[kTRDChi2NDF]     = (track->GetNDF()>0. ? track->GetChiSq()/track->GetNDF() : -999.);
  // accessors via first FairTrackParam
  TVector3 mom;
  track->GetParamFirst()->Momentum(mom);
  values[kTRDPin]         = mom.Mag();
  values[kTRDPtin]        = mom.Pt();
  track->GetParamLast()->Momentum(mom);
  values[kTRDPout]        = mom.Mag();
  values[kTRDPtout]       = mom.Pt();
  //  values[kTRDCharge]      = (track->GetParamFirst()->GetQp()>0. ? +1. : -1. );
 
}

inline void PairAnalysisVarManager::FillVarStsTrack(const CbmStsTrack *track, Double_t * const values)
{
  //
  // Fill track information for the Sts track into array
  //

  // Protect
  if(!track) return;

  // Calculate first hit position
  Double_t min = 9999.;
  TClonesArray *hits   = fgEvent->GetHits(kSTS);
  if(hits /*&& Req(kSTSFirstHitPosZ)*/ ) {
    for (Int_t ihit=0; ihit < track->GetNofHits(); ihit++){
      Int_t idx = track->GetHitIndex(ihit);
      CbmStsHit* hit = (CbmStsHit*) hits->At(idx);
      if(hit && min > hit->GetZ()) {
	min = hit->GetZ();
	//	Printf("hit %d idx %d position %.5f",ihit,idx,min);
      }
    }
  }

  // Set
  values[kMVDHits]        = track->GetNofMvdHits();
  values[kImpactParZ]     = track->GetB();  //Impact parameter of track at target z, in units of its error
  //  printf(" mom %f   impactparz %f \n",values[kPout],values[kImpactParZ]);
  // accessors via CbmTrack
  values[kSTSHits]        = track->GetNofHits();
  values[kSTSChi2NDF]     = (track->GetNDF()>0. ? track->GetChiSq()/track->GetNDF() : -999.);
  // accessors via first FairTrackParam
  TVector3 mom;
  track->GetParamFirst()->Momentum(mom);
  values[kSTSPin]         = mom.Mag();
  values[kSTSPtin]        = mom.Pt();
  track->GetParamFirst()->Position(mom);
  values[kSTSXv]          = mom.X();
  values[kSTSYv]          = mom.Y();
  values[kSTSZv]          = mom.Z();
  track->GetParamLast()->Momentum(mom);
  values[kSTSPout]        = mom.Mag();
  values[kSTSPtout]       = mom.Pt();
  //  values[kSTSCharge]      = (track->GetParamFirst()->GetQp()>0. ? +1. : -1. );

  // TODO propberly implement 
  // using CbmL1PFFitter
  /*
   CbmL1PFFitter fPFFitter;
   vector<CbmStsTrack> stsTracks;
   stsTracks.resize(1);
   stsTracks[0] = *track;
   vector<L1FieldRegion> vField;
   vector<float> chiPrim;
   fPFFitter.GetChiToVertex(stsTracks, vField, chiPrim, fgKFVertex, 3e6);
   values[kSTSChi2NDF] = (stsTracks[0].GetNDF()>0. ? stsTracks[0].GetChiSq() / stsTracks[0].GetNDF() : -999.);
  */
  values[kSTSChi2NDFtoVtx]  = fgKFFitter->GetChiToVertex(const_cast<CbmStsTrack*>(track),fgEvent->GetPrimaryVertex());
  values[kSTSFirstHitPosZ]  = min;

}

inline void PairAnalysisVarManager::FillVarMCParticle(const CbmMCTrack *p1, const CbmMCTrack *p2, Double_t * const values) {
  //
  // fill 2 track information starting from MC legs
  //

  // Protect
  if(!p1 || !p2) return;

  // Get the MC interface if available
  PairAnalysisMC *mc = PairAnalysisMC::Instance();
  if (!mc->HasMC()) return;

  // Reset
  ResetArrayDataMC(kPairMaxMC, values);

  // Set
  CbmMCTrack* mother=0x0;
  Int_t mLabel1 = p1->GetMotherId();
  Int_t mLabel2 = p2->GetMotherId();
  if(mLabel1==mLabel2) mother = mc->GetMCTrackFromMCEvent(mLabel1);
  if(mother)
    FillVarMCTrack(mother,values);
  else {
    PairAnalysisPair *pair = new PairAnalysisPairLV();
    pair->SetMCTracks(p1,p2);
    values[kPxMC]        = pair->Px();
    values[kPyMC]        = pair->Py();
    values[kPzMC]        = pair->Pz();
    values[kPtMC]        = pair->Pt();
    values[kPtSqMC]      = pair->Pt()*pair->Pt();
    values[kPMC]         = pair->P();

    values[kXvMC]        = 0.;
    values[kYvMC]        = 0.;
    values[kZvMC]        = 0.;
    //TODO  values[kTMC]         = 0.;

    values[kOneOverPtMC] = (pair->Pt()>1.0e-3 ? pair->OneOverPt() : 0.0);
    values[kPhiMC]       = (TMath::IsNaN(pair->Phi()) ? -999. : TVector2::Phi_0_2pi(pair->Phi()));
    values[kThetaMC]     = pair->Theta();
    //    values[kEtaMC]       = pair->Eta();
    values[kYMC]         = pair->Y() - values[kYbeam];
    values[kYlabMC]      = pair->Y();
    values[kEMC]         = pair->E();
    values[kMMC]         = pair->M();
    values[kChargeMC]    = p1->GetCharge()*p2->GetCharge();

    // delete the surplus pair
    delete pair;
  }

}

inline void PairAnalysisVarManager::FillVarMCTrack(const CbmMCTrack *particle, Double_t * const values) {
  //
  // fill particle information from MC leg
  //

  // Protect
  if(!particle) return;

  // Get the MC interface if available
  PairAnalysisMC *mc = PairAnalysisMC::Instance();
  if (!mc->HasMC()) return;

  // Reset
  ResetArrayDataMC(kTrackMaxMC, values);

  // Set
  CbmMCTrack* mother=0x0;
  Int_t mLabel1 = particle->GetMotherId();
  mother = mc->GetMCTrackFromMCEvent(mLabel1);
  
  values[kPdgCode]            = particle->GetPdgCode();
  values[kPdgCodeMother]      = (mother ? mother->GetPdgCode() : -99999. );
  CbmMCTrack* granni = 0x0;
  if(mother) granni = mc->GetMCTrackMother(mother);
  Int_t gLabel1 = (mother ? mother->GetMotherId() : -5);
  values[kPdgCodeGrandMother] = (granni ? granni->GetPdgCode() : -99999. );//mc->GetMotherPDG(mother);
  values[kGeantId]            = particle->GetGeantProcessId();

  values[kPxMC]        = particle->GetPx();
  values[kPyMC]        = particle->GetPy();
  values[kPzMC]        = particle->GetPz();
  values[kPtMC]        = particle->GetPt();
  values[kPtSqMC]      = particle->GetPt()*particle->GetPt();
  values[kPMC]         = particle->GetP();

  values[kXvMC]        = particle->GetStartX();
  values[kYvMC]        = particle->GetStartY();
  values[kZvMC]        = particle->GetStartZ();
  //TODO  values[kTMC]         = particle->GetStartT(); [ns]

  TLorentzVector mom;
  if(particle) particle->Get4Momentum(mom);
  values[kOneOverPtMC] = (particle->GetPt()>1.0e-3 ? 1./particle->GetPt() : 0.0);
  values[kPhiMC]       = (TMath::IsNaN(mom.Phi()) ? -999. : TVector2::Phi_0_2pi(mom.Phi()));
  values[kThetaMC]     = mom.Theta();
  //  values[kEtaMC]       = mom.Eta();
  values[kYMC]         = particle->GetRapidity() - values[kYbeam];;
  values[kYlabMC]      = particle->GetRapidity();
  values[kEMC]         = particle->GetEnergy();
  values[kMMC]         = mom.M();//particle->GetMass();
  values[kChargeMC]    = particle->GetCharge();

  // detector info
  values[kRICHHitsMC]  = particle->GetNPoints(kRICH);
  values[kTRDHitsMC]   = particle->GetNPoints(kTRD);
  values[kMVDHitsMC]   = particle->GetNPoints(kMVD);
  values[kSTSHitsMC]   = particle->GetNPoints(kSTS);  

}

inline void PairAnalysisVarManager::FillVarPairAnalysisPair(const PairAnalysisPair *pair, Double_t * const values)
{
  //
  // Fill pair information available for histogramming into an array
  //

  // Protect
  if(!pair) return;

  // Reset
  ResetArrayData(kPairMax, values);

  // Set
  values[kPairType]  = pair->GetType();

  values[kPx]        = pair->Px();
  values[kPy]        = pair->Py();
  values[kPz]        = pair->Pz();
  values[kPt]        = pair->Pt();
  values[kPtSq]      = pair->Pt()*pair->Pt();
  values[kP]         = pair->P();

  values[kXv]        = pair->Xv();
  values[kYv]        = pair->Yv();
  values[kZv]        = pair->Zv();

  values[kOneOverPt] = (pair->Pt()>1.0e-3 ? pair->OneOverPt() : 0.0);
  values[kPhi]       = (TMath::IsNaN(pair->Phi()) ? -999. : TVector2::Phi_0_2pi(pair->Phi()));
  values[kTheta]     = pair->Theta();
  //  values[kEta]       = pair->Eta();
  values[kY]         = pair->Y() - values[kYbeam];
  values[kYlab]      = pair->Y();
  values[kE]         = pair->E();
  values[kM]         = pair->M();
  values[kCharge]    = pair->Charge();

  ///TODO: check
  /* values[kPdgCode]=-1; */
  /* values[kPdgCodeMother]=-1; */
  /* values[kPdgCodeGrandMother]=-1; */
  values[kWeight]    = pair->GetWeight();

  Double_t thetaHE=0;
  Double_t phiHE=0;
  Double_t thetaCS=0;
  Double_t phiCS=0;
  if(Req(kThetaHE) || Req(kPhiHE) || Req(kThetaCS) || Req(kPhiCS)) {
    pair->GetThetaPhiCM(thetaHE,phiHE,thetaCS,phiCS);

    values[kThetaHE]      = thetaHE;
    values[kPhiHE]        = phiHE;
    values[kThetaSqHE]    = thetaHE * thetaHE;
    values[kCos2PhiHE]    = TMath::Cos(2.0*phiHE);
    values[kCosTilPhiHE]  = (thetaHE>0)?(TMath::Cos(phiHE-TMath::Pi()/4.)):(TMath::Cos(phiHE-3*TMath::Pi()/4.));
    values[kThetaCS]      = thetaCS;
    values[kPhiCS]        = phiCS;
    values[kThetaSqCS]    = thetaCS * thetaCS;
    values[kCos2PhiCS]    = TMath::Cos(2.0*phiCS);
    values[kCosTilPhiCS]  = (thetaCS>0)?(TMath::Cos(phiCS-TMath::Pi()/4.)):(TMath::Cos(phiCS-3*TMath::Pi()/4.));
  }

  if(Req(kChi2NDF))          values[kChi2NDF]          = pair->GetChi2()/pair->GetNdf();
  if(Req(kDecayLength))      values[kDecayLength]      = pair->GetDecayLength();
  if(Req(kR))                values[kR]                = pair->GetR();
  if(Req(kOpeningAngle))     values[kOpeningAngle]     = pair->OpeningAngle();
  if(Req(kCosPointingAngle)) values[kCosPointingAngle] = fgEvent ? pair->GetCosPointingAngle(fgEvent->GetPrimaryVertex()) : -1;

  if(Req(kLegDist))   values[kLegDist]      = pair->DistanceDaughters();
  if(Req(kLegDistXY)) values[kLegDistXY]    = pair->DistanceDaughtersXY();
  if(Req(kDeltaEta))  values[kDeltaEta]     = pair->DeltaEta();
  if(Req(kDeltaPhi))  values[kDeltaPhi]     = pair->DeltaPhi();

  // Armenteros-Podolanski quantities
  if(Req(kArmAlpha)) values[kArmAlpha]     = pair->GetArmAlpha();
  if(Req(kArmPt))    values[kArmPt]        = pair->GetArmPt();

  //if(Req(kPsiPair))  values[kPsiPair]      = fgEvent ? pair->PsiPair(fgEvent->GetMagneticField()) : -5;
  //if(Req(kPhivPair)) values[kPhivPair]      = fgEvent ? pair->PhivPair(fgEvent->GetMagneticField()) : -5;

  // impact parameter
  Double_t d0z0[2]={-999., -999.};
  if( (Req(kImpactParXY) || Req(kImpactParZ)) && fgEvent) pair->GetDCA(fgEvent->GetPrimaryVertex(), d0z0);
  values[kImpactParXY]   = d0z0[0];
  values[kImpactParZ]    = d0z0[1];

  if(Req(kRndmPair)) values[kRndmPair] = gRandom->Rndm();

}


inline void PairAnalysisVarManager::FillVarPixelHit(const CbmPixelHit *hit, Double_t * const values)
{
  //
  // Fill hit information for the rich hit into array
  //

  // Protect
  if(!hit) return;

  // accessors via CbmPixelHit
  values[kPosX]     = hit->GetX();
  values[kPosY]     = hit->GetY();
  // accessors via CbmHit
  values[kPosZ]     = hit->GetZ();

}

inline void PairAnalysisVarManager::FillVarStsHit(const CbmStsHit *hit, Double_t * const values)
{
  //
  // Fill hit information for the sts hit into array
  //

  // Protect
  if(!hit) return;

  // Reset array
  ResetArrayData(  kHitMax,   values);

  // accessors via CbmPixelHit & CbmHit
  FillVarPixelHit(hit, values);

  // Set
  // ...

}

inline void PairAnalysisVarManager::FillVarRichHit(const CbmRichHit *hit, Double_t * const values)
{
  //
  // Fill hit information for the rich hit into array
  //

  // Protect
  if(!hit) return;

  // Reset array
  ResetArrayData(  kHitMax,   values);

  // accessors via CbmPixelHit & CbmHit
  FillVarPixelHit(hit, values);

  // Set
  values[kNPhotons] = hit->GetNPhotons();
  values[kPmtId]    = hit->GetPmtId();


}

inline void PairAnalysisVarManager::FillVarTrdHit(const CbmTrdHit *hit, Double_t * const values)
{
  //
  // Fill hit information for the trd hit into array
  //

  // Protect
  if(!hit) return;

  // Reset array
  ResetArrayData(  kHitMax,   values);

  // accessors via CbmPixelHit & CbmHit
  FillVarPixelHit(hit, values);

  // Set
  values[kEloss]   = hit->GetELoss(); // dEdx + TR
  values[kElossTR] = hit->GetELossTR(); // TR
  //  Printf("eloss trd: %.3e (%.3e TR)",hit->GetELoss(),hit->GetELossTR());
}

inline void PairAnalysisVarManager::FillVarMCPoint(const FairMCPoint *hit, Double_t * const values)
{
  //
  // Fill MC hit information
  //

  // Protect
  if(!hit) return;

  // Reset array
  ResetArrayDataMC(  kHitMaxMC,   values);

  // Set
  values[kPosXMC]     = hit->GetX();
  values[kPosYMC]     = hit->GetY();
  values[kPosZMC]     = hit->GetZ();
  values[kElossMC]    = hit->GetEnergyLoss();

}

inline void PairAnalysisVarManager::SetEvent(PairAnalysisEvent * const ev)
{
  //
  // set event and vertex
  //

  // Set
  fgEvent = ev;

  // Reset
  if (fgKFVertex) delete fgKFVertex;
  fgKFVertex=0x0;

  // Set
  if (ev && ev->GetPrimaryVertex()) fgKFVertex=new CbmKFVertex(*ev->GetPrimaryVertex());
  Fill(fgEvent, fgData);
}

inline void PairAnalysisVarManager::SetEventData(const Double_t data[PairAnalysisVarManager::kNMaxValuesMC])
{
  for (Int_t i=0; i<kNMaxValuesMC;++i) fgData[i]=0.;
  //  for (Int_t i=kPairMax; i<kNMaxValuesMC;++i) fgData[i]=data[i];
}


inline UInt_t* PairAnalysisVarManager::GetArray(ValueTypes var0,
						 ValueTypes var1, ValueTypes var2, ValueTypes var3,
						 ValueTypes var4, ValueTypes var5, ValueTypes var6,
						 ValueTypes var7, ValueTypes var8, ValueTypes var9) {
  //
  // build var array for e.g. TFormula's, THnBase's
  //
  static UInt_t arr[10] = {0};
  arr[0]=static_cast<UInt_t>(var0);
  arr[1]=static_cast<UInt_t>(var1);
  arr[2]=static_cast<UInt_t>(var2);
  arr[3]=static_cast<UInt_t>(var3);
  arr[4]=static_cast<UInt_t>(var4);
  arr[5]=static_cast<UInt_t>(var5);
  arr[6]=static_cast<UInt_t>(var6);
  arr[7]=static_cast<UInt_t>(var7);
  arr[8]=static_cast<UInt_t>(var8);
  arr[9]=static_cast<UInt_t>(var9);
  return arr;

}

inline void PairAnalysisVarManager::InitFormulas() {
  if(fgFormula[1]) return;
  for(Int_t i=1; i<kNMaxValuesMC-1; ++i) {
    fgFormula[i] = new TFormula(fgkParticleNames[i][0],"[0]");
    fgFormula[i]->SetParameter(0, i);
    fgFormula[i]->SetParName(  0, fgkParticleNames[i][0] );
  }
}

inline void PairAnalysisVarManager::InitKFFitter() {
  fgKFFitter = new CbmStsKFTrackFitter();
  fgKFFitter->Init();
}

#endif

