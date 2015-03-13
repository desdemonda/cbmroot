/**
 * \file CbmTrdGeoHandler.cxx
 * \author Florian Uhlig <f.uhlig@gsi.de>
 */

#include "CbmTrdGeoHandler.h"
#include "CbmTrdAddress.h"

#include "FairLogger.h"

#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TVirtualMC.h"

#include <string>
#include <cstdlib>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::atoi;

CbmTrdGeoHandler::CbmTrdGeoHandler() 
  : TObject(),
    fIsSimulation(kFALSE),
    fGeoPathHash(0),
    fCurrentVolume(NULL),
    fVolumeShape(NULL),    
    fGlobal(),    
    fGlobalMatrix(NULL),
    fLayerId(0),
    fModuleId(0),
    fModuleType(0),
    fRotation(0),
    fStation(0),
    fLayer(0),
    fModuleCopy(0)
{
}

CbmTrdGeoHandler::~CbmTrdGeoHandler()
{

}

void CbmTrdGeoHandler::Init(Bool_t isSimulation)
{
   fIsSimulation = isSimulation;
}

Int_t CbmTrdGeoHandler::GetModuleAddress()
{

  // In the simulation we can not rely on the TGeoManager information
  // In the simulation we get the correct information only from gMC 
  Int_t copyNr;
  if(fIsSimulation) { 
    LOG(DEBUG4) << gMC->CurrentVolPath() << FairLogger::endl;
    // get the copy number of the mother volume (1 volume up in hierarchy) 
    gMC->CurrentVolOffID(1, copyNr); 
  } else {
    LOG(DEBUG4) << gGeoManager->GetPath() << FairLogger::endl;
    // We take the mother node (module) of the current node we are in (gas).
    TGeoNode* node = gGeoManager->GetMother();
    // Get the module copy number to get the information about layerId and moduleId.
    copyNr = node->GetNumber();
  }
  LOG(DEBUG4) << "CopyNr: " << copyNr << FairLogger::endl;

  Int_t layerId  = 0;
  Int_t moduleId = 0;

  if ((copyNr / 100000000) > 0)  // has 9 digits => 2014 format
  {
    // In TGeoManager numbering starts with 1, so we have to subtract 1.
    layerId = ((copyNr / 1000) % 100) - 1;
    moduleId = (copyNr % 1000) - 1;
    LOG(DEBUG4) << "2014 ";
  }
  else // 2013 and earlier
  {
    // In TGeoManager numbering starts with 1, so we have to subtract 1.
    layerId = ((copyNr / 100) % 100) - 1;
    moduleId = (copyNr % 100) - 1;
    LOG(DEBUG4) << "2013 ";
  }

  LOG(DEBUG4) << copyNr / 100000000 << " copy " << copyNr << " layerID " << layerId << " moduleId " << moduleId  << FairLogger::endl;
  // Form the module address.
  return CbmTrdAddress::GetAddress(layerId, moduleId, 0, 0, 0);
}

Int_t CbmTrdGeoHandler::GetModuleAddress(const TString& path)
{
  if (fGeoPathHash != path.Hash()) {
    NavigateTo(path);
  }
  return GetModuleAddress();
}

//Int_t CbmTrdGeoHandler::GetModuleOrientation()
//{
//  // We take the mother node (module) of the current node we are in (gas).
//  TGeoNode* modulenode = gGeoManager->GetMother();
//  // Get the module copy number to get the information about layerId and moduleId.
//  Int_t modulecopyNr = modulenode->GetNumber();
//  // In TGeoManager numbering starts with 1, so we have to subtract 1.
//  fRotation  = ((modulecopyNr /  10000) %  10);  // from module copy number
//  //  std::cout << "fRotation: " << modulecopyNr << " " << fRotation << std::endl;
//  return fRotation;
//}

Int_t CbmTrdGeoHandler::GetModuleOrientation(
      const TString& path)
{
  if (fGeoPathHash != path.Hash()) {
    NavigateTo(path);
  }
  return fRotation;
}

Double_t CbmTrdGeoHandler::GetSizeX(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDX();
}

Double_t CbmTrdGeoHandler::GetSizeY(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDY();
}

Double_t CbmTrdGeoHandler::GetSizeZ(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDZ();
}

Double_t CbmTrdGeoHandler::GetZ(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[2];
}

Double_t CbmTrdGeoHandler::GetY(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[1];
}

Double_t CbmTrdGeoHandler::GetX(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[0];
}

Int_t CbmTrdGeoHandler::GetModuleType(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fModuleType;
}

Int_t CbmTrdGeoHandler::GetStation(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fStation;
}

Int_t CbmTrdGeoHandler::GetLayer(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
     //      cout << "path : " << path.Data() << endl;
      NavigateTo(path);
   }
   return fLayer;
}

Int_t CbmTrdGeoHandler::GetModuleCopyNr(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fModuleCopy;
}

void CbmTrdGeoHandler::NavigateTo(
      const TString& path)
{
  //   cout << "path : " << path.Data() << endl;
   if (fIsSimulation) {
      LOG(FATAL) << "This method is not supported in simulation mode" << FairLogger::endl;
   } else {
      gGeoManager->cd(path.Data());
      fGeoPathHash = path.Hash();
      fCurrentVolume = gGeoManager->GetCurrentVolume();
      fVolumeShape = (TGeoBBox*)fCurrentVolume->GetShape();
      Double_t local[3] = {0., 0., 0.};  // Local center of volume
      gGeoManager->LocalToMaster(local, fGlobal);
      fGlobalMatrix = gGeoManager->GetCurrentMatrix();
      // Get module type information which is decoded in copy number.
      const char* moduleName = gGeoManager->GetMother()->GetName();
      fModuleType = std::atoi(string(1, *(moduleName + 6)).c_str()); // 6th element module type

      // We take the mother of the mother node (layer) of the current node we are in (gas).
      TGeoNode* layernode = gGeoManager->GetMother(2);   // get layer
      Int_t layercopyNr = layernode->GetNumber();
      // fIsRotated is the 4th digit from the back
      fStation   = ((layercopyNr / 10000) % 10);  // from layer copy number
      //      std::cout << "DE fStation: " << fStation << " " << std::endl;
      fLayer     = ((layercopyNr /   100) % 10);  // from layer copy number

      // We take the mother node (module) of the current node we are in (gas).
      TGeoNode* modulenode = gGeoManager->GetMother();
      // Get the module copy number to get the information about layerId and moduleId.
      Int_t modulecopyNr = modulenode->GetNumber();

      if ((modulecopyNr / 100000000) > 0)  // has 9 digits => 2014 format
      {
        // In TGeoManager numbering starts with 1, so we have to subtract 1.
        fModuleCopy= ((modulecopyNr / 1000000) % 100);  // from module copy number
        fRotation  = ((modulecopyNr /  100000) %  10);  // from module copy number
      }
      else // 2013 and earlier
      {
        // In TGeoManager numbering starts with 1, so we have to subtract 1.
        fModuleCopy= ((modulecopyNr / 100000) % 100);  // from module copy number
        fRotation  = ((modulecopyNr /  10000) %  10);  // from module copy number
      }

      //      std::cout << "fRotation: " << modulecopyNr << " " << fRotation << std::endl;
      //      fLayerId   = ((modulecopyNr /    100) % 100) - 1;
      //      fModuleId  = ((modulecopyNr /      1) % 100) - 1;
   }
}
 
ClassImp(CbmTrdGeoHandler)
