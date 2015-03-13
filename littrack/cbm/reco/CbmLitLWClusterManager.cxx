/*
 * \file CbmLitLWClusterManager.cxx
 * \brief Manages Clusters for the LW ClusterFinder
 * \author: Jonathan Pieper <j.pieper@gsi.de>
 * \date 2015
 */

#include "CbmLitLWClusterManager.h"

#include "CbmTrdCluster.h"
#include "TObject.h"                    // for TObject
#include "TClonesArray.h"
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
  fUnmergedCluster = new TClonesArray("CbmTrdCluster", 1000);
  fMergedCluster = new TClonesArray("CbmTrdCluster", 1000);
}

void CbmLitLWClusterManager::Add(CbmTrdCluster *cluster)
{
  static Int_t size = 0;
  fUnmergedCluster->AddAt(cluster, size);
  size++;
}

ClassImp(CbmLitLWClusterManager)
