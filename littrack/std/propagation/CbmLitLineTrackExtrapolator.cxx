/** CbmLitLineTrackExtrapolator.cxx
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2007
 **
 **/

#include "propagation/CbmLitLineTrackExtrapolator.h"

#include "data/CbmLitTrackParam.h"

CbmLitLineTrackExtrapolator::CbmLitLineTrackExtrapolator()
{
}

CbmLitLineTrackExtrapolator::~CbmLitLineTrackExtrapolator()
{
}

LitStatus CbmLitLineTrackExtrapolator::Extrapolate(
   const CbmLitTrackParam* parIn,
   CbmLitTrackParam* parOut,
   litfloat zOut,
   std::vector<litfloat>* F)
{
   *parOut = *parIn;
   return Extrapolate(parOut, zOut, F);
}

LitStatus CbmLitLineTrackExtrapolator::Extrapolate(
   CbmLitTrackParam* par,
   litfloat zOut,
   std::vector<litfloat>* F)
{
   litfloat X[5] = { par->GetX(), par->GetY(),
                par->GetTx(), par->GetTy(),
                par->GetQp()
              };

   litfloat dz = zOut - par->GetZ();

   //transport state vector F*X*F.T()
   X[0] = X[0] + dz * X[2];
   X[1] = X[1] + dz * X[3];

   par->SetX(X[0]);
   par->SetY(X[1]);

   std::vector<litfloat> C = par->GetCovMatrix();
   //transport covariance matrix F*C*F.T()
   litfloat t3 = C[2] + dz * C[9];
   litfloat t7 = dz * C[10];
   litfloat t8 = C[3] + t7;
   litfloat t19 = C[7] + dz * C[12];
   C[0] = C[0] + dz * C[2] + t3 * dz;
   C[1] = C[1] + dz * C[6] + t8 * dz;
   C[2] = t3;
   C[3] = t8;
   C[4] = C[4] + dz * C[11];
   C[5] = C[5] + dz * C[7] + t19 * dz;
   C[6] = C[6] + t7;
   C[7] = t19;
   C[8] = C[8] + dz * C[13];

   par->SetCovMatrix(C);
   par->SetZ(zOut);

   // Transport matrix calculation
   if (F != NULL) {
      F->assign(25, 0.);
      (*F)[0] = 1.;
      (*F)[6] = 1.;
      (*F)[12] = 1.;
      (*F)[18] = 1.;
      (*F)[24] = 1.;
      (*F)[2] = dz;
      (*F)[8] = dz;
   }

   return kLITSUCCESS;
}
