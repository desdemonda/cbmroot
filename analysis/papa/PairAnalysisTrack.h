#ifndef PAIRANALYSISTRACK_H
#define PAIRANALYSISTRACK_H

//#############################################################
//#                                                           #
//#         Class PairAnalysisTrack                             #
//#         Class for management of the gobal track           #
//#                                                           #
//#  Authors:                                                 #
//#   Julian    Book,     Uni Ffm / Julian.Book@cern.ch       #
//#                                                           #
//#############################################################

#include <TNamed.h>
#include <TLorentzVector.h>

#include "CbmDetectorList.h"
//#include "CbmTrackMatchNew.h"

class CbmTrack;
class CbmGlobalTrack;
class CbmStsTrack;
class CbmTrdTrack;
class CbmRichRing;
class CbmTofHit;
class CbmMCTrack;
class CbmTrackMatchNew;

//class TObjArray;
//class TProcessID;
//class FairRootManager;

class PairAnalysisTrack : public TNamed {
public:

  PairAnalysisTrack();
  PairAnalysisTrack(const char* name, const char* title);
  PairAnalysisTrack(CbmGlobalTrack *gtrk, CbmStsTrack *ststrk, CbmTrdTrack *trdtrk, CbmRichRing *richring, CbmTofHit *tofhit,
		  CbmMCTrack *mctrk,
		  CbmTrackMatchNew *stsmatch, CbmTrackMatchNew *trdMatch, CbmTrackMatchNew *richMatch);

  virtual ~PairAnalysisTrack();

  // setter
  void SetPdgCode(Int_t pdg)              { fPdgCode=pdg; }
  void SetLabel(Int_t lbl)                { fLabel=lbl; }
  void SetWeight(Double_t wght)            { fWeight=wght; }
  void SetMassHypo(Int_t pdg1, Int_t pdg2);

  // track getters
  CbmGlobalTrack *GetGlobalTrack()  const { return fGlblTrack; }
  CbmStsTrack    *GetStsTrack()     const { return fStsTrack;  }
  CbmTrdTrack    *GetTrdTrack()     const { return fTrdTrack;  }
  CbmRichRing    *GetRichRing()     const { return fRichRing;  }
  CbmTofHit      *GetTofHit()       const { return fTofHit;    }
  CbmTrack       *GetTrack(DetectorId det) const;
  // mc track
  CbmMCTrack     *GetMCTrack()      const { return fMCTrack;   }
  CbmTrackMatchNew *GetTrackMatch(DetectorId det) const;

  // default kinematics
  Double_t Px()         const { return fMomentum.Px(); }
  Double_t Py()         const { return fMomentum.Py(); }
  Double_t Pz()         const { return fMomentum.Pz(); }
  Double_t Pt()         const { return fMomentum.Pt(); }
  Double_t P()          const { return fMomentum.P(); }

  Double_t Xv()         const { return fPosition.X(); }
  Double_t Yv()         const { return fPosition.Y(); }
  Double_t Zv()         const { return fPosition.Z(); }

  Double_t OneOverPt()  const { return 1./fMomentum.Pt(); }
  Double_t Phi()        const { return fMomentum.Phi(); }
  Double_t Theta()      const { return fMomentum.Theta(); }

  Double_t E()          const { return fMomentum.Energy(); }
  Double_t M()          const { return fMomentum.M(); }
 
  //  Double_t Eta()        const { return fMomentum.Eta(); }
  Double_t Y()          const { return fMomentum.Rapidity(); }
  
  Short_t Charge()      const { return fCharge;  }
  Short_t PdgCode()     const { return fPdgCode; }
  Int_t   GetLabel()    const { return fLabel;   }
  Double_t GetWeight()  const { return fWeight;  }

private:
  CbmGlobalTrack   *fGlblTrack;       // global track
  CbmStsTrack      *fStsTrack;        // sts track
  CbmTrdTrack      *fTrdTrack;        // trd track
  CbmRichRing      *fRichRing;        // rich ring
  CbmTofHit        *fTofHit;          // tof hit

  CbmMCTrack       *fMCTrack;         // matched mc track
  CbmTrackMatchNew *fStsTrackMatch;   // sts track match
  CbmTrackMatchNew *fTrdTrackMatch;   // trd track match
  CbmTrackMatchNew *fRichRingMatch;   // rich ring match

  TLorentzVector fMomentum;           // momentum vector
  TLorentzVector fPosition;           // position vector
  Short_t        fCharge;             // charge
  Int_t          fPdgCode;            // pdg code
  Int_t          fLabel;              // MC label
  Double_t       fWeight;             // weighting
  Int_t          fMultiMatch;         // MC label for n-times matched tracks

  PairAnalysisTrack(const PairAnalysisTrack& track);
  PairAnalysisTrack &operator=(const PairAnalysisTrack &c);

  ClassDef(PairAnalysisTrack,1)         // PairAnalysis Track
};



#endif
