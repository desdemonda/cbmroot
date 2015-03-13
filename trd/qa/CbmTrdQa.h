#ifndef CBMTRDQA_H
#define CBMTRDQA_H

#include "FairTask.h"

#include <map>
class CbmTrdRadiator;
class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TClonesArray;
class CbmTrdPoint;
class CbmTrdDigi;
class CbmTrdCluster;
class CbmTrdHit;
class TProfile;
class TProfile2D;
class TH1F;
class TH1I;
class TH2I;
class TH2F;
class TCanvas;
class TPolyLine;
class CbmTrdQa : public FairTask {

 public:
  CbmTrdQa(CbmTrdRadiator *radiator=NULL);
  CbmTrdQa(const char *name, const char *title="CBM Task", const char *geo="", Double_t triggerThreshold = 1.0e-6, CbmTrdRadiator *radiator=NULL);
  virtual ~CbmTrdQa();
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  virtual void Exec(Option_t * option);
  void CreateLayerView(std::map<Int_t, TH1*>& Map, TString folder, TString pics, TString zAxisTitle, Double_t fmax, Double_t fmin, Bool_t logScale);
  virtual void FinishEvent();
  virtual void FinishTask();
  void Register();
  void SetTriggerThreshold(Double_t triggerthreshold);
  void SetTriangularPads(Bool_t triangles);
 private:
  Double_t CalcAngle(const CbmTrdPoint* pointA, const CbmTrdPoint* pointB);
  void SaveHistos();
  TPolyLine *CreateTriangularPad(Int_t column, Int_t row, Double_t content);
  void NormalizeHistos();
  void CreateLayerView();
  void GetPadInfos(Int_t moduleAddress, Double_t x, Double_t y, Int_t &iCol, Int_t &iRow, Double_t &padSizeX, Double_t &padSizeY);
  Double_t GetTrackLength(CbmTrdPoint* point);

  CbmTrdQa& operator=(const CbmTrdQa&);
  CbmTrdQa(const CbmTrdQa&);

  TClonesArray *fMCTracks;
  TClonesArray *fPoints; //!
  TClonesArray *fDigis; //!
  TClonesArray *fClusters;//!
  TClonesArray *fHits; //!
  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdGeoHandler *fGeoHandler; //!
  std::map<Int_t, TCanvas*> fLayerMap; //!
  std::map<Int_t, TCanvas*>::iterator fLayerMapIt; //! 
  std::map<Int_t, TH2I*> fLayerPointMap; 
  std::map<Int_t, TH2I*>::iterator fLayerPointMapIt; //! 
  std::map<Int_t, TH2I*> fLayerHitMap; 
  std::map<Int_t, TH2I*>::iterator fLayerHitMapIt; //! 
 
  std::map<Int_t, std::vector<Int_t> > fModulePointMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModulePointMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleDigiMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleDigiMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleClusterMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleClusterMapIt; //!  
  std::map<Int_t, std::vector<Int_t> > fModuleHitMap; //!
  std::map<Int_t, std::vector<Int_t> >::iterator fModuleHitMapIt; //!  


  std::map<Int_t, TH1I*> fModuleAveragePointsMap;
  std::map<Int_t, TH1I*>::iterator fModuleAveragePointsMapIt;
  std::map<Int_t, TH1I*> fModuleTrackableMap2; //!
  std::map<Int_t, TH1I*>::iterator fModuleTrackableMap2It; //! 
  std::map<Int_t, TH1I*> fModuleTrackableMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleTrackableMapIt; //! 
  std::map<Int_t, TH1I*> fModuleClusterSizeMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleClusterSizeMapIt; //! 
  std::map<Int_t, TH1I*> fModuledEdxMap; //!
  std::map<Int_t, TH1I*>::iterator fModuledEdxMapIt; //! 
  std::map<Int_t, TH1I*> fModuleTracklengthMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleTracklengthMapIt; //! 
  std::map<Int_t, TH1I*> fModuleMultiPointMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleMultiPointMapIt; //!  
  std::map<Int_t, TH1I*> fModuleDeltaEMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleDeltaEMapIt; //! 
  std::map<Int_t, TH1I*> fModuleGhostMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleGhostMapIt; //!  
  std::map<Int_t, TH1I*> fModuleLostMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleLostMapIt; //!
  std::map<Int_t, TH1I*> fModuleEfficiencyMap; //!
  std::map<Int_t, TH1I*>::iterator fModuleEfficiencyMapIt; //!

  Double_t fTriggerThreshold;
  TString fGeo;

  //histos

  Bool_t fD;
  Bool_t fT;
  Bool_t fP;
  Bool_t fC;
  Bool_t fH;

  Bool_t fTrianglePads;

  TH2I *fLayerDummy;
  TH2F *fStsTrdPoints;
  TH2F *fStsMuchPoints;
  TH2F *fStsTofPoints;
  TH2F *fMuchTrdPoints[11];
  TH2F *fMuchTofPoints[11];
  TH2F *fTrdTofPoints[11];

  TH2F *fStsTrdPointsTrackable;
  TH2F *fTrdPointsPerMcTrack_PID;
  TH2F *fTrdPointsPerMcTrack_PT;
  TH2F *fTrdPointsPerMcTrack_P;

  TH2F *fTrdTrackCrossedRadiator;

  TH1I *fMultiHitSamePadPerMcTrack;
  TH1I *fMultiHitSamePadPerMcTrack_angle;
  TH2I *fMultiHitSamePadPerMcTrack_zBirth;
  TH2I *fMultiHitSamePadPerMcTrack_PID;
  TH2I *fMultiHitSamePadPerMcTrack_motherPID;
  TH2I *fMultiHitSamePadPerMcTrack_motherzBirth;

  TH1I *fMultiHitAdjacentPadPerMcTrack;
  TH1I *fMultiHitAdjacentPadPerMcTrack_angle;
  TH2I *fMultiHitAdjacentPadPerMcTrack_zBirth;
  TH2I *fMultiHitAdjacentPadPerMcTrack_PID;
  TH2I *fMultiHitAdjacentPadPerMcTrack_motherzBirth;
  TH2I *fMultiHitAdjacentPadPerMcTrack_motherPID;

  TH1I *fDistanceMcToHit;
  TH1I *fDistanceMcToHitAll;

  TH1I *fPositionResolutionShort;
  TH1I *fPositionResolutionLong;

  TH1F *fdEdxPoint;
  TH1F *fdEdxDigi;
  TH1F *fdEdxCluster;
  TH1F *fdEdxHit;

  TH1F *fdEdxPionMc;
  TH1F *fdEdxPionHit;
  TH1F *fdEdxPionGhost;
  TH1F *fdEdxElectronMc;
  TH1F *fdEdxElectronHit;
  TH1F *fdEdxElectronGhost;

  TH1I *fClusterSize;
  TH1I *fPointsPerDigi;
  TH1I *fDigiPerCluster;
  TH1I *fClusterPerHit;

  TProfile *fMultiPointssVsR;
  TProfile *fGhostPointsVsR;
  TProfile *fMultiHitsVsR;
  TProfile *fGhostHitVsR;
  TProfile *fLostPointVsR;
  TProfile *fHitToPointEfficiencyVsR;
  TProfile *fMultiHitsVsAlpha;
  TProfile *fGhostHitVsAlpha;
  TProfile *fLostPointVsAlpha;
  TProfile *fHitToPointEfficiencyVsAlpha;
  TProfile *fPRF_1D;
  TH2I *fPRF_2D;
  //LayerView

  CbmTrdRadiator *fRadiator;

  ClassDef(CbmTrdQa,1);
};
#endif // 
