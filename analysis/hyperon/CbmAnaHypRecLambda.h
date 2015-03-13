// -------------------------------------------------------------------------
// -----               CbmAnaHypRecLambda header file                  -----
// -----               Created 02/03/05  by E. Kryshen                 -----
// -------------------------------------------------------------------------
//Updated 2006-12-22
#ifndef CBMRECLAMBDATASKANA_H
#define CBMRECLAMBDATASKANA_H

/** @class CbmAnaHypRecLambda.h
 ** @author E.Kryshen  <e.kryshen@gsi.de>
 ** @date 22.12.2006
 **
 ** Full reconstruction for lambda decay.
 **
 */

#include "FairTask.h"

class CbmStsTrack;
class CbmVertex;
class CbmKF;
class CbmAnaHypTools;
class CbmKFSecondaryVertexFinder;
class CbmStsKFTrackFitter;

class TClonesArray;
class TFile;
class TTree;
class TVector3;

#include "CbmVertex.h"
#include <vector>
#include "CbmKFTrack.h"
#include "CbmAnaHypTrack.h"

using std::vector;

class CbmAnaHypRecLambda : public FairTask
{

public:

  /** Default constructor */
  CbmAnaHypRecLambda();

  /** Standard constructor */
  CbmAnaHypRecLambda(const char *name, Int_t iVerbose);

  /** Destructor */
  virtual ~CbmAnaHypRecLambda(){};

  /** Initialization of the task */
  InitStatus Init();

  /** Execute task */
  virtual void Exec(Option_t* option);

  void SetPid(Int_t pid) { fPid=pid; }
  void SetCutChi1   (Double_t cut)     { fCutChi1 = cut;          }
  void SetCutChi2   (Double_t cut)     { fCutChi2 = cut;          }
  void SetCheckChi  (Int_t checkChi)   { fCheckChi  = checkChi;   }
  Int_t GetMCTrack(Int_t iRC);

  void Extrapolate(Double_t z, CbmKFTrack &tr, TVector3 &v, TVector3 &p);

  Double_t CalculateDca  (CbmKFTrack tp, CbmKFTrack tn, Double_t z,
   Double_t &alpha, Double_t &ttp, Double_t &mK0, Double_t &mLa);

  void GetAP(TVector3 p1, TVector3 p2, Double_t &alpha, Double_t &ptt);
  Int_t GetPid(CbmAnaHypTrack tr);


protected:
  Int_t fPid;
  Int_t fCheckChi;
  Int_t fEvent; // current event

  TClonesArray* fMCTracks; ///< MCTracks
  TClonesArray* fRCTracks; ///< Reconstructed StsTracks
  TClonesArray* fRCMatch;  ///<  StsTrack matches
  CbmVertex*    fPV;           ///< Reconstructed primary vertex
  TClonesArray*  fListV0Candidates;

  // Input parameters
  Double_t fCutChi1;      ///< bn  cut value
  Double_t fCutChi2;      ///< bp  cut value

  //Fitter;
  CbmKF* fKF;              ///< Pointer to the fitter interface
  CbmAnaHypTools* fTools;  ///< Pointer to some useful tools, like SetFlags
  CbmKFSecondaryVertexFinder* fSVF;
  CbmStsKFTrackFitter* fFitter;

 private:

  CbmAnaHypRecLambda(const CbmAnaHypRecLambda&);
  CbmAnaHypRecLambda operator=(const CbmAnaHypRecLambda&);

  ClassDef(CbmAnaHypRecLambda,1);

};

#endif
