/**
 * \file CbmTrdCreateDigiPar.cxx
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \date 06/06/2008
 */

#include "CbmTrdCreateDigiPar.h"

#include "CbmTrdAddress.h"
#include "CbmTrdPads.h"
#include "CbmTrdPadsMap.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"
#include "CbmTrdDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairLogger.h"

CbmTrdCreateDigiPar::CbmTrdCreateDigiPar()
  :FairTask("TrdCreateDigiPar"),
   fMaxSectors(0),
   fModuleMap(), 
   fDigiPar(NULL),
   fGeoHandler(new CbmTrdGeoHandler())
{
   // Get the maximum number of sectors. All arrays will have this number of entries.
   fMaxSectors = fst1_sect_count;
}

CbmTrdCreateDigiPar::~CbmTrdCreateDigiPar()
{
}

void CbmTrdCreateDigiPar::SetParContainers()
{
   FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
   fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdCreateDigiPar::Init()
{
   FillModuleMap();
   FillDigiPar();
   return kSUCCESS;
}

void CbmTrdCreateDigiPar::Finish()
{
   FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
   fDigiPar = (CbmTrdDigiPar*) (rtdb->getContainer("CbmTrdDigiPar"));
   fDigiPar->print();
}

void CbmTrdCreateDigiPar::Exec(
      Option_t * option)
{
}

void CbmTrdCreateDigiPar::FillModuleMap()
{
   // The geometry structure is treelike with cave as
   // the top node. For the TRD there are keeping volume
   // trd_vXXy_1 which is only container for the different layers.
   // The trd layer is again only a container for all volumes of this layer.
   // Loop over all nodes below the top node (cave). If one of
   // the nodes contains a string trd it must be TRD detector.
   // Now loop over the layers and
   // then over all modules of the layer to extract in the end
   // all active regions (gas) of the complete TRD. For each
   // of the gas volumes get the information about size and
   // position from the geomanager and the sizes of the sectors
   // and pads from the definitions in CbmTrdPads. This info
   // is then stored in a CbmTrdModule object for each of the
   // TRD modules.

   TGeoNode* topNode = gGeoManager->GetTopNode();
   TObjArray* nodes = topNode->GetNodes();
   for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
      TGeoNode* node = static_cast<TGeoNode*>(nodes->At(iNode));
      if (!TString(node->GetName()).Contains("trd")) continue; // trd_vXXy top node, e.g. trd_v13a, trd_v14b
      TGeoNode* station = node;

      TObjArray* layers = station->GetNodes();
      for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {
         TGeoNode* layer = static_cast<TGeoNode*>(layers->At(iLayer));
         if (!TString(layer->GetName()).Contains("layer")) continue; // only layers

         TObjArray* modules = layer->GetNodes();
         for (Int_t iModule = 0; iModule < modules->GetEntriesFast(); iModule++) {
            TGeoNode* module = static_cast<TGeoNode*>(modules->At(iModule));

            TObjArray* parts = module->GetNodes();
            for (Int_t iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
               TGeoNode* part = static_cast<TGeoNode*>(parts->At(iPart));
               if (!TString(part->GetName()).Contains("gas")) continue; // only active gas volume

               // Put together the full path to the interesting volume, which
               // is needed to navigate with the geomanager to this volume.
               // Extract the geometry information (size, global position)
               // from this volume.
               TString path = TString("/") + topNode->GetName() + "/" + station->GetName() + "/"
                  + layer->GetName() + "/" + module->GetName() + "/" + part->GetName();

               CreateModule(path);
            }
         }
      }
   }
}

void CbmTrdCreateDigiPar::CreateModule(
      const TString& path)
{
   Int_t moduleAddress = fGeoHandler->GetModuleAddress(path);
   Int_t orientation   = fGeoHandler->GetModuleOrientation(path);

   Double_t sizeX = fGeoHandler->GetSizeX(path);
   Double_t sizeY = fGeoHandler->GetSizeY(path);
   Double_t sizeZ = fGeoHandler->GetSizeZ(path);
   Double_t x = fGeoHandler->GetX(path);
   Double_t y = fGeoHandler->GetY(path);
   Double_t z = fGeoHandler->GetZ(path);

   TArrayD sectorSizeX(fMaxSectors);
   TArrayD sectorSizeY(fMaxSectors);
   TArrayD padSizeX(fMaxSectors);
   TArrayD padSizeY(fMaxSectors);
   Int_t moduleType = fGeoHandler->GetModuleType(path);
   for (Int_t i = 0; i < fst1_sect_count; i++) {
      sectorSizeX.AddAt(fst1_pad_type[moduleType - 1][i][0], i);
      sectorSizeY.AddAt(fst1_pad_type[moduleType - 1][i][1], i);
      padSizeX.AddAt(fst1_pad_type[moduleType - 1][i][2], i);
      padSizeY.AddAt(fst1_pad_type[moduleType - 1][i][3], i);
   }

//   // Orientation of the detector layers
//   // Odd  layers (1,3,5..) have resolution in x-direction (isRotated == 0) - vertical pads
//   // Even layers (2,4,6..) have resolution in y-direction (isRotated == 1) - horizontal pads
//   Int_t layerNr = CbmTrdAddress::GetLayerId(moduleAddress) + 1;
//   Int_t isRotated = fGeoHandler->GetPadOrientation(path);
//   //   printf("layer %02d %d isRotated\n", layerNr, isRotated);   // check, if even layers are isRotated == 1
//   if( (isRotated%2) == 1 ) {  // flip pads for even layers
//      Double_t copybuf;
//      for (Int_t i = 0; i < fMaxSectors; i++) {
//         copybuf = padSizeX.At(i);
//         padSizeX.AddAt(padSizeY.At(i), i);
//         padSizeY.AddAt(copybuf, i);
//         copybuf = sectorSizeX.At(i);
//         sectorSizeX.AddAt(sectorSizeY.At(i), i);
//         sectorSizeY.AddAt(copybuf, i);
//      }
//   }

   // Create new CbmTrdModule and add it to the map
   fModuleMap[moduleAddress] = new CbmTrdModule(moduleAddress, orientation, x, y, z,
         sizeX, sizeY, sizeZ, fMaxSectors, sectorSizeX, sectorSizeY, padSizeX, padSizeY);
}

void CbmTrdCreateDigiPar::FillDigiPar()
{
   Int_t nofModules = fModuleMap.size();
   fDigiPar->SetNrOfModules(nofModules);
   fDigiPar->SetMaxSectors(fMaxSectors);

   TArrayI moduleId(nofModules);
   Int_t iModule = 0;
   std::map<Int_t, CbmTrdModule*>::iterator it;
   for (it = fModuleMap.begin() ; it != fModuleMap.end(); it++) {
      moduleId.AddAt(it->second->GetModuleAddress(), iModule);
      iModule++;
   }

   fDigiPar->SetModuleIdArray(moduleId);
   fDigiPar->SetModuleMap(fModuleMap);
}

ClassImp(CbmTrdCreateDigiPar)
