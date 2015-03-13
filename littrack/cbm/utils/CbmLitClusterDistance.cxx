/*
 * \file CbmLitClusterDistance.cxx
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date January 2015
 * \brief Computes the distance between two given clusters
 */

#include "CbmLitClusterDistance.h"
#include "CbmCluster.h"

// Standard Library
#include<iostream>



CbmLitClusterDistance::CbmLitClusterDistance(CbmCluster *ClusterA, CbmCluster *ClusterB):
   fClusterA(ClusterA),
   fClusterB(ClusterB)
{
}
