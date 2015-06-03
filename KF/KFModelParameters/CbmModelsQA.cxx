/*
 *====================================================================
 *
 *  CBM Models
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

// Models
//#include "CbmThermalModelNoFlow.h"
#include "HRGModel/CbmHRGModel.h"
#include "MultiscatteringModel/CbmMultiscatteringModel.h"
#include "ImpactParameterModel/CbmImpactParameterModel.h"
#include "InverseSlope/CbmInverseSlope.h"
#include "BoltzmannDistribution/CbmBoltzmannDistribution.h"
#include "BlastWaveModel/CbmBlastWave.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <cmath>
using namespace std;
using std::vector;

ClassImp(CbmModelsQA)

CbmModelsQA::CbmModelsQA(Int_t iVerbose, int findParticlesMode, int perf, KFParticleTopoReconstructor *tr, const char *name, const char *title, float ekin_):
  FairTask(name,iVerbose),
  fPrimVtx(NULL),
  outfileName("CbmModelsQA.root"),
  histodir(0),
  vStsHitMatch(), vStsPointMatch(),
  vMvdPointMatch(), vMCTrackMatch(),
  fNEvents(0),
  fTopoReconstructor(tr),
  ekin(ekin_),
  Models(),
  histodirmod(NULL)
{
  TDirectory *currentDir = gDirectory;
  
  gDirectory->mkdir("Models");
  gDirectory->cd("Models");
  
  histodirmod = gDirectory;
  
  gDirectory = currentDir;
}

CbmModelsQA::~CbmModelsQA()
{
   for(unsigned int i=0;i<Models.size();++i) {
	if (Models[i]!=NULL) delete Models[i];
   }
}

InitStatus CbmModelsQA::ReInit()
{
  return Init();
}

InitStatus CbmModelsQA::Init()
{
  FairRootManager *fManger = FairRootManager::Instance();
  
  for(unsigned int i=0;i<Models.size();++i) {
	if (Models[i]!=NULL) Models[i]->ReInit(fManger);
   }
   //if (ThermalNoFlow!=NULL) ThermalNoFlow->ReInit(fManger);
   //if (HRGModel!=NULL) HRGModel->ReInit(fManger);
  //if (MSSModel!=NULL) MSSModel->ReInit(fManger);

  return kSUCCESS;
}

void CbmModelsQA::Exec(Option_t * option)
{
   for(unsigned int i=0;i<Models.size();++i) {
	if (Models[i]!=NULL) Models[i]->Exec();
   }
   //if (ThermalNoFlow!=NULL) ThermalNoFlow->Exec();
   //if (HRGModel!=NULL) HRGModel->Exec();
  //if (MSSModel!=NULL) MSSModel->Exec();
}

void CbmModelsQA::Finish()
{
   for(unsigned int i=0;i<Models.size();++i) {
	if (Models[i]!=NULL) Models[i]->Finish();
   }
  if(!(outfileName == ""))
  {
    TDirectory *curr = gDirectory;
    TFile *currentFile = gFile;
    // Open output file and write histograms
    TFile* outfile;

    outfile = new TFile(outfileName.Data(),"RECREATE");
    outfile->cd();
	if (histodirmod!=NULL) WriteHistos(histodirmod);
    outfile->Close();
    outfile->Delete();
    gFile = currentFile;
    gDirectory = curr;
  }
  else
  {
	if (histodirmod!=NULL) WriteHistosCurFile(histodirmod);
  }
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

void CbmModelsQA::AddHRGAnalysis(int TracksType, double SystError, TString name, int EventStats, Bool_t UseWidth, Bool_t UseStatistics, double rad) {
	CbmHRGModel *HRGModel = new CbmHRGModel(TracksType, 1, name, EventStats, fTopoReconstructor, UseWidth, UseStatistics, rad);
	
	// Default set of ratios
	HRGModel->AddRatio(-211, 211, SystError);	//pi-/pi+
    HRGModel->AddRatio(-321, 321, SystError);	//K-/K+
    HRGModel->AddRatio(321, 211, SystError);	//K+/pi+
    HRGModel->AddRatio(2212, -211, SystError);	//p/pi- 
    //HRGModel->AddRatio(-2212, 2212, SystError); //pbar/p
    //HRGModel->AddRatio(-3122, 3122, SystError);	//Lambdabar/Lambda
	//HRGModel->AddRatio(-3312, 3312, SystError);	//Xi-bar/Xi-
	
	
	// HRGModel->AddRatio(211, -211, SystError);	//pi+/pi-
    // HRGModel->AddRatio(-321, -211, SystError);	//K-/pi-
    // HRGModel->AddRatio(321, -211, SystError);	//K+/pi-
    // HRGModel->AddRatio(2212, -211, SystError);	//p/pi-
	//HRGModel->AddRatio(310, -211, SystError);	//K0S/pi- 
	//HRGModel->AddRatio(3122, -211, SystError);	//Lambda/pi- 
	
	
    //Models.push_back((CbmModelBase*)HRGModel);
	Models.push_back(static_cast<CbmModelBase*>(HRGModel));
}

void CbmModelsQA::AddMultiscatteringAnalysis(int TracksType, double SystError, TString name, int EventStats) {

	CbmMultiscatteringModel *MultiscatteringModel = new CbmMultiscatteringModel(TracksType, 1, name, EventStats, fTopoReconstructor, ekin);
	
    // Models.push_back((CbmMultiscatteringModel*)MultiscatteringModel);
	Models.push_back(static_cast<CbmMultiscatteringModel*>(MultiscatteringModel));
}

void CbmModelsQA::AddInverseSlopeAnalysis(int PDGID, const char *pname, int TracksType, double SystError, TString name, int EventStats)
{
	//int trackNumber = 1;
	
	CbmInverseSlope *InverseSlope = new CbmInverseSlope(TracksType, 1, name, PDGID, TString(pname), EventStats, fTopoReconstructor, ekin);
	//InverseSlope->AddRapidityInterval(-2.0, -1.8);
	//InverseSlope->AddRapidityInterval(-1.8, -1.6);
	//InverseSlope->AddRapidityInterval(-1.6, -1.4);
	//InverseSlope->AddRapidityInterval(-1.4, -1.2);
	//InverseSlope->AddRapidityInterval(-1.2, -1.0);
	//InverseSlope->AddRapidityInterval(-1.0, -0.8);
	//InverseSlope->AddRapidityInterval(-0.8, -0.6);
	InverseSlope->AddRapidityInterval(-0.6, -0.4);
	InverseSlope->AddRapidityInterval(-0.4, -0.2);
	InverseSlope->AddRapidityInterval(-0.2, 0.0);
	InverseSlope->AddRapidityInterval(0.0, 0.2);
	InverseSlope->AddRapidityInterval(0.2, 0.4);
	InverseSlope->AddRapidityInterval(0.4, 0.6);
	InverseSlope->AddRapidityInterval(0.6, 0.8);
	InverseSlope->AddRapidityInterval(0.8, 1.0);
	InverseSlope->AddRapidityInterval(1.0, 1.2);
	InverseSlope->AddRapidityInterval(1.2, 1.4);
	//InverseSlope->AddRapidityInterval(1.4, 1.6);
	//InverseSlope->AddRapidityInterval(1.6, 1.8);
	//InverseSlope->AddRapidityInterval(1.8, 2.0);
	InverseSlope->AddHistos();
	
    // Models.push_back((CbmInverseSlope*)InverseSlope);
	Models.push_back(static_cast<CbmInverseSlope*>(InverseSlope));
}

void CbmModelsQA::AddBoltzmannAnalysis(int PDGID, const char *pname, int TracksType, double SystError, TString name, int EventStats)
{
	CbmBoltzmannDistribution *BoltzmannDistribution = new CbmBoltzmannDistribution(TracksType, 1, name, PDGID, TString(pname), EventStats, fTopoReconstructor, ekin);

    // Models.push_back((CbmBoltzmannDistribution*)BoltzmannDistribution);
	Models.push_back(static_cast<CbmBoltzmannDistribution*>(BoltzmannDistribution));
}

void CbmModelsQA::AddBlastWaveAnalysis(int PDGID, const char *pname, int TracksType, double SystError, TString name, int EventStats, double Tlong)
{
	
	CbmBlastWave *BlastWave = new CbmBlastWave(TracksType, 1, name, PDGID, TString(pname), EventStats, fTopoReconstructor, ekin, Tlong);
	
	// BlastWave->AddRapidityInterval(-0.6, -0.4);
	// BlastWave->AddRapidityInterval(-0.4, -0.2);
	// BlastWave->AddRapidityInterval(-0.2, 0.0);
	// BlastWave->AddRapidityInterval(0.0, 0.2);
	// BlastWave->AddRapidityInterval(0.2, 0.4);
	// BlastWave->AddRapidityInterval(0.4, 0.6);
	// BlastWave->AddRapidityInterval(0.6, 0.8);
	// BlastWave->AddRapidityInterval(0.8, 1.0);
	// BlastWave->AddRapidityInterval(1.0, 1.2);
	// BlastWave->AddRapidityInterval(1.2, 1.4);
	// BlastWave->AddHistos();
	
    // Models.push_back((CbmBlastWave*)BlastWave);
	Models.push_back(static_cast<CbmBlastWave*>(BlastWave));
}

void CbmModelsQA::AddImpactParameterAnalysis(int TracksType, double SystError, TString name, TString InputTable) {
	
	CbmImpactParameterModel *ImpactParameterModel = new CbmImpactParameterModel(TracksType, 1, name, fTopoReconstructor, ekin, InputTable);
	
    // Models.push_back((CbmImpactParameterModel*)ImpactParameterModel);
	Models.push_back(static_cast<CbmImpactParameterModel*>(ImpactParameterModel));
}