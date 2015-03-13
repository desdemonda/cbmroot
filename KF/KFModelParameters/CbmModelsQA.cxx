/*
 *====================================================================
 *
 *  CBM KF Models
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *
 *====================================================================
 */
#include "CbmModelsQA.h"

#include "CbmKF.h"
/*#include "CbmKFParticle.h"*/
#include "CbmKFVertex.h"

#include "CbmStsHit.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"

#include "CbmMatch.h"

#include "CbmVertex.h"
#include "CbmMCTrack.h"
#include "CbmMvdPoint.h"
#include "CbmMvdHitMatch.h"
#include "CbmStsPoint.h"

#include "TClonesArray.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TAxis.h"

#include "CbmL1PFFitter.h"
#include "L1Field.h"

#include "FairRunAna.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <cmath>
using namespace std;
using std::vector;

ClassImp(CbmModelsQA)

CbmModelsQA::CbmModelsQA(Int_t iVerbose, int findParticlesMode, int perf, const char *name, const char *title, float ekin_):
  FairTask(name,iVerbose),
  outfileName("CbmModelsQA.root"),
  histodir(0),
  fNEvents(0),
  ThermalNoFlow(0),
  histodirmod(0)
{
  TDirectory *currentDir = gDirectory;
  
  gDirectory->mkdir("Models");
  gDirectory->cd("Models");
  
  histodirmod = gDirectory;
  
  gDirectory = currentDir;
  
  int recoLevel = 3;
  int usePID = 1;
  int trackNumber = 1;
//   ThermalNoFlow = NULL;
  //MSSModel = NULL;
   ThermalNoFlow = new CbmThermalModelNoFlow(ekin_, recoLevel, usePID, trackNumber, iVerbose);
  //MSSModel = new CbmMSS(ekin_, recoLevel, usePID, trackNumber, iVerbose);
}

CbmModelsQA::~CbmModelsQA()
{
   if (ThermalNoFlow!=NULL) delete ThermalNoFlow;
  //if (MSSModel!=NULL) delete ThermalNoFlow;
}

InitStatus CbmModelsQA::ReInit()
{
  return Init();
}

InitStatus CbmModelsQA::Init()
{
  FairRootManager *fManger = FairRootManager::Instance();
  
   if (ThermalNoFlow!=NULL) ThermalNoFlow->ReInit(fManger);
  //if (MSSModel!=NULL) MSSModel->ReInit(fManger);

  return kSUCCESS;
}

void CbmModelsQA::Exec(Option_t * option)
{
   if (ThermalNoFlow!=NULL) ThermalNoFlow->Exec();
  //if (MSSModel!=NULL) MSSModel->Exec();
}

void CbmModelsQA::Finish()
{
   if (ThermalNoFlow!=NULL) ThermalNoFlow->Finish();
  //if (MSSModel!=NULL) MSSModel->Finish();
  if(!(outfileName == ""))
  {
    TDirectory *curr = gDirectory;
    TFile *currentFile = gFile;
    // Open output file and write histograms
    TFile* outfile;

    outfile = new TFile(outfileName.Data(),"RECREATE");
    outfile->cd();
    //WriteHistos(histodir);
	if (histodirmod!=NULL) WriteHistos(histodirmod);
	//WriteHistos(gDirectory);
    outfile->Close();
    outfile->Delete();
    gFile = currentFile;
    gDirectory = curr;
  }
  else
  {
    //WriteHistosCurFile(histodir);
	if (histodirmod!=NULL) WriteHistosCurFile(histodirmod);
	//WriteHistosCurFile(gDirectory);
  }
  //std::fstream eff(fEfffileName.Data(),fstream::out);
  //eff << fParteff;
  //eff.close();
}

void CbmModelsQA::WriteHistos( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->mkdir(obj->GetName());
    sub->cd();
    TList *listSub = (static_cast<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) WriteHistos(obj1);
    cur->cd();
  }
}

void CbmModelsQA::WriteHistosCurFile( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->GetDirectory(obj->GetName());
    sub->cd();
    TList *listSub = (static_cast<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) WriteHistosCurFile(obj1);
    cur->cd();
  }
}