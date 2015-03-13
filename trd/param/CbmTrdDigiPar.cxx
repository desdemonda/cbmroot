// -------------------------------------------------------------------------
// -----                CbmTrdDigiPar source file                      -----
// -----                Created 05/06/08  by F. Uhlig                  -----
// -------------------------------------------------------------------------

#include "CbmTrdDigiPar.h"
#include "CbmTrdModule.h"

#include "FairParamList.h"
#include "FairDetParIo.h"
#include "FairParIo.h"
#include "FairLogger.h"

#include "TString.h"

// -----   Standard constructor   ------------------------------------------
CbmTrdDigiPar::CbmTrdDigiPar(const char* name, 
			     const char* title,
			     const char* context)
  : FairParGenericSet(name, title, context), 
    fModuleMap(),
    fModuleMapIt(),
    fModuleIdArray(),
    fNrOfModules(-1),
    fMaxSectors(-1)
{
  detName="Trd";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdDigiPar::~CbmTrdDigiPar() 
{
  FairLogger *logger = FairLogger::GetLogger();
  logger->Debug4(MESSAGE_ORIGIN, "Enter CbmTrdDigiPar::~CbmTrdDigiPar");
  for (fModuleMapIt = fModuleMap.begin(); fModuleMapIt != fModuleMap.end(); 
       ++fModuleMapIt) {
    delete fModuleMapIt->second;
  }      
  fModuleMap.clear();
  clear();
  logger->Debug4(MESSAGE_ORIGIN, "Leave CbmTrdDigiPar::~CbmTrdDigiPar");
}
// -------------------------------------------------------------------------


// -----   Public method clear   -------------------------------------------
void CbmTrdDigiPar::clear() 
{
  status = kFALSE;
  resetInputVersions();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------

void CbmTrdDigiPar::putParams(FairParamList* l) 
{

  if (!l) return;

   l->add("NrOfModules",   fNrOfModules);
   l->add("MaxSectors",    fMaxSectors);
   l->add("ModuleIdArray", fModuleIdArray);

   // Instead of a fixed number of values the number of values to
   // store now depends on the maximum number of sectors per module
   // The first six parameters are for the complete module. The rest
   // of the parameters depend on the number of sectors.
   // The parametrs are:
   // X, Y, Z             : position of the middle of the gaslayer.
   // SizeX, SizeY, SizeZ : size of the gaslayer. The values are only
   //                       the half size which are the values returned
   //                       by geant.
   // SectorSizeX(Y)      : size of a sector
   // PadSizeX(Y)         : size of the pads in this sector

   Int_t nrValues = 7 + ( fMaxSectors * 4 );
   TArrayD values(nrValues);
 
   for (Int_t i=0; i < fNrOfModules; i++){
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetOrientation(),0);         
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetX(),1);         
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetY(),2);          
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetZ(),3);          
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizeX(),4);      
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizeY(),5); 
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizeZ(),6); 
     for (Int_t j=0; j < fMaxSectors; j++){       
       Int_t k=7+(j*4);
       values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSectorSizeX(j),k+0);   
       values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSectorSizeY(j),k+1);   
       values.AddAt(fModuleMap[fModuleIdArray[i]]->GetPadSizeX(j),k+2);   
       values.AddAt(fModuleMap[fModuleIdArray[i]]->GetPadSizeY(j),k+3);   
     }
     TString text;
     text += fModuleIdArray[i];
     l->add(text.Data(), values);
   }

}

//------------------------------------------------------

Bool_t CbmTrdDigiPar::getParams(FairParamList* l) {

  if (!l) return kFALSE;
  
  if ( ! l->fill("NrOfModules", &fNrOfModules) ) return kFALSE;
  if ( ! l->fill("MaxSectors", &fMaxSectors) ) return kFALSE;
  
  fModuleIdArray.Set(fNrOfModules);
  if ( ! l->fill("ModuleIdArray", &fModuleIdArray) ) return kFALSE;
  
  // Instead of a fixed number of values the number of values to
  // store now depends on the maximum number of sectors per module
  Int_t nrValues = 7 + ( fMaxSectors * 4 );
  TArrayD *values = new TArrayD(nrValues);
  TArrayD sectorSizeX(fMaxSectors);
  TArrayD sectorSizeY(fMaxSectors);
  TArrayD padSizeX(fMaxSectors);
  TArrayD padSizeY(fMaxSectors);
  Int_t orientation;
  Double_t x;
  Double_t y;
  Double_t z;
  Double_t sizex;
  Double_t sizey;
  Double_t sizez;

  TString text;
  for (Int_t i=0; i < fNrOfModules; i++){
    text="";
    text += fModuleIdArray[i];
    if ( ! l->fill(text.Data(), values) ) return kFALSE;
    Int_t VolumeID = text.Atoi();
    orientation=values->At(0);
    x=values->At(1);
    y=values->At(2);
    z=values->At(3);
    sizex= values->At(4);
    sizey= values->At(5);
    sizez= values->At(6);
    for (Int_t j=0; j < fMaxSectors; j++){       
      Int_t k=7+(j*4);
      sectorSizeX.AddAt(values->At(k+0),j);
      sectorSizeY.AddAt(values->At(k+1),j);
      padSizeX.AddAt(values->At(k+2),j);
      padSizeY.AddAt(values->At(k+3),j);
    }

    fModuleMap[VolumeID] = new CbmTrdModule(VolumeID, orientation, x, y, z,
					    sizex, sizey, sizez, fMaxSectors,
					    sectorSizeX, sectorSizeY,
					    padSizeX, padSizeY);
  }

  delete values;
  return kTRUE;
}


ClassImp(CbmTrdDigiPar)
