/**
 * \file CbmLitMaterialEffects.h
 * \author A.Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for material effects calculation algorithm.
 */

#ifndef CBMLITMATERIALEFFECTS_H_
#define CBMLITMATERIALEFFECTS_H_

#include "base/CbmLitEnums.h"

class CbmLitTrackParam;
class CbmLitMaterialInfo;

/**
 * \class CbmLitMaterialEffects
 * \author A.Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for material effects calculation algorithm.
 */
class CbmLitMaterialEffects
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitMaterialEffects() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitMaterialEffects() {}

   /**
    * \brief Main function to be implemented for concrete material effects calculation algorithm.
    * \param[in,out] par Input/Output track parameters.
    * \param[in] mat Material information.
    * \param[in] pdg PDG code
    * \param[in] downstream Propagation direction (true for downstream, false for upstream).
    * \return Status code.
    */
   virtual LitStatus Update(
      CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat,
      int pdg,
      bool downstream) = 0;
};

#endif /*CBMLITMATERIALEFFECTS_H_*/
