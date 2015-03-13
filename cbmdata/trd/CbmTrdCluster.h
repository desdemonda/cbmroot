/**
 * \file CbmTrdCluster.h
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \brief Data Container for TRD clusters.
 **/

#ifndef CBMTRDCLUSTER_H
#define CBMTRDCLUSTER_H 

#include "CbmCluster.h"

/**
 * \class CbmTrdCluster
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \brief Data Container for TRD clusters.
 */
class CbmTrdCluster : public CbmCluster
{
public:

  /**
   * \brief Default constructor.
   */
  CbmTrdCluster();

  /**
   * \brief Destructor.
   */
  virtual ~CbmTrdCluster();

  ClassDef(CbmTrdCluster, 2)
};
#endif
