//-----------------------------------------------------------
//-----------------------------------------------------------

// Cbm Headers ----------------------
#include "CbmKFParticleFinderQA.h"
#include "CbmTrack.h"
#include "CbmMCTrack.h"
#include "CbmTrackMatchNew.h"

#include "FairRunAna.h"

//KF Particle headers
#include "KFParticleTopoReconstructor.h"
#include "KFTopoPerformance.h"
#include "KFMCTrack.h"
#include "KFParticleMatch.h"

//ROOT headers
#include "TClonesArray.h"
#include "TFile.h"
#include "TObject.h"
#include "TMath.h"
#include "TDatabasePDG.h"

//c++ and std headers
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
using std::vector;

CbmKFParticleFinderQA::CbmKFParticleFinderQA(const char* name, Int_t iVerbose, KFParticleTopoReconstructor* tr, TString outFileName):
  FairTask(name, iVerbose), fMCTracksBranchName("MCTrack"), fTrackMatchBranchName("StsTrackMatch"), fMCTrackArray(0), fTrackMatchArray(0), 
  fRecParticles(0), fMCParticles(0), fMatchParticles(0), fSaveParticles(0), fSaveMCParticles(0),
  fOutFileName(outFileName), fOutFile(0), fEfffileName("Efficiency.txt"), fTopoPerformance(0), fPrintFrequency(100), fNEvents(0)
{
  for(Int_t i=0; i<5; i++)
    fTime[i] = 0;
  
  fTopoPerformance = new KFTopoPerformance;
  fTopoPerformance->SetTopoReconstructor(tr);

  TFile* curFile = gFile;
  TDirectory* curDirectory = gDirectory;
  
  if(!(fOutFileName == ""))
    fOutFile = new TFile(fOutFileName.Data(),"RECREATE");
  else
    fOutFile = gFile;
  fTopoPerformance->CreateHistos("KFTopoReconstructor",fOutFile);
  
  gFile = curFile;
  gDirectory = curDirectory;
}

CbmKFParticleFinderQA::~CbmKFParticleFinderQA()
{
  if(fTopoPerformance) delete fTopoPerformance;  
  
  if(fSaveParticles)
    fRecParticles->Delete();
  if(fSaveMCParticles)
  {
    fMCParticles->Delete();
    fMatchParticles->Delete();
  }
}

InitStatus CbmKFParticleFinderQA::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
  {
    Error("CbmKFParticleFinderQA::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  //MC Tracks
  fMCTrackArray=(TClonesArray*) ioman->GetObject(fMCTracksBranchName);
  if(fMCTrackArray==0)
  {
    Error("CbmKFParticleFinderQA::Init","mc track array not found!");
    return kERROR;
  }
  
  //Track match
  fTrackMatchArray=(TClonesArray*) ioman->GetObject(fTrackMatchBranchName);
  if(fTrackMatchArray==0)
  {
    Error("CbmKFParticleFinderQA::Init","track match array not found!");
    return kERROR;
  }
  
  if(fSaveParticles)
  {
    // create and register TClonesArray with output reco particles
    fRecParticles = new TClonesArray("KFParticle",100);
    ioman->Register("RecoParticles", "KFParticle", fRecParticles, kTRUE);
  }

  if(fSaveMCParticles)
  {
    // create and register TClonesArray with output MC particles
    fMCParticles = new TClonesArray("KFMCParticle",100);
    ioman->Register("KFMCParticles", "KFParticle", fMCParticles, kTRUE);

    // create and register TClonesArray with matching between reco and MC particles
    fMatchParticles = new TClonesArray("KFParticleMatch",100);
    ioman->Register("KFParticleMatch", "KFParticle", fMatchParticles, kTRUE);
  }
  return kSUCCESS;
}

void CbmKFParticleFinderQA::Exec(Option_t* opt)
{ 
  if(fSaveParticles)
    fRecParticles->Delete();
  if(fSaveMCParticles)
  {
    fMCParticles->Delete();
    fMatchParticles->Delete();
  }
  
  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
  vector<KFMCTrack> mcTracks(nMCTracks);
  for(Int_t iMC=0; iMC<nMCTracks; iMC++)
  {
    CbmMCTrack *cbmMCTrack = (CbmMCTrack*)fMCTrackArray->At(iMC);

    
    mcTracks[iMC].SetX ( cbmMCTrack->GetStartX() );
    mcTracks[iMC].SetY ( cbmMCTrack->GetStartY() );
    mcTracks[iMC].SetZ ( cbmMCTrack->GetStartZ() );
    mcTracks[iMC].SetPx( cbmMCTrack->GetPx() );
    mcTracks[iMC].SetPy( cbmMCTrack->GetPy() );
    mcTracks[iMC].SetPz( cbmMCTrack->GetPz() );
    
    Int_t pdg = cbmMCTrack->GetPdgCode();
    Double_t q=1;
    if ( pdg < 9999999 && ( (TParticlePDG *)TDatabasePDG::Instance()->GetParticle(pdg) ))
      q = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.0;
    else if(pdg ==  1000010020) q =  1;
    else if(pdg == -1000010020) q = -1;
    else if(pdg ==  1000010030) q =  1;
    else if(pdg == -1000010030) q = -1;
    else if(pdg ==  1000020030) q =  2;
    else if(pdg == -1000020030) q = -2;
    else if(pdg ==  1000020040) q =  2;
    else if(pdg == -1000020040) q = -2;
    else q = 0;
    Double_t p = cbmMCTrack->GetP();
    
    mcTracks[iMC].SetMotherId(cbmMCTrack->GetMotherId());
    mcTracks[iMC].SetQP(q/p);
    mcTracks[iMC].SetPDG(pdg);
    mcTracks[iMC].SetNMCPoints(0);
  }
  
  Int_t ntrackMatches=fTrackMatchArray->GetEntriesFast();
  vector<int> trackMatch(ntrackMatches, -1);

  for(int iTr=0; iTr<ntrackMatches; iTr++)
  {
    CbmTrackMatchNew* stsTrackMatch = (CbmTrackMatchNew*) fTrackMatchArray->At(iTr);
    if(stsTrackMatch -> GetNofLinks() == 0) continue;
    Float_t bestWeight = 0.f;
    Float_t totalWeight = 0.f;
    Int_t mcTrackId = -1;
    for(int iLink=0; iLink<stsTrackMatch -> GetNofLinks(); iLink++)
    {
      totalWeight += stsTrackMatch->GetLink(iLink).GetWeight();
      if( stsTrackMatch->GetLink(iLink).GetWeight() > bestWeight)
      {
        bestWeight = stsTrackMatch->GetLink(iLink).GetWeight();
        mcTrackId = stsTrackMatch->GetLink(iLink).GetIndex();
      }
    }
    if(bestWeight/totalWeight < 0.7) continue;
    if(mcTrackId >= nMCTracks || mcTrackId < 0)
    {
      std::cout << "Sts Matching is wrong!    StsTackId = " << mcTrackId << " N mc tracks = " << nMCTracks << std::endl;
      continue;
    }
    
    if(TMath::Abs(mcTracks[mcTrackId].PDG()) > 2500 && 
       !(TMath::Abs(mcTracks[mcTrackId].PDG()) == 1000010020 || TMath::Abs(mcTracks[mcTrackId].PDG()) == 1000010030 ||
         TMath::Abs(mcTracks[mcTrackId].PDG()) == 1000020030 || TMath::Abs(mcTracks[mcTrackId].PDG()) == 1000020040 ) )
      continue;
    mcTracks[mcTrackId].SetReconstructed();
    trackMatch[iTr] = mcTrackId;
  }

  fTopoPerformance->SetMCTracks(mcTracks);
  fTopoPerformance->SetTrackMatch(trackMatch);
  
  fTopoPerformance->CheckMCTracks();
  fTopoPerformance->MatchTracks();
  fTopoPerformance->FillHistos();
  
  fNEvents++;
  fTime[4] += fTopoPerformance->GetTopoReconstructor()->Time();
  for(int iT=0; iT<4; iT++)
    fTime[iT] += fTopoPerformance->GetTopoReconstructor()->StatTime(iT);
  if(fNEvents%fPrintFrequency == 0)
  {
    std::cout << "Topo reconstruction time"
      << " Real = " << std::setw( 10 ) << fTime[4]/fNEvents * 1.e3 << " ms" << std::endl;
    std::cout << "    Init                " << fTime[0]/fNEvents * 1.e3 << " ms" << std::endl;
    std::cout << "    PV Finder           " << fTime[1]/fNEvents * 1.e3 << " ms" << std::endl;
    std::cout << "    Sort Tracks         " << fTime[2]/fNEvents * 1.e3 << " ms" << std::endl;
    std::cout << "    KF Particle Finder  " << fTime[3]/fNEvents * 1.e3 << " ms" << std::endl;
  }
  
  // save particles to a ROOT file
  if(fSaveParticles)
  {
    for(unsigned int iP=0; iP < fTopoPerformance->GetTopoReconstructor()->GetParticles().size(); iP++)
    {
      new((*fRecParticles)[iP]) KFParticle(fTopoPerformance->GetTopoReconstructor()->GetParticles()[iP]);
    }
  }

  if(fSaveMCParticles)
  {
    for(unsigned int iP=0; iP < fTopoPerformance->GetTopoReconstructor()->GetParticles().size(); iP++)
    {
      new((*fMatchParticles)[iP]) KFParticleMatch();
      KFParticleMatch *p = (KFParticleMatch*)( fMatchParticles->At(iP) );

      Short_t matchType = 0;
      int iMCPart = -1;
      if(!(fTopoPerformance->ParticlesMatch()[iP].IsMatchedWithPdg())) //background
      {
        if(fTopoPerformance->ParticlesMatch()[iP].IsMatched())
        {
          iMCPart = fTopoPerformance->ParticlesMatch()[iP].GetBestMatchWithPdg();
          matchType = 1;
        }
      }
      else
      {
        iMCPart = fTopoPerformance->ParticlesMatch()[iP].GetBestMatchWithPdg();
        matchType = 2;
      }

      p->SetMatch(iMCPart);
      p->SetMatchType(matchType);
    }

    for(unsigned int iP=0; iP < fTopoPerformance->MCParticles().size(); iP++)
    {
      new((*fMCParticles)[iP]) KFMCParticle(fTopoPerformance->MCParticles()[iP]);
    }
  }
}

void CbmKFParticleFinderQA::Finish()
{
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  // Open output file and write histograms

  fOutFile->cd();
  WriteHistosCurFile(fTopoPerformance->GetHistosDirectory());
//         if (histodirmod!=NULL) WriteHistos(histodirmod);
        //WriteHistos(gDirectory);
  if(!(fOutFileName == ""))
  {   
    fOutFile->Close();
    fOutFile->Delete();
  }
  gFile = currentFile;
  gDirectory = curr;

  std::fstream eff(fEfffileName.Data(),std::fstream::out);
  eff << fTopoPerformance->fParteff;
  eff.close();
}

void CbmKFParticleFinderQA::WriteHistosCurFile( TObject *obj ){
  
  
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TFile *currentFile = gFile;

    TDirectory *sub = cur->GetDirectory(obj->GetName());
    sub->cd();
    TList *listSub = (static_cast<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) WriteHistosCurFile(obj1);
    cur->cd();
    gFile = currentFile;
    gDirectory = cur;
  }
}

void CbmKFParticleFinderQA::SetPrintEffFrequency(Int_t n)
{ 
  fTopoPerformance->SetPrintEffFrequency(n); 
  fPrintFrequency = n;
}

ClassImp(CbmKFParticleFinderQA);
