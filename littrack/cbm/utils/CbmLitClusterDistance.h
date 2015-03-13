/*
 * \file CbmLitClusterDistance.h
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date January 2015
 * \brief Computes the distance between two given clusters
 */

#ifndef CBMLITCLUSTERDISTANCE_H_
#define CBMLITCLUSTERDISTANCE_H_

class CbmCluster;

class CbmLitClusterDistance
{
public:
   /**
    * \brief Default Constructor
    */
   CbmLitClusterDistance(CbmCluster ClusterA, CbmCluster ClusterB);

   /**
    * \brief Destructor
    */
   ~CbmLitClusterDistance();

private:
   CbmCluster *fClusterA;
   CbmCluster *fClusterB;


   /**
    * \brief Copy Constructor
    */
   CbmLitClusterDistance(const CbmLitClusterDistance&);
   CbmLitClusterDistance& operator=(const CbmLitClusterDistance&);

   ClassDef(CbmLitClusterDistance, 1);
};


#endif /* CBMLITCLUSTERDISTANCE_H_ */
