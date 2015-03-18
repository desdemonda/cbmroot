/*
 * \file CbmLitLWClusterManager.h
 * \brief Manages Clusters for the LW ClusterFinder
 * \author: Jonathan Pieper <j.pieper@gsi.de>
 * \date 2015
 */

#ifndef CBMLITLWCLUSTERMANAGER_H_
#define CBMLITLWCLUSTERMANAGER_H_

#include<vector>
#include<set>
#include "Rtypes.h"                     // for Bool_t, Int_t, UInt_t, etc

class CbmTrdCluster;
class TClonesArray;
class TList;
class TTree;

using std::vector;
using std::set;

class CbmLitLWClusterManager
{
public:
  /**
   * \brief Default Constructor
   */
  CbmLitLWClusterManager();

  /**
   * \brief Default Destructor
   */
  ~CbmLitLWClusterManager();

  /**
   * \brief Simple Initializer
   */
  void Init();

  /**
   * \brief Add Cluster
   */
  void Add(CbmTrdCluster *cluster, Int_t layer, Int_t sector, Int_t module);


private:
//  Int_t fSize;
  CbmTrdCluster *fCluster;
  TClonesArray *fUnmergedCluster;
  TClonesArray *fMergedCluster;
  TTree *fClusterSorter;
  set<Int_t> *fSeenLayers, *fSeenSectors, *fSeenModules;

  /**
   * \brief Copy Constructor
   */
  CbmLitLWClusterManager(const CbmLitLWClusterManager&);
  CbmLitLWClusterManager& operator=(const CbmLitLWClusterManager&);

//  ClassDef(CbmLitLWClusterManager, 1);
};


#endif /* CBMLITLWCLUSTERMANAGER_H_ */
