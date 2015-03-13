/**
 * \file CbmLitTrackUpdate.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track update algorithm.
 **/

#ifndef CBMLITTRACKUPDATE_H_
#define CBMLITTRACKUPDATE_H_

#include "base/CbmLitEnums.h"
#include "base/CbmLitFloat.h"

class CbmLitHit;
class CbmLitTrackParam;

/**
 * \class CbmLitTrackUpdate
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track update algorithm.
 **/
class CbmLitTrackUpdate
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackUpdate() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackUpdate() {}

   /**
    * \brief Main function to be implemented for concrete track update algorithm.
    * \param[in] pParamIn Pointer to input track parameter.
    * \param[out] pParamOut Pointer to output track parameter.
    * \parma[in] pHit Pointer to hit.
    * \param[out] chiSq Output value of contribution to chi-square.
    * \return Status code.
    */
   virtual LitStatus Update(
      const CbmLitTrackParam* pParamIn,
      CbmLitTrackParam* pParamOut,
      const CbmLitHit* pHit,
      litfloat& chiSq) = 0;

   /**
    * \brief Main function to be implemented for concrete track update algorithm.
    * \param[in,out] pParam Pointer to input/output track parameter.
    * \param[in] pHit Pointer to hit.
    * \param[out] chiSq Output value of contribution to chi-square.
    * \return Status code.
    */
   virtual LitStatus Update(
      CbmLitTrackParam* pParam,
      const CbmLitHit* pHit,
      litfloat& chiSq) = 0;
};

#endif /* CBMLITTRACKUPDATE_H_ */

