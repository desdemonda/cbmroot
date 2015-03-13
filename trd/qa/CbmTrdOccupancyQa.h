#ifndef CBMTRDOCCUPANCYQA_H_
#define CBMTRDOCCUPANCYQA_H_

#include "FairTask.h"

#include <map>
#include <list>
//#include <vector>
#include "CbmTrdDigi.h"

#include "TH1.h"
#include "TH2.h"

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;

class TClonesArray;
class TH1F;
//class TH1I;
//class TH2I;
class TH2F;
class TProfile;
class TProfile2D;
class TCanvas;

typedef struct Pad
{
  Double_t SizeX;
  Double_t SizeY;
  Bool_t Fired;
Pad() : SizeX(0), SizeY(0), Fired(false) {}
} Pad;

typedef struct OccupancyModule
{
  Int_t Station;
  Int_t Layer;
  Int_t moduleAddress;
  Int_t nPad;
  Int_t nxPad;
  Int_t nyPad;
  /*
    Int_t NoSectors;
    std::vector<std::vector<Pad*> > PadPlane;
  
    std::vector<Float_t> SectorSizeX;
    std::vector<Float_t> SectorSizeY;
    std::vector<Float_t> PadSizeX;
    std::vector<Float_t> PadSizeY;
    std::vector<Int_t> SecxPad;
    std::vector<Int_t> SecyPad;
  */
  Float_t ModuleSizeX;
  Float_t ModuleSizeY;
  Float_t ModulePositionX;
  Float_t ModulePositionY;
  Float_t ModulePositionZ;

  OccupancyModule () : Station(-1), Layer(-1), moduleAddress(-1), //PadPlane(), 
    nPad(0),
    nxPad(0), nyPad(0),
		       /* NoSectors(0), SectorSizeX(), SectorSizeY(), PadSizeX(), 
			  PadSizeY(), SecxPad(), SecyPad(), */
    ModuleSizeX(0.), ModuleSizeY(0.), 
    ModulePositionX(0.), ModulePositionY(0.), ModulePositionZ() {}
} OccupancyModule;

class CbmTrdOccupancyQa : public FairTask {

 public:
  CbmTrdOccupancyQa();
  //CbmTrdOccupancyQa(const char *name, const char *title="CBM Task", const char *geo="");
  CbmTrdOccupancyQa(const char *name, const char *title="CBM Task", const char *geo="", Double_t triggerThreshold = 1.0e-6);
  virtual ~CbmTrdOccupancyQa();
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  virtual void Exec(Option_t * option);
  virtual void FinishEvent();
  virtual void FinishTask();
  void Register();
  void SaveHistos2File();
  void CreateLayerView();
  void CopyEvent2MemoryMap();
  void SetNeighbourReadout(Bool_t neighbourReadout);
  void SetTriggerThreshold(Double_t triggerthreshold);
  void SetNeighbourTrigger(Bool_t trigger);
 private:
  CbmTrdOccupancyQa& operator=(const CbmTrdOccupancyQa&);
  CbmTrdOccupancyQa(const CbmTrdOccupancyQa&);
  TClonesArray *fDigis; //! TRD digis
  TClonesArray*     fClusters; 
  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdGeoHandler* fGeoHandler; //!
  TH1I *fDigiChargeSpectrum;
  TH2I *fLayerDummy;
  Double_t fmin;
  Double_t fmax;
  std::map<Int_t, OccupancyModule*> fModuleMap; //!
  std::map<Int_t, OccupancyModule*>::iterator fModuleMapIt; //!  
  std::map<Int_t, TH2I*> fModuleOccupancyMap; //!
  std::map<Int_t, TH2I*>::iterator fModuleOccupancyMapIt; //!
  std::map<Int_t, TH1F*> fModuleOccupancyMemoryMap; //!
  std::map<Int_t, TH1F*>::iterator fModuleOccupancyMemoryMapIt; //!
  std::map<Int_t, TCanvas*> fLayerOccupancyMap; //!
  std::map<Int_t, TCanvas*>::iterator fLayerOccupancyMapIt; //!
  std::map<Int_t, TProfile*> fLayerAverageOccupancyMap; //!
  std::map<Int_t, TProfile*>::iterator fLayerAverageOccupancyMapIt; //!
  Double_t fTriggerThreshold;
  Bool_t fNeigbourReadout;
  TString fGeo;

  ClassDef(CbmTrdOccupancyQa,3)

    };
#endif // 
