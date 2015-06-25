#include "CbmEcalMatching.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TTree.h"

#include "CbmEcalRecParticle.h"
#include "CbmEcalPoint.h"
#include "CbmEcalCluster.h"
#include "CbmEcalStructure.h"
#include "CbmEcalShLib.h"
#include "CbmEcalParam.h"
#include "CbmEcalCellMC.h"

#include <iostream>

using namespace std;


/** Standard constructor **/
CbmEcalMatching::CbmEcalMatching(const char* name, const Int_t verbose, const char* config)
  : FairTask(name, verbose),
    fReco(NULL),
    fMCTracks(NULL),
    fPoints(NULL),
    fClusters(NULL),
    fStr(NULL),
    fShLib(NULL),
    fThreshold(0.8),
    fPhotonThr(0.95),
    fMotherThr(1.05),
    fEv(0),
    fPreCluster(),
    fClusterE(0.),
    fE(),
    fEpred(),
    fEsum(0.),
    fS(0.),
    fP(),
    fCell(NULL),
    fToTree(kFALSE),
    fTree(NULL),
    fMCNum(0),
    fPx(0.),
    fPy(0.),
    fPz(0.),
    fRE(0.),
    fMCPx(0.),
    fMCPy(0.),
    fMCPz(0.),
    fMCE(0.),
    fPDG(0),
    fChi2(0.),
    fNM(0),
    fNC(0),
    fConfigName(config),
    fAlgo(0)
{
  CbmEcalParam* par=new CbmEcalParam("MatchingParam", config);

  fThreshold=par->GetDouble("threshold");
  if (fThreshold<0.5)
    Warning("CbmEcalMatching","Energy threshold less than half energy deposition in cluster.");
  fMotherThr=par->GetDouble("motherthreshold");
  fPhotonThr=par->GetDouble("photonthreshold");
  fAlgo=par->GetInteger("algorithm");
  if (fVerbose>10)
  {
    Info("CbmEcalMatching", "Using Threshold --- %f, Mother Threshold --- %f, Photon Threshold --- %f.", fThreshold, fMotherThr, fPhotonThr);
    switch (fAlgo)
    {
      case 1: Info("CbmEcalMatching", "Prefer match with photons. (Algorithm)"); break;
      case 2: Info("CbmEcalMatching", "Using complex algorithm"); break;
      case 3: Info("CbmEcalMatching", "Using simple algorithm"); break;
      default: Warning("CbmEcalMatching", "Unknown algorithm");
    }
  }

  delete par;
}


/** Task initialization **/
InitStatus CbmEcalMatching::Init()
{
  FairRootManager* io=FairRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fReco=(TClonesArray*)io->GetObject("EcalReco");
  if (!fReco)
  {
    Fatal("Init", "Can't find an array of reconstructed photons.");
    return kFATAL;
  }
  fMCTracks=(TClonesArray*)io->GetObject("MCTrack");
  if (!fMCTracks)
  {
    Fatal("Init", "Can't find array of MC tracks");
    return kFATAL;
  }
  fPoints=(TClonesArray*)io->GetObject("EcalPoint");
  if (!fPoints)
  {
    Fatal("Init", "Can't find array of Ecal Points");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init","Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fClusters=(TClonesArray*)io->GetObject("EcalClusters");
  if (!fClusters)
  {
    Fatal("Init", "Can't find array of clusters");
    return kFATAL;
  }
  fShLib=(CbmEcalShLib*)io->GetObject("EcalShLib");
  if (!fShLib)
  {
    Fatal("Init", "No shower library found in system. Will continue without chi2 calculation.");
    return kFATAL;
  }

  fEv=0; 
  return kSUCCESS;
}

/** Exec for task **/
void CbmEcalMatching::Exec(Option_t* opt)
{
  fEv++;
  if (fVerbose>0)
    Info("Exec", "Event number %d. ", fEv);
  Int_t i;
  Int_t n=fReco->GetEntriesFast();
  CbmEcalRecParticle* p;
  if (fToTree) InitTree();
  for(i=0;i<n;i++)
  {
    p=(CbmEcalRecParticle*)fReco->At(i);
    FormEpred(p);
    if (fEsum==0)
    {
      if (fVerbose>0)
	Warning("Exec", "Predicted energy for reconstructed particle %d is zero. Matching failed", i);
      continue;
    }
    MatchP(p);
/*
    FormPreCluster(p);
    FormE();
    Match(p);
*/
  }
}

/** Get form of energy deposition of the particle **/
void CbmEcalMatching::FormEpred(CbmEcalRecParticle* p)
{
  CbmEcalCluster* cluster=(CbmEcalCluster*)fClusters->At(p->ClusterNum());
  Int_t k=0;
  Int_t type;
  static Double_t module=fStr->GetEcalInf()->GetModuleSize();
//  Double_t cellsize;
  CbmEcalCell* cell;
  Double_t x;
  Double_t y;
  Double_t cx;
  Double_t cy;
  Double_t r;
  Double_t theta;
  Double_t phi;
  
  fEpred.Set(cluster->Size());
  fEsum=0;
  for(k=0;k<cluster->Size();k++)
  {
    cell=fStr->GetHitCell(cluster->CellNum(k));
    type=cell->GetType();
//    cellsize=module/type;
    cx=cell->GetCenterX(); x=cx;
    cy=cell->GetCenterY(); y=cy;
    r=TMath::Sqrt(x*x+y*y);
    x-=p->X(); 
    y-=p->Y();

    /** TODO: should be Z of the cell**/
    theta=TMath::ATan(r/fStr->GetEcalInf()->GetZPos());
    theta*=TMath::RadToDeg();
    phi=TMath::ACos(cx/r)*TMath::RadToDeg();
    if (cy<0) phi=360.0-phi;

    fEpred[k]=fShLib->GetResponse(x, y, phi, theta, p->E(), type);
    fEsum+=fEpred[k];
  }
  fS=0;
  for(k=0;k<cluster->Size();k++)
  {
    cell=fStr->GetHitCell(cluster->CellNum(k));
    fS+=fEpred[k]/fEsum*cell->GetTotalEnergy();
  }
/*
  {
    for(k=0;k<cluster->Size();k++)
    {
      cell=fStr->GetHitCell(cluster->CellNum(k));
      type=cell->GetType();
      cellsize=module/type;
      cx=cell->GetCenterX(); x=cx;
      cy=cell->GetCenterY(); y=cy;
      r=TMath::Sqrt(x*x+y*y);
      x-=p->X(); 
      y-=p->Y();

      theta=TMath::ATan(r/fStr->GetEcalInf()->GetZPos());
      theta*=TMath::RadToDeg();
      phi=TMath::ACos(cx/r)*TMath::RadToDeg();
      
//      cerr << "(" << x << "," << y << ") " << fEpred[k] << "," << fS << endl;
    }
  }
*/
}

/** Add to particle constants to track and all its mothers **/
void CbmEcalMatching::AddTrackP(Int_t track, Double_t e, Int_t cell)
{
  CbmMCTrack* tr;
  Int_t num;
  map<Int_t, Double_t>::const_iterator i;

  for(i=fP.begin(); i!=fP.end();++i)
    if ((*i).first==track) break;
  if (i!=fP.end())
    fP[track]+=e*fEpred[cell]/fEsum;
  else
    fP[track]=e*fEpred[cell]/fEsum;

  tr=(CbmMCTrack*)fMCTracks->At(track);
  if (tr==NULL)
  {
    Warning("AddTrackP", "Can't find MCTrack number %d.", track);
    return;
  }
  num=tr->GetMotherId();
  if (num<0) return;
  tr=(CbmMCTrack*)fMCTracks->At(num);
  if (tr==NULL)
  {
    Warning("AddTrackP", "Can't find MCTrack number %d.", num);
    return;
  }
  if (tr->GetPdgCode()==22||TMath::Abs(tr->GetPdgCode())==11)
    AddTrackP(num, e, cell);
}

  
/** Match MCTrack and reconstructed partile using shower shape **/
void CbmEcalMatching::MatchP(CbmEcalRecParticle* p)
{
  CbmEcalCluster* cluster=(CbmEcalCluster*)fClusters->At(p->ClusterNum());
  fP.clear();
  Int_t k;
  CbmEcalCellMC* cell;
  map<Int_t, Float_t>::const_iterator q;
  map<Int_t, Double_t>::const_iterator i;
  pair<Int_t, Double_t> max; max.first=-1111; max.second=0;
  pair<Int_t, Double_t> max_photon; max_photon.second=0;
  CbmMCTrack* tr;
  CbmEcalCluster* cls;

  
  for(k=0;k<cluster->Size();k++)
  {
    cell=(CbmEcalCellMC*)fStr->GetHitCell(cluster->CellNum(k));
    for(q=cell->GetTrackEnergyBegin();q!=cell->GetTrackEnergyEnd();++q)
    {
//      cerr << "Energy: " << (*q).second << ", " << (*q).first << "[" << fEpred[k] << "," << fEsum << "]" << endl;
      AddTrackP((*q).first, (*q).second, k);
    }
  }

  switch (fAlgo)
  {
    case 1:MatchP1(p); break;
    case 2:MatchP2(p); break;
    case 3:MatchP3(p); break;
    default: Error("MatchP", "Unknown algorithm: %d.", fAlgo);
  }

  if (!fTree) return;
  fMCNum=p->MCTrack();
  if (fMCNum!=-1111)
  {
    tr=(CbmMCTrack*)fMCTracks->At(fMCNum);
    fPDG=tr->GetPdgCode();
    fMCE=tr->GetEnergy();
    fMCPx=tr->GetPx();
    fMCPy=tr->GetPy();
    fMCPz=tr->GetPz();
  }
  else
  {
    fPDG=0;
    fMCE=-1111;
    fMCPx=-1111;
    fMCPy=-1111;
    fMCPz=-1111;
  }
  fPx=p->Px();
  fPy=p->Py();
  fPz=p->Pz();
  fRE=p->E();
  cls=(CbmEcalCluster*)fClusters->At(p->ClusterNum());
  fChi2=cls->Chi2();
  fNM=cls->Maxs();
  fNC=cls->Size();
  fTree->Fill();
}

/** First realization of matching **/
void CbmEcalMatching::MatchP1(CbmEcalRecParticle* p)
{
  CbmMCTrack* tr;
  pair<Int_t, Double_t> max; max.first=-1111; max.second=0;
  pair<Int_t, Double_t> max_photon; max_photon.second=0;
  map<Int_t, Double_t>::const_iterator i;

//  cout << "here!" << endl;
  for(i=fP.begin(); i!=fP.end();++i)
  {
//    cout << (*i).second/fS << endl;
    if ((*i).second/fS<fThreshold) continue;
    if ((*i).second>max.second)
      max=(*i);
    tr=(CbmMCTrack*)fMCTracks->At((*i).first);
    if (tr==NULL)
    {
      Warning("Match", "Can't find MCTrack number %d.", (*i).first);
      return;
    }
    if (tr->GetPdgCode()!=22) continue;
    if ((*i).second>max_photon.second)
      max_photon=(*i);
  }
  if (max.first==-1111)
  {
    if (fVerbose>2) 
      Info("MatchP1", "Matching failed for reconstructed photon (%f, %f).", p->X(), p->Y());
    return;
  }
  if (max_photon.second>fPhotonThr*max.second)
    p->SetMCTrack(max_photon.first);
  else
    p->SetMCTrack(max.first);
}

/** Second realization of matching **/
void  CbmEcalMatching::MatchP2(CbmEcalRecParticle* p)
{
  CbmMCTrack* tr;
  pair<Int_t, Double_t> max; max.first=-1111; max.second=0;
  list<pair<Int_t, Double_t> > good;
  list<pair<Int_t, Double_t> > remove;
  list<pair<Int_t, Double_t> >::const_iterator j;
  pair<Int_t, Double_t> max_photon; max_photon.second=0;
  map<Int_t, Double_t>::const_iterator i;
  Int_t oldsize=0;

  good.clear();
  //cerr << fP.size() << endl;
  for(i=fP.begin(); i!=fP.end();++i)
  {
    if ((*i).second/fS<fThreshold) continue;
    good.push_back((*i));
  }
  if (good.size()==0)
  {
    if (fVerbose>2)
      Info("MatchP2", "Matching failed for reconstructed photon (%f, %f).", p->X(), p->Y());
    return;
  }
  while(good.size()!=1)
  {
    remove.clear();
    oldsize=good.size();
    if (fVerbose>100)
      Info("MatchP2", "Size of matching track list: %d.", (Int_t)good.size());
    for(j=good.begin();j!=good.end();j++)
    {
      tr=(CbmMCTrack*)fMCTracks->At((*j).first);
      if (tr==NULL)
      {
        Warning("MatchP2", "Can't find MCTrack number %d.", (*i).first);
        return;
      }
      while(tr!=NULL)
      {
        for(i=fP.begin(); i!=fP.end();++i)
	  if ((*i).first==tr->GetMotherId())
	    break;
        if (i!=fP.end())
	{
	  if ((*i).second>fMotherThr*(*j).second)
	  {
	    remove.push_back(*j);
	    break;
	  }
          else
	    remove.push_back(*i);
	}
	if (tr->GetMotherId()>=0)
	  tr=(CbmMCTrack*)fMCTracks->At(tr->GetMotherId());
	else
	  break;
        if (tr==NULL)
        {
          Warning("MatchP2", "Can't find MCTrack.");
          return;
        }
	if (tr->GetPdgCode()!=22&&TMath::Abs(tr->GetPdgCode())!=11)
	  break;
      }
    }

    if (remove.empty())
    {
      if (fVerbose>2)
      {
	Info("MatchP2","Can't remove any from %d tracks.", (Int_t)good.size());
	if (fVerbose>100)
        for(j=good.begin();j!=good.end();j++)
        {
	  tr=(CbmMCTrack*)fMCTracks->At((*j).first);
//	  cerr << "Good: (" << (*j).first << "[" << tr->GetPdgCode() << "," << tr->GetEnergy() <<"]"<< "," << (*j).second << ") " << p->E() << endl;
        }
      }
      return;
    }
    if (fVerbose>100)
    {
      Info("MatchP2", "Size of list of tracks to remove: %d.", (Int_t)remove.size());
      for(j=remove.begin();j!=remove.end();j++)
      {
	tr=(CbmMCTrack*)fMCTracks->At((*j).first);
//	cerr << "Remove: (" << (*j).first << "[" << tr->GetPdgCode() << "," << tr->GetEnergy() << "]"<< "," << (*j).second << ")" << endl;
      }
      for(j=good.begin();j!=good.end();j++)
      {
	tr=(CbmMCTrack*)fMCTracks->At((*j).first);
//	cerr << "Good: (" << (*j).first << "[" << tr->GetPdgCode() << "," << tr->GetEnergy() <<"]"<< "," << (*j).second << ") " << p->E() << endl;
      }
    }
    for(j=remove.begin();j!=remove.end();j++)
//      good.remove(find(good.begin(), good.end(), *j));
      good.remove(*j);
    if (oldsize==(Int_t)good.size())
    {
      if (fVerbose>2)
	Info("MatchP2","No tracks was removed!");
      return;
    }
  }
  p->SetMCTrack((*good.begin()).first);
}

/** Third realization of matching **/
void  CbmEcalMatching::MatchP3(CbmEcalRecParticle* p)
{
  CbmMCTrack* tr;
  pair<Int_t, Double_t> max; max.first=-1111; max.second=0;
  map<Int_t, Double_t>::const_iterator i;
  Int_t n;

  for(i=fP.begin(); i!=fP.end();++i)
  {
    if ((*i).second/fS<fThreshold) continue;
    if ((*i).second>max.second)
      max=(*i);
    else
      if ((*i).second==max.second&&(*i).first<max.first)
	max=(*i);
  }
  if (max.first==-1111)
  {
    if (fVerbose>2) 
      Info("MatchP1", "Matching failed for reconstructed photon (%f, %f).", p->X(), p->Y());
    return;
  }
  p->SetMCTrack(max.first);
}

/** Form a precluster **/
void CbmEcalMatching::FormPreCluster(CbmEcalRecParticle* p)
{
  list<CbmEcalCell*> neib;
  CbmEcalCell* celli;
  Int_t i;
  CbmEcalCluster* cls=(CbmEcalCluster*)fClusters->At(p->ClusterNum());
  CbmEcalCell* cell;

  fPreCluster.clear();
  fClusterE=0;
  cell=fStr->GetHitCell(p->CellNum());
  cell->GetNeighborsList(0, neib);
  for(i=0;i<cls->Size();i++)
  {
    celli=fStr->GetHitCell(cls->CellNum(i));
    if (find(neib.begin(), neib.end(), celli)!=neib.end())
    {
      fPreCluster.push_back(celli);
      fClusterE+=celli->GetTotalEnergy();
    }
  }
  fPreCluster.push_back(cell);
  fClusterE+=cell->GetTotalEnergy();
}

/** Add energy to track and all its mothers **/
void CbmEcalMatching::AddTrackE(Int_t track, Double_t e)
{
  CbmMCTrack* tr;
  Int_t num;

  fE[track]+=e;

  tr=(CbmMCTrack*)fMCTracks->At(track);
  if (tr==NULL)
  {
    Warning("AddTrackE", "Can't find MCTrack number %d.", track);
    return;
  }
  num=tr->GetMotherId();
  if (num<0) return;
  tr=(CbmMCTrack*)fMCTracks->At(num);
  if (tr==NULL)
  {
    Warning("AddTrackE", "Can't find MCTrack number %d.", num);
    return;
  }
  if (tr->GetPdgCode()==22||TMath::Abs(tr->GetPdgCode())==11)
    AddTrackE(num, e);
}

/** Form a map of energies for precluster **/
void CbmEcalMatching::FormE()
{
  list<CbmEcalCell*>::const_iterator i;
  map<Int_t, Float_t>::const_iterator p;
  CbmEcalCellMC* cell;
  fE.clear();
  for(i=fPreCluster.begin();i!=fPreCluster.end();++i)
  {
    fCell=(*i); cell=(CbmEcalCellMC*)fCell;
    for(p=cell->GetTrackEnergyBegin();p!=cell->GetTrackEnergyEnd();++p)
      AddTrackE((*p).first, (*p).second);
  }
}

/** Match MCTrack and reconstructed particle **/
void CbmEcalMatching::Match(CbmEcalRecParticle* p)
{
  map<Int_t, Double_t>::const_iterator i;
  pair<Int_t, Double_t> max; max.first=-1111; max.second=0;
  pair<Int_t, Double_t> max_photon; max_photon.second=0;
  CbmMCTrack* tr;
  CbmEcalCluster* cls;

  for(i=fE.begin(); i!=fE.end();++i)
  {
    if ((*i).second/fClusterE<fThreshold) continue;
    if ((*i).second>max.second)
      max=(*i);
    tr=(CbmMCTrack*)fMCTracks->At((*i).first);
    if (tr==NULL)
    {
      Warning("Match", "Can't find MCTrack number %d.", (*i).first);
      return;
    }
    if (tr->GetPdgCode()!=22) continue;
    if ((*i).second>max_photon.second)
      max_photon=(*i);
  }
  if (max.first==-1111)
  {
    if (fVerbose>2)
      Info("Match", "Matching failed for reconstructed photon (%f, %f).", p->X(), p->Y());
    return;
  }
  if (max_photon.second>fPhotonThr*max.second)
    p->SetMCTrack(max_photon.first);
  else
    p->SetMCTrack(max.first);
  if (!fTree) return;
  fMCNum=p->MCTrack();
  tr=(CbmMCTrack*)fMCTracks->At(fMCNum);
  fPx=p->Px();
  fPy=p->Py();
  fPz=p->Pz();
  fRE=p->E();
  cls=(CbmEcalCluster*)fClusters->At(p->ClusterNum());
  fChi2=cls->Chi2();
  fNM=cls->Maxs();
  fNC=cls->Size();
  fPDG=tr->GetPdgCode();
  fMCE=tr->GetEnergy();
  fMCPx=tr->GetPx();
  fMCPy=tr->GetPy();
  fMCPz=tr->GetPz();
  fTree->Fill();
}

/** Initialization of output tree **/
void CbmEcalMatching::InitTree()
{
  if (fTree) return;
  fTree=new TTree("ecal_matching","Ecal matching tree");
  fTree->Branch("ev", &fEv, "ev/I");
  fTree->Branch("mcn", &fMCNum, "mcn/I");
  fTree->Branch("nm", &fNM, "nm/I");
  fTree->Branch("nc", &fNC, "nc/I");
  fTree->Branch("pdg", &fPDG, "PDG/I");
  fTree->Branch("chi2", &fChi2, "chi2/D");
  fTree->Branch("px", &fPx, "px/D");
  fTree->Branch("py", &fPy, "py/D");
  fTree->Branch("pz", &fPz, "pz/D");
  fTree->Branch("e", &fRE, "e/D");
  fTree->Branch("mcpx", &fMCPx, "mcpx/D");
  fTree->Branch("mcpy", &fMCPy, "mcpy/D");
  fTree->Branch("mcpz", &fMCPz, "mcpz/D");
  fTree->Branch("mce", &fMCE, "mce/D");
}

/** Finish task **/
void CbmEcalMatching::Finish()
{
  if (fTree) fTree->Write();
}

/** virtual destructor **/
CbmEcalMatching::~CbmEcalMatching()
{
  ;
}
  
/** Only to comply with framework **/
CbmEcalMatching::CbmEcalMatching()
  : FairTask(),
    fReco(NULL),
    fMCTracks(NULL),
    fPoints(NULL),
    fClusters(NULL),
    fStr(NULL),
    fShLib(NULL),
    fThreshold(0.8),
    fPhotonThr(0.95),
    fMotherThr(1.05),
    fEv(0),
    fPreCluster(),
    fClusterE(0.),
    fE(),
    fEpred(),
    fEsum(0.),
    fS(0.),
    fP(),
    fCell(NULL),
    fToTree(kFALSE),
    fTree(NULL),
    fMCNum(0),
    fPx(0.),
    fPy(0.),
    fPz(0.),
    fRE(0.),
    fMCPx(0.),
    fMCPy(0.),
    fMCPz(0.),
    fMCE(0.),
    fPDG(0),
    fChi2(0.),
    fNM(0),
    fNC(0),
    fConfigName(""),
    fAlgo(0)
{
}
ClassImp(CbmEcalMatching)
