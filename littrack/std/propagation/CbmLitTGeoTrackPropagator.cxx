/** CbmLitTGeoTrackPropagator.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 1.0
 **/

#include "CbmLitTGeoTrackPropagator.h"

#include "base/CbmLitDefaultSettings.h"
#include "data/CbmLitTrackParam.h"
#include "interface/CbmLitTrackExtrapolator.h"
#include "propagation/CbmLitTGeoNavigator.h"
#include "propagation/CbmLitMaterialEffectsImp.h"
#include "propagation/CbmLitMaterialInfo.h"
#include "utils/CbmLitMath.h"
#include "utils/CbmLitMatrixMath.h"

#include <vector>
#include <cmath>
#include <iostream>

using std::cout;
using std::endl;


litfloat CbmLitTGeoTrackPropagator::MAXIMUM_PROPAGATION_STEP_SIZE = 10.;

CbmLitTGeoTrackPropagator::CbmLitTGeoTrackPropagator(
   TrackExtrapolatorPtr extrapolator):
   fExtrapolator(extrapolator)
{
   fNavigator = GeoNavigatorPtr(new CbmLitTGeoNavigator());
   fMaterial = MaterialEffectsPtr(new CbmLitMaterialEffectsImp());
}

CbmLitTGeoTrackPropagator::~CbmLitTGeoTrackPropagator()
{
}

LitStatus CbmLitTGeoTrackPropagator::Propagate(
   const CbmLitTrackParam* parIn,
   CbmLitTrackParam* parOut,
   litfloat zOut,
   int pdg,
   std::vector<litfloat>* F,
   litfloat* length)
{
   *parOut = *parIn;
   return Propagate(parOut, zOut, pdg, F, length);
}

LitStatus CbmLitTGeoTrackPropagator::Propagate(
   CbmLitTrackParam* par,
   litfloat zOut,
   int pdg,
   std::vector<litfloat>* F,
   litfloat* length)

{
   if (!IsParCorrect(par)) { return kLITERROR; }

   litfloat zIn = par->GetZ();
   litfloat dz = zOut - zIn;

   if(std::fabs(dz) < lit::CbmLitDefaultSettings::MINIMUM_PROPAGATION_DISTANCE) { return kLITSUCCESS; }

   // Check whether upstream or downstream
   // TODO check upstream/downstream
   bool downstream = dz > 0;

   if (F != NULL) {
      F->assign(25, 0.);
      (*F)[0] = 1.;
      (*F)[6] = 1.;
      (*F)[12] = 1.;
      (*F)[18] = 1.;
      (*F)[24] = 1.;
   }

   int nofSteps = int(std::abs(dz) / CbmLitTGeoTrackPropagator::MAXIMUM_PROPAGATION_STEP_SIZE);
   litfloat stepSize;
   if (nofSteps == 0) { stepSize = std::abs(dz); }
   else { stepSize = CbmLitTGeoTrackPropagator::MAXIMUM_PROPAGATION_STEP_SIZE; }
   litfloat z = zIn;

   if (length) *length = 0;

   // Loop over steps + additional step to propagate to virtual plane at zOut
   for (int iStep = 0; iStep < nofSteps + 1; iStep++) {
      if (!IsParCorrect(par)) {
         return kLITERROR;
      }
      // Check if already at exit position
      if (z == zOut) break;

      // Update current z position
      if (iStep != nofSteps) { z = (downstream) ? z + stepSize : z - stepSize; }
      else { z = zOut; }

      // Get intersections with materials for this step
      std::vector<CbmLitMaterialInfo> inter;
      if (fNavigator->FindIntersections(par, z, inter) == kLITERROR) {
         return kLITERROR;
      }

      // Loop over material layers
      for(unsigned int  iMat = 0; iMat < inter.size() ; iMat++) {
         CbmLitMaterialInfo mat = inter[iMat];

         // Check if track parameters are correct
         if (!IsParCorrect(par)) {
            return kLITERROR;
         }

         std::vector<litfloat>* Fnew = NULL;
         if (F != NULL) { Fnew = new std::vector<litfloat>(25, 0.); }
         // Extrapolate to the next boundary
         if (fExtrapolator->Extrapolate(par, mat.GetZpos(), Fnew) == kLITERROR) {
            return kLITERROR;
         }

         // Update transport matrix
         if (F != NULL) { UpdateF(*F, *Fnew); }
         if (Fnew != NULL) delete Fnew;

         // Add material effects
         fMaterial->Update(par, &mat, pdg, downstream);

         if (length) *length += mat.GetLength();
      }
   }

   if (!IsParCorrect(par)) { return kLITERROR; }
   else { return kLITSUCCESS; }
}

void CbmLitTGeoTrackPropagator::UpdateF(
   std::vector<litfloat>& F,
   const std::vector<litfloat>& newF)
{
   std::vector<litfloat> A(25);
   Mult25(newF, F, A);
   F.assign(A.begin(), A.end());
}

bool CbmLitTGeoTrackPropagator::IsParCorrect(
   const CbmLitTrackParam* par)
{
   litfloat maxSlope = 5.;
   litfloat minSlope = 1e-6;
   litfloat maxQp = 1000.; // p = 10 MeV

   if (std::abs(par->GetTx()) > maxSlope ||
         std::abs(par->GetTy()) > maxSlope ||
         std::abs(par->GetTx()) < minSlope ||
         std::abs(par->GetTy()) < minSlope ||
         std::abs(par->GetQp()) > maxQp) {
      return false;
   }

   if (std::isnan(par->GetX()) || std::isnan(par->GetY()) ||
         std::isnan(par->GetTx()) || std::isnan(par->GetTy()) ||
         std::isnan(par->GetQp())) { return false; }

   return true;
}
