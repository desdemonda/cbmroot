/** CbmHsdLvmgenerator.cxx
 **@author V. Friese <v.friese@gsi.de>
 **@since 12.05.2009
 **/
#include "CbmHsdLvmGenerator.h"

#include "CbmMCEventHeader.h"

#include "FairPrimaryGenerator.h"

#include "TLorentzRotation.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TRandom.h"

#include <iostream>
using std::cout;
using std::endl;


const Double_t kProtonMass   = 0.938271998;
const Double_t kElectronMass = 0.51099892e-3;
const Double_t kMuonMass     = 0.105658369;



// -----   Default constructor   ------------------------------------------
CbmHsdLvmGenerator::CbmHsdLvmGenerator() 
  : FairGenerator(),
    fInputFile(NULL),
    fFileName(NULL),
    fDecayMode(0),
    fBetaCm(0.),
    fGammaCm(0.),
    fMd(0.),
    fPid1(0),
    fPid2(0),
    fCurrentEvent(-1)
{ 
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
CbmHsdLvmGenerator::CbmHsdLvmGenerator(const char* fileName, 
				       Double_t pBeam,
				       Int_t decayMode) 
  : FairGenerator(),
    fInputFile(NULL),
    fFileName(NULL),
    fDecayMode(0),
    fBetaCm(0.),
    fGammaCm(0.),
    fMd(0.),
    fPid1(0),
    fPid2(0),
    fCurrentEvent(-1)
{
  // Lab transformation
  Double_t eBeam = TMath::Sqrt( pBeam*pBeam + kProtonMass*kProtonMass );
  fBetaCm = pBeam / ( eBeam + kProtonMass );
  fGammaCm = 1. / TMath::Sqrt( 1. - fBetaCm*fBetaCm );
  cout << "-I- CbmHsdLvmGenerator: Beam lab momentum is    " << pBeam 
       << " GeV" << endl;
  cout << "-I- CbmHsdLvmGenerator: Transformation beta is  " << fBetaCm 
       << endl;
  cout << "-I- CbmHsdLvmGenerator: Transformation gamma is " << fGammaCm 
       << endl;

  // Decay mode
  cout << "-I- CbmHsdLvmgenerator: Decay Mode is ";
  if ( decayMode == 1 ) {
    fDecayMode = 1;
    fMd = kElectronMass;
    fPid1 =  11;
    fPid2 = -11;
    cout << "e+ e-" << endl;
  }
  else if (decayMode ==2 ) {
    fDecayMode = 2;
    fMd = kMuonMass;
    fPid1 =  13;
    fPid2 = -13;
    cout << "mu+ mu-" << endl;
  }
  else {
    cout << "unknown (" << decayMode << ")!!" << endl;
    cout << "-E- CbmHsdLvmGenerator: Please specify valid decay mode!" 
	 << endl;
    Fatal("CbmHsdLvmGenerator", "Unknown decay mode");
  }
	
  // Input file
  fFileName = fileName;
  cout << "-I- CbmHsdLvmGenerator: Opening input file " << fileName << endl;
  fInputFile = fopen(fFileName, "r");
  if ( ! fInputFile ) Fatal("CbmHsdLvmgenerator","Cannot open input file.");
  
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------
CbmHsdLvmGenerator::~CbmHsdLvmGenerator() {
  if ( fInputFile ) {
    fclose(fInputFile);
    fInputFile = NULL;
  }
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t CbmHsdLvmGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // --> Convert to CbmPrimaryrGenerator
//  CbmPrimaryGenerator* cbmGen = (CbmPrimaryGenerator*) primGen;


  // ---> Check for input file  
  if ( ! fInputFile ) {
    cout << "-E- CbmHsdLvmGenerator: Input file not open! " << endl; 
    return kFALSE;
  }

  // ---> Check for primary generator
  if ( ! primGen ) {
    cout << "-E- CbmHsdLvmGenerator::ReadEvent: "
	 << "No PrimaryGenerator!" << endl;
    return kFALSE;
  }

  // ---> Define event variables to be read from file
  float m0, px0, py0, pz0, e0, t1, t2, b;
  char read[100];


  // ---> Read one line from input file. Check for eof.
  // ---> Check for validity of meson (t1-t2).
  do { 
    fscanf(fInputFile, "%e", &m0);
    fscanf(fInputFile, "%e", &px0);
    fscanf(fInputFile, "%e", &py0);
    fscanf(fInputFile, "%e", &pz0);
    fscanf(fInputFile, "%e", &e0);
    fscanf(fInputFile, "%e", &t1);
    fscanf(fInputFile, "%e", &t2);
    fscanf(fInputFile, "%e", &b);
    if(feof(fInputFile)) {
      cout << "-I CbmHsdLvmGenerator: End of input file reached " << endl;
      fclose(fInputFile);
      return kFALSE;
    }
  } while ( TMath::Abs(t1-t2) > 0.001 );

  cout << "-I- CbmHsdLvmGenerator::ReadEvent: p = (" << px0 << ", " 
       << py0 << ", " << pz0 << ") GeV, m = " << m0 << " GeV, b = " 
       << b << " fm" << endl;

  // Lorentztransformation of mother into lab
  Double_t ener0  = TMath::Sqrt(px0*px0 + py0*py0 + pz0*pz0 + m0*m0);
  pz0 = fGammaCm * (pz0 + fBetaCm*ener0);

  
  // ---> Perform two-particle decay  
  // Generate decay angles in rest frame of mother
  Double_t rTheta = gRandom->Rndm();
  Double_t theta  = TMath::ACos(1.-2.*rTheta);             // polar angle
  Double_t phi    = gRandom->Uniform(0., 2.*TMath::Pi());  // azimuth

  // Decay the mother particle in its rest frame
  Double_t pDecay = 0.5 * TMath::Sqrt( m0*m0 - 4.*fMd*fMd );  // Decay momentum
  Double_t px1 = pDecay * TMath::Sin(theta) * TMath::Cos(phi);
  Double_t py1 = pDecay * TMath::Sin(theta) * TMath::Sin(phi);
  Double_t pz1 = pDecay * TMath::Cos(theta);
  Double_t e1  = TMath::Sqrt(px1*px1 + py1*py1 + pz1*pz1 + fMd*fMd);
  TLorentzVector p1(px1, py1, pz1, e1);
  TLorentzVector p2(-px1, -py1, -pz1, e1);

  // Lorentztransformation to lab
  ener0 = TMath::Sqrt( px0*px0 + py0*py0 + pz0*pz0 + m0*m0 );
  TLorentzRotation lt(px0/ener0, py0/ener0, pz0/ener0);
  p1 = lt * p1;
  p2 = lt * p2;


  // Pass tracks to the stack (via FairPrimaryGenerator)
  primGen->AddTrack(fPid1, p1.X(), p1.Y(), p1.Z(), 0., 0., 0.);
  primGen->AddTrack(fPid2, p2.X(), p2.Y(), p2.Z(), 0., 0., 0.);


  // Set event information
  CbmMCEventHeader* event = dynamic_cast<CbmMCEventHeader*>(primGen->GetEvent());
  if ( ! event->IsSet() ) {
    event->SetEventID(++fCurrentEvent);
    event->SetB(b);
    event->SetPhi(0.);
  }


  return kTRUE;
}
// ------------------------------------------------------------------------


ClassImp(CbmHsdLvmGenerator);
