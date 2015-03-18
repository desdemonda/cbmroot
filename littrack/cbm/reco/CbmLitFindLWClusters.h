/*
 * \file CbmLitFindLWClusters.h
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date January 2015
 * \brief Class to create clusters from digis using the Agglomerative Hierachical Algorithm
 */

#ifndef CBMLITFINDLWCLUSTERS_H_
#define CBMLITFINDLWCLUSTERS_H_

#include "FairTask.h"
#include<utility>
#include<map>
#include<set>

class TClonesArray;
class CbmTrdModule;
class CbmTrdDigiPar;
class CbmTrdGeoHandler;
//class CbmLitClusterDistance;
class CbmCluster;
class CbmLitLWClusterManager;

class CbmLitFindLWClusters : public FairTask
{
public:
   /**
    * \brief Default Constructor
    */
   CbmLitFindLWClusters();

   /**
    * \brief Destructor
    */
   ~CbmLitFindLWClusters();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \breif Inherited from FairTask.
    */
   virtual void Exec(Option_t * option);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

protected:
   /**
    * \brief Create an array to measure the distance between Clusters
    */
//   virtual void create_dm() const;

   /**
    * \brief Update the distance array
    */
//   virtual void update_dm(Int_t p, Int_t q, Int_t r) const = 0;

   /**
    * \brief Checks the distance between all Clusters and links them together
    */
   //virtual void linkage() const;

   // Protected Variables
//   mutable std::map<UInt_t, Double_t> fDistanceMap;
//   mutable Forest fForest; 					// Forest of Clusters
//   mutable SizeMap fClusterSize; 				// Map of Tree Sizes
//   mutable std::map<TString, CbmCluster> fResults;
//   CbmLitClusterDistance *fDistance; 			// new CbmDistance = Distance

private:
   TClonesArray *fDigis;       /** Input array of CbmTrdDigi **/
   TClonesArray *fClusters;    /** Output array of CbmTrdCluster **/
   TClonesArray *fHits;        /** Output array of CbmTrdHit **/
   TClonesArray *fModules;     /** Output array of UniqueModuleID -> [iDigi, iDigi2, ...] **/
   TClonesArray *fModuleDetails;     /** Output array of CbmTrdModule **/
   TClonesArray *fUnmergedClusters;  /** Clusters to be merged **/

   CbmTrdModule *fModuleInfo;     // Information about the Trd Module
   CbmTrdDigiPar *fDigiPar;       // Parameters of Digis
   CbmTrdGeoHandler* fGeoHandler; // GeoHandler
   CbmLitLWClusterManager* fClusterManager;  // Cluster Manager

   /**
    * \brief Copy Constructor
    */
   CbmLitFindLWClusters(const CbmLitFindLWClusters&);
   CbmLitFindLWClusters& operator=(const CbmLitFindLWClusters&);

   ClassDef(CbmLitFindLWClusters, 1);
};

#endif /* CBMLITFINDLWCLUSTERS_H_ */
