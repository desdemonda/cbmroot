// -------------------------------------------------------------------------
// -----                CbmUrqmdGenerator source file                  -----
// -----                Created 24/06/04  by V. Friese                 -----
// -------------------------------------------------------------------------

#include "CbmUrqmdGenerator.h"

#include "CbmMCEventHeader.h"

#include "FairPrimaryGenerator.h"
#include "FairLogger.h"

#include "TLorentzVector.h"
#include "TMath.h"
#include "TRandom.h"

#include <iostream>
using std::cout;
using std::endl;
using std::ifstream;

const Double_t kProtonMass = 0.938271998;


// -----   Default constructor   ------------------------------------------
CbmUrqmdGenerator::CbmUrqmdGenerator() 
  : FairGenerator(),
    fInputFile(NULL),
    fParticleTable(),
    fFileName(NULL),
    fPhiMin(0.), 
    fPhiMax(0.),
    fEventPlaneSet(kFALSE) 
{ 
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
CbmUrqmdGenerator::CbmUrqmdGenerator(const char* fileName) 
  : FairGenerator(),
    fInputFile(NULL),
    fParticleTable(),
    fFileName(fileName),
    fPhiMin(0.), 
    fPhiMax(0.),
    fEventPlaneSet(kFALSE) 
{
  LOG(INFO) << "CbmUrqmdGenerator: Opening input file " 
	    << fileName << FairLogger::endl;
  fInputFile = fopen(fFileName, "r");
  if ( ! fInputFile ) 
    LOG(FATAL) << "Cannot open input file." << FairLogger::endl;
  ReadConversionTable();
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------
CbmUrqmdGenerator::~CbmUrqmdGenerator() {
  if ( fInputFile ) {
    fclose(fInputFile);
    fInputFile = NULL;
  }
}
// ------------------------------------------------------------------------



// -----   Public method SetEventPlane   ----------------------------------
void CbmUrqmdGenerator::SetEventPlane(Double_t phiMin, Double_t phiMax) {
  fPhiMin = phiMin;
  fPhiMax = phiMax;
  fEventPlaneSet = kTRUE;
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t CbmUrqmdGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // ---> Check for input file  
  if ( ! fInputFile ) {
    LOG(ERROR) << "CbmUrqmdGenerator: Input file not open!" << FairLogger::endl;
    return kFALSE;
  }

  // ---> Check for primary generator
  if ( ! primGen ) {
    LOG(ERROR) << "CbmUrqmdGenerator::ReadEvent: No PrimaryGenerator!" << FairLogger::endl;
    return kFALSE;
  }

  // ---> Define event variables to be read from file
  int evnr=0, ntracks=0, aProj=0, zProj=0, aTarg=0, zTarg=0;
  float beta = 0., b = 0., phi = 0., ekin = 0., plab = 0.;
  
  int ityp=0, i3=0, ichg=0, pid=0;
  float ppx=0., ppy=0., ppz=0., m=0.;

  // ---> Read and check first event header line from input file
  char read[200];
  fgets(read, 200, fInputFile);
  if ( feof(fInputFile) ) {
    LOG(INFO) << "CbmUrqmdGenerator : End of input file reached." << FairLogger::endl;
    fclose(fInputFile);
    fInputFile = NULL;
    return kFALSE;
  }
  if ( read[0] != 'U' ) {
    LOG(ERROR) << "CbmUrqmdGenerator: Wrong event header" << FairLogger::endl;
    return kFALSE;
  }
 
  // ---> Read rest of event header
  // Line 2: projectile and target
  fgets(read, 26, fInputFile);
  fscanf(fInputFile, "%d", &aProj);
  fscanf(fInputFile, "%d", &zProj);
  fgets(read, 25, fInputFile);
  fscanf(fInputFile, "%d", &aTarg);
  fscanf(fInputFile, "%d", &zTarg);
  cout << aProj << " " << zProj << " " << aTarg << " " << zTarg << endl;
  fgets(read, 200, fInputFile);
  // Line 3: Lab transformation
  fgets(read, 48, fInputFile);
  fscanf(fInputFile, "%f", &beta);
  fgets(read, 200, fInputFile);
  // Line 4: Impact parameter
  fgets(read, 36, fInputFile);
  fscanf(fInputFile, "%f", &b);
  fgets(read, 200, fInputFile);
  // Line 5: Lab energy and momentum
  fgets(read, 39, fInputFile);
  fscanf(fInputFile, "%e", &ekin);
  fgets(read, 42, fInputFile);
  fscanf(fInputFile, "%e", &plab);
  fgets(read, 200, fInputFile);
  // Line 6: Event number
  fgets(read, 7, fInputFile);
  fscanf(fInputFile, "%d", &evnr);
  fgets(read, 200, fInputFile);
  // Skip next 8 lines
  for (int iline=0; iline<8; iline++)  { fgets(read, 200,fInputFile); }
  // Line 15: Number of tracks
  fscanf(fInputFile, "%d", &ntracks);
  fgets(read, 200, fInputFile);
  // Skip line 16
  fgets(read, 200, fInputFile);
  
  // ---> Calculate gamma for Lorentztransformation
  Double_t gamma = TMath::Sqrt( 1. / ( 1. - beta*beta) );

  // ---> Generate rotation angle  D
  if ( fEventPlaneSet ) phi = gRandom->Uniform(fPhiMin, fPhiMax);

  LOG(INFO) << "CbmUrqmdGenerator: Event " << evnr << ",  b = " << b 
       << " fm,  multiplicity " << ntracks  << ", ekin: " << ekin 
       << " GeV, plab: " << plab << " GeV, beta: " << beta 
       << ", phi: " << phi << FairLogger::endl;

  // Set event id and impact parameter in MCEvent if not yet done
  CbmMCEventHeader* event = dynamic_cast<CbmMCEventHeader*>(primGen->GetEvent());
  if ( event && (! event->IsSet()) ) {
    event->SetEventID(evnr);
    event->SetB(b);
    event->SetPhi(phi);
    event->SetNPrim(ntracks);
    event->MarkSet(kTRUE);
  }
  
 
  // ---> Loop over tracks in the current event
  for(int itrack=0; itrack<ntracks; itrack++) {

    // Read momentum and PID from file
    fgets(read, 81, fInputFile);
    fscanf(fInputFile, "%e", &ppx);
    fscanf(fInputFile, "%e", &ppy);
    fscanf(fInputFile, "%e", &ppz);
    fscanf(fInputFile, "%e", &m);
    fscanf(fInputFile, "%d", &ityp);
    fscanf(fInputFile, "%d", &i3);
    fscanf(fInputFile, "%d", &ichg);
    fgets(read, 200, fInputFile);

    // Convert UrQMD type and charge to unique pid identifier
    if (ityp >= 0) pid =  1000 * (ichg+2) + ityp;
    else           pid = -1000 * (ichg+2) + ityp;
     
    // Convert Unique PID into PDG particle code 
    if (fParticleTable.find(pid) == fParticleTable.end()) {
      LOG(WARNING) << "CbmUrqmdGenerator: PID " << ityp << " charge "
	   << ichg << " not found in table (" << pid << ")" << FairLogger::endl;
      continue;
    }
    Int_t pdgID = fParticleTable[pid];

    // Lorentztransformation to lab
    Double_t mass = Double_t(m);
    Double_t px   = Double_t(ppx);
    Double_t py   = Double_t(ppy);
    Double_t pz   = Double_t(ppz);
    Double_t e    = sqrt( mass*mass + px*px + py*py + pz*pz );
    pz = gamma * ( pz + beta * e );
    Double_t ee = sqrt( mass*mass + px*px + py*py + pz*pz );

    // Rotate momenta by event plane angle
    if ( fEventPlaneSet ) {
      Double_t pt = TMath::Sqrt(px*px + py*py);
      Double_t azim = TMath::ATan2(py,px);
      azim += phi;
      px = pt * TMath::Cos(azim);
      py = pt * TMath::Sin(azim);
    }
      

    TVector3 aa(px,py,pz);
    TLorentzVector pp;
    pp.SetPx( px );
    pp.SetPy( py );
    pp.SetPz( pz);
    pp.SetE( ee );

    // Give track to PrimaryGenerator
    primGen->AddTrack(pdgID, px, py, pz, 0., 0., 0.);
    
  }
  
  return kTRUE;
}
// ------------------------------------------------------------------------


// -----   Public method ReadEvent   --------------------------------------
Bool_t CbmUrqmdGenerator::SkipEvents(Int_t count) {
  if (count<=0) return kTRUE;

  for(Int_t ii=0; ii<count;ii++)
  {
    // ---> Check for input file  
    if ( ! fInputFile ) {
      LOG(ERROR) << "CbmUrqmdGenerator: Input file not open! " << FairLogger::endl;
      return kFALSE;
    }

    // ---> Define event variables to be read from file
    int evnr=0, ntracks=0, aProj=0, zProj=0, aTarg=0, zTarg=0;
    float b = 0., ekin = 0.;
  
    // ---> Read and check first event header line from input file
    char read[200];
    fgets(read, 200, fInputFile);
    if ( feof(fInputFile) ) {
      LOG(INFO) << "CbmUrqmdGenerator : End of input file reached." << FairLogger::endl;
      fclose(fInputFile);
      fInputFile = NULL;
      return kFALSE;
    }
    if ( read[0] != 'U' ) {
      LOG(ERROR) << "CbmUrqmdGenerator: Wrong event header" << FairLogger::endl;
      return kFALSE;
    }
 
    // ---> Read rest of event header
    fgets(read, 26, fInputFile);
    fscanf(fInputFile, "%d", &aProj);
    fscanf(fInputFile, "%d", &zProj);
    fgets(read, 25, fInputFile);
    fscanf(fInputFile, "%d", &aTarg);
    fscanf(fInputFile, "%d", &zTarg);
    fgets(read, 200, fInputFile);
    fgets(read, 200, fInputFile);
    fgets(read, 36, fInputFile);
    fscanf(fInputFile, "%f", &b);
    fgets(read, 200, fInputFile);
    fgets(read, 39, fInputFile);
    fscanf(fInputFile, "%e", &ekin);
    fgets(read, 200, fInputFile);
    fgets(read, 7, fInputFile);
    fscanf(fInputFile, "%d", &evnr);
    fgets(read, 200, fInputFile);
    for (int iline=0; iline<8; iline++)  { fgets(read, 200,fInputFile); }
    fscanf(fInputFile, "%d", &ntracks);
    fgets(read, 200, fInputFile);
    fgets(read, 200, fInputFile);
  
    LOG(INFO) << "CbmUrqmdGenerator: Event " << evnr << " skipped!" << FairLogger::endl;

    // ---> Loop over tracks in the current event
    for(int itrack=0; itrack<ntracks; itrack++) {

      // Read momentum and PID from file
      fgets(read, 81, fInputFile);
      fgets(read, 200, fInputFile);     
    }
  }
  return kTRUE;
}
// ------------------------------------------------------------------------



// -----   Private method ReadConverisonTable   ---------------------------
void CbmUrqmdGenerator::ReadConversionTable() {
 
  TString work      = getenv("VMCWORKDIR");
  TString fileName  = work + "/input/urqmd_pdg.dat";  
  ifstream* pdgconv = new ifstream(fileName.Data());
    
  Int_t index = 0;  
  Int_t pdgId = 0; 
    
  while ( ! pdgconv->eof() ) {
      index =pdgId =0 ;
      *pdgconv >> index >> pdgId ;   
      fParticleTable[index] = pdgId;
  }
    
  pdgconv->close();
  delete pdgconv; 

  LOG(INFO) << "CbmUrqmdGenerator: Particle table for conversion from UrQMD loaded" << FairLogger::endl;
}
// ------------------------------------------------------------------------



ClassImp(CbmUrqmdGenerator);
