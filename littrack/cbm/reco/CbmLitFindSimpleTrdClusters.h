/*
 * \file CbmLitFindSimpleTrdClusters.h
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date Dec, 2014
 * \brief Class to interpret each digi as one cluster and one hit (for reference)
 */

#ifndef CBMLITFINDSIMPLETRDCLUSTERS_H_
#define CBMLITFINDSIMPLETRDCLUSTERS_H_

#include "FairTask.h"

class TClonesArray;
class CbmTrdModule;
class CbmTrdDigiPar;

class CbmLitFindSimpleTrdClusters : public FairTask
{
public:
   /**
    * \brief Default Constructor
    */
   CbmLitFindSimpleTrdClusters();

   /**
    * \brief Destructor
    */
   ~CbmLitFindSimpleTrdClusters();

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

private:
   TClonesArray *fDigis;       /** Input array of CbmTrdDigi **/
   TClonesArray *fClusters;    /** Output array of CbmTrdCluster **/
   TClonesArray *fHits;        /** Output array of CbmTrdHit **/

   CbmTrdModule *fModuleInfo;     // Information about the Trd Module
   CbmTrdDigiPar *fDigiPar;       // Parameters of Digis

   /**
    * \brief Copy Constructor
    */
   CbmLitFindSimpleTrdClusters(const CbmLitFindSimpleTrdClusters&);
   CbmLitFindSimpleTrdClusters& operator=(const CbmLitFindSimpleTrdClusters&);

   ClassDef(CbmLitFindSimpleTrdClusters, 1);
};

#endif /* CBMLITFINDSIMPLETRDCLUSTERS_H_ */
