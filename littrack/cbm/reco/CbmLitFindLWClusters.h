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
#include<vector>

class TClonesArray;
class CbmTrdModule;
class CbmTrdDigi;
class CbmTrdDigiPar;
class CbmTrdCluster;

using std::vector;
using std::map;
typedef vector<vector<CbmTrdDigi*>> DigiVector_t;

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
    * \brief Checks the distance between two Digis and returns, if you can link them together.
    * \out[bool] true: if the Digis fit together, otherwise false.
    */
   bool CheckLink(vector<CbmTrdDigi*> &list, CbmTrdDigi* digi, CbmTrdDigi* oldDigi);

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

   std::set<Int_t> fSeenDigis;

   /**
    * \brief
    */
   void FindRecursive(const DigiVector_t &digiArray,
		      Int_t col,
		      Int_t row,
		      vector<CbmTrdDigi*> &result,
		      const map<Int_t, Int_t> &digiIdMap);

   void HitFinder();

   /**
    * \brief Copy Constructor
    */
   CbmLitFindLWClusters(const CbmLitFindLWClusters&);
   CbmLitFindLWClusters& operator=(const CbmLitFindLWClusters&);

   ClassDef(CbmLitFindLWClusters, 1);
};

#endif /* CBMLITFINDLWCLUSTERS_H_ */
