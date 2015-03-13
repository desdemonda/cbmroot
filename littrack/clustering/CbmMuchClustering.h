/*
 * CbmMuchClustering.h
 *
 *  Created on: Mar 5, 2012
 *      Author: kozlov
 */

#ifndef CBMMUCHCLUSTERING_H_
#define CBMMUCHCLUSTERING_H_

#include "FairTask.h"
#include "CbmClusteringGeometry.h"
#include "CbmClusteringA1.h"
#include "CbmClusteringSL.h"
#include "CbmClusteringWard.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModuleGem.h"

#include "TH1F.h"

class TClonesArray;

class CbmMuchClustering: public FairTask
{
public:
   CbmMuchClustering(const char* digiFileName);
   virtual ~CbmMuchClustering();
   virtual InitStatus Init();
   virtual void Exec(Option_t* opt);
   virtual void Finish();

   void SetAlgorithmVersion(Int_t AlgorithmVersion) { fAlgorithmVersion = AlgorithmVersion;}

   void CreateModulesGeometryArray();
   void SetDigiCharges();
   void ClearDigiCharges();
   void ClusteringMainFunction();

private:

   void ReadDataBranches();

   /* Clustering algorithms
    * 1 - Developed algorithm, using all neighbors;
    * 2 - Developed algorithm, do not using diagonal neighbors;
    * 3 - Simple Single Linkage method, using all neighbors;
    * 4 - Simple Single Linkage method, do not using diagonal neighbors;
    * 5 - Ward's method (!) not tested
    */
   Int_t fAlgorithmVersion;
   Int_t fNofModules;                       // Number of modules in detector
   Int_t fNofClusters;                      // Number of clusters for event

   CbmMuchGeoScheme* fScheme;               // MuCh geometry scheme
   TString           fDigiFile;             // Digitization file

   vector<CbmClusteringGeometry*> fModulesGeometryArray;
   map <Int_t, Int_t> fModulesByDetId;

   TClonesArray* fMuchDigi;                 // Input array of CbmMuchDigi
   TClonesArray* fCluster;                  // Output array of CbmMuchCluster
   TClonesArray* fHit;                      // Output array of CbmMuchHit
   Int_t fNofEvents;                        // Event counter

   void ClusteringA1(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver);
   void ClusteringSL(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver);
   void ClusteringWard(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2/*, Int_t Ver*/);

   CbmMuchClustering(const CbmMuchClustering&);
   CbmMuchClustering& operator=(const CbmMuchClustering&);

   ClassDef(CbmMuchClustering, 1);
};

#endif
