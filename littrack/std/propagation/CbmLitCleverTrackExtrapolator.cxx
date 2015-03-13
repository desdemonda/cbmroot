/** CbmLitCleverTrackExtrapolator.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2009
 **/

#include "propagation/CbmLitCleverTrackExtrapolator.h"

#include "base/CbmLitEnums.h"
#include "base/CbmLitDefaultSettings.h"
#include "../cbm/base/CbmLitToolFactory.h"
#include "data/CbmLitTrackParam.h"
#include "interface/CbmLitTrackExtrapolator.h"
#include "utils/CbmLitMath.h"
#include "utils/CbmLitMatrixMath.h"

const litfloat lit::CbmLitDefaultSettings::LINE_EXTRAPOLATION_START_Z = 450.; // z coordinate [cm] a
const litfloat lit::CbmLitDefaultSettings::ENERGY_LOSS_CONST = 0.00354; // energy lost constant [GeV
const litfloat lit::CbmLitDefaultSettings::MINIMUM_PROPAGATION_DISTANCE = 1e-6; // minimum propagati
const litfloat lit::CbmLitDefaultSettings::MAXIMUM_TGEO_NAVIGATION_DISTANCE = 25.; // maximum distan


CbmLitCleverTrackExtrapolator::CbmLitCleverTrackExtrapolator()
{
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   fLineExtrapolator = factory->CreateTrackExtrapolator("line");
   fRK4Extrapolator = factory->CreateTrackExtrapolator("rk4");
}

CbmLitCleverTrackExtrapolator::~CbmLitCleverTrackExtrapolator()
{
}

LitStatus CbmLitCleverTrackExtrapolator::Extrapolate(
   const CbmLitTrackParam* parIn,
   CbmLitTrackParam* parOut,
   litfloat zOut,
   std::vector<litfloat>* F)
{
   *parOut = *parIn;
   return Extrapolate(parOut, zOut, F);
}

LitStatus CbmLitCleverTrackExtrapolator::Extrapolate(
   CbmLitTrackParam* par,
   litfloat zOut,
   std::vector<litfloat>* F)
{
   litfloat zIn = par->GetZ();
   litfloat zStart = lit::CbmLitDefaultSettings::LINE_EXTRAPOLATION_START_Z;

   if (zIn >= zStart && zOut >= zStart) {
      return fLineExtrapolator->Extrapolate(par, zOut, F);
   } else if (zIn < zStart && zOut < zStart) {
      return fRK4Extrapolator->Extrapolate(par, zOut, F);
   } else if (zOut > zIn && zIn < zStart && zOut > zStart) {
      std::vector<litfloat> F1(25), F2(25);
      LitStatus result;
      if (F != NULL) { result = fRK4Extrapolator->Extrapolate(par, zStart, &F1); }
      else { result = fRK4Extrapolator->Extrapolate(par, zStart, NULL); }
      if (result == kLITERROR) {
         return result;
      } else {
         LitStatus result1;
         if (F != NULL) { result1 = fLineExtrapolator->Extrapolate(par, zOut, &F2); }
         else { result1 = fLineExtrapolator->Extrapolate(par, zOut, NULL); }
         if (F != NULL && result1 == kLITSUCCESS) { Mult25(F2, F1, *F); }
         return result1;
      };
   } else if (zOut < zIn && zIn > zStart && zOut < zStart) {
      std::vector<litfloat> F1(25), F2(25);
      LitStatus result;
      if (F != NULL) { result = fLineExtrapolator->Extrapolate(par, zStart, &F1); }
      else { result = fLineExtrapolator->Extrapolate(par, zStart, NULL); }
      if (result == kLITERROR) {
         return result;
      } else {
         LitStatus result1;
         if (F != NULL) { result1 = fRK4Extrapolator->Extrapolate(par, zOut, &F2); }
         else { result1 = fRK4Extrapolator->Extrapolate(par, zOut, NULL); }
         if (F != NULL && result1 == kLITSUCCESS) { Mult25(F2, F1, *F); }
         return result1;
      }
   }

   return kLITSUCCESS;
}
