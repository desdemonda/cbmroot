#ifndef PAIRANALYSISMC_H
#define PAIRANALYSISMC_H
/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//#####################################################
//#                                                   # 
//#              Class PairAnalysisMC                   #
//#       Cut Class for Jpsi->e+e- analysis           #
//#                                                   #
//#   by Julian Book, Uni Ffm, Julian.Book@cern.ch    #
//#                                                   #
//#####################################################

#ifndef ROOT_TObject
#include <TObject.h>
#include <TMCProcess.h>
#endif
class TParticle;
class CbmMCTrack;
class PairAnalysisTrack;

#include "PairAnalysisSignalMC.h"
#include "PairAnalysisPair.h"

class PairAnalysisMC : public TObject{
  
public:  
  PairAnalysisMC();
  virtual ~PairAnalysisMC();

  void SetHasMC(Bool_t hasMC) { fHasMC=hasMC; }
  Bool_t HasMC() const { return fHasMC; }
  
  static PairAnalysisMC* Instance();
  
  Int_t GetNMCTracks();                                // return number of generated tracks
  Int_t GetMotherPDG(const PairAnalysisTrack* _track);   // return mother PID from the MC stack
  Int_t GetMotherPDG(const CbmMCTrack* _track);        // return mother PID from the MC stack

  Bool_t ConnectMCEvent();

  Bool_t IsMotherPdg(const PairAnalysisPair* pair, Int_t pdgMother);
  Bool_t IsMotherPdg(const PairAnalysisTrack *particle1, const PairAnalysisTrack *particle2, Int_t pdgMother);

  Bool_t IsMCTruth(const PairAnalysisPair* pair, const PairAnalysisSignalMC* signalMC) const;
  Bool_t IsMCTruth(const PairAnalysisTrack *trk, PairAnalysisSignalMC* signalMC, Int_t branch) const;
  Bool_t IsMCTruth(Int_t label, PairAnalysisSignalMC* signalMC, Int_t branch) const;

  Int_t GetMothersLabel(Int_t daughterLabel) const;
  Int_t GetPdgFromLabel(Int_t label) const;

  Bool_t CheckGEANTProcess(Int_t label, TMCProcess process) const;
  //  Bool_t IsPhysicalPrimary(Int_t label) const;  // checks if a particle is physical primary
  //  Bool_t IsSecondaryFromWeakDecay(Int_t label) const;
  //  Bool_t IsSecondaryFromMaterial(Int_t label) const;

  Bool_t HaveSameMother(const PairAnalysisPair *pair) const;
  
  Int_t GetLabelMotherWithPdg(const PairAnalysisPair* pair, Int_t pdgMother);
  Int_t GetLabelMotherWithPdg(const PairAnalysisTrack *particle1, const PairAnalysisTrack *particle2, Int_t pdgMother);
  
  CbmMCTrack* GetMCTrackFromMCEvent(Int_t label) const;           // return MC track directly from MC event
  CbmMCTrack* GetMCTrack(const PairAnalysisTrack* _track);       // return MC track associated with reco track
  
  CbmMCTrack* GetMCTrackMother(const PairAnalysisTrack* _track); // return MC mother for reco track
  CbmMCTrack* GetMCTrackMother(const CbmMCTrack* _track);         // return MC mother for mc rtack

  Int_t NumberOfDaughters(const CbmMCTrack* particle);                 // return number of daughters

  void GetDaughters(const TObject *mother, CbmMCTrack* &d1, CbmMCTrack* &d2);
  Bool_t CheckParticleSource(Int_t label, PairAnalysisSignalMC::ESource source) const;
  //  Bool_t GetPrimaryVertex(Double_t &primVtxX, Double_t &primVtxY, Double_t &primVtxZ);
  //  AliMCEvent* GetMCEvent() { return fMCEvent; }         // return the AliMCEvent
  
private:
  TObject    *fMCEvent;   // MC event object
  Bool_t fHasMC;          // Do we have an MC handler?
  TClonesArray* fMCArray; //mcArray for AOD MC particles 
 
  static PairAnalysisMC* fgInstance; //! singleton pointer

  PairAnalysisMC(const PairAnalysisMC &c);
  PairAnalysisMC &operator=(const PairAnalysisMC &c);

  Bool_t ComparePDG(Int_t particlePDG, Int_t requiredPDG, Bool_t pdgExclusion, Bool_t checkBothCharges) const;
  Bool_t CheckIsRadiative(Int_t label) const;
  Bool_t CheckRadiativeDecision(Int_t mLabel, const PairAnalysisSignalMC * const signalMC) const;

  Bool_t CheckIsDalitz(Int_t label, const PairAnalysisSignalMC * const signalMC) const;
  Bool_t CheckDalitzDecision(Int_t mLabel, const PairAnalysisSignalMC * const signalMC) const;

  ClassDef(PairAnalysisMC, 0)
};

//
// inline functions
//
inline Bool_t PairAnalysisMC::IsMotherPdg(const PairAnalysisPair* pair, Int_t pdgMother)
{
  return IsMotherPdg(pair->GetFirstDaughter(),pair->GetSecondDaughter(),pdgMother);
}
//___________________________________________________________
inline Bool_t PairAnalysisMC::IsMotherPdg(const PairAnalysisTrack *particle1, const PairAnalysisTrack *particle2, Int_t pdgMother){
  return GetLabelMotherWithPdg(particle1,particle2,pdgMother)>=0;
}
//___________________________________________________________
inline Int_t PairAnalysisMC::GetLabelMotherWithPdg(const PairAnalysisPair* pair, Int_t pdgMother){
  return GetLabelMotherWithPdg(pair->GetFirstDaughter(),pair->GetSecondDaughter(),pdgMother);
}

#endif

