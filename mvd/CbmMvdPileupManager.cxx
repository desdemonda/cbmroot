// -------------------------------------------------------------------------
// -----                  CbmMvdPileupManager source file              -----
// -----                  Created 08/11/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmMvdPileupManager.h"

#include "TClonesArray.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TTree.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdPileupManager::CbmMvdPileupManager()
  : TObject(),
    fBuffer(NULL) 
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdPileupManager::CbmMvdPileupManager(TString fileName,
					 TString branchName,
					 Int_t nEvents) 
  : TObject(),
    fBuffer(new TObjArray(nEvents)) 
{
  if ( ! FillBuffer(fileName, branchName, nEvents) )
    Fatal("CbmMvdPileupManager", "Error in filling buffer");
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdPileupManager::~CbmMvdPileupManager() {
  fBuffer->Delete();
  delete fBuffer;
}
// -------------------------------------------------------------------------



// -----   Public method GetEvent   ----------------------------------------
TClonesArray* CbmMvdPileupManager::GetEvent(Int_t iEvent) {

  if ( ! fBuffer ) Fatal("CbmMvdPileupManager::GetEvent",
			 "No event buffer!");

  if (iEvent > fBuffer->GetEntriesFast() ) {
    cout << "-W- CbmMvdPileupManager::GetEvent: Event " << iEvent 
	 << " not present in buffer! " << endl;
    cout << "                                   Returning NULL pointer! "
	 << endl;
    return NULL;
  }

  TClonesArray* pArray = (TClonesArray*) fBuffer->At(iEvent);

  if ( ! pArray ) {
    cout << "-W CbmMvdPileupManager::GetEvent: Returning NULL pointer!"
	 << endl;
    return NULL;
  }

  return pArray;

}
// -------------------------------------------------------------------------



// -----   Private method FillBuffer   -------------------------------------
Int_t CbmMvdPileupManager::FillBuffer(TString fileName, 
				      TString branchName, Int_t nEvents) {

  if ( ! fBuffer ) Fatal("Fill Buffer",
			 "No event buffer!");

  fBuffer->Delete();

  TClonesArray* pointArray = NULL;
  TFile* saveGFile = gFile;

  TFile* bgfile = new TFile(fileName);
  if ( ! bgfile ) {
    cout << "-W- CbmMvdPileupManager::FillBuffer:  Background file "
	 << fileName << " could noy be opened! " << endl;
    return 0;
  }
  cout << "-I- CbmMvdPileupManager::FillBuffer: Opening file " << endl;
  cout << fileName << endl;

  TTree* bgtree = (TTree*) bgfile->Get("cbmsim");
  if ( ! bgtree ) {
    cout << "-W- CbmMvdPileupManager::FillBuffer:  "
	 << "Could not find cbmsim tree in background file " << endl;
    return 0;
  }

  Int_t nEventsInFile = bgtree->GetEntries();
  cout << "-I- CbmMvdPileupManager::FillBuffer: " << nEventsInFile
       << " events in file" << endl;
  Int_t nBuffer = TMath::Min(nEvents, nEventsInFile);
  cout << "-I- CbmMvdPileupManager::FillBuffer: Buffering " 
       << nBuffer << " events" << endl;
  if ( nBuffer < 100 ) 
    cout << "-W- CbmMvdPileupManager::FillBuffer: "
	 << "Number of events may not be sufficient for appropriate "
	 << "presentation of background pattern!" << endl;

  bgtree->SetBranchAddress(branchName, &pointArray);

  for (Int_t iEvent=0; iEvent<nBuffer; iEvent++) {
    bgtree->GetEntry(iEvent);
    fBuffer->AddAt(pointArray->Clone(), iEvent);
  }

  delete bgtree;
  bgfile->Close();
  delete bgfile;
  saveGFile->cd();

  return nBuffer;

}
// -------------------------------------------------------------------------



ClassImp(CbmMvdPileupManager)
