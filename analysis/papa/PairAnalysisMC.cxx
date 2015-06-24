/*************************************************************************
* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
**************************************************************************/

#include <TClonesArray.h>
#include <TParticle.h>
#include <TMCProcess.h>

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "PairAnalysisTrack.h"
#include "PairAnalysisSignalMC.h"
#include "PairAnalysisMC.h"


ClassImp(PairAnalysisMC)

PairAnalysisMC* PairAnalysisMC::fgInstance=0x0;

//____________________________________________________________
PairAnalysisMC* PairAnalysisMC::Instance()
{
  //
  // return pointer to singleton implementation
  //
  if (fgInstance) return fgInstance;

  fgInstance=new PairAnalysisMC(); 
  //  fgInstance->SetHasMC(kTRUE);
   
  return fgInstance;
}

//____________________________________________________________
PairAnalysisMC::PairAnalysisMC():
  fMCEvent(0x0),
  fHasMC(kFALSE),
  fMCArray(0x0)
{
  //
  // default constructor
  //

}


//____________________________________________________________
PairAnalysisMC::~PairAnalysisMC()
{
  //
  // default destructor
  //
  
}

//____________________________________________________________
Int_t PairAnalysisMC::GetNMCTracks()
{
  //
  //  return the number of generated tracks from MC event
  //
  if(!fMCArray) { Error("PairAnalysisMC::","No fMCArray"); return 0; }
  return fMCArray->GetEntriesFast();

}

//____________________________________________________________
CbmMCTrack* PairAnalysisMC::GetMCTrackFromMCEvent(Int_t label) const
{
  //
  // return MC track directly from MC event
  // used not only for tracks but for mothers as well, therefore do not use abs(label)
  //
  if (label<0) return NULL;
  if (!fMCArray){ Error("PairAnalysisMC::","No fMCArray"); return NULL;}

  if (label>fMCArray->GetEntriesFast()) { 
    Info("PairAnalysisMC::",Form("track %d out of array size %d",
				   label,fMCArray->GetEntriesFast())); 
    return NULL;
  }

  CbmMCTrack *track = static_cast<CbmMCTrack*>( fMCArray->At(label) ); //  tracks from MC event
  return track;
}

//____________________________________________________________
Bool_t PairAnalysisMC::ConnectMCEvent()
{
  //
  // connect MC array of tracks
  //

  fMCArray = 0x0;
  fMCEvent = 0x0;
  
  FairRootManager* man = FairRootManager::Instance();
  if (!man) { Fatal("PairAnalysisMC::Instance","No FairRootManager!"); }
 
  fMCArray     = dynamic_cast<TClonesArray*>( man->GetObject("MCTrack") );
  if(!fMCArray)   {
    Error("PairAnalysisMC::Instance","Initialization of MC object failed!");
    return kFALSE;
  }
  else fHasMC=kTRUE;
  printf("PairAnalysisMC::ConnectMCEvent: size of mc array: %04d \n",fMCArray->GetSize());
  return kTRUE;
}


//____________________________________________________________
CbmMCTrack* PairAnalysisMC::GetMCTrack( const PairAnalysisTrack* _track)
{
  //
  // return MC track
  //
  return ( _track->GetMCTrack() );

}

//______________________________________________________________
CbmMCTrack* PairAnalysisMC::GetMCTrackMother(const PairAnalysisTrack* _track)
{
  //
  // return MC track mother
  //
  CbmMCTrack* mcpart = GetMCTrack(_track);
  if (!mcpart) return NULL;
  return ( GetMCTrackMother(mcpart) );
}

//____________________________________________________________
CbmMCTrack* PairAnalysisMC::GetMCTrackMother(const CbmMCTrack* _particle){
  //
  // return MC track mother
  //
  if( _particle->GetMotherId() < 0) return NULL;
  CbmMCTrack* mcmother = dynamic_cast<CbmMCTrack *>(fMCArray->At(_particle->GetMotherId()));
  return mcmother;
}


//________________________________________________________
Int_t PairAnalysisMC::GetMotherPDG( const PairAnalysisTrack* _track)
{
  //
  // return PDG code of the mother track from the MC truth info
  //
  CbmMCTrack* mcmother = GetMCTrackMother(_track);
  if(!mcmother)  return -99999;
  return mcmother->GetPdgCode();
}

//________________________________________________________
Int_t PairAnalysisMC::GetMotherPDG( const CbmMCTrack* _track)
{
  //
  // return PDG code of the mother track from the MC truth info
  //
  if (!_track)   return -99999;
  CbmMCTrack* mcmother = GetMCTrackMother(_track);
  if(!mcmother)  return -99999;
  return mcmother->GetPdgCode();
}

//____________________________________________________________
Int_t PairAnalysisMC::NumberOfDaughters(const CbmMCTrack* particle)
{
  //
  // returns the number of daughters
  //
  CbmMCTrack *mcmother=GetMCTrackMother(particle);
  if(!mcmother) return -9999;
  return 0;//TODO: maybe request number of daughters or remove function
  //mcmother->GetNDaughters();
}

//____________________________________________________________
Int_t PairAnalysisMC::GetLabelMotherWithPdg(const PairAnalysisTrack *particle1, const PairAnalysisTrack *particle2, Int_t pdgMother)
{
  //
  // test if mother of particle 1 and 2 has pdgCode pdgMother and is the same;
  //
  if (!fMCArray) return -1;
  CbmMCTrack *mcPart1=particle1->GetMCTrack();
  CbmMCTrack *mcPart2=particle2->GetMCTrack();
  if(!mcPart1 || !mcPart2) return -1;

  // get mother
  Int_t lblMother1=mcPart1->GetMotherId();
  Int_t lblMother2=mcPart2->GetMotherId();
  if (lblMother1!=lblMother2) return -1;
  CbmMCTrack *mcMother1=GetMCTrackFromMCEvent(lblMother1);
  if (!mcMother1) return -1;

  // compare mc truth with expectation
  if (TMath::Abs(mcPart1->GetPdgCode())!=particle1->PdgCode()) return -1;
  if (TMath::Abs(mcPart2->GetPdgCode())!=particle2->PdgCode()) return -1;
  //  if (mcPart1->GetPdgCode()!=-mcPart2->GetPdgCode()) return -1;
  if (mcMother1->GetPdgCode()!=pdgMother) return -1;
  
  return lblMother1;
}

//____________________________________________________________
void PairAnalysisMC::GetDaughters(const TObject *mother, CbmMCTrack* &d1, CbmMCTrack* &d2)
{
  //
  // Get First two daughters of the mother
  // TODO: theres NO connection from mother to daughters
  Int_t lblD1=-1;
  Int_t lblD2=-1;
  d1=0;
  d2=0;
  if(!fMCArray) return;
  return;  // TOBEDONE
  /*  
  const CbmMCTrack *mom=static_cast<const CbmMCTrack*>(mother);
  lblD1=mom->GetDaughter(0);
  lblD2=mom->GetDaughter(1);
  d1 = (CbmMCTrack*)fMCArray->At(lblD1);
  d2 = (CbmMCTrack*)fMCArray->At(lblD2);
  */
}


//________________________________________________________________________________
Int_t PairAnalysisMC::GetMothersLabel(Int_t daughterLabel) const {
  //
  //  Get the label of the mother for particle with label daughterLabel
  //  NOTE: for tracks, the absolute label should be passed
  //
  if(daughterLabel<0) return -1;
  if(!fMCArray)       return -1;
  if(GetMCTrackFromMCEvent(daughterLabel))
    return (GetMCTrackFromMCEvent(daughterLabel))->GetMotherId();
  return -1;
}


//________________________________________________________________________________
Int_t PairAnalysisMC::GetPdgFromLabel(Int_t label) const {
  //
  //  Get particle code using the label from stack
  //  NOTE: for tracks, the absolute label should be passed
  //
  if(label<0) return 0;
  if(!fMCArray) return 0;
  return (GetMCTrackFromMCEvent(label)->GetPdgCode());
  return 0;
}


//________________________________________________________________________________
Bool_t PairAnalysisMC::ComparePDG(Int_t particlePDG, Int_t requiredPDG, Bool_t pdgExclusion, Bool_t checkBothCharges) const {
  //
  //  Test the PDG codes of particles with the required ones
  //
  Bool_t result = kTRUE;
  Int_t absRequiredPDG = TMath::Abs(requiredPDG);

  switch(absRequiredPDG) {
  case 0:
    result = kTRUE;    // PDG not required (any code will do fine)
    break;
  case 100:     // light flavoured mesons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=100 && TMath::Abs(particlePDG)<=199;
    else {
      if(requiredPDG>0) result = particlePDG>=100 && particlePDG<=199;
      if(requiredPDG<0) result = particlePDG>=-199 && particlePDG<=-100;
    }
    break;
  case 1000:     // light flavoured baryons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=1000 && TMath::Abs(particlePDG)<=1999;
    else {
      if(requiredPDG>0) result = particlePDG>=1000 && particlePDG<=1999;
      if(requiredPDG<0) result = particlePDG>=-1999 && particlePDG<=-1000;
    }
    break;
  case 200:     // light flavoured mesons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=200 && TMath::Abs(particlePDG)<=299;
    else {
      if(requiredPDG>0)result = particlePDG>=200 && particlePDG<=299;
      if(requiredPDG<0)result = particlePDG>=-299 && particlePDG<=-200;
    }
    break;
  case 2000:     // light flavoured baryons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=2000 && TMath::Abs(particlePDG)<=2999;
    else {
      if(requiredPDG>0) result = particlePDG>=2000 && particlePDG<=2999;
      if(requiredPDG<0) result = particlePDG>=-2999 && particlePDG<=-2000;
    }
    break;
  case 300:     // all strange mesons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=300 && TMath::Abs(particlePDG)<=399;
    else {
      if(requiredPDG>0) result = particlePDG>=300 && particlePDG<=399;
      if(requiredPDG<0) result = particlePDG>=-399 && particlePDG<=-300;
    }
    break;
  case 3000:     // all strange baryons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=3000 && TMath::Abs(particlePDG)<=3999;
    else {
      if(requiredPDG>0) result = particlePDG>=3000 && particlePDG<=3999;
      if(requiredPDG<0) result = particlePDG>=-3999 && particlePDG<=-3000;
    }
    break;
  case 400:     // all charmed mesons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=400 && TMath::Abs(particlePDG)<=499;
    else {
      if(requiredPDG>0) result = particlePDG>=400 && particlePDG<=499;
      if(requiredPDG<0) result = particlePDG>=-499 && particlePDG<=-400;
    }
    break;
  case 401:     // open charm mesons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=400 && TMath::Abs(particlePDG)<=439;
    else {
      if(requiredPDG>0) result = particlePDG>=400 && particlePDG<=439;
      if(requiredPDG<0) result = particlePDG>=-439 && particlePDG<=-400;
    }
    break;
  case 402:     // open charm mesons and baryons together
    if(checkBothCharges)
      result = (TMath::Abs(particlePDG)>=400 && TMath::Abs(particlePDG)<=439) ||
	       (TMath::Abs(particlePDG)>=4000 && TMath::Abs(particlePDG)<=4399);
    else {
      if(requiredPDG>0) result = (particlePDG>=400 && particlePDG<=439) ||
			         (particlePDG>=4000 && particlePDG<=4399);
      if(requiredPDG<0) result = (particlePDG>=-439 && particlePDG<=-400) ||
 			         (particlePDG>=-4399 && particlePDG<=-4000);
    }
    break;
  case 403:     // all charm hadrons
    if(checkBothCharges)
      result = (TMath::Abs(particlePDG)>=400 && TMath::Abs(particlePDG)<=499) ||
	       (TMath::Abs(particlePDG)>=4000 && TMath::Abs(particlePDG)<=4999);
    else {
      if(requiredPDG>0) result = (particlePDG>=400 && particlePDG<=499) ||
			         (particlePDG>=4000 && particlePDG<=4999);
      if(requiredPDG<0) result = (particlePDG>=-499 && particlePDG<=-400) ||
 			         (particlePDG>=-4999 && particlePDG<=-4000);
    }
    break;
  case 4000:     // all charmed baryons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=4000 && TMath::Abs(particlePDG)<=4999;
    else {
      if(requiredPDG>0) result = particlePDG>=4000 && particlePDG<=4999;
      if(requiredPDG<0) result = particlePDG>=-4999 && particlePDG<=-4000;
    }
    break;
  case 4001:     // open charm baryons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=4000 && TMath::Abs(particlePDG)<=4399;
    else {
      if(requiredPDG>0) result = particlePDG>=4000 && particlePDG<=4399;
      if(requiredPDG<0) result = particlePDG>=-4399 && particlePDG<=-4000;
    }
    break;
  case 500:      // all beauty mesons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=500 && TMath::Abs(particlePDG)<=599;
    else {
      if(requiredPDG>0) result = particlePDG>=500 && particlePDG<=599;
      if(requiredPDG<0) result = particlePDG>=-599 && particlePDG<=-500;
    }
    break;
  case 501:      // open beauty mesons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=500 && TMath::Abs(particlePDG)<=549;
    else {
      if(requiredPDG>0) result = particlePDG>=500 && particlePDG<=549;
      if(requiredPDG<0) result = particlePDG>=-549 && particlePDG<=-500;
    }
    break;
  case 502:      // open beauty mesons and baryons
    if(checkBothCharges)
      result = (TMath::Abs(particlePDG)>=500 && TMath::Abs(particlePDG)<=549) ||
	       (TMath::Abs(particlePDG)>=5000 && TMath::Abs(particlePDG)<=5499);
    else {
      if(requiredPDG>0) result = (particlePDG>=500 && particlePDG<=549) ||
			         (particlePDG>=5000 && particlePDG<=5499);
      if(requiredPDG<0) result = (particlePDG>=-549 && particlePDG<=-500) ||
                                 (particlePDG>=-5499 && particlePDG<=-5000);
    }
    break;
  case 503:      // all beauty hadrons
    if(checkBothCharges)
      result = (TMath::Abs(particlePDG)>=500 && TMath::Abs(particlePDG)<=599) ||
	       (TMath::Abs(particlePDG)>=5000 && TMath::Abs(particlePDG)<=5999);
    else {
      if(requiredPDG>0) result = (particlePDG>=500 && particlePDG<=599) ||
			         (particlePDG>=5000 && particlePDG<=5999);
      if(requiredPDG<0) result = (particlePDG>=-599 && particlePDG<=-500) ||
                                 (particlePDG>=-5999 && particlePDG<=-5000);
    }
    break;
  case 5000:      // all beauty baryons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=5000 && TMath::Abs(particlePDG)<=5999;
    else {
      if(requiredPDG>0) result = particlePDG>=5000 && particlePDG<=5999;
      if(requiredPDG<0) result = particlePDG>=-5999 && particlePDG<=-5000;
    }
    break;
  case 5001:      // open beauty baryons
    if(checkBothCharges)
      result = TMath::Abs(particlePDG)>=5000 && TMath::Abs(particlePDG)<=5499;
    else {
      if(requiredPDG>0) result = particlePDG>=5000 && particlePDG<=5499;
      if(requiredPDG<0) result = particlePDG>=-5499 && particlePDG<=-5000;
    }
    break;
  case 902:      // // open charm,beauty  mesons and baryons together
    if(checkBothCharges)
      result = (TMath::Abs(particlePDG)>=400 && TMath::Abs(particlePDG)<=439) ||
	(TMath::Abs(particlePDG)>=4000 && TMath::Abs(particlePDG)<=4399) ||
	(TMath::Abs(particlePDG)>=500 && TMath::Abs(particlePDG)<=549) ||
	(TMath::Abs(particlePDG)>=5000 && TMath::Abs(particlePDG)<=5499);
    else {
      if(requiredPDG>0) result = (particlePDG>=400 && particlePDG<=439) ||
			  (particlePDG>=4000 && particlePDG<=4399)      ||
			  (particlePDG>=500 && particlePDG<=549)        ||
			  (particlePDG>=5000 && particlePDG<=5499);
      if(requiredPDG<0) result = (particlePDG>=-439 && particlePDG<=-400) ||
			  (particlePDG>=-4399 && particlePDG<=-4000)      ||
			  (particlePDG>=-549 && particlePDG<=-500)        ||
			  (particlePDG>=-5499 && particlePDG<=-5000);
    }
    break;
  default:          // all specific cases
    if(checkBothCharges)
      result = (absRequiredPDG==TMath::Abs(particlePDG));
    else
      result = (requiredPDG==particlePDG);
  }

  if(absRequiredPDG!=0 && pdgExclusion) result = !result;
  return result;
}

//________________________________________________________________________________
Bool_t PairAnalysisMC::CheckGEANTProcess(Int_t label, TMCProcess process) const {
  //
  //  Check the GEANT process for the particle
  //
  if(label<0) return kFALSE;

  if(!fMCArray) return kFALSE;
  UInt_t processID = static_cast<CbmMCTrack*>(GetMCTrackFromMCEvent(label))->GetGeantProcessId();
  //  printf("process: id %d --> %s \n",processID,TMCProcessName[processID]);
  return (process==processID);

}

//________________________________________________________________________________
Bool_t PairAnalysisMC::CheckParticleSource(Int_t label, PairAnalysisSignalMC::ESource source) const {
  //
  //  Check the source for the particle 
  //  NOTE: TODO: check and clarify different sources, UPDATE!
  //

  switch (source) {
    case PairAnalysisSignalMC::kDontCare :
      return kTRUE;
    break;
    case PairAnalysisSignalMC::kPrimary :
      // NOTE: This includes all physics event history (initial state particles,
      //       exchange bosons, quarks, di-quarks, strings, un-stable particles, final state particles)
      //       Only the final state particles make it to the detector!!
      // NOTE: according to CbmMCTrack::fMotherId
      return (label<0); // was ==-1
    break;
    case PairAnalysisSignalMC::kSecondary :
      // particles which are created by the interaction of final state primaries with the detector
      // or particles from strange weakly decaying particles (e.g. lambda, kaons, etc.)
      return (label>=0);
    break;
    /*
    case PairAnalysisSignalMC::kFinalState :         
      // primary particles created in the collision which reach the detectors
      // These would be:
      // 1.) particles produced in the collision
      // 2.) stable particles with respect to strong and electromagnetic interactions
      // 3.) excludes initial state particles
      // 4.) includes products of directly produced Sigma0 hyperon decay
      // 5.) includes products of directly produced pi0 decays
      // 6.) includes products of directly produced beauty hadron decays
      return IsPhysicalPrimary(label);
    break;
    case PairAnalysisSignalMC::kDirect :
      // Primary particles which do not have any mother
      // This is the case for:
      // 1.) Initial state particles (the 2 protons in Pythia pp collisions)
      // 2.) In some codes, with sudden freeze-out, all particles generated from the fireball are direct.
      //     There is no history for these particles.
      // 3.) Certain particles added via MC generator cocktails (e.g. J/psi added to pythia MB events)
      return (label>=0 && GetMothersLabel(label)<0);
      break;
    case PairAnalysisSignalMC::kNoCocktail :
      // Particles from the HIJING event and NOT from the AliGenCocktail
      return (label>=0 && GetMothersLabel(label)>=0);
      break;
    case PairAnalysisSignalMC::kSecondary :          
      // particles which are created by the interaction of final state primaries with the detector
      // or particles from strange weakly decaying particles (e.g. lambda, kaons, etc.)
      return (label>=GetNPrimary() && !IsPhysicalPrimary(label));
    break;
    case PairAnalysisSignalMC::kSecondaryFromWeakDecay :          
      // secondary particle from weak decay
      // or particles from strange weakly decaying particles (e.g. lambda, kaons, etc.)
      return (IsSecondaryFromWeakDecay(label));
    break;
    case PairAnalysisSignalMC::kSecondaryFromMaterial :
      // secondary particle from material
      return (IsSecondaryFromMaterial(label));
    break;
  */
    default :
      return kFALSE;
  }
  return kFALSE;
}

//________________________________________________________________________________
Bool_t PairAnalysisMC::CheckIsRadiative(Int_t label) const
{
  //
  // Check if the particle has a three body decay, one being a photon
  // NOTE: no information on # of daugthers available
  // TODO: OBSOLETE can be checked via CheckIsDalitz function
  if(label<0) return kFALSE;
  
  if(!fMCArray) return kFALSE;
  CbmMCTrack *mother=static_cast<CbmMCTrack*>(GetMCTrackFromMCEvent(label));
  if (!mother) return kFALSE;

  /*
  const Int_t nd=mother->GetNDaughters();
  if (nd==2) return kFALSE;
  for (Int_t i=2; i<nd; ++i)
    if (GetMCTrackFromMCEvent(mother->GetDaughter(0)+i)->PdgCode()!=22) return kFALSE; //last daughter is photon
  */
  return kTRUE;
}

//________________________________________________________________________________
Bool_t PairAnalysisMC::CheckIsDalitz(Int_t label, const PairAnalysisSignalMC * const signalMC) const
{
  //
  // Check if the particle has a three body decay, one being a dalitz pdg

  // loop over the MC tracks
  //  for(Int_t ipart=0; ipart<fMCArray->GetEntriesFast(); ++ipart) { // super slow
  for(Int_t ipart=label; ipart<label+5; ++ipart) { // embedded particles are sorted
    CbmMCTrack *daughter=static_cast<CbmMCTrack*>(GetMCTrackFromMCEvent(ipart));
    if(!daughter) continue;
    if( daughter->GetPdgCode()  != signalMC->GetDalitzPdg() ) continue;
    if( daughter->GetMotherId() == label ) return kTRUE;
  }
  return kFALSE;
}

//________________________________________________________________________________
Bool_t PairAnalysisMC::CheckRadiativeDecision(Int_t mLabel, const PairAnalysisSignalMC * const signalMC) const
{
  //
  // Check for the decision of the radiative type request
  //
  
  if (!signalMC) return kFALSE;
  
  if (signalMC->GetJpsiRadiative()==PairAnalysisSignalMC::kAll) return kTRUE;
  
  Bool_t isRadiative=CheckIsRadiative(mLabel);
  if ((signalMC->GetJpsiRadiative()==PairAnalysisSignalMC::kIsRadiative) && !isRadiative) return kFALSE;
  if ((signalMC->GetJpsiRadiative()==PairAnalysisSignalMC::kIsNotRadiative) && isRadiative) return kFALSE;
  
  return kTRUE;
}

//________________________________________________________________________________
Bool_t PairAnalysisMC::CheckDalitzDecision(Int_t mLabel, const PairAnalysisSignalMC * const signalMC) const
{
  //
  // Check for the decision of the dalitz type request
  //

  if (!signalMC) return kFALSE;

  if (signalMC->GetDalitz()==PairAnalysisSignalMC::kWhoCares) return kTRUE;

  Bool_t isDalitz=CheckIsDalitz(mLabel,signalMC);
  if ((signalMC->GetDalitz()==PairAnalysisSignalMC::kIsDalitz)    && !isDalitz) return kFALSE;
  if ((signalMC->GetDalitz()==PairAnalysisSignalMC::kIsNotDalitz) &&  isDalitz) return kFALSE;

  return kTRUE;
}

//________________________________________________________________________________
Bool_t PairAnalysisMC::IsMCTruth(Int_t label, PairAnalysisSignalMC* signalMC, Int_t branch) const {
  //
  // Check if the particle corresponds to the MC truth in signalMC in the branch specified
  //
  
  if(label<0) return kFALSE; // to be checked
  //  if(label<0) label *= -1; 

  CbmMCTrack* part = GetMCTrackFromMCEvent(label);
  if (!part) {
    Error("PairAnalysisMC::",Form("Could not find MC particle with label %d",label));
    return kFALSE;
  }

  // check geant process if set
  if(signalMC->GetCheckGEANTProcess() && !CheckGEANTProcess(label,signalMC->GetGEANTProcess()))
    return kFALSE;

  // check the LEG
  if( !ComparePDG(part->GetPdgCode(),
		  signalMC->GetLegPDG(branch),
		  signalMC->GetLegPDGexclude(branch),
		  signalMC->GetCheckBothChargesLegs(branch)) ) return kFALSE;

  // Check the source (primary, secondary, embedded) for the particle
  if(!CheckParticleSource(label, signalMC->GetLegSource(branch))) return kFALSE;

  // check the MOTHER
  CbmMCTrack* mcMother=0x0;
  Int_t mLabel = -1;
  if(signalMC->GetMotherPDG(branch)   !=0 || 
     signalMC->GetMotherSource(branch)!=PairAnalysisSignalMC::kDontCare) {
    mLabel = GetMothersLabel(label);
    mcMother = GetMCTrackFromMCEvent(mLabel);

    if( !mcMother && !signalMC->GetMotherPDGexclude(branch)) return kFALSE;

    if( !ComparePDG((mcMother ? mcMother->GetPdgCode() : -99999 ),
		    signalMC->GetMotherPDG(branch),
		    signalMC->GetMotherPDGexclude(branch),
		    signalMC->GetCheckBothChargesMothers(branch)) ) return kFALSE;
    if( !CheckParticleSource(mLabel, signalMC->GetMotherSource(branch))) return kFALSE;

    //check for radiative decay
    //    if( !CheckRadiativeDecision(mLabel, signalMC) ) return kFALSE;
    //check for dalitz decay
    if( !CheckDalitzDecision(mLabel, signalMC) ) return kFALSE;
  }

  // check the grandmother
  CbmMCTrack* mcGrandMother=0x0;
  if(signalMC->GetGrandMotherPDG(branch)   !=0 ||
     signalMC->GetGrandMotherSource(branch)!=PairAnalysisSignalMC::kDontCare) {
    Int_t gmLabel = -1;
    if(mcMother) {
      gmLabel = GetMothersLabel(mLabel);
      mcGrandMother = GetMCTrackFromMCEvent(gmLabel);
    }
    if( !mcGrandMother && !signalMC->GetGrandMotherPDGexclude(branch)) return kFALSE;

    if( !ComparePDG((mcGrandMother ? mcGrandMother->GetPdgCode() : 0),
		    signalMC->GetGrandMotherPDG(branch),
		    signalMC->GetGrandMotherPDGexclude(branch),
		    signalMC->GetCheckBothChargesGrandMothers(branch))) return kFALSE;
    if( !CheckParticleSource(gmLabel, signalMC->GetGrandMotherSource(branch))) return kFALSE;
  }

  return kTRUE;
}

//________________________________________________________________________________
Bool_t PairAnalysisMC::IsMCTruth(const PairAnalysisTrack *trk, PairAnalysisSignalMC* signalMC, Int_t branch) const {
  //
  // Check if the particle corresponds to the MC truth in signalMC in the branch specified
  //
  return IsMCTruth(trk->GetLabel(),signalMC,branch);
}

//________________________________________________________________________________
Bool_t PairAnalysisMC::IsMCTruth(const PairAnalysisPair* pair, const PairAnalysisSignalMC* signalMC) const {
  //
  // Check if the pair corresponds to the MC truth in signalMC 
  //
 
  // legs (daughters)
  const PairAnalysisTrack * mcD1 = pair->GetFirstDaughter();
  const PairAnalysisTrack * mcD2 = pair->GetSecondDaughter();
  Int_t labelD1 = (mcD1 ? mcD1->GetLabel() : -1);
  Int_t labelD2 = (mcD2 ? mcD2->GetLabel() : -1);
  Int_t d1Pdg = GetPdgFromLabel(labelD1);
  Int_t d2Pdg = GetPdgFromLabel(labelD2);

  // check geant process if set
  Bool_t processGEANT = kTRUE;
  if(signalMC->GetCheckGEANTProcess()) {
    if(!CheckGEANTProcess(labelD1,signalMC->GetGEANTProcess()) &&
       !CheckGEANTProcess(labelD2,signalMC->GetGEANTProcess())   )
      return kFALSE;
  }
  
  // mothers
  CbmMCTrack* mcM1=0x0;
  CbmMCTrack* mcM2=0x0;
  
  // grand-mothers
  CbmMCTrack* mcG1 = 0x0;
  CbmMCTrack* mcG2 = 0x0;
  
  // make direct(1-1 and 2-2) and cross(1-2 and 2-1) comparisons for the whole branch
  Bool_t directTerm = kTRUE;
  // daughters
  directTerm = directTerm && mcD1 && ComparePDG(d1Pdg,signalMC->GetLegPDG(1),signalMC->GetLegPDGexclude(1),signalMC->GetCheckBothChargesLegs(1)) 
    && CheckParticleSource(labelD1, signalMC->GetLegSource(1));
  
  directTerm = directTerm && mcD2 && ComparePDG(d2Pdg,signalMC->GetLegPDG(2),signalMC->GetLegPDGexclude(2),signalMC->GetCheckBothChargesLegs(2))
    && CheckParticleSource(labelD2, signalMC->GetLegSource(2));
    
  // mothers
  Int_t labelM1 = -1;
  if(signalMC->GetMotherPDG(1)!=0 || signalMC->GetMotherSource(1)!=PairAnalysisSignalMC::kDontCare) {
    labelM1 = GetMothersLabel(labelD1);
    if(labelD1>-1 && labelM1>-1) mcM1 = GetMCTrackFromMCEvent(labelM1);
    directTerm = directTerm && (mcM1 || signalMC->GetMotherPDGexclude(1))
      && ComparePDG((mcM1 ? mcM1->GetPdgCode() : -99999),signalMC->GetMotherPDG(1),signalMC->GetMotherPDGexclude(1),signalMC->GetCheckBothChargesMothers(1))
      && CheckParticleSource(labelM1, signalMC->GetMotherSource(1))
      //      && CheckRadiativeDecision(labelM1,signalMC)
      && CheckDalitzDecision(labelM1,signalMC);
  }
  
  Int_t labelM2 = -1;
  if(signalMC->GetMotherPDG(2)!=0 || signalMC->GetMotherSource(2)!=PairAnalysisSignalMC::kDontCare) {
    labelM2 = GetMothersLabel(labelD2);
    if(labelD2>-1 && labelM2>-1) mcM2 = GetMCTrackFromMCEvent(labelM2);
    directTerm = directTerm && (mcM2 || signalMC->GetMotherPDGexclude(2))
      && ComparePDG((mcM2 ? mcM2->GetPdgCode() : -99999 ),signalMC->GetMotherPDG(2),signalMC->GetMotherPDGexclude(2),signalMC->GetCheckBothChargesMothers(2))
      && CheckParticleSource(labelM2, signalMC->GetMotherSource(2))
      //      && CheckRadiativeDecision(labelM2,signalMC)
      && CheckDalitzDecision(labelM2,signalMC);
  }

  // grand-mothers
  Int_t labelG1 = -1;
  if(signalMC->GetGrandMotherPDG(1)!=0 || signalMC->GetGrandMotherSource(1)!=PairAnalysisSignalMC::kDontCare) {
    labelG1 = GetMothersLabel(labelM1);
    if(mcM1 && labelG1>-1) mcG1 = GetMCTrackFromMCEvent(labelG1);
    directTerm = directTerm && (mcG1 || signalMC->GetGrandMotherPDGexclude(1))
                 && ComparePDG((mcG1 ? mcG1->GetPdgCode() : 0),signalMC->GetGrandMotherPDG(1),signalMC->GetGrandMotherPDGexclude(1),signalMC->GetCheckBothChargesGrandMothers(1))
                 && CheckParticleSource(labelG1, signalMC->GetGrandMotherSource(1));
  }

  Int_t labelG2 = -1;
  if(signalMC->GetGrandMotherPDG(2)!=0 || signalMC->GetGrandMotherSource(2)!=PairAnalysisSignalMC::kDontCare) {
    labelG2 = GetMothersLabel(labelM2);
    if(mcM2 && labelG2>-1) mcG2 = GetMCTrackFromMCEvent(labelG2);
    directTerm = directTerm && (mcG2 || signalMC->GetGrandMotherPDGexclude(2))
                 && ComparePDG((mcG2 ? mcG2->GetPdgCode() : 0),signalMC->GetGrandMotherPDG(2),signalMC->GetGrandMotherPDGexclude(2),signalMC->GetCheckBothChargesGrandMothers(2))
                 && CheckParticleSource(labelG2, signalMC->GetGrandMotherSource(2));
  }

  // Cross term
  Bool_t crossTerm = kTRUE;
  // daughters
  crossTerm = crossTerm && mcD2 
    && ComparePDG(d2Pdg,signalMC->GetLegPDG(1),signalMC->GetLegPDGexclude(1),signalMC->GetCheckBothChargesLegs(1))
    && CheckParticleSource(labelD2, signalMC->GetLegSource(1));

  crossTerm = crossTerm && mcD1 
    && ComparePDG(d1Pdg,signalMC->GetLegPDG(2),signalMC->GetLegPDGexclude(2),signalMC->GetCheckBothChargesLegs(2))
    && CheckParticleSource(labelD1, signalMC->GetLegSource(2));
  
  // mothers  
  if(signalMC->GetMotherPDG(1)!=0 || signalMC->GetMotherSource(1)!=PairAnalysisSignalMC::kDontCare) {
    if(!mcM2 && labelD2>-1) {
      labelM2 = GetMothersLabel(labelD2);
      if(labelM2>-1) mcM2 = GetMCTrackFromMCEvent(labelM2);
    }
    crossTerm = crossTerm && (mcM2 || signalMC->GetMotherPDGexclude(1))
      && ComparePDG((mcM2 ? mcM2->GetPdgCode() : -99999),signalMC->GetMotherPDG(1),signalMC->GetMotherPDGexclude(1),signalMC->GetCheckBothChargesMothers(1))
      && CheckParticleSource(labelM2, signalMC->GetMotherSource(1))
      //      && CheckRadiativeDecision(labelM2,signalMC)
      && CheckDalitzDecision(labelM2,signalMC);
  }
  
  if(signalMC->GetMotherPDG(2)!=0 || signalMC->GetMotherSource(2)!=PairAnalysisSignalMC::kDontCare) {
    if(!mcM1 && labelD1>-1) {
      labelM1 = GetMothersLabel(labelD1);
      if(labelM1>-1) mcM1 = GetMCTrackFromMCEvent(labelM1);  
    }
    crossTerm = crossTerm && (mcM1 || signalMC->GetMotherPDGexclude(2))
      && ComparePDG((mcM1 ? mcM1->GetPdgCode() : -99999),signalMC->GetMotherPDG(2),signalMC->GetMotherPDGexclude(2),signalMC->GetCheckBothChargesMothers(2))
      && CheckParticleSource(labelM1, signalMC->GetMotherSource(2))
      //      && CheckRadiativeDecision(labelM1,signalMC)
      && CheckDalitzDecision(labelM1,signalMC);
  }

  // grand-mothers
  if(signalMC->GetGrandMotherPDG(1)!=0 || signalMC->GetGrandMotherSource(1)!=PairAnalysisSignalMC::kDontCare) {
    if(!mcG2 && mcM2) {
      labelG2 = GetMothersLabel(labelM2);
      if(labelG2>-1) mcG2 = GetMCTrackFromMCEvent(labelG2);
    }
    crossTerm = crossTerm && (mcG2 || signalMC->GetGrandMotherPDGexclude(1))
                && ComparePDG((mcG2 ? mcG2->GetPdgCode() : 0),signalMC->GetGrandMotherPDG(1),signalMC->GetGrandMotherPDGexclude(1),signalMC->GetCheckBothChargesGrandMothers(1))
                && CheckParticleSource(labelG2, signalMC->GetGrandMotherSource(1));
  }

  if(signalMC->GetGrandMotherPDG(2)!=0 || signalMC->GetGrandMotherSource(2)!=PairAnalysisSignalMC::kDontCare) {
    if(!mcG1 && mcM1) {
      labelG1 = GetMothersLabel(labelM1);
      if(labelG1>-1) mcG1 = GetMCTrackFromMCEvent(labelG1);
    }
    crossTerm = crossTerm && (mcG1 || signalMC->GetGrandMotherPDGexclude(2))
                && ComparePDG((mcG1 ? mcG1->GetPdgCode() : 0),signalMC->GetGrandMotherPDG(2),signalMC->GetGrandMotherPDGexclude(2),signalMC->GetCheckBothChargesGrandMothers(2))
                && CheckParticleSource(labelG1, signalMC->GetGrandMotherSource(2));
  }

  Bool_t motherRelation = kTRUE;
  if(signalMC->GetMothersRelation()==PairAnalysisSignalMC::kSame) {
    motherRelation = motherRelation && HaveSameMother(pair);
  }
  if(signalMC->GetMothersRelation()==PairAnalysisSignalMC::kDifferent) {
    motherRelation = motherRelation && !HaveSameMother(pair);
  }


  return ((directTerm || crossTerm) && motherRelation && processGEANT);
}

//____________________________________________________________
Bool_t PairAnalysisMC::HaveSameMother(const PairAnalysisPair * pair) const
{
  //
  // Check whether two particles have the same mother
  //

  const PairAnalysisTrack * daughter1 = pair->GetFirstDaughter();
  const PairAnalysisTrack * daughter2 = pair->GetSecondDaughter();
  if (!daughter1 || !daughter2) return 0;

  CbmMCTrack *mcDaughter1=GetMCTrackFromMCEvent(daughter1->GetLabel());
  CbmMCTrack *mcDaughter2=GetMCTrackFromMCEvent(daughter2->GetLabel());
  if (!mcDaughter1 || !mcDaughter2) return 0;

  Int_t labelMother1=mcDaughter1->GetMotherId();
  Int_t labelMother2=mcDaughter2->GetMotherId();
  Bool_t sameMother=(labelMother1>-1)&&(labelMother2>-1)&&(labelMother1==labelMother2);

  return sameMother;
}
