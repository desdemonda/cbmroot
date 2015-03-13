#ifndef CBMTRDDIGITIZERPRF_H
#define CBMTRDDIGITIZERPRF_H

#include "FairTask.h"
#include "TRandom3.h"
#include <map>
using std::map;
using std::pair;

class CbmTrdPoint;
class CbmTrdDigi;
class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdRadiator;
class CbmMatch;
class TClonesArray;

class CbmTrdDigitizerPRF : public FairTask {
 public:

  /**
   * \brief Standard constructor.
   **/
  CbmTrdDigitizerPRF(CbmTrdRadiator* radiator);

  /**
   * \brief Destructor.
   **/
  virtual ~CbmTrdDigitizerPRF();

  /**
   * \brief Inherited from FairTask.
   **/
  virtual InitStatus Init();

  /**
   * \brief Inherited from FairTask.
   **/
  virtual void SetParContainers();

  /**
   * \brief Inherited from FairTask.
   **/
  virtual void Exec(Option_t * option);

  void SetTriangularPads(Bool_t triangles);
  void SetNCluster(Int_t nCluster);
  void SetNoiseLevel(Double_t sigma_keV);
  void SetTriggerThreshold(Double_t minCharge); //only for debugging. has no impact on the output!!!!
  void SetPadPlaneScanArea(Int_t column, Int_t row);
  void SetCbmLinkWeightDistance(Bool_t dist);
 private:

  CbmTrdDigitizerPRF& operator=(const CbmTrdDigitizerPRF&);
  CbmTrdDigitizerPRF(const CbmTrdDigitizerPRF&);

  Double_t CalcPRF(Double_t x, Double_t W, Double_t h);

  void ScanPadPlane(const Double_t* local_point, Double_t clusterELoss);

  void ScanPadPlaneTriangle(const Double_t* local_point, Double_t clusterELoss);

  Double_t TriangleIntegration(Bool_t even, Double_t displacement_x, Double_t W, Double_t displacement_y, Double_t H, Double_t h);

  Double_t TriangleIteration(Bool_t even, Int_t step, Double_t displacement_x, Double_t W, Double_t displacement_y, Double_t H, Double_t h);

  void SplitTrackPath(const CbmTrdPoint* point, Double_t ELoss);

  void AddDigi(Int_t pointId, Int_t address, Double_t charge, Double_t time);

  Bool_t fDebug;
  Bool_t fTrianglePads;
  Bool_t fCbmLinkWeightDistance;

  Double_t fSigma_noise_keV;
  TRandom3 *fNoise;
  Double_t fMinimumChargeTH;
  Double_t fTime;

  Int_t fnClusterConst;
  Int_t fnScanRowConst;
  Int_t fnScanColConst;
  Int_t fnRow;
  Int_t fnCol;
  Int_t fLayerId;
  Int_t fModuleId;
  Int_t fMCPointId;

  TClonesArray* fPoints; //! Trd MC points
  TClonesArray* fDigis; //! TRD digis
  TClonesArray* fDigiMatches; //! Corresponding MCPoints to TRD digis
  TClonesArray* fMCTracks;  //! MC Track information

  CbmTrdDigiPar* fDigiPar;    //!
  CbmTrdModule* fModuleInfo; //!
  CbmTrdRadiator* fRadiator;  //!

  std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> > fDigiMap; // Temporary storage for digis.

  ClassDef(CbmTrdDigitizerPRF, 2);
};
#endif // CBMTRDDIGITIZER_PRF_H
