// -------------------------------------------------------------------------
// -----                CbmHsdGenerator source file                  -----
// -----                Created 24/06/04  by V. Friese                 -----
// -------------------------------------------------------------------------
#include "CbmHsdGenerator.h"

#include "FairPrimaryGenerator.h"
#include "FairMCEventHeader.h"

#include "TMCProcess.h"
#include "TObjArray.h"
#include "TPDGCode.h"
#include "TParticle.h"
#include "TRandom.h"
#include "TString.h"
#include "TVirtualMCStack.h"
#include "TLorentzVector.h"

#include <iostream>
#include <cstring>

using std::cout;
using std::endl;

const Double_t kProtonMass = 0.938271998;

// -----   Default constructor   ------------------------------------------
CbmHsdGenerator::CbmHsdGenerator() 
: FairGenerator(),
  fInputFile(NULL),
  fFileName(NULL),
  fPid(0), 
  fPdg(0)
{ 
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
CbmHsdGenerator::CbmHsdGenerator(const char* fileName, TString particle) 
: FairGenerator(),
  fInputFile(NULL),
  fFileName(fileName),
  fPid(0), 
  fPdg(0)
{
  cout << "-I CbmHsdGenerator: Opening input file " << fileName << endl;

  fInputFile = fopen(fFileName, "r");
  if ( ! fInputFile ) Fatal("CbmHsdgenerator","Cannot open input file.");
  
  if(particle == "D0"){fPid = 1;  fPdg = 421;}
  else if(particle == "D+"){fPid = 3;  fPdg = 411;}
  else if(particle == "Ds"){fPid = 9;  fPdg = 431;}
  else if(particle == "Jpsi"){fPid = 14; fPdg = 443;}
  else if(particle == "rho"){fPid = 113; fPdg = 113;}
  else Fatal("CbmHsdgenerator","Signal particle is not specificated!"); 

  
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------
CbmHsdGenerator::~CbmHsdGenerator() {
  if ( fInputFile ) {
    fclose(fInputFile);
    fInputFile = NULL;
  }
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t CbmHsdGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // ---> Check for input file  
  if ( ! fInputFile ) {
    cout << "-E CbmHsdGenerator: Input file not open! " << endl; 
    return kFALSE;
  }

  // ---> Check for primary generator
  if ( ! primGen ) {
    cout << "-E- CbmHsdGenerator::ReadEvent: "
	 << "No PrimaryGenerator!" << endl;
    return kFALSE;
  }

  // ---> Define event variables to be read from file
  int pid, ntracks=1;
  float ppx=0., ppy=0., ppz=0., pp0=0., m = 0.;
  int dummy1 = 0, dummy2 = 0, dummy3 = 0, dummy4 = 0;
  
  // ---> Read and check first event header line from input file
  char read[114];
  if ( feof(fInputFile) ) {
    cout << "-I CbmHsdGenerator : End of input file reached." << endl;
    fclose(fInputFile);
    fInputFile = NULL;
    return kFALSE;
  }
  
  Double_t ekin = 0.2908e2;
//  Double_t ekin = 0.2408e2; // 25 AGeV
  Double_t eBeam   = ekin + kProtonMass;
  Double_t pBeam   = TMath::Sqrt(eBeam*eBeam - kProtonMass*kProtonMass);
  Double_t betaCM  = pBeam / (eBeam + kProtonMass);
  Double_t gammaCM = TMath::Sqrt( 1. / ( 1. - betaCM*betaCM) );
  
  // ---> Loop over tracks in the current event
 if(fPid != 113)
 {
  for(int itrack=0; itrack<ntracks; itrack++) {

    // Read momentum and PID from file
    fscanf(fInputFile, "%d", &pid);
    fscanf(fInputFile, "%d", &dummy2);
    fscanf(fInputFile, "%d", &dummy3);
    fscanf(fInputFile, "%d", &dummy4);
    fscanf(fInputFile, "%e", &ppx);
    fscanf(fInputFile, "%e", &ppy);
    fscanf(fInputFile, "%e", &ppz);
    fscanf(fInputFile, "%e", &pp0);
    fgets(read, 114, fInputFile);
    
    if(feof(fInputFile))
    {
     cout << "-I CbmHsdGenerator: End of input file reached " << endl;
     fclose(fInputFile);
     return kFALSE;
    }
    
    if(pid != fPid)continue;
    
    Double_t px   = Double_t(ppx);
    Double_t py   = Double_t(ppy);
    Double_t pz   = Double_t(ppz);
    Double_t e   = Double_t(pp0);
    pz = gammaCM * ( pz + betaCM * e );
    
    cout <<  "HSD particle: " << fPdg << ", Px = " << ppx << ", Py = " <<  ppy << ", Pz = " <<  ppz << endl;
    
    // Give track to PrimaryGenerator
    //   if (pid == 14)
      primGen->AddTrack(fPdg, px, py, pz, 0., 0., 0.);
    
  }
 }
 else
 {
  for(int itrack=0; itrack<ntracks; itrack++) {

    // Read momentum and PID from file
    fscanf(fInputFile, "%e", &m);
    fscanf(fInputFile, "%e", &ppx);
    fscanf(fInputFile, "%e", &ppy);
    fscanf(fInputFile, "%e", &ppz);
    fscanf(fInputFile, "%e", &pp0);
    fgets(read, 114, fInputFile);
    
    if(feof(fInputFile))
    {
     cout << "-I CbmHsdGenerator: End of input file reached " << endl;
     fclose(fInputFile);
     return kFALSE;
    }
    
    Double_t px   = Double_t(ppx);
    Double_t py   = Double_t(ppy);
    Double_t pz   = Double_t(ppz);
    Double_t e   = Double_t(pp0);
    pz = gammaCM * ( pz + betaCM * e );
    
    cout <<  "HSD particle: " << fPdg << ", Px = " << ppx << ", Py = " <<  ppy << ", Pz = " <<  ppz << endl;
    
    // Give track to PrimaryGenerator
    primGen->AddTrack(fPdg, px, py, pz, 0., 0., 0.);
  }
 } 
  return kTRUE;
}
// ------------------------------------------------------------------------


ClassImp(CbmHsdGenerator);
