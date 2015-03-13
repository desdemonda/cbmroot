#ifndef CBMTRDDIGITIZERMATHIESON_H
#define CBMTRDDIGITIZERMATHIESON_H

#include "FairTask.h"

#include <map>
#include <list>
#include <vector>
#include "CbmTrdDigi.h"

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdRadiator;
class CbmTrdGeoHandler;

class TClonesArray;
class TH1F;
class TH2F;
class TProfile;

typedef struct MyPoint
{
  Double_t global_inC[3];
  Double_t global_outC[3];
  Double_t local_inC[3];
  Double_t local_outC[3];
  Int_t Sec_in;
  Int_t Row_in;
  Int_t Col_in;
  Int_t Sec_out;
  Int_t Row_out;
  Int_t Col_out;
  Double_t clusterPosC[3];
  Int_t Sec_cluster;
  Int_t Row_cluster;
  Int_t Col_cluster;

  Double_t global_inLL[3];
  Double_t global_outLL[3];
  Double_t local_inLL[3];
  Double_t local_outLL[3];
  Double_t clusterPosLL[3];

  Double_t deltaR;

  Double_t ELoss;
  Double_t ELossTR;
  Double_t ELossdEdX;

  Float_t alpha;
  Float_t beta;
  Float_t gamma;
} MyPoint;

typedef struct ModulePara
{
  Int_t Station;
  Int_t Layer;
  Int_t moduleId;
  Int_t xPos;
  Int_t yPos;
  Int_t zPos;
  Int_t nCol;
  Int_t nRow;
  Int_t NoSectors;
  
  std::vector<Float_t> SectorSizeX;
  std::vector<Float_t> SectorSizeY;
  std::vector<Float_t> PadSizeX;
  std::vector<Float_t> PadSizeY;
  std::vector<Int_t> SecCol;
  std::vector<Int_t> SecRow;
  /*
  std::vector<Int_t> PadSizeXArray;
  std::vector<Int_t> PadSizeYArray;
  */
  Float_t ModuleSizeX;
  Float_t ModuleSizeY;
  Float_t ModulePositionX;
  Float_t ModulePositionY;
  Float_t ModulePositionZ;

ModulePara() : Station(-1), Layer(-1), moduleId(-1), xPos(0), yPos(0), zPos(),
    nCol(0), nRow(0), NoSectors(0), SectorSizeX(), SectorSizeY(), PadSizeX(), 
    PadSizeY(), SecCol(), SecRow(), ModuleSizeX(0.), ModuleSizeY(0.), 
    ModulePositionX(0.), ModulePositionY(0.), ModulePositionZ() {}
} ModulePara;

class CbmTrdDigitizerMathieson : public FairTask {

 public:

  /** Default constructor **/
  CbmTrdDigitizerMathieson();

  /** Standard constructor **/
  CbmTrdDigitizerMathieson(const char *name, const char *title="CBM Task",
		    CbmTrdRadiator *radiator=NULL);

  /** Destructor **/
  virtual ~CbmTrdDigitizerMathieson();

  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();

  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish (called after each event) **/
  virtual void FinishEvent();
 
  /** Finish task (called after all event) **/
  virtual void FinishTask(){;}

  /*
    void AddHit(TVector3 &posHit, TVector3 &posHitErr,
    Int_t TrackID, Int_t PlaneID, Int_t ref, Double_t ELoss,
    Double_t ELossTR, Double_t ELossdEdX);
  */

  void Register();

 private:

  CbmTrdDigitizerMathieson& operator=(const CbmTrdDigitizerMathieson&);
  CbmTrdDigitizerMathieson(const CbmTrdDigitizerMathieson&);

  void GetModuleInformationFromDigiPar(Int_t VolumeID);
  void GetModuleInformation();

  void AddDigi(const Int_t pointID, Int_t iCol, Int_t iRow, Int_t nCol, Int_t nRow, Double_t iCharge);

  float GetFloatPositionY(Double_t tempPosY);

  void SplitPathSlices(Bool_t approx, Bool_t fast, Bool_t lookup, Bool_t gaus, const Int_t pointID, MyPoint *point, Double_t* PadChargeModule, 
		       Int_t j, Double_t* padW, Double_t* padH );

  void WireQuantisation(/*Double_t *ClusterMLL,*/ MyPoint *point);

  void GetIntegrationArea(TH1F* PadX ,TH1F* PadY);

  void ChargeConservation(MyPoint *point);

  void ClusterMapping(MyPoint *point, Double_t* PadChargeModule);

  Double_t ApproxMathieson(Double_t x, Double_t W);

  Double_t CalcMathieson(Double_t r);

  Double_t CalcGaus(Double_t r);

  void FillMathiesonVector();

  Double_t LookupMathiesonVector(Double_t r);

  void PadPlaneSampling( Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H);

  void SlowIntegration(Bool_t lookup, Bool_t gaus, Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H);

  void FastIntegration(Bool_t lookup, Bool_t gaus, Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H);

  void TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate, Double_t* StrucDim);
 
  void TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate, Double_t* StrucDim);

  double GetPadHeight(Int_t iSector);

  int GetRow(Double_t tempPosY);

  int GetCol(Double_t tempPosX);

  int GetSector(Double_t tempPosY);

  void GetPadSizeMatrix(MyPoint *point, Double_t* H, Double_t* W, Double_t* padH, Double_t* padW);

  Double_t DeltaGrid(Double_t doubleV, Double_t offset);

  //void TransformMod2Pad(Double_t* MCoordinate, Double_t* PCoordinate, Double_t* StrucDim);
 
  Int_t Digicounter;
  Double_t fLayerZ[12];
  Double_t fTime;
  Int_t   fModuleType;
  Int_t   fModuleCopy;
  Int_t   fModuleID;//Unique number for detector module
  Int_t   fMCindex;// index to MCPoint
  /*
    Int_t fStation;
    Int_t fLayer;
    Int_t fmoduleId;
    Int_t fnCol;
    Int_t fnRow;
    Int_t fNoSectors;
  
    std::vector<Float_t> fSectorSizeX;
    std::vector<Float_t> fSectorSizeY;
    std::vector<Float_t> fPadSizeX;
    std::vector<Float_t> fPadSizeY;
    std::vector<Int_t> fSecCol;
    std::vector<Int_t> fSecRow;
    std::vector<Float_t> fPadSizeXArray;
    std::vector<Float_t> fPadSizeYArray;
   
    Float_t fModuleSizeX;
    Float_t fModuleSizeY;
    Float_t fModulePositionX;
    Float_t fModulePositionY;
    Float_t fModulePositionZ;
  */




  Double_t tempx;
  Double_t tempy;

  static const Int_t accuracy = 1;// '1/accuracy' integration step width [mm]
  static const Int_t Accuracy = 1000; // fMathieson array accuracy in values per mm

  static const Int_t fPadNrX = 11;//15;//7; // has to be odd
  static const Int_t fPadNrY = 11;//5;//5; // has to be odd
  Double_t fPadCharge[fPadNrY][fPadNrX]; //Charge on 3 adjacent pads calculated by using the Mathieson formula
  /*
    Int_t fPadNrX;//7; // has to be odd
    Int_t fPadNrY;//5; // has to be odd
    std::vector< std::vector <Double_t> > fPadCharge;
  */
  //static const Int_t fNoSectors = 3;
  static const Int_t endOfMathiesonArray = 35; //+- mm
  Double_t fMathieson[endOfMathiesonArray * 1000];//endOfMathiesonArray * Accuracy
  Double_t fModuleSize[3];
 
 
  Float_t fEfficiency; // Digi production efficiency (0-100%)

  TClonesArray *fTrdPoints; //! Trd MC points
  TClonesArray *fDigiCollection; //! TRD digis
  TClonesArray *fDigiMatchCollection; //! Corresponding MCPoints to TRD digis
  TClonesArray *fMCStacks;  //! MC Track information

  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdRadiator *fRadiators;  //!

  CbmTrdGeoHandler* fGeoHandler; //!
    
  /**  map to store digis for pair of x,y position in module **/
  // map<pair<ModuleID,pair<x,y>>, CbmTrdDigi*>
  std::map<std::pair< Int_t, std::pair< Int_t, Int_t > >, CbmTrdDigi* > fDigiMap; //!
  /**  iterator over map to store digis for pair of x,y position in module **/
  std::map<std::pair< Int_t, std::pair< Int_t, Int_t > >, CbmTrdDigi* >::iterator fDigiMapIt; //! iterator over array above

  std::map<Int_t, ModulePara*> fModuleParaMap;
  std::map<Int_t, ModulePara*>::iterator fModuleParaMapIt;

  //TH1F* fIntegralTest;

  ClassDef(CbmTrdDigitizerMathieson,2)

    };
#endif // CBMTRDDIGITIZERMATHIESON_H
