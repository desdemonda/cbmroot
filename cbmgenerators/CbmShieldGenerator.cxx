// -------------------------------------------------------------------------
// -----                CbmShieldGenerator source file                 -----
// -----                Created 15/09/06  by V. Friese                 -----
// -------------------------------------------------------------------------
#include "CbmShieldGenerator.h"

#include "FairPrimaryGenerator.h"
#include "CbmMCEventHeader.h"

#include "FairIon.h"
#include "FairRunSim.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::ifstream;

// -----   Default constructor   ------------------------------------------
CbmShieldGenerator::CbmShieldGenerator() 
  : FairGenerator(),
    fInputFile(NULL),
    fFileName(NULL),
    fPDG(NULL),
    fpartType(-1),
    fIonMap()
{
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
CbmShieldGenerator::CbmShieldGenerator(const char* fileName) 
  : FairGenerator(),
    fInputFile(NULL),
    fFileName(fileName),
    fPDG(TDatabasePDG::Instance()),
    fpartType(-1),
    fIonMap()
{
  cout << "-I- CbmShieldGenerator: Opening input file " << fileName << endl;
  fInputFile = new ifstream(fFileName);
  if ( ! fInputFile->is_open() ) 
    Fatal("CbmShieldGenerator","Cannot open input file.");
  cout << "-I- CbmShieldGenerator: Looking for ions..." << endl;
  Int_t nIons = RegisterIons();
  cout << "-I- CbmShieldGenerator: " << nIons << " ions registered." 
       << endl;
  CloseInput();
  cout << "-I- CbmShieldGenerator: Reopening input file " << fileName 
       << endl;
  fInputFile = new ifstream(fFileName);
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------
CbmShieldGenerator::~CbmShieldGenerator() {
  CloseInput();
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t CbmShieldGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // Check for input file
  if ( ! fInputFile->is_open() ) {
    cout << "-E- CbmShieldGenerator: Input file not open!" << endl;
    return kFALSE;
  }


  // Define event variable to be read from file
  Int_t    eventId = 0;
  Int_t    nTracks = 0;
  Double_t pBeam   = 0.;
  Double_t bx       = 0.;  
  Double_t by       = 0.;
  Double_t b       = 0.;  //SELIM


  // Define track variables to be read from file
  Int_t    iPid    = 0;
  Int_t    iMass   = 0;
  Int_t    iCharge = 0;
  Double_t px      = 0.;
  Double_t py      = 0.;
  Double_t pz      = 0.;
  Int_t    pdgType = 0;
  Double_t etot    = 0.;   //SELIM

  // Read event header line from input file
  //*fInputFile >> eventId >> nTracks >> pBeam >> bx >> by;
  *fInputFile >> eventId >> b >> bx >> by >> nTracks;  //SELIM: update of SHIELD file structure

  // If end of input file is reached : close it and abort run
  if ( fInputFile->eof() ) {
    cout << "-I- CbmShieldGenerator: End of input file reached " << endl;
    CloseInput();
    return kFALSE;
  }


  cout << "-I- CbmShieldGenerator: Event " << eventId << ",  pBeam = "
       << pBeam << "GeV, b = " << bx << " "<< by<< " fm, multiplicity " << nTracks 
       << endl;
  TVector2 bb;
  bb.Set(bx,by); 


  // Set event id and impact parameter in MCEvent if not yet done
  CbmMCEventHeader* event = dynamic_cast<CbmMCEventHeader*>(primGen->GetEvent());
  if ( event && (! event->IsSet()) ) {
    event->SetEventID(eventId);
    event->SetB(bb.Mod());
    event->SetPhi(bb.Phi());
    event->MarkSet(kTRUE);
  }


  
  // Loop over tracks in the current event
  for (Int_t itrack=0; itrack<nTracks; itrack++) {
    
    // Read PID and momentum from file
    //*fInputFile >> iPid >> iMass >> iCharge >> px >> py >> pz;
    *fInputFile >> pdgType >> iMass >> iCharge >> px >> py >> pz >> etot; //SELIM: update of SHIELD file structure

    // Case ion
    /*                       //SELIM
    if ( iPid == 1000 ) {       
      char ionName[20];
      sprintf(ionName, "Ion_%d_%d", iMass, iCharge);
      TParticlePDG* part = fPDG->GetParticle(ionName);
      if ( ! part ) {
	cout << "-W- CbmShieldGenerator::ReadEvent: Cannot find "
	     << ionName << " in database!" << endl;
	continue;
      }
      pdgType = part->PdgCode();
    }
    else pdgType = fPDG->ConvertGeant3ToPdg(iPid);  // "normal" particle
    */

    // Give track to PrimaryGenerator

    if (fpartType == 1 && pdgType == 2112) primGen->AddTrack(pdgType, px, py, pz, 0., 0., 0.);
    if (fpartType == 2 && pdgType == 2212) primGen->AddTrack(pdgType, px, py, pz, 0., 0., 0.);
    if (fpartType == 3 && pdgType >= 1000000000) primGen->AddTrack(pdgType, px, py, pz, 0., 0., 0.);

    if (fpartType == -1) primGen->AddTrack(pdgType, px, py, pz, 0., 0., 0.);

  }


  return kTRUE;
}
// ------------------------------------------------------------------------



// -----   Private method CloseInput   ------------------------------------
void CbmShieldGenerator::CloseInput() {
  if ( fInputFile ) {
    if ( fInputFile->is_open() ) {
       cout << "-I- CbmShieldGenerator: Closing input file " 
	    << fFileName << endl;
       fInputFile->close();
    }
    delete fInputFile;
    fInputFile = NULL;
  }
}
// ------------------------------------------------------------------------



// -----   Private method RegisterIons   ----------------------------------
Int_t CbmShieldGenerator::RegisterIons() {

  cout<<" CbmShieldGenerator::RegisterIons() start "<<endl;
  Int_t nIons = 0;
  Int_t eventId, nTracks, iPid, iMass, iCharge;
  Double_t pBeam, bx, by, px, py, pz;
  Double_t b;           //SELIM
  Double_t etot = 0.;   //SELIM
  Int_t pdgType = 0;    //SELIM
  fIonMap.clear();

  while ( ! fInputFile->eof()) {
    
    //*fInputFile >> eventId >> nTracks >> pBeam >> bx >>by;
      *fInputFile >> eventId >> b >> bx >> by >> nTracks; //SELIM: update of SHIELD file structure
      if ( fInputFile->eof() ) continue;
    for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {
      //*fInputFile >> iPid >> iMass >> iCharge >> px >> py >> pz;
        *fInputFile >> pdgType >> iMass >> iCharge >> px >> py >> pz >> etot; //SELIM: update of SHIELD file structure
	//if ( iPid == 1000 ) { // ion
        if ( pdgType > 1000000000 )               //SELIM
	{ // ion
	char buffer[20];
	sprintf(buffer, "Ion_%d_%d", iMass, iCharge);
	TString ionName(buffer);
	if ( fIonMap.find(ionName) == fIonMap.end() ) { // new ion
	  FairIon* ion = new FairIon(ionName, iCharge, iMass, iCharge);
	  fIonMap[ionName] = ion;
	  nIons++;
	}  // new ion
      }    // ion
    }      // track loop

  }        // event loop

  FairRunSim* run = FairRunSim::Instance();
  map<TString, FairIon*>::iterator mapIt;
  for (mapIt=fIonMap.begin(); mapIt!=fIonMap.end(); mapIt++) {
    FairIon* ion = (*mapIt).second;
    run->AddNewIon(ion);
  }

  return nIons;
}
// ------------------------------------------------------------------------


      
 


ClassImp(CbmShieldGenerator)

