/*
 * \file CbmLitLWClusterManager.cxx
 * \brief Manages Clusters for the LW ClusterFinder
 * \author: Jonathan Pieper <j.pieper@gsi.de>
 * \date 2015
 */

#include "CbmLitLWClusterManager.h"

#include "TTree.h"
#include "TList.h"
#include "TClonesArray.h"
#include "FairLogger.h"
#include "CbmTrdCluster.h"

using std::set;

CbmLitLWClusterManager::CbmLitLWClusterManager():
      fCluster(NULL),
      fUnmergedCluster(NULL),
      fMergedCluster(NULL),
      fSeenLayers(NULL),
      fSeenSectors(NULL),
      fSeenModules(NULL),
      fClusterSorter(NULL)
{
  Init();
}

void CbmLitLWClusterManager::Init()
{
  fUnmergedCluster = new TClonesArray("CbmTrdCluster");
  fMergedCluster = new TClonesArray("CbmTrdCluster");
  fSeenLayers = new set<Int_t>;
  fSeenSectors = new set<Int_t>;
  fSeenModules = new set<Int_t>;
  fClusterSorter = new TTree();
}

void CbmLitLWClusterManager::Add(CbmTrdCluster *cluster, Int_t layer, Int_t sector, Int_t module)
{
  static Int_t size = 0;

  // ERROR //
  CbmTrdCluster* tmpCluster = new ((*fUnmergedCluster)[size]) CbmTrdCluster();
  tmpCluster = cluster; // push cluster to the end of all unmerged clusters

  if(fSeenLayers->find(layer) == fSeenLayers->end())
    fSeenLayers->insert(layer);
  if(fSeenSectors->find(layer) == fSeenSectors->end())
    fSeenSectors->insert(sector);
  if(fSeenModules->find(layer) == fSeenModules->end())
    fSeenModules->insert(module);

  size++;
}

ClassImp(CbmLitLWClusterManager)
