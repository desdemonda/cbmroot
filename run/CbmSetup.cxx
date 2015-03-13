/** @file CbmSetup.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 13.06.2013
 **/


#include <iomanip>
#include <sstream>
#include "TFile.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TKey.h"
#include "TSystem.h"
#include "FairLogger.h"
#include "FairModule.h"
#include "FairRunSim.h"
#include "CbmAddress.h"
#include "CbmModuleList.h"
#include "CbmSetup.h"
#include "CbmFieldMap.h"
#include "CbmFieldMapSym2.h"
#include "CbmFieldMapSym3.h"

#include "CbmMvd.h"
#include "CbmStsMC.h"
#include "CbmRich.h"
#include "CbmMuch.h"
#include "CbmTrd.h"
#include "CbmTof.h"
#include "CbmEcal.h"
#include "CbmPsd.h"
#include "CbmMagnet.h"
#include "CbmTarget.h"
#include "CbmPipe.h"
#include "CbmFieldMapData.h"



// -----   Instantiate module   --------------------------------------------
FairModule* CbmSetup::CreateModule(Int_t moduleId, Bool_t active) {

  switch (moduleId) {

    // --- Detector systems
    case kMvd:  return new CbmMvd( "MVD",  active); break;
    case kSts:  return new CbmStsMC(active); break;
    case kRich: return new CbmRich("RICH", active); break;
    case kMuch: return new CbmMuch("MUCH", active); break;
    case kTrd:  return new CbmTrd( "TRD",  active); break;
    case kTof:  return new CbmTof( "TOF",  active); break;
    case kEcal: return new CbmEcal("ECAL", active); break;
    case kPsd:  return new CbmPsd( "PSD",  active); break;

    // --- Passive modules
    case kMagnet: return new CbmMagnet("MAGNET"); break;
    case kTarget: return new CbmTarget("TARGET"); break;
    case kPipe:   return new CbmPipe("PIPE");     break;

    // --- Default
    default: return NULL; break;

  }

  return NULL;
}
// -------------------------------------------------------------------------


// -----   Get field map type   --------------------------------------------
CbmFieldMap* CbmSetup::CreateFieldMap() {


  // --- Open the map file
  TString mapName = "field_";
  mapName += fFieldTag;
  TString fileName = gSystem->Getenv("VMCWORKDIR");
  fileName = fileName + "/input/" + mapName + ".root";
  TFile mapFile(fileName);
  if ( ! mapFile.IsOpen() ) {
    LOG(ERROR) << "Could not open field map file " << fileName
               << FairLogger::endl;
    return NULL;
  }

  // ---   Get map file type
  CbmFieldMapData* data = NULL;
  mapFile.GetObject(mapName, data);
  if ( ! data ) {
    LOG(ERROR) << "Could not find CbmFieldMapData object " << mapName << " in file "
               << fileName << FairLogger::endl;
    return NULL;
  }
  Int_t fieldType = data->GetType();


  // --- Instantiate field
  CbmFieldMap* field = NULL;
  switch (fieldType) {
    case 2: field = new CbmFieldMapSym2(mapName); break;
    case 3: field = new CbmFieldMapSym3(mapName); break;
    default: LOG(ERROR) << "Unknown field type " << fieldType
                        << FairLogger::endl; break;
  }

  return field;
}
// -------------------------------------------------------------------------



// -----   Get the geometry file name and path   ---------------------------
Bool_t CbmSetup::GetFile(Int_t moduleId, TString& fileName) {

  // --- Check whether module is present in setup
  if ( fGeoTags.find(moduleId) == fGeoTags.end() ) return kFALSE;

  // --- Module name
  TString moduleName = CbmModuleList::GetModuleName(moduleId);

  // --- Path to geometry file
  TString path = gSystem->Getenv("VMCWORKDIR");
  path += "/geometry/";
  TString dir = moduleName;
  if ( moduleId >= kNofSystems ) dir = "passive";

  // --- First try with ROOT file
  fileName = dir + "/" + moduleName + "_" + fGeoTags[moduleId]
           + ".root";
  TString fullPath = path + fileName;
  LOG(DEBUG1) << "Trying " << fullPath << FairLogger::endl;
  FileStat_t dummy;
  if ( ! gSystem->GetPathInfo(fullPath.Data(), dummy)) {
    LOG(DEBUG) << "Found " << fullPath << FairLogger::endl;
    return kTRUE;
  }

  // --- No ROOT file? Maybe ASCII is there...
  fileName = dir + "/" + moduleName + "_" + fGeoTags[moduleId]
           + ".geo";
  fullPath = path + fileName;
  LOG(DEBUG1) << "Trying " << fullPath << FairLogger::endl;
  if ( ! gSystem->GetPathInfo(fullPath.Data(), dummy)) {
    LOG(DEBUG) << "Found " << fullPath << FairLogger::endl;
    return kTRUE;
  }

  // --- Neither ROOT nor ASCII.
  LOG(DEBUG) << "No geometry file for " << moduleName << " found"
            << FairLogger::endl;
  return kFALSE;
}
// -------------------------------------------------------------------------



// ----- Get the position of the magnet centre   ---------------------------
Bool_t CbmSetup::GetMagnetPosition(const char* geoTag,
                                   Double_t& x, Double_t& y, Double_t& z) {

  TString fileName = gSystem->Getenv("VMCWORKDIR");
  fileName = fileName + "/geometry/passive/magnet_" + geoTag + ".root";

  TFile file(fileName.Data());
  if ( ! file.IsOpen() ) {
    LOG(ERROR) << "Cannot open magnet geometry file " << fileName
               << FairLogger::endl;
    return kFALSE;
  }

  TList* list = file.GetListOfKeys();
  TKey* key = (TKey*)list->At(0);
  TGeoVolume* topVol = dynamic_cast<TGeoVolume*> (key->ReadObj() );
  if ( ! topVol ) {
    LOG(ERROR) << "No top volume in file " << fileName
               << FairLogger::endl;
    return kFALSE;
  }
  TGeoNode* magnetNode = topVol->GetNode(0);
  if ( ! magnetNode ) {
    LOG(ERROR) << "No magnet node in file " << fileName
               << FairLogger::endl;
    return kFALSE;
  }

  TGeoMatrix* matrix = magnetNode->GetMatrix();
  const Double_t* pos = matrix->GetTranslation();
  LOG(INFO) << "Magnet position is (" << pos[0] << ", " << pos[1]
            << ", " << pos[2] << ") cm" << FairLogger::endl;

  x = pos[0];
  y = pos[1];
  z = pos[2];

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Intialisation    ------------------------------------------------
void CbmSetup::Init(FairRunSim* run) {

  // --- Create and register modules with proper geometries
  map<Int_t, TString>::iterator it;
  for (it = fGeoTags.begin(); it != fGeoTags.end(); it++) {

    TString fileName;
    if ( GetFile(it->first, fileName) ) {

      LOG(INFO) << "Registering "
                << setw(8) << CbmModuleList::GetModuleName(it->first);
      if ( fActive[it->first] ) LOG(INFO) << "  **ACTIVE**  ";
      else                      LOG(INFO) << "  -Inactive-  ";
      LOG(INFO) << fileName << FairLogger::endl;

      if ( run ) {
        FairModule* module = CreateModule(it->first, fActive[it->first]);
        module->SetGeometryFileName(fileName);
        run->AddModule(module);
      }

    }  // Geometry file name found

    else LOG(ERROR) << "Could not register "
                    << CbmModuleList::GetModuleName(it->first)
                    << " with tag " << it->second << FairLogger::endl;

  }  // Module loop

  Double_t x = 0.;
  Double_t y = 0.;
  Double_t z = 0.;
  GetMagnetPosition(fGeoTags[kMagnet], x, y, z);

  CbmFieldMap* fieldMap = CreateFieldMap();
  fieldMap->SetScale(fFieldScale);
  fieldMap->SetPosition(x, y, z);
  LOG(INFO) << "Registering field " << fFieldTag << ", scale "
            << fFieldScale << ", position (" << x << ", " << y << ", "
            << z << ") cm" << FairLogger::endl;
  if ( run ) run->SetField(fieldMap);


}
// -------------------------------------------------------------------------



// -----   Print setup   ---------------------------------------------------
void CbmSetup::Print(Option_t* opt) {
  map<Int_t, TString>::iterator it;
  LOG(INFO) << "CBM setup: " << FairLogger::endl;
  for ( it = fGeoTags.begin(); it != fGeoTags.end(); it++ ) {
    LOG(INFO) << setw(8) << CbmModuleList::GetModuleName(it->first) << "  "
              << it->second;
    if ( fActive[it->first] ) LOG(INFO) << "  *ACTIVE* ";
    LOG(INFO) << FairLogger::endl;
  }
}
// -------------------------------------------------------------------------



// -----   Read setup   ----------------------------------------------------
void CbmSetup::ReadSetup(const char* setupName) {

  // --- Open setup file
  TString setupFile = gSystem->Getenv("VMCWORKDIR");
  setupFile = setupFile  + "/geometry/setup/" + setupName + ".setup.txt";
  LOG(DEBUG) << "Setup file is " << setupFile << FairLogger::endl;
  ifstream file(setupFile.Data());
  if ( ! file.is_open() ) {
    LOG(ERROR) << "Setup file " << setupFile << " could not be opened!"
               << FairLogger::endl;
    return;
  }

  // --- Parse file line by line
  std::string line;
  while ( kTRUE ) {
    getline(file, line);
    if ( file.eof() ) break;

    // --- Discard empty lines or comment lines
    if ( ( ! line.size() ) || line[0] == '#' ) continue;

    // --- Decompose line into substrings (separated by blanks)
    stringstream ss(line);
    string module;
    string geoTag;
    ss >> module;
    ss >> geoTag;
    if ( geoTag[0] != 'v' ) {
      LOG(ERROR) << "Illegal geometry tag " << geoTag << " for module "
                 << module << FairLogger::endl;
      continue;
    }

    // --- Get module Id
    Int_t moduleId = CbmModuleList::GetModuleId(module.data());
    if ( moduleId < 0 ) {
      LOG(ERROR) << "Unknown module name" << module << FairLogger::endl;
      continue;
    }

    // --- Add module to setup
    SetModule(moduleId, geoTag.data());

  }    // Next line in file

}
// -------------------------------------------------------------------------



// -----   Remove a module   -----------------------------------------------
void CbmSetup::RemoveModule(Int_t moduleId) {

  // Check validity of module Id
  if ( moduleId < 0 || moduleId > kNofSystems ) {
    if ( moduleId < kMagnet || moduleId > kTarget ) {
      LOG(ERROR) << "Illegal module Id " << moduleId << FairLogger::endl;
      return;
    }
  }

  // Give debug output if module does not exist in setup
  if ( fGeoTags.find(moduleId) == fGeoTags.end() ) {
    LOG(WARNING) << "Module " << moduleId << " does not exist in setup!";
    return;
  }

  fGeoTags.erase(moduleId);
  fActive.erase(moduleId);

}
// -------------------------------------------------------------------------



// -----   Activate or deactivate a detector   -----------------------------
void CbmSetup::SetActive(Int_t moduleId, Bool_t active) {

  // Check validity of module Id
  if ( moduleId < 0 || moduleId > kNofSystems ) {
    if ( moduleId < kMagnet || moduleId > kTarget ) {
      LOG(ERROR) << "Illegal module Id " << moduleId << FairLogger::endl;
      return;
    }
  }

  // Check presence of module in current setup
  if ( fGeoTags.find(moduleId) == fGeoTags.end() ) {
    LOG(WARNING) << "Module " << moduleId << " does not exist in setup!";
    return;
  }

  // Set activity flag
  fActive[moduleId] = active;

}
// -------------------------------------------------------------------------



// -----   Set the field map   ---------------------------------------------
void CbmSetup::SetField(const char* tag, Double_t scale) {

  if ( fGeoTags.find(kMagnet) != fGeoTags.end() ) {
    LOG(WARNING) << "Overriding field map  "
                 << fGeoTags.find(kMagnet)->second
                 << " (according to magnet geometry) with field map "
                 << tag << FairLogger::endl;
  }

  fFieldTag = tag;
  fFieldScale = scale;

}
// -------------------------------------------------------------------------



// -----   Add or replace a module in the setup   --------------------------
void CbmSetup::SetModule(Int_t moduleId, const char* geoTag,
                         Bool_t active) {

  // Check validity of module Id
  if ( moduleId < 0 || moduleId > kNofSystems ) {
    if ( moduleId < kMagnet || moduleId > kTarget ) {
      LOG(ERROR) << "Illegal module Id " << moduleId << FairLogger::endl;
      return;
    }
  }

  // Check presence of module in current setup
  if ( fGeoTags.find(moduleId) != fGeoTags.end() )
    LOG(DEBUG) << "Changing module " << moduleId << ": "
               << fGeoTags.find(moduleId)->second << " -> " << geoTag
               << FairLogger::endl;

  // Set tags
  fGeoTags[moduleId] = geoTag;
  if ( moduleId > kNofSystems ) fActive[moduleId] = kFALSE;
  else                          fActive[moduleId] = active;

  // In case of magnet, set field tag accordingly
  if ( moduleId == kMagnet ) {
    fFieldTag   = geoTag;
    fFieldScale = 1.;
  }

}
// -------------------------------------------------------------------------


