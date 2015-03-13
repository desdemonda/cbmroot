/******************************************************************************
 ** Creation of STS digitisation parameter file in ASCII format.
 **
 ** @file create_stsdigipar_v12.C
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since 21 June 2012
 ** @date 21.06.2012
 **
 ** This macro is compatible with geometries created with create_geometry_v12.C
 **
 ** The digitisation properties are the same for all sensors.
 ** The numbering of modules (sectors) is consecutively in each station,
 ** in the order given by the geometry. This has to be the same in 
 ** CbmStsDigiScheme::Init().
 *****************************************************************************/


#include <iostream>
#include <cstdio>
#include "TString.h"

using namespace std;


// -------------   Steering variables       -----------------------------------

// ---> Sensor digitisation type. 
// ---> 1 = pixel sensor, 2 = double-sided strip sensor,
// ---> 3 = double-sided strip sensor with double metal layer
Int_t    gkSensorType = 3;

// ---> Size of guard ring. It defines an inactive border on each side of the
// ---> sensor. MCPoints in this area will not be processed.
Double_t gkGuardRingWidth = 0.13;

// ---> Strip pitch on front and back side
Double_t gkStripPitchFront = 0.0058;
Double_t gkStripPitchBack  = 0.0058;

// ---> Strip stereo angle on front and back side
Double_t gkStereoFront =  0.;
Double_t gkStereoBack  =  7.5;

// ---> Geometry and digitisation tags
TString gkGeoTag = "v12b";
TString gkDigiTag = "std";
// ----------------------------------------------------------------------------



// -------------   Other global variables   -----------------------------------
// ---> TGeoManager (too lazy to write out 'Manager' all the time
TGeoManager*   geoMan           = NULL;  // will be set later
// ----------------------------------------------------------------------------




// ============================================================================
// ======                         Main function                           =====
// ============================================================================
void create_stsdigipar_v12(const char* geoTag  = gkGeoTag,
                           const char* digiTag = gkDigiTag)
{

  // ---> Open parameter file
  TString parFileName = "sts_";
  if ( digiTag != "")
    parFileName = parFileName + geoTag + "_" + digiTag + ".digi.par";
  else
    parFileName = parFileName + geoTag + ".digi.par";
  FILE* parFile;
  parFile = fopen(parFileName.Data(), "w");
  if ( ! parFile ) {
    cout << "-E- Could not open parameter " << parFileName << endl;
    exit(0);
  }
  fprintf(parFile, "%s\n", "[CbmStsDigiPar]");


  // ---> Read geometry from file
  TString geoPath = getenv("VMCWORKDIR");
  TString geoFileName = geoPath + "/geometry/sts/sts_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  Bool_t init = CreateGeoFromFile(geoFileName.Data());
  if ( ! init ) {
    cout << "-E- Could not read STS geometry!" << endl;
    exit(0);
  }
  TGeoManager* geoMan = gGeoManager;
      

  // ---> Top node
  geoMan->CdTop();
  TGeoNode* cave = geoMan->GetCurrentNode();
  cout << "Top node: " << cave->GetName() 
       << ", daughters: " << cave->GetNdaughters() << endl;


  // ---> STS
  geoMan->CdDown(0);
  TGeoNode* sts = geoMan->GetCurrentNode();
  if ( ! TString(sts->GetName()).Contains("STS") ) {
    cout << "-E- Illegal STS node name " << sts->GetName() << endl;
    exit(0);
  }
  cout << "STS node: " <<  sts->GetName() 
       << ", daughters: " << sts->GetNdaughters()
       << ", stations: " << GetNofDaughters(sts, "Station") << endl;


  // ---> Stations
  for (Int_t iCaveD = 0; iCaveD < sts->GetNdaughters(); iCaveD++) {
    geoMan->CdDown(iCaveD);
    TGeoNode* station = geoMan->GetCurrentNode();
    if ( ! TString(station->GetName()).Contains("Station") ) {
      geoMan->CdUp();
      continue;
    }
    Int_t statNr = station->GetNumber();
    Int_t nModules = GetNofModules(station);
    cout << "Station Nr. " << statNr << ", node " << station->GetName()
	 << ", modules: " << nModules << endl;
    fprintf(parFile, "%d   %d   %d\n", statNr, 0, nModules);
    Int_t moduleNr = 0;        // Running module number

    // ---> Ladders
    for (Int_t iStatD = 0; iStatD < station->GetNdaughters(); iStatD++) {
      geoMan->CdDown(iStatD);
      TGeoNode* ladder = geoMan->GetCurrentNode();

      // ---> Halfladders
      for (Int_t iLadD = 0; iLadD < ladder->GetNdaughters(); iLadD++) {
	geoMan->CdDown(iLadD);
	TGeoNode* hLadder = geoMan->GetCurrentNode();

	// ---> Modules
	for (Int_t iHladD = 0; iHladD < hLadder->GetNdaughters(); iHladD++) {
	  geoMan->CdDown(iHladD);
	  TGeoNode* module = geoMan->GetCurrentNode();
	  if ( ! TString(module->GetName()).Contains("Module") ) {
	    geoMan->CdUp();
	    continue;
	  }
	  moduleNr++;
	  fprintf(parFile, "%4d %4d\n", moduleNr, GetNofDaughters(module, "Sensor"));

	  // ---> Sensors
	  for (Int_t iModD = 0; iModD < module->GetNdaughters(); iModD++) {
	    geoMan->CdDown(iModD);
	    TGeoNode* sensor = geoMan->GetCurrentNode();
	    if ( ! TString(sensor->GetName()).Contains("Sensor") ) {
	      geoMan->CdUp();
	      continue;
	    }

	    // ---> Sensor number
	    Int_t sensNr = sensor->GetNumber();

	    // ---> Get sensor position
	    TGeoMatrix* matrix = geoMan->GetCurrentMatrix();
	    Double_t* xTrans = matrix->GetTranslation();
   
	    // ---> Get sensor angle. 
	    // Sensors in lower half of station will be read out to bottom.
	    Int_t phi = 0.;
	    if ( xTrans[1] < 0. ) phi = 180;

	    // ---> Get sensor size
	    TGeoBBox* shape = (TGeoBBox*) sensor->GetVolume()->GetShape();
	    Double_t dx = 2. * shape->GetDX();
	    Double_t dy = 2. * shape->GetDY();
	    Double_t dz = 2. * shape->GetDZ();

	    // ---> Subtract guard rings
	    dx -= 2 * gkGuardRingWidth;
	    dy -= 2 * gkGuardRingWidth;

	    // ---> Write sensor information
	    fprintf(parFile, 
		    "%6d %6d %9.3f %9.3f %9.3f %8d %8.2f %8.2f %8.2f %8.4f %8.4f %8.2f %8.2f\n",
		    sensNr, gkSensorType, xTrans[0], xTrans[1], xTrans[2], phi,
		    dx, dy, dz, gkStripPitchFront, gkStripPitchBack, 
		    gkStereoFront, gkStereoBack);


	    geoMan->CdUp();  // back to module
	  }                  // sensor loop
	  geoMan->CdUp();    // back to halfladder
	}                    // module loop
	geoMan->CdUp();      // back to ladder
      }                      // halfladder loop
      geoMan->CdUp();        // back to station
    }                        // ladder loop
    geoMan->CdUp();          // back to sts
  }                          // station loop               
	  


  delete geoMan;
  fclose(parFile);
  cout << "-I- Parameters successfully written to " << parFileName << endl;
}
// ============================================================================
// ======                   End of main function                          =====
// ============================================================================



/** ===========================================================================
 ** CreateGeoFromFile
 **
 ** Create geometry, read STS node from file and add it to geometry.
 **/
Bool_t CreateGeoFromFile(const char* fileName) {

  // ---> Create TGeoManager and top node (cave)
  geoMan = new TGeoManager("CBM STS", "STS Geometry");
  TGeoVolume* cave = geoMan->MakeBox("Cave", NULL, 2000., 2000., 2000.);
  geoMan->SetTopVolume(cave);
  
  // ---> Open geometry file
  cout << "Reading geometry from file " << fileName << endl;
  TFile* geoFile = new TFile(fileName, "READ");
  if ( ! geoFile->IsOpen() ) {
    cout << "-E- Could not open file!" << endl;
    return kFALSE;
  }

  // ---> Get STS node from file
  TList* keyList = geoFile->GetListOfKeys();
  TIter next(keyList);
  TKey* key = NULL;
  TGeoVolume* top = NULL;
  while ( (key = (TKey*)next() ) ) {
    TString className(key->GetClassName());
    if ( className.BeginsWith("TGeoVolume") ) {
      top = dynamic_cast<TGeoVolume*> (key->ReadObj() );
      std::cout << "Found volume " << top->GetName() << endl;
      break;
    }
  }
  if ( ! top ) {
    cout << "-E- Could not find volume object in file" << endl;
    return kFALSE;
  }
  TGeoVolume* sts = top->GetNode(0)->GetVolume();
  TString stsName = sts->GetName();
  if ( ! stsName.BeginsWith("STS") ) {
    cout << "-E- No STS volume found on second level" << endl;
    return kFALSE;
  }
  TString stsVersion = stsName;
  stsVersion.Remove(0, 4);
  cout << "Geometry version " << stsVersion << endl;
  top->GetNode(0)->InspectNode();
  
  // ---> Add STS node to cave
  TGeoMatrix* stsmatrix = top->GetNode(0)->GetMatrix();
  cave->AddNode(sts, 0, stsmatrix);
  
  // ---> Finish geometry and close geometry file
  geoMan->CloseGeometry();
  geoFile->Close();

  return kTRUE;  
}
/** ======================================================================= **/



Int_t GetNofModules(TGeoNode* station) {

  Int_t nModules = 0;

  // --- Ladder
  Int_t nLadders = station->GetNdaughters();
  for (Int_t iLadder = 0; iLadder < nLadders; iLadder++) {
    TGeoNode* ladder = station->GetDaughter(iLadder);

    // --- Halfladder
    Int_t nHladders = ladder->GetNdaughters();
    for (Int_t iHladder = 0; iHladder < nHladders; iHladder++) {
      TGeoNode* hladder = ladder->GetDaughter(iHladder);

      // --- Modules
      for (Int_t iModule = 0; iModule < hladder->GetNdaughters(); iModule++) {
	TGeoNode* module = hladder->GetDaughter(iModule);
	if ( TString(module->GetName()).Contains("Module") ) nModules++;
      }
    }
  }

  return nModules;
}
					    

Int_t GetNofDaughters(TGeoNode* node, const char* name) {
  Int_t nDaughters = 0;
  for (Int_t iNode = 0; iNode < node->GetNdaughters(); iNode++)
    if ( TString(node->GetDaughter(iNode)->GetName()).Contains(name) )
      nDaughters++;
  return nDaughters;
}
