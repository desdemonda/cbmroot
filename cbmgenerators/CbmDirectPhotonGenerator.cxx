// -------------------------------------------------------------------------
// -----                CbmDirectPhotonGenerator source file                  -----
// -----          Created 13/07/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------
#include "CbmDirectPhotonGenerator.h"

#include "FairPrimaryGenerator.h"

#include "TChain.h"

#include <iostream>

using namespace std;
// -----   Default constructor   ------------------------------------------
CbmDirectPhotonGenerator::CbmDirectPhotonGenerator()
  : FairGenerator(),
    fNEvent(-2),
    fInput(NULL),
    fPX(0.),
    fPY(0.),
    fPZ(0.),
    fPDGId(0),
    fEv(0),
    fEntry(0),
    fEntries(0)
{
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
CbmDirectPhotonGenerator::CbmDirectPhotonGenerator(const Char_t* fileName)
  : FairGenerator(),
    fNEvent(-1),
    fInput(NULL),
    fPX(0.),
    fPY(0.),
    fPZ(0.),
    fPDGId(0),
    fEv(0),
    fEntry(0),
    fEntries(0)
{
  fInput=new TChain("dphoton");
  fInput->AddFile(fileName);
  fInput->SetBranchAddress("px", &fPX);
  fInput->SetBranchAddress("py", &fPY);
  fInput->SetBranchAddress("pz", &fPZ);
  fInput->SetBranchAddress("ev", &fEv);
  fInput->SetBranchAddress("pdg", &fPDGId);
}
// ------------------------------------------------------------------------

void CbmDirectPhotonGenerator::AddFile(const char* filename)
{
  fInput->AddFile(filename);
}

// -----   Destructor   ---------------------------------------------------
CbmDirectPhotonGenerator::~CbmDirectPhotonGenerator()
{
  ;
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t CbmDirectPhotonGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  if (fNEvent==-2||fInput==NULL)
    Fatal("ReadEvent", "No input chain.");
  if (fNEvent==-1)
  {
    fNEvent=0;
    fEntry=0;
    fEntries=fInput->GetEntries();
    fInput->GetEntry(fEntry++);
  }
  for(;;)
  {
    if (fEv>fNEvent) break;
    if (fPX!=0||fPY!=0||fPZ!=0)
      // Give track to PrimaryGenerator
      primGen->AddTrack(fPDGId, fPX, fPY, fPZ, 0, 0, 0);
    if (fEntry>=fEntries)
    {
      Error("ReadEvent", "No more events in input chain!");
      return kFALSE;
    }
    fInput->GetEntry(fEntry++);
  }
  fNEvent++;

  return kTRUE;

}
// ------------------------------------------------------------------------


// -----   Public method SkipEvent   --------------------------------------
Bool_t CbmDirectPhotonGenerator::SkipEvent()
{
  if (fNEvent==-2||fInput==NULL)
    Fatal("SkipEvent", "No input chain.");
  if (fNEvent==-1)
  {
    fNEvent=0;
    fEntry=0;
    fEntries=fInput->GetEntries();
    fInput->GetEntry(fEntry++);
  }
  for(;;)
  {
    if (fEv>fNEvent) break;
    if (fEntry>=fEntries)
    {
      Error("SkipEvent", "No more events in input chain!");
      return kFALSE;
    }
    fInput->GetEntry(fEntry++);
  }
  fNEvent++;

  return kTRUE;

}
// ------------------------------------------------------------------------

ClassImp(CbmDirectPhotonGenerator)
