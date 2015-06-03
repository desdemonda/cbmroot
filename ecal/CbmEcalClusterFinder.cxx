#include "CbmEcalClusterFinder.h"

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "CbmEcalStructure.h"
#include "CbmEcalCell.h"
#include "CbmEcalInf.h"
#include "CbmEcalCluster.h"
#include "CbmEcalParam.h"
#include "CbmEcalCalibration.h"
#include "CbmEcalPreCluster.h"
#include "CbmEcalMaximum.h"

#include <iostream>
#include <list>

using namespace std;

/** Exec a task **/
void CbmEcalClusterFinder::Exec(Option_t* option)
{
  fEv++;

  ClearPreClusters();
  FormPreClusters();
  FormClusters();
}

InitStatus CbmEcalClusterFinder::Init()
{
  FairRootManager* io=FairRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fInf=fStr->GetEcalInf();
  fMaximums=(TClonesArray*)io->GetObject("EcalMaximums");
  if (!fMaximums)
  {
    Fatal("Init", "Can't find array of calorimeter maximums in the system.");
    return kFATAL;
  }
  fCal=(CbmEcalCalibration*)io->GetObject("EcalCalibration");
  if (!fCal)
  {
    Fatal("Init", "Can't find EcalCalibration");
    return kFATAL;
  }

  fClusters=new TClonesArray("CbmEcalCluster", 2000);
  io->Register("EcalClusters", "ECAL", fClusters, kTRUE);
  fEv=0;
  return kSUCCESS;
}

/** Finish a task **/
void CbmEcalClusterFinder::Finish()
{
  ;
}

/** Destructor **/
CbmEcalClusterFinder::~CbmEcalClusterFinder()
{
  if (fClusters)
  {
    fClusters->Delete();
    delete fClusters;
  }
}

/** Form a preclusters.
 ** A precluster --- a group of cells neighbor to maximum cell.
 ** A cluster is a group of preclusters with common cells. **/
void CbmEcalClusterFinder::FormPreClusters()
{
  Int_t nm=fMaximums->GetEntriesFast();
  Int_t i=0;
  CbmEcalMaximum* max;
  list<CbmEcalCell*> all;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*> cls;
  CbmEcalCell* cell;
  CbmEcalCell* min=NULL;
  Double_t e;
  Double_t ecls;
  CbmEcalPreCluster* precluster;
 

  for(;i<nm;i++)
  {
    max=(CbmEcalMaximum*)fMaximums->At(i);
    if (max==NULL) continue;
    /** Remove maximums matched with charged tracks **/
    if (max->Mark()!=0) continue;
    cell=max->Cell();
    ecls=cell->GetTotalEnergy();
//    cout << ecls << endl;
    /** Remove low energy maximums **/
    if (fCal->GetEnergy(ecls, cell)<fMinMaxE) continue;
    cell->GetNeighborsList(0, all);
    cell->GetNeighborsList(max->I(), cls);
    e=1e10;
    for(p=all.begin();p!=all.end();++p)
      if ((*p)->GetTotalEnergy()<e)
      {
	e=(*p)->GetTotalEnergy(); min=(*p);
      }
    if (find(cls.begin(), cls.end(), min)==cls.end())
      cls.push_back(min);
    for(p=cls.begin();p!=cls.end();++p)
      ecls+=(*p)->GetTotalEnergy();
//    cout << ":" << ecls << endl;
    /** Remove low energy clusters **/
    if (fCal->GetEnergy(ecls, cell)<fMinClusterE) continue;
    cls.push_back(cell);
    precluster=new CbmEcalPreCluster(cls, cell, min);
    fPreClusters.push_back(precluster);
  }
}

/** Form clusters from precluster **/
void CbmEcalClusterFinder::FormClusters()
{
  /** CbmEcalCluster needs a destructor call :-( **/
  fClusters->Delete();
  Int_t fN=0;
  list<CbmEcalPreCluster*>::const_iterator p1=fPreClusters.begin();
  list<CbmEcalPreCluster*>::const_iterator p2;
  list<CbmEcalCell*> cluster;
  list<CbmEcalCell*> minimums;
  list<CbmEcalCell*>::const_iterator pc;
  list<CbmEcalCell*>::const_iterator pc1;
  UInt_t oldsize;
  Int_t MaxSize=0;
  Int_t Maximums=0;
  Int_t max;
  
  if (fVerbose>9)
  {
    Info("FormClusters", "Total %d preclusters found.", (Int_t)fPreClusters.size());
  }
  for(;p1!=fPreClusters.end();++p1)
  if ((*p1)->fMark==0)
  {
    cluster.clear(); oldsize=0;
    minimums.clear();
    cluster=(*p1)->fCells;
    if ((*p1)->fMinimum!=NULL)
      minimums.push_back((*p1)->fMinimum);
    max=1;
    while(cluster.size()!=oldsize)
    {
      oldsize=cluster.size();
      p2=p1;
      for(++p2;p2!=fPreClusters.end();++p2)
      if ((*p2)->fMark==0)
      {
        pc=cluster.begin();
	for(;pc!=cluster.end();++pc)
	{
	  if ((*p2)->fMinimum!=NULL&&(*pc)==(*p2)->fMinimum) continue;
	  pc1=find((*p2)->fCells.begin(), (*p2)->fCells.end(), (*pc));
	  if (pc1==(*p2)->fCells.end()) continue;
	  pc1=find(minimums.begin(), minimums.end(), (*pc));
	  if (pc1!=minimums.end()) continue;
	  break;
	}
	if (pc!=cluster.end())
	{
	  (*p2)->fMark=1;
	  pc=(*p2)->fCells.begin();
	  for(;pc!=(*p2)->fCells.end();++pc)
	    if (find(cluster.begin(), cluster.end(), (*pc))==cluster.end())
	      cluster.push_back(*pc);
	  if ((*p2)->fMinimum!=NULL&&find(minimums.begin(), minimums.end(), (*p2)->fMinimum)==minimums.end())
	    minimums.push_back((*p2)->fMinimum);
	  max++;
	}
      }
    }
    (*p1)->fMark=1;
    if ((Int_t)cluster.size()>MaxSize)
      MaxSize=cluster.size();
    if (max>Maximums) Maximums=max;
    CbmEcalCluster* cls=new ((*fClusters)[fN]) CbmEcalCluster(fN, cluster); fN++;
    cls->Init(cluster);
  }
  if (fVerbose>0)
  {
    Info("FormClusters", "Total %d clusters formed.", fN);
    Info("FormClusters", "Maximum size of cluster is %d cells.",  MaxSize);
    Info("FormClusters", "Maximum number of photons per cluster is %d.",  Maximums);
  }
}

/** Clear a preclusters list **/
void CbmEcalClusterFinder::ClearPreClusters()
{
  list<CbmEcalPreCluster*>::const_iterator p=fPreClusters.begin();
  for(;p!=fPreClusters.end();++p)
    delete (*p);
  fPreClusters.clear();
}

/** Standard constructor **/
CbmEcalClusterFinder::CbmEcalClusterFinder(const char* name, const Int_t verbose, const char* cfg)
  : FairTask(name, verbose),
    fEv(0),
    fMaximums(NULL),
    fClusters(NULL),
    fStr(NULL),
    fInf(NULL),
    fCal(NULL),
    fPreClusters(),
    fMinClusterE(0.),
    fMinCellE(0.),
    fMinMaxE(0.)   
{
  CbmEcalParam* par=new CbmEcalParam("ClusterParam", cfg);
 
  fMinClusterE=par->GetDouble("minclustere");
  fMinMaxE=par->GetDouble("minmaxe");
  delete par;
}


/** Only to comply with frame work. **/
CbmEcalClusterFinder::CbmEcalClusterFinder()
  : FairTask(),
    fEv(0),
    fMaximums(NULL),
    fClusters(NULL),
    fStr(NULL),
    fInf(NULL),
    fCal(NULL),
    fPreClusters(),
    fMinClusterE(0.),
    fMinCellE(0.),
    fMinMaxE(0.)   
{
}

ClassImp(CbmEcalClusterFinder)
