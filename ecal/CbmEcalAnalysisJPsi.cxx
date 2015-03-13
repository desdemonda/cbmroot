#include "CbmEcalAnalysisJPsi.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "CbmEcalIdParticle.h"
#include "CbmEcalStructure.h"

#include "CbmTrackMatch.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"

#include "TClonesArray.h"
#include "TTree.h"

#include <iostream>

using namespace std;

/** Loop procedure **/
void CbmEcalAnalysisJPsi::Exec(Option_t* option)
{
  fEv++;

  InitTree();
  Int_t n=fStsTracks->GetEntriesFast();
  Int_t i;
  Int_t j;
  CbmStsTrack* tr;
  CbmTrackMatch* trm;
  FairTrackParam trb;
  FairTrackParam* trp;
  TVector3 p;
  CbmMCTrack* mctr;
  CbmMCTrack* mmctr;
  CbmEcalIdParticle* ecalid;
  Int_t idn=fEcalId->GetEntriesFast();
  Int_t mid;

  for(i=0;i<n;i++)
  {
    InitVariables();
    tr=(CbmStsTrack*)fStsTracks->At(i);
    if (!tr) continue;
    if (fPrimVertex)
      fFitter->Extrapolate(tr, fPrimVertex->GetZ(), &trb);
    else
      fFitter->Extrapolate(tr, 0.0, &trb);
    trb.Momentum(p);
    if (trb.GetQp()>0)
      fCharge=1;
    else
      fCharge=-1;
/*
    trp=tr->GetParamLast();
    trp->Momentum(p);
*/
    fPX=p.Px();
    fPY=p.Py();
    fPZ=p.Pz();
    fP=p.Mag();
    fPT=p.Pt();
    trm=(CbmTrackMatch*)fStsTracksMatch->At(i);
    if (trm)
    {
      mctr=(CbmMCTrack*)fMC->At(trm->GetMCTrackId());
      if (mctr)
      {
	fMCPDG=mctr->GetPdgCode();
	mid=mctr->GetMotherId();
	if (mid>=0)
	{
	  mmctr=(CbmMCTrack*)fMC->At(mctr->GetMotherId());
	  if (mmctr)
	    fMotherMCPDG=mmctr->GetPdgCode();
	}
      }
    }
    for(j=0;j<idn;j++)
    {
      ecalid=(CbmEcalIdParticle*)fEcalId->At(j);
      if (ecalid->Track()==i)
	break;
    }
    if (j!=idn)
    {
      fCellType=fStr->GetHitCell(ecalid->CellNum())->GetType();
      fEProb=ecalid->EProb();
      fShape=ecalid->Shape();
      fCaloE=ecalid->E()*fP;
      fChi2=ecalid->PSE();
      if (fEProb>0.05&&fShape>0.5)
	fIsE=1;
      else
	fIsE=0;
    }
    fOut->Fill();
  }
}


CbmEcalAnalysisJPsi::CbmEcalAnalysisJPsi()
  : FairTask(),
    fName(),
    fOut(NULL),
    fEv(0),
    fPX(0.),
    fPY(0.),
    fPZ(0.),
    fPT(0.),
    fP(0.),
    fEProb(0.),
    fShape(0.),
    fIsE(0),
    fMCPDG(0),
    fMotherMCPDG(0),
    fCharge(0),
    fCaloE(0.),
    fChi2(0.),
    fCellType(0),
    fMC(NULL),
    fStsTracks(NULL),
    fStsTracksMatch(NULL),
    fEcalId(NULL),
    fPrimVertex(NULL),
    fFitter(NULL),
    fStr(NULL)
{
}


CbmEcalAnalysisJPsi::CbmEcalAnalysisJPsi(const char* name, const Int_t iVerbose, const char* fname)
  : FairTask(name, iVerbose),
    fName(fname),
    fOut(NULL),
    fEv(0),
    fPX(0.),
    fPY(0.),
    fPZ(0.),
    fPT(0.),
    fP(0.),
    fEProb(0.),
    fShape(0.),
    fIsE(0),
    fMCPDG(0),
    fMotherMCPDG(0),
    fCharge(0),
    fCaloE(0.),
    fChi2(0.),
    fCellType(0),
    fMC(NULL),
    fStsTracks(NULL),
    fStsTracksMatch(NULL),
    fEcalId(NULL),
    fPrimVertex(NULL),
    fFitter(NULL),
    fStr(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisJPsi::Init()
{
  FairRootManager* io=FairRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fEcalId=(TClonesArray*)io->GetObject("EcalId");
  if (!fEcalId)
  {
    Fatal("Init", "Can't find array of identified particles");
    return kFATAL;
  }
  fStsTracks=(TClonesArray*)io->GetObject("StsTrack");
  if (!fStsTracks)
  {
    Fatal("Init()","Can't find STS tracks array");
    return kFATAL;
  }
  fMC=(TClonesArray*)io->GetObject("MCTrack");
  if (!fMC)
  {
    Fatal("Init", "Can't find array of MC tracks in system.");
    return kFATAL;
  }
  fStsTracksMatch=(TClonesArray*)io->GetObject("StsTrackMatch");
  if (!fStsTracksMatch)
  {
    Fatal("Init()","Can't find STS track matching information");
    return kFATAL;
  }  
  fPrimVertex=(CbmVertex*)io->GetObject("PrimaryVertex");
  if (!fPrimVertex)
  {
    Fatal("Init()","Can't find primary vertex.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }

  fFitter=new CbmStsKFTrackFitter();

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisJPsi::Finish()
{
  if (fOut)
    fOut->Write();
}

void CbmEcalAnalysisJPsi::InitTree()
{
  if (fOut) return;

  fOut=new TTree("ecaljpsi","None here");
  fOut->Branch("ev", &fEv, "ev/I");
  fOut->Branch("px", &fPX, "px/D");
  fOut->Branch("py", &fPY, "py/D");
  fOut->Branch("pz", &fPZ, "pz/D");
  fOut->Branch("p", &fP, "p/D");
  fOut->Branch("pt", &fPT, "pt/D");
  fOut->Branch("eprob", &fEProb, "eprob/D");
  fOut->Branch("shape", &fShape, "shape/D");
  fOut->Branch("ise", &fIsE, "ise/I");
  fOut->Branch("mcpdg", &fMCPDG, "mcpdg/I");
  fOut->Branch("mmcpdg", &fMotherMCPDG, "mmcpdg/I");
  fOut->Branch("charge", &fCharge, "charge/I");
  fOut->Branch("caloe", &fCaloE, "caloe/D");
  fOut->Branch("tcell", &fCellType, "tcell/I");
  fOut->Branch("chi2", &fChi2, "chi2/D");
}

void CbmEcalAnalysisJPsi::InitVariables()
{
  fPX=-1111;
  fPY=-1111;
  fPZ=-1111;
  fPT=-1111;
  fP=-1111;
  fEProb=-1111;
  fShape=-1111;
  fIsE=-1111;
  fMCPDG=-1111;
  fMotherMCPDG=-1111;
  fCharge=-1111;
  fCaloE=-1111;
  fChi2=-1111;
}

ClassImp(CbmEcalAnalysisJPsi)
