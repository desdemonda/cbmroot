/**
 * \file CbmLitGeoNavigator.h
 * \author A.Lebedev <alebedev@jinr.ru>
 * \date 2008
 * \brief Interface for geometry navigation algorithm.
 */

#ifndef CBMLITGEONAVIGATOR_H_
#define CBMLITGEONAVIGATOR_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitEnums.h"
#include "propagation/CbmLitMaterialInfo.h"
#include <vector>
using std::vector;

class CbmLitTrackParam;

/**
 * \class CbmLitGeoNavigator
 * \author A.Lebedev <alebedev@jinr.ru>
 * \date 2008
 * \brief Interface for geometry navigation algorithm.
 */
class CbmLitGeoNavigator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitGeoNavigator() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitGeoNavigator() {}

   /**
    * \brief Find intersection points with detector material in a certain interval.
    * \param[in] par Input track parameter. Define initial direction and Z position.
    * \param[in] zOut Output Z position [cm].
    * \param[out] inter Output vector with crossed materials.
    * \return Status code.
    */
   virtual LitStatus FindIntersections(
      const CbmLitTrackParam* par,
      litfloat zOut,
      vector<CbmLitMaterialInfo>& inter) = 0;
};

#endif /*CBMLITGEONAVIGATOR_H_*/
