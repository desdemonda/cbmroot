/** CbmLitTGeoNavigator.cxx
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **/

#include "propagation/CbmLitTGeoNavigator.h"

#include "base/CbmLitDefaultSettings.h"
#include "data/CbmLitTrackParam.h"

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoMaterial.h"
#include "TGeoVolume.h"

#include <iostream>
#include <cmath>

CbmLitTGeoNavigator::CbmLitTGeoNavigator()
{
}

CbmLitTGeoNavigator::~CbmLitTGeoNavigator()
{
}

LitStatus CbmLitTGeoNavigator::FindIntersections(
   const CbmLitTrackParam* par,
   litfloat zOut,
   std::vector<CbmLitMaterialInfo>& inter)
{
   Bool_t downstream = zOut >= par->GetZ();
  // std::cout << "zOut=" << zOut << " Z=" << par->GetZ() << " downstream=" << downstream << std::endl;

   InitTrack(par, downstream);
   if(gGeoManager->IsOutside()) { return kLITERROR; }

   CbmLitMaterialInfo stepInfo;
   bool last = false;

   std::cout.precision(7);

   do {
      gGeoManager->PushPoint();
      stepInfo = MakeStep();
//      std::cout << "stepInfo=" << stepInfo.ToString();
      // Check if outside of the geometry
      if(gGeoManager->IsOutside()) {
//         std::cout << "Error! CbmLitTGeoNavigator::FindIntersections: Outside geometry.\n";
         gGeoManager->PopDummy();
         return kLITERROR;
      }
      // Check for NaN values
      if (std::isnan(gGeoManager->GetCurrentPoint()[0]) ||
            std::isnan(gGeoManager->GetCurrentPoint()[1]) ||
            std::isnan(gGeoManager->GetCurrentPoint()[2]) ) {
//         std::cout << "Error! CbmLitTGeoNavigator::FindIntersections: NaN values.\n";
         gGeoManager->PopDummy();
         return kLITERROR;
      }
      // Check if we currently at the output position
      Bool_t away = (downstream) ? stepInfo.GetZpos() >= zOut : stepInfo.GetZpos() <= zOut;
      if (away) { //|| gGeoManager->IsNullStep()){
         gGeoManager->PopPoint();
         litfloat l = CalcLength(zOut);
         stepInfo.SetLength(l);
         stepInfo.SetZpos(zOut);
         last = true;
      } else {
         gGeoManager->PopDummy();
      }
      inter.push_back(stepInfo);
   } while (!last);
   return kLITSUCCESS;
}

void CbmLitTGeoNavigator::InitTrack(
   const CbmLitTrackParam* par,
   Bool_t downstream) const
{
   litfloat nx, ny, nz;
   par->GetDirCos(nx, ny, nz);
   // Change track direction for upstream
   if (!downstream) {
      nx = -nx;
      ny = -ny;
      nz = -nz;
   }
   gGeoManager->InitTrack(par->GetX(), par->GetY(), par->GetZ(), nx, ny, nz);
}

CbmLitMaterialInfo CbmLitTGeoNavigator::MakeStep(
   litfloat step) const
{
   // fill current material information and then make a step
   CbmLitMaterialInfo matInfo;
   TGeoMaterial* mat = gGeoManager->GetCurrentNode()->GetMedium()->GetMaterial();
   matInfo.SetRL(mat->GetRadLen());
   matInfo.SetRho(mat->GetDensity());
   matInfo.SetZ(mat->GetZ());
   matInfo.SetA(mat->GetA());
   matInfo.SetName(gGeoManager->GetCurrentNode()->GetName());

   if (step == 0.) {
      gGeoManager->FindNextBoundaryAndStep(lit::CbmLitDefaultSettings::MAXIMUM_TGEO_NAVIGATION_DISTANCE);
   } else {
      gGeoManager->SetStep(step);
      gGeoManager->Step(kFALSE);
   }

   matInfo.SetLength(gGeoManager->GetStep());
   matInfo.SetZpos(gGeoManager->GetCurrentPoint()[2]);

   return matInfo;
}

litfloat CbmLitTGeoNavigator::CalcLength(
   litfloat zOut) const
{
   //find intersection point of straight line with plane
   litfloat nx = gGeoManager->GetCurrentDirection()[0];
   litfloat ny = gGeoManager->GetCurrentDirection()[1];
   litfloat nz = gGeoManager->GetCurrentDirection()[2];
   litfloat z = gGeoManager->GetCurrentPoint()[2];

   litfloat t0 = (zOut - z) / nz;

   litfloat dx = nx * t0;
   litfloat dy = ny * t0;
   litfloat dz = (zOut - z);//nz * t0;

   //calculate distance between two points
   return std::sqrt( dx * dx + dy * dy + dz * dz);
}
