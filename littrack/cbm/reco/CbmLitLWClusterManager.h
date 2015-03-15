/*
 * \file CbmLitLWClusterManager.h
 * \brief Manages Clusters for the LW ClusterFinder
 * \author: Jonathan Pieper <j.pieper@gsi.de>
 * \date 2015
 */

#ifndef CBMLITLWCLUSTERMANAGER_H_
#define CBMLITLWCLUSTERMANAGER_H_

#include<vector>

class CbmTrdCluster;
class TClonesArray;
class TObjArray;
class TList;

using std::vector;

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
  void Add(CbmTrdCluster *cluster);


private:
//  Int_t fSize;
  CbmTrdCluster *fCluster;
  vector<CbmTrdCluster*> *fUnmergedCluster;
  TObjArray *fMergedCluster;

  /**
   * \brief Copy Constructor
   */
  CbmLitLWClusterManager(const CbmLitLWClusterManager&);
  CbmLitLWClusterManager& operator=(const CbmLitLWClusterManager&);

//  ClassDef(CbmLitLWClusterManager, 1);
};


#endif /* CBMLITLWCLUSTERMANAGER_H_ */
