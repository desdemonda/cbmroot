// -------------------------------------------------------------------------
// -----               CbmAnaHypRecCascade header file                 -----
// -----               Created 02/03/05  by E. Kryshen                 -----
// -------------------------------------------------------------------------


/** @class CbmAnaHypRecCascade.h
 ** @author E.Kryshen  <e.kryshen@gsi.de>
 ** @date 22.06.2006
 ** @date 19.01.2007
 ** Full reconstruction for cascade multistrange hyperons decay.
 **
 */

#ifndef CBMANAHYPRECCASCADE_H
#define CBMANAHYPRECCASCADE_H


#include "CbmAnaHypRecLambda.h"
#include "CbmKFVertex.h"

class TClonesArray;
class TTree;
class TVector3;
class CbmAnaHypLambdaCandidate;
class CbmAnaHypTrack;

class CbmAnaHypRecCascade : public CbmAnaHypRecLambda
{

public:

  /** Default constructor */
  CbmAnaHypRecCascade();

  /** Standard constructor */
  CbmAnaHypRecCascade(const char *name, Int_t iVerbose);

  /** Destructor */
  virtual ~CbmAnaHypRecCascade(){};

  /** Initialization of the task */
  InitStatus Init();

  /** Execute task */
  void Exec(Option_t* option);

  // Setters
  void SetPid(Int_t pid)              { fPid=pid;            }
  void SetMotherPdg  (Int_t pdg)      { fMotherPdg = pdg;    }
  void SetSisterPdg  (Int_t pdg)      { fSisterPdg = pdg;    }
  void SetCutChiDa   (Double_t cut)   { fCutChiDa  = cut;    }
  void SetCutBla     (Double_t cut)   { fCutBla    = cut;    }
  void SetCheckB     (Int_t checkB)   { fCheckB    = checkB; }

protected:
  void CreateDaVector();
  void CreatePair(CbmAnaHypTrack da, CbmAnaHypLambdaCandidate v0, Int_t iLa=-1);

  TTree* fTree;
  TClonesArray* fListRCLambda; ///< Reconstructed lambda candidates
  TClonesArray* fListCascades; ///< Cascade candidates
  Int_t    fCascades;

  Int_t    fMotherPdg;
  Int_t    fSisterPdg;
  Double_t fCutChiDa;
  Double_t fCutBla;
  Int_t    fPid;
  Int_t    fCheckB;
  vector<CbmAnaHypTrack> vDa;

  CbmKFVertex kfPV;
  TVector3 pv;

 private:

  CbmAnaHypRecCascade(const CbmAnaHypRecCascade&);
  CbmAnaHypRecCascade operator=(const CbmAnaHypRecCascade&);

  ClassDef(CbmAnaHypRecCascade,1);
};

#endif
