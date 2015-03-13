/**
 * \brief CbmLitTrackPropagator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track propagation algorithm.
 */

#ifndef CBMLITTRACKPROPAGATOR_H_
#define CBMLITTRACKPROPAGATOR_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitEnums.h"

#include <vector>
#include <cstddef>

class CbmLitTrackParam;

/**
 * \brief CbmLitTrackPropagator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track propagation algorithm.
 * Propagation algorithm has to take into account material and
 * can use CbmLitTrackExtrapolator to extrapolate tracks between material layers.
 */
class CbmLitTrackPropagator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackPropagator() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackPropagator() {}

   /**
    * \brief Track parameter propagation.
    * \param[in] parIn Pointer to initial track parameters.
    * \param[out] parOut Pointer to output track parameters.
    * \param[in] zOut Z position to propagate to [cm].
    * \param[in] pdg PDG code of particle.
    * \param[out] F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * \param[out] length Length of track segment.
    * \return Propagation status.
    */
   virtual LitStatus Propagate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      litfloat zOut,
      int pdg,
      std::vector<litfloat>* F = NULL,
      litfloat* length = NULL) = 0;

   /**
    * \brief Track parameter propagation.
    * \param[in,out] par Pointer to initial and output track parameters.
    * \param[in] zOut Z position to propagate to [cm].
    * \param[in] pdg PDG code of particle.
    * \param[out] F Output transport matrix. If F == NULL than transport matrix is not calculated.
    * \param[out] length Length of track segment.
    * \return Propagation status.
    */
   virtual LitStatus Propagate(
      CbmLitTrackParam* par,
      litfloat zOut,
      int pdg,
      std::vector<litfloat>* F = NULL,
      litfloat* length = NULL) = 0;
};

#endif
