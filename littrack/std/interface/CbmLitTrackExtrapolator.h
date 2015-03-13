/**
 * \file CbmLitTrackExtrapolator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track extrapolation algorithm.
 */

#ifndef CBMLITTRACKEXTRAPOLATOR_H_
#define CBMLITTRACKEXTRAPOLATOR_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitEnums.h"

#include <vector>
#include <cstddef>

class CbmLitTrackParam;

/**
 * \class CbmLitTrackExtrapolator
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track extrapolation algorithm.
 *
 * Extrapolation algorithm do not have to take into account material effects.
 */
class CbmLitTrackExtrapolator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackExtrapolator() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackExtrapolator() {}

   /**
    * \brief Track parameters extrapolation with calculation of transport matrix.
    * \param[in] parIn Pointer to initial track parameters.
    * \param[out] parOut Pointer to output track parameters.
    * \param[in] zOut Z position to extrapolate to [cm].
    * \param[out] F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * \return Extrapolation status.
    */
   virtual LitStatus Extrapolate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      litfloat zOut,
      std::vector<litfloat>* F = NULL) = 0;

   /**
    * brief Track parameters extrapolation with calculation of transport matrix.
    * \param[in,out] par Pointer to initial and output track parameters.
    * \param[in] zOut Z position to extrapolate to [cm].
    * \param[out] F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * \return Extrapolation status.
    */
   virtual LitStatus Extrapolate(
      CbmLitTrackParam* par,
      litfloat zOut,
      std::vector<litfloat>* F = NULL) = 0;
};

#endif
