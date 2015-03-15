/*
 * \file CbmLitLWClusterManager.cxx
 * \brief Manages Clusters for the LW ClusterFinder
 * \author: Jonathan Pieper <j.pieper@gsi.de>
 * \date 2015
 */

#include "CbmLitLWClusterManager.h"

#include "CbmTrdCluster.h"
#include "TObject.h"                    // for TObject
#include "TList.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "Rtypes.h"                     // for Bool_t, Int_t, UInt_t, etc

CbmLitLWClusterManager::CbmLitLWClusterManager():
      fCluster(NULL),
      fUnmergedCluster(NULL),
      fMergedCluster(NULL)
{
  Init();
}

void CbmLitLWClusterManager::Init()
{
//  fUnmergedCluster = new vector();
//  fMergedCluster = new TList();
}

void CbmLitLWClusterManager::Add(CbmTrdCluster *cluster)
{
  static Int_t size = 0;

  fCluster = cluster;

  fUnmergedCluster->push_back(cluster);
  size++;
}

ClassImp(CbmLitLWClusterManager)
