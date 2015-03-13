
/** The CbmV0Analysis class
 *
 * @author  Y.Vassiliev
 * @version 0.0
 * @since   24.01.06
 * 
 */

#ifndef CBMV0ANALYSIS_H
#define CBMV0ANALYSIS_H

#include "FairTask.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmMCTrack.h"
#include "CbmVertex.h"
#include "TString.h"

class TClonesArray;
class CbmStsTrack;
class CbmMCTrack;
class TTree;
class TFile;

class CbmV0TrackInfo :public TObject{
 public:
  CbmV0TrackInfo(){}
  virtual ~CbmV0TrackInfo(){}
  int iEvent;
  CbmStsTrack track;
  CbmMCTrack mc_track;
  double p, px, py, pz, tx, ty, pt, m, q, E;
  double p_mc, px_mc, py_mc, pz_mc, q_mc;
  double imx, imy, imr,ChiPrimary;
  double mc_x, mc_y, mc_z, mc_MotherZv;
  bool is_ghost, is_long;
  int pdg, mc_pdg, mc_MotherID, mc_MotherPDG, NHits, NHitsMvd;
  int mc_GrandMotherPDG, mc_GrandMotherID;
  ClassDef(CbmV0TrackInfo,1);
};


class CbmV0Analysis :public FairTask {

 public:

  CbmV0Analysis(const char *name="V0", const char *title="Cbm V0 Analysis", Int_t iVerbose = 1);
  ~CbmV0Analysis();
 
  /// * CbmTask methods
  
  void SetParContainers();
  InitStatus ReInit();
  InitStatus Init();
  void Exec(Option_t * option);
  void Finish();

  void EventAnalysis();
 
  Bool_t IsLong(CbmStsTrack &track);

  void Set( CbmV0TrackInfo &T, int it, int iEvent );
  void ReFit( CbmV0TrackInfo &T, int pdg );
  void Fill( CbmV0TrackInfo &T );
  void SetFromMC( CbmV0TrackInfo &T, int it, int iEvent );


  TClonesArray *fSTSTrackMatch;
  TClonesArray *listMCTracks;
  TClonesArray *listStsPts;
  TClonesArray *listStsTracks;
  CbmVertex* fPrimVtx;
  TClonesArray * fHitArray;
  TClonesArray * fMvdHitArray;

  CbmStsKFTrackFitter Fitter;

 private:

  CbmVertex GenVtx;
  TTree *RecoData;
  TFile *ff;

 public: 
  
  ClassDef( CbmV0Analysis, 1 );
};

#endif /* !CBMV0ANALYSIS_H */
