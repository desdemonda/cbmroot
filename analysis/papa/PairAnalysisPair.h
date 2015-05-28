#ifndef PAIRANALYSISPAIR_H
#define PAIRANALYSISPAIR_H

//#############################################################
//#                                                           #
//#                  PairAnalysisPair                           #
//#               Interface class to handle pair information  #
//#                                                           #
//#                                                           #
//#  Authors:                                                 #
//#   Julian    Book,     Uni Ffm / Julian.Book@cern.ch       #
//#                                                           #
//#############################################################

#include <TMath.h>
#include <TRef.h>

#include "PairAnalysisTrack.h"
#include "PairAnalysisTrackRotator.h"

class CbmVertex;
class CbmMCTrack;

class PairAnalysisPair : public PairAnalysisTrack {
public:
  PairAnalysisPair();
  virtual ~PairAnalysisPair();
  PairAnalysisPair(const PairAnalysisPair& pair);

  PairAnalysisPair(Char_t type);

  virtual void SetTracks(PairAnalysisTrack * const particle1, Int_t pid1,
			 PairAnalysisTrack * const particle2, Int_t pid2)  = 0;

  virtual void SetGammaTracks(PairAnalysisTrack * const particle1, Int_t pid1,
			      PairAnalysisTrack * const particle2, Int_t pid2) = 0;

  virtual void SetMCTracks(const CbmMCTrack * const particle1,
			   const CbmMCTrack * const particle2)  = 0;

  /*
  virtual void SetTracks(const AliKFParticle * const particle1,
			 const AliKFParticle * const particle2,
			 PairAnalysisTrack * const refParticle1,
			 PairAnalysisTrack * const refParticle2) = 0;
  */

  //PairAnalysisTrack interface
  // kinematics
  virtual Double_t Px() const { return -999.; }
  virtual Double_t Py() const { return -999.; }
  virtual Double_t Pz() const { return -999.; }
  virtual Double_t Pt() const { return -999.; }
  virtual Double_t P() const  { return -999.;  }
  virtual Bool_t   PxPyPz(Double_t p[3]) const { p[0]=Px(); p[1]=Py(); p[2]=Pz(); return kTRUE; }
  
  virtual Double_t Xv() const { return -999.; }
  virtual Double_t Yv() const { return -999.; }
  virtual Double_t Zv() const { return -999.; }
  virtual Bool_t   XvYvZv(Double_t x[3]) const { x[0]=Xv(); x[1]=Yv(); x[2]=Zv(); return kTRUE; }
  
  virtual Double_t OneOverPt() const { return Pt()>0.?1./Pt():0.; }  //TODO: check
  virtual Double_t Phi()       const { return -999.;}
  virtual Double_t Theta()     const { return Pz()!=0?TMath::ATan(Pt()/Pz()):0.; } //TODO: check
  
  
  virtual Double_t E() const { return -999.; }
  virtual Double_t M() const { return -999.; }
  
  //  virtual Double_t Eta() const { return -999.; }
  virtual Double_t Y()   const { return -999.; }
  
  virtual Short_t Charge() const    { return -1;}
  Int_t    GetLabel()   const  { return fLabel;       }
  Double_t GetWeight()  const  { return fWeight;      }
  // PID
  const Double_t *PID() const { return 0;} //TODO: check

  UChar_t GetType() const { return fType; }
  void SetType(Char_t type) { fType=type; }

  static void SetBeamEnergy(Double_t beamEbyHand=-1.);

  // MC information
  void SetLabel(Int_t label)     { fLabel=label;}
  void SetWeight(Double_t wght)  { fWeight=wght;}
  void SetPdgCode(Int_t pdgCode) { fPdgCode=pdgCode; }
  Int_t PdgCode() const {return fPdgCode;}

  ////  virtual void SetProductionVertex(const AliKFParticle &/*Vtx*/) { return; }
  
  //inter leg information
  virtual Double_t GetChi2()              const { return -999.;     }
  virtual Int_t    GetNdf()               const { return -999.;     }
  virtual Double_t GetDecayLength()       const { return -999.;     }
  virtual Double_t GetR()                 const { return -999.;     }
  virtual Double_t OpeningAngle()         const { return -999.;     }
  virtual Double_t OpeningAngleXY()       const { return -999.;     }
  virtual Double_t OpeningAngleRZ()       const { return -999.;     }
  virtual Double_t DistanceDaughters()    const { return -999.;     }
  virtual Double_t DistanceDaughtersXY()  const { return -999.;     }
  virtual Double_t DeviationDaughters()   const { return -999.;     }
  virtual Double_t DeviationDaughtersXY() const { return -999.;     }
  virtual Double_t DeltaEta()             const { return -999.;     }
  virtual Double_t DeltaPhi()             const { return -999.;     }

  // calculate cos(theta*) and phi* in HE and CS pictures
  virtual void GetThetaPhiCM(Double_t &thetaHE, Double_t &phiHE, Double_t &thetaCS, Double_t &phiCS) const = 0;

  virtual Double_t ThetaPhiCM(Bool_t isHE, Bool_t isTheta) const;
  static  Double_t ThetaPhiCM(const PairAnalysisTrack* d1, const PairAnalysisTrack* d2,
			      Bool_t isHE, Bool_t isTheta);

  virtual Double_t PsiPair(Double_t MagField)  const = 0;  //Angle cut w.r.t. to magnetic field
  virtual Double_t PhivPair(Double_t MagField) const = 0; //Angle of ee plane w.r.t. to magnetic field

  //Calculate the angle between ee decay plane and variables
  virtual Double_t GetPairPlaneAngle(Double_t kv0CrpH2, Int_t VariNum) const = 0;

  virtual Double_t GetCosPointingAngle(const CbmVertex */*primVtx*/) const = 0;

  // TODO: replace by AliKFPArticleBase function
  virtual Double_t GetArmAlpha() const = 0;
  virtual Double_t GetArmPt()    const = 0;
  void GetDCA(const CbmVertex *primVtx, Double_t d0z0[2]) const; // TOCHECK

  // Calculate inner product of strong magnetic field and ee plane
  virtual Double_t PairPlaneMagInnerProduct(Double_t ZDCrpH1) const = 0;

  // daughter references
  void SetRefFirstDaughter(PairAnalysisTrack * const track)  {fRefD1 = track;}
  void SetRefSecondDaughter(PairAnalysisTrack * const track) {fRefD2 = track;}

  PairAnalysisTrack* GetFirstDaughter()   const { return dynamic_cast<PairAnalysisTrack*>(fRefD1.GetObject()); }
  PairAnalysisTrack* GetSecondDaughter()  const { return dynamic_cast<PairAnalysisTrack*>(fRefD2.GetObject()); }
  Int_t GetFirstDaughterPid()         const { return fPid1; }
  Int_t GetSecondDaughterPid()        const { return fPid2; }

  // rotations
  virtual void RotateTrack(PairAnalysisTrackRotator *rot) = 0;

  void SetKFUsage(Bool_t KFUsage) {fKFUsage = KFUsage;}
  Bool_t GetKFUsage() const {return fKFUsage;}

protected:
  Char_t   fType;         // type of the pair e.g. like sign SE, unlike sign SE, ... see PairAnalysis
  Int_t    fLabel;        // MC label
  Double_t fWeight;       // weighting factor
  Int_t    fPdgCode;      // pdg code in case it is a MC particle
  static Double_t fBeamEnergy; //!beam energy

  TRef fRefD1;           // Reference to first daughter
  TRef fRefD2;           // Reference to second daughter

  Int_t fPid1;           // reference to first daughter pid
  Int_t fPid2;           // reference to second daughter pid

  Bool_t fKFUsage;       // Use KF for vertexing

  ClassDef(PairAnalysisPair,1)
};

#endif

