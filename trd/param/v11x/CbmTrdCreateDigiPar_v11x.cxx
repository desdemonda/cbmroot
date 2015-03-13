#include "CbmTrdCreateDigiPar.h"

#include "CbmTrdPads.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiPar.h"
#include "CbmDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TMath.h"
#include "TPRegexp.h"

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdCreateDigiPar::CbmTrdCreateDigiPar()
  :FairTask("TrdCreateDigiPar"),
   fStation(-1),
   fLayer(-1),
   fModuleType(-1),
   fModuleCopy(-1),
   fSector(-1),
   fPadSizeX(0), 
   fPadSizeY(0), 
   fsizex(-1.),    
   fsizey(-1.),    
   fSectorSizeX(0),
   fSectorSizeY(0),
   fColumn(-1),     
   fRow(-1),     
   fModuleAddress(-1),
   fMaxSectors(0),
   fPosX(-1.),    
   fPosY(-1.),    
   fModuleMap(), 
   fModuleMapIt(),
   fDigiPar(NULL),
   fTrdDetId()
{
  // Get the maximum number of sectors. All arrays will have
  // this number of entries.  
  fMaxSectors=TMath::Max(fst1_sect_count,fst2_sect_count); 
  fMaxSectors=TMath::Max(fMaxSectors,fst3_sect_count); 

  fSectorSizeX.Set(fMaxSectors);
  fSectorSizeY.Set(fMaxSectors);
  fPadSizeX.Set(fMaxSectors);
  fPadSizeY.Set(fMaxSectors);
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdCreateDigiPar::CbmTrdCreateDigiPar(const char *name, const char *title)
  :FairTask(name),
   fStation(-1),
   fLayer(-1),
   fModuleType(-1),
   fModuleCopy(-1),
   fSector(-1),
   fPadSizeX(0), 
   fPadSizeY(0), 
   fsizex(-1.),    
   fsizey(-1.),    
   fSectorSizeX(0),
   fSectorSizeY(0),
   fColumn(-1),     
   fRow(-1),     
   fModuleAddress(-1),
   fMaxSectors(0),
   fPosX(-1.),    
   fPosY(-1.),    
   fModuleMap(), 
   fModuleMapIt(),
   fDigiPar(NULL),
   fTrdDetId()
{
  // Get the maximum number of sectors. All arrays will have
  // this number of entries.  
  fMaxSectors=TMath::Max(fst1_sect_count,fst2_sect_count); 
  fMaxSectors=TMath::Max(fMaxSectors,fst3_sect_count); 

  fSectorSizeX.Set(fMaxSectors);
  fSectorSizeY.Set(fMaxSectors);
  fPadSizeX.Set(fMaxSectors);
  fPadSizeY.Set(fMaxSectors);
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdCreateDigiPar::~CbmTrdCreateDigiPar()
{

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdCreateDigiPar::SetParContainers(){

  cout<<" * CbmTrdCreateDigiPar * :: SetParContainers() "<<endl;

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
             (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdCreateDigiPar::ReInit(){

  cout<<" * CbmTrdCreateDigiPar * :: ReInit() "<<endl;

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
             (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdCreateDigiPar::Init(){

  cout<<" * CbmTrdCreateDigiPar * :: Init() "<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  
  FillModuleMap();

  return kSUCCESS;
}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdCreateDigiPar::Exec(Option_t * option){

}
// --------------------------------------------------------------------


void CbmTrdCreateDigiPar::GetModuleInformation(){

  // Extract the information about station, layer, module type
  // and cpoy number of the module from the full path to the
  // node.
  // The full path is tokenized at the "/" which diide the different
  // levels of the geometry.
  // Knowing the nameing scheme of the volumes one gets the required
  // information with simple string manipulation.
  // This is probably not the fastest way, but the speed has to be checked.
  // The methode works only for versions of Root > 5.20.0, before the
  // class TStringTocken is not implemented

  TString path = gGeoManager->GetPath();
  TStringToken* bla = new TStringToken(path,"/");
  
  //put here as an example the full path string and the substrings

  while (bla->NextToken()) {
    if (bla->Contains("mod")) {
      TString bla3 = (TString) *bla;
      Ssiz_t pos = bla3.Last('_');
      Ssiz_t substringLength=bla3.Length()-pos-1;
      TString bla2 = bla3((bla3.Last('_')+1),substringLength);
      TString bla1 = bla3(3,1);
      fStation=bla1.Atoi();
// no layers in Jul10  
      //      fLayer=bla2.Atoi();
    }
    if (bla->Contains("mod")){
      TString bla3 = (TString) *bla;
      Ssiz_t pos = bla3.Last('_');
      Ssiz_t substringLength=bla3.Length()-pos-1;
      TString bla2 = bla3(pos+1,substringLength);
      substringLength=pos-7;
      TString bla1 = bla3(7,substringLength);     

      fModuleType = bla1.Atoi();
      fModuleCopy = bla2.Atoi();
      fLayer = fModuleCopy / 1000;
      fModuleCopy = fModuleCopy % 1000;
      break; // Don't know why this is here
    } 
  }
}

// --------------------------------------------------------------------

void CbmTrdCreateDigiPar::CalculateModuleId(){

  Int_t detInfo_array[6]={kTRD, fStation, fLayer, fModuleType, 
                          fModuleCopy, 0};      
  fModuleAddress = fTrdDetId.SetDetectorInfo(detInfo_array);

}

// --------------------------------------------------------------------

void CbmTrdCreateDigiPar::FillModuleMap(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TRD there are keeping volumes
  // trd1-trd4 for each station which are only containers 
  // for the different layers of one station. The trdlayer
  // is again only a container for all volumes of this layer.   
  // Loop over all nodes below the top node (cave). If one of
  // the nodes containes a string trd it must be one of the
  // stations. Now loop over the layers of this station and 
  // then over all modules of the layer to extract in the end
  // all active regions (gas) of the complete TRD. For each
  // of the gas volumes get the information about size and
  // position from the geomanager and the sizes of the sectors
  // and pads from the definitions in CbmTrdPads. This info
  // is then stored in a TrdModule object for each of the
  // TRD modules.

  Int_t nmodules = 0;
  TString TopNode = gGeoManager->GetTopNode()->GetName();
  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    if (TString(node->GetName()).Contains("trd")) {
      TString StationNode = node->GetName();
      TGeoNode* station = node;

// no layers in Jul10  
//      TObjArray* layers = station->GetNodes();
//      for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {
//        TGeoNode* layer = (TGeoNode*) layers->At(iLayer);
//        TString LayerNode = layer->GetName();
//	TObjArray* modules = layer->GetNodes();

	TObjArray* modules = station->GetNodes();
	for (Int_t iLayerPart = 0; iLayerPart < modules->GetEntriesFast(); iLayerPart++) {
          TGeoNode* module = (TGeoNode*) modules->At(iLayerPart);
          TString ModuleNode = module->GetName();
          TObjArray* parts = module->GetNodes();
	  for (Int_t iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
            TGeoNode* part = (TGeoNode*) parts->At(iPart);
            if (TString(part->GetName()).Contains("gas")) {
              TString PartNode = part->GetName();
              // Put together the full path to the interesting volume, which
	      // is needed to navigate with the geomanager to this volume.
              // Extract the geometry information (size, global position)
              // from this volume.

// no layers in Jul10  
//              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + 
//                                 LayerNode + "/" + ModuleNode + "/" + PartNode;

              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + 
                                 ModuleNode + "/" + PartNode;
              gGeoManager->cd(FullPath.Data());

              TGeoVolume *curvol = gGeoManager->GetCurrentVolume();
              TGeoBBox *shape = (TGeoBBox*)curvol->GetShape(); 
              Float_t sizex = shape->GetDX();
              Float_t sizey = shape->GetDY();
              Float_t sizez = shape->GetDZ();

              // Fill fStation, fLayer, fModuleType
	      // and fModuleCopy                   
	      GetModuleInformation(); 
                                      
              // Calculate unique id for each module
              CalculateModuleId(); 

	      Double_t local[3] = {0., 0., 0.};  // Local centre of volume
              Double_t global[3];                // Global centre of volume
              gGeoManager->LocalToMaster(local, global);
              TGeoHMatrix *matrix = gGeoManager->GetCurrentMatrix(); 
              Double_t x = global[0];
              Double_t y = global[1];
              Double_t z = global[2]; 

              // Get Information about the padstructure for a
              // given trd module defined by the station and
              // layer numbers, the module type and the copy
              // number
              FillPadInfo();

              //Orientation of the detector layers
              //Layer 1 and 3 have resolution in x-direction (Orientation=1)
              //Layer 2 and 4 have resolution in y-direction (Orientation=0)
              Int_t Orientation=fLayer%2;
              if( 0 == Orientation) {  // flip pads for even layers
		Double_t copybuf;
                for ( Int_t i=0; i<fMaxSectors; i++) {
		  copybuf = fPadSizeX.At(i);
		  fPadSizeX.AddAt(fPadSizeY.At(i),i);
		  fPadSizeY.AddAt(copybuf,i);
		  copybuf = fSectorSizeX.At(i);
		  fSectorSizeX.AddAt(fSectorSizeY.At(i),i);
		  fSectorSizeY.AddAt(copybuf,i);
                }
              }
              nmodules++;

              // Create new CbmTrdModule and add it to the map
	      fModuleMap[fModuleAddress] = 
                new CbmTrdModule(fModuleAddress, x, y, z, sizex, sizey, sizez,
				 fMaxSectors, fSectorSizeX, fSectorSizeY, 
				 fPadSizeX, fPadSizeY);
	    }
	  }

// no layers in Jul10  
//	}

      }
    }
  }


  Int_t Nrmodules = (Int_t)fModuleMap.size();
  cout <<"Nr. of modules: "<<Nrmodules<<endl;

  fDigiPar->SetNrOfModules(Nrmodules);
  fDigiPar->SetMaxSectors(fMaxSectors);

  TArrayI *ModuleId  = new TArrayI(Nrmodules);
  
  Int_t iDigi=0; 
  for ( fModuleMapIt=fModuleMap.begin() ; fModuleMapIt != fModuleMap.end(); fModuleMapIt++ ){
    ModuleId->AddAt(fModuleMapIt->second->GetDetectorId(),iDigi);
    iDigi++;
  }  

  fDigiPar->SetModuleIdArray(*ModuleId);
  fDigiPar->SetModuleMap(fModuleMap);

}

void CbmTrdCreateDigiPar::FinishTask(){

  cout<<" * CbmTrdCreateDigiPar * :: FinishTask() "<<endl;

  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
             (rtdb->getContainer("CbmTrdDigiPar"));

  fDigiPar->print();
}

void CbmTrdCreateDigiPar::FillPadInfo(){
  
  // Reset the Array in case we have different sector sizes for
  // different detector modules

  for (Int_t i=0; i < fMaxSectors; i++ ) {
    fSectorSizeX.AddAt(0.,i);
    fSectorSizeY.AddAt(0.,i);
    fPadSizeX.AddAt(   0.,i);
    fPadSizeY.AddAt(   0.,i);
  }

  Int_t moduleType;

  //-----------------------------------------------------------------------
  // station 1
  // v11x:  8 + X +16 + X +26 +  X   -- number of detector sizes
  // v11x:  4 + 4 +12 + 4 + 6 + 20   -- number of pad plane types
  // v11x:  4   8  20  24  30   50   -- sum of modules for this station
  //
  // jul10: 4 + 4 + 4 + 8 + 6 + 30   -- number of module types
  // jul10: 4   8  12  20  26   56   -- sum of modules for this station

  if (fStation==1) {

    if (fModuleType==1) {
      if (fModuleCopy <= 4) {
	moduleType=0;
      } else {
	moduleType=1;
      }
    }
    
    if (fModuleType==2) {
      if (fModuleCopy <= 12) { //v11x
	moduleType=2;
      } else {
	moduleType=3;
      }
    }
    
    if (fModuleType==3) {
      if (fModuleCopy <= 6) {
	moduleType=4;
      } else {
	moduleType=5;
      }
    }

    for (Int_t i=0; i < fst1_sect_count; i++ ) {
      fSectorSizeX.AddAt(fst1_pad_type[moduleType][i][0],i);
      fSectorSizeY.AddAt(fst1_pad_type[moduleType][i][1],i);
      fPadSizeX.AddAt(   fst1_pad_type[moduleType][i][2],i);
      fPadSizeY.AddAt(   fst1_pad_type[moduleType][i][3],i);
    }
  }

  //-------------------------------------------------------------------------
  // station 2
  // v11x:  8 + X +16 + X +54 +  X   -- number of detector sizes
  // v11x:  4 + 4 +12 + 4 +54 +  X   -- number of pad plane types
  // v11x:  4   8  20  24  78    X   -- sum of modules for this station
  //
  // jul10: 4 + 4 + 4 + 8 + 6 + 74   -- number of module types
  // jul10: 4   8  12  20  26  100   -- sum of modules for this station
   
  if (fStation==2) {

    if (fModuleType==1) {
      if (fModuleCopy <= 4) {
	moduleType=0;
      } else {
	moduleType=1;
      }
    }

    if (fModuleType==2) {
      if (fModuleCopy <= 12) { //v11x
	moduleType=2;
      } else {
	moduleType=3;
      }
    }

    if (fModuleType==3) {
      if (fModuleCopy <= 6) {
 	moduleType=4;
      } else {
	moduleType=5;
      }
    }

    for (Int_t i=0; i < fst2_sect_count; i++ ) {
      fSectorSizeX.AddAt(fst2_pad_type[moduleType][i][0],i);
      fSectorSizeY.AddAt(fst2_pad_type[moduleType][i][1],i);
      fPadSizeX.AddAt(   fst2_pad_type[moduleType][i][2],i);
      fPadSizeY.AddAt(   fst2_pad_type[moduleType][i][3],i);
    }
  }
  //------------------------------------------------------------------------
  // station 3
  // v11x:  X + X + X + X +98 +  X   -- number of detector sizes
  // v11x:  X + X + X + X + 8 + 90   -- number of pad plane types
  // v11x:                  8   98   -- sum of modules for this station
  //
  // jul10: 8 + 136   -- number of module types
  // jul10: 8   144   -- sum of modules for this station
  
  if (fStation==3) {
    if (fModuleType==3) {
      if (fModuleCopy <= 8) {
 	moduleType=0;
      } else {
 	moduleType=1;
      }
    }

    for (Int_t i=0; i < fst3_sect_count; i++ ) {
      fSectorSizeX.AddAt(fst3_pad_type[moduleType][i][0],i);
      fSectorSizeY.AddAt(fst3_pad_type[moduleType][i][1],i);
      fPadSizeX.AddAt(   fst3_pad_type[moduleType][i][2],i);
      fPadSizeY.AddAt(   fst3_pad_type[moduleType][i][3],i);
    }
  }
  //-------------------------------------------------------------------------
}

ClassImp(CbmTrdCreateDigiPar)
