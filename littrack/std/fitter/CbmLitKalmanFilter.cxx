/** CbmLitKalmanFilter.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/

#include "fitter/CbmLitKalmanFilter.h"

#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrackParam.h"
#include "utils/CbmLitMatrixMath.h"

#include <iostream>
#include <cmath>

CbmLitKalmanFilter::CbmLitKalmanFilter()
{
}

CbmLitKalmanFilter::~CbmLitKalmanFilter()
{
}

LitStatus CbmLitKalmanFilter::Update(
   const CbmLitTrackParam* parIn,
   CbmLitTrackParam* parOut,
   const CbmLitHit* hit,
   litfloat& chiSq)
{
   *parOut = *parIn;
   return Update(parOut, hit, chiSq);
}

LitStatus CbmLitKalmanFilter::Update(
   CbmLitTrackParam* par,
   const CbmLitHit* hit,
   litfloat& chiSq)
{
   LitStatus result = kLITSUCCESS;
   if (hit->GetType() == kLITSTRIPHIT) {
      result = Update(par, static_cast<const CbmLitStripHit*>(hit), chiSq);
   } else if (hit->GetType() == kLITPIXELHIT) {
      result = Update(par, static_cast<const CbmLitPixelHit*>(hit), chiSq);
   }
   return result;
}

LitStatus CbmLitKalmanFilter::Update(
   CbmLitTrackParam* par,
   const CbmLitPixelHit* hit,
   litfloat& chiSq)
{
   std::vector<litfloat> cIn = par->GetCovMatrix();

   static const litfloat ONE = 1., TWO = 2.;

   litfloat dxx = hit->GetDx() * hit->GetDx();
   litfloat dxy = hit->GetDxy();
   litfloat dyy = hit->GetDy() * hit->GetDy();

   // calculate residuals
   litfloat dx = hit->GetX() - par->GetX();
   litfloat dy = hit->GetY() - par->GetY();

   // Calculate and inverse residual covariance matrix
   litfloat t = ONE / (dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] -
                  dxy * dxy - TWO * dxy * cIn[1] - cIn[1] * cIn[1]);
   litfloat R00 = (dyy + cIn[5]) * t;
   litfloat R01 = -(dxy + cIn[1]) * t;
   litfloat R11 = (dxx + cIn[0]) * t;

   // Calculate Kalman gain matrix
   litfloat K00 = cIn[0] * R00 + cIn[1] * R01;
   litfloat K01 = cIn[0] * R01 + cIn[1] * R11;
   litfloat K10 = cIn[1] * R00 + cIn[5] * R01;
   litfloat K11 = cIn[1] * R01 + cIn[5] * R11;
   litfloat K20 = cIn[2] * R00 + cIn[6] * R01;
   litfloat K21 = cIn[2] * R01 + cIn[6] * R11;
   litfloat K30 = cIn[3] * R00 + cIn[7] * R01;
   litfloat K31 = cIn[3] * R01 + cIn[7] * R11;
   litfloat K40 = cIn[4] * R00 + cIn[8] * R01;
   litfloat K41 = cIn[4] * R01 + cIn[8] * R11;

   // Calculate filtered state vector
   litfloat xOut[5] = { par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp() };
   xOut[0] += K00 * dx + K01 * dy;
   xOut[1] += K10 * dx + K11 * dy;
   xOut[2] += K20 * dx + K21 * dy;
   xOut[3] += K30 * dx + K31 * dy;
   xOut[4] += K40 * dx + K41 * dy;

   // Calculate filtered covariance matrix
   std::vector<litfloat> cOut = cIn;

   cOut[0]  += -K00 * cIn[0] - K01 * cIn[1];
   cOut[1]  += -K00 * cIn[1] - K01 * cIn[5];
   cOut[2]  += -K00 * cIn[2] - K01 * cIn[6];
   cOut[3]  += -K00 * cIn[3] - K01 * cIn[7];
   cOut[4]  += -K00 * cIn[4] - K01 * cIn[8];

   cOut[5]  += -K11 * cIn[5] - K10 * cIn[1];
   cOut[6]  += -K11 * cIn[6] - K10 * cIn[2];
   cOut[7]  += -K11 * cIn[7] - K10 * cIn[3];
   cOut[8]  += -K11 * cIn[8] - K10 * cIn[4];

   cOut[9]  += -K20 * cIn[2] - K21 * cIn[6];
   cOut[10] += -K20 * cIn[3] - K21 * cIn[7];
   cOut[11] += -K20 * cIn[4] - K21 * cIn[8];

   cOut[12] += -K30 * cIn[3] - K31 * cIn[7];
   cOut[13] += -K30 * cIn[4] - K31 * cIn[8];

   cOut[14] += -K40 * cIn[4] - K41 * cIn[8];

   // Copy filtered state to output
   par->SetX(xOut[0]);
   par->SetY(xOut[1]);
   par->SetTx(xOut[2]);
   par->SetTy(xOut[3]);
   par->SetQp(xOut[4]);
   par->SetCovMatrix(cOut);

   // Calculate chi-square
   litfloat xmx = hit->GetX() - par->GetX();
   litfloat ymy = hit->GetY() - par->GetY();
   litfloat C0 = cOut[0];
   litfloat C1 = cOut[1];
   litfloat C5 = cOut[5];

   litfloat norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5
              - dxy * dxy + 2 * dxy * C1 - C1 * C1;

   chiSq = ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx
            +(-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;

   return kLITSUCCESS;
}


LitStatus CbmLitKalmanFilter::UpdateWMF(
   CbmLitTrackParam* par,
   const CbmLitPixelHit* hit,
   litfloat& chiSq)
{
   litfloat xIn[5] = { par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp() };

   std::vector<litfloat> cIn = par->GetCovMatrix();
   std::vector<litfloat> cInInv = par->GetCovMatrix();

   litfloat dxx = hit->GetDx() * hit->GetDx();
   litfloat dxy = hit->GetDxy();
   litfloat dyy = hit->GetDy() * hit->GetDy();

   // Inverse predicted cov matrix
   InvSym15(cInInv);
   // Calculate C1
   std::vector<litfloat> C1 = cInInv;
   litfloat det = dxx * dyy - dxy * dxy;
   C1[0] += dyy / det;
   C1[1] += -dxy / det;
   C1[5] += dxx / det;
   // Inverse C1 -> output updated covariance matrix
   InvSym15(C1);

   std::vector<litfloat> t(5);
   t[0] = cInInv[0]*par->GetX() + cInInv[1]*par->GetY() + cInInv[2]*par->GetTx()
          + cInInv[3]*par->GetTy() + cInInv[4]*par->GetQp()
          + (dyy * hit->GetX() - dxy * hit->GetY()) / det;
   t[1] = cInInv[1]*par->GetX() + cInInv[5]*par->GetY() + cInInv[6]*par->GetTx()
          + cInInv[7]*par->GetTy() + cInInv[8]*par->GetQp()
          + (- dxy * hit->GetX() + dxx * hit->GetY()) / det;
   t[2] = cInInv[2]*par->GetX() + cInInv[6]*par->GetY() + cInInv[9]*par->GetTx()
          + cInInv[10]*par->GetTy() + cInInv[11]*par->GetQp();
   t[3] = cInInv[3]*par->GetX() + cInInv[7]*par->GetY() + cInInv[10]*par->GetTx()
          + cInInv[12]*par->GetTy() + cInInv[13]*par->GetQp();
   t[4] = cInInv[4]*par->GetX() + cInInv[8]*par->GetY() + cInInv[11]*par->GetTx()
          + cInInv[13]*par->GetTy() + cInInv[14]*par->GetQp();

   std::vector<litfloat> xOut(5);
   Mult15On5(C1, t, xOut);

   // Copy filtered state to output
   par->SetX(xOut[0]);
   par->SetY(xOut[1]);
   par->SetTx(xOut[2]);
   par->SetTy(xOut[3]);
   par->SetQp(xOut[4]);
   par->SetCovMatrix(C1);

   // Calculate chi square
   litfloat dx0 = xOut[0] - xIn[0];
   litfloat dx1 = xOut[1] - xIn[1];
   litfloat dx2 = xOut[2] - xIn[2];
   litfloat dx3 = xOut[3] - xIn[3];
   litfloat dx4 = xOut[4] - xIn[4];
   litfloat xmx = hit->GetX() - par->GetX();
   litfloat ymy = hit->GetY() - par->GetY();
   chiSq = ((xmx * dyy - ymy * dxy) * xmx + (-xmx * dxy + ymy * dxx) * ymy) / det
           + (dx0 * cInInv[0] + dx1 * cInInv[1] + dx2 * cInInv[2 ] + dx3 * cInInv[3 ] + dx4 * cInInv[4 ]) * dx0
           + (dx0 * cInInv[1] + dx1 * cInInv[5] + dx2 * cInInv[6 ] + dx3 * cInInv[7 ] + dx4 * cInInv[8 ]) * dx1
           + (dx0 * cInInv[2] + dx1 * cInInv[6] + dx2 * cInInv[9 ] + dx3 * cInInv[10] + dx4 * cInInv[11]) * dx2
           + (dx0 * cInInv[3] + dx1 * cInInv[7] + dx2 * cInInv[10] + dx3 * cInInv[12] + dx4 * cInInv[13]) * dx3
           + (dx0 * cInInv[4] + dx1 * cInInv[8] + dx2 * cInInv[11] + dx3 * cInInv[13] + dx4 * cInInv[14]) * dx4;

   return kLITSUCCESS;
}

LitStatus CbmLitKalmanFilter::Update(
   CbmLitTrackParam* par,
   const CbmLitStripHit* hit,
   litfloat& chiSq)
{
   litfloat xIn[5] = { par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp() };
   std::vector<litfloat> cIn = par->GetCovMatrix();

   litfloat u = hit->GetU();
   litfloat duu = hit->GetDu() * hit->GetDu();
   litfloat phiCos = hit->GetCosPhi();
   litfloat phiSin = hit->GetSinPhi();
   litfloat phiCosSq = phiCos * phiCos;
   litfloat phiSinSq = phiSin * phiSin;
   litfloat phi2SinCos = 2 * phiCos * phiSin;

   // Inverted covariance matrix of predicted residual
   litfloat R = 1./(duu + cIn[0] * phiCosSq + phi2SinCos * cIn[1] + cIn[5] * phiSinSq);

   // Calculate Kalman gain matrix
   litfloat K0 = cIn[0] * phiCos + cIn[1] * phiSin;
   litfloat K1 = cIn[1] * phiCos + cIn[5] * phiSin;
   litfloat K2 = cIn[2] * phiCos + cIn[6] * phiSin;
   litfloat K3 = cIn[3] * phiCos + cIn[7] * phiSin;
   litfloat K4 = cIn[4] * phiCos + cIn[8] * phiSin;

   litfloat KR0 = K0 * R;
   litfloat KR1 = K1 * R;
   litfloat KR2 = K2 * R;
   litfloat KR3 = K3 * R;
   litfloat KR4 = K4 * R;

   // Residual of predictions
   litfloat r = u - xIn[0] * phiCos - xIn[1] * phiSin;

   // Calculate filtered state vector
   std::vector<litfloat> xOut(5);
   xOut[0] = xIn[0] + KR0 * r;
   xOut[1] = xIn[1] + KR1 * r;
   xOut[2] = xIn[2] + KR2 * r;
   xOut[3] = xIn[3] + KR3 * r;
   xOut[4] = xIn[4] + KR4 * r;

   // Calculate filtered covariance matrix
   std::vector<litfloat> cOut(15);
   cOut[0] = cIn[0] - KR0 * K0;
   cOut[1] = cIn[1] - KR0 * K1;
   cOut[2] = cIn[2] - KR0 * K2;
   cOut[3] = cIn[3] - KR0 * K3;
   cOut[4] = cIn[4] - KR0 * K4;

   cOut[5] = cIn[5] - KR1 * K1;
   cOut[6] = cIn[6] - KR1 * K2;
   cOut[7] = cIn[7] - KR1 * K3;
   cOut[8] = cIn[8] - KR1 * K4;

   cOut[9] = cIn[9] - KR2 * K2;
   cOut[10] = cIn[10] - KR2 * K3;
   cOut[11] = cIn[11] - KR2 * K4;

   cOut[12] = cIn[12] - KR3 * K3;
   cOut[13] = cIn[13] - KR3 * K4;

   cOut[14] = cIn[14] - KR4 * K4;

   // Copy filtered state to output
   par->SetX(xOut[0]);
   par->SetY(xOut[1]);
   par->SetTx(xOut[2]);
   par->SetTy(xOut[3]);
   par->SetQp(xOut[4]);
   par->SetCovMatrix(cOut);

   // Filtered resuduals
   litfloat ru = u - xOut[0] * phiCos - xOut[1] * phiSin;

   // Calculate chi-square
   chiSq = (ru * ru) / (duu - phiCosSq*cOut[0] - phi2SinCos*cOut[1] - phiSinSq*cOut[5]);

   return kLITSUCCESS;
}

LitStatus CbmLitKalmanFilter::UpdateWMF(
   CbmLitTrackParam* par,
   const CbmLitStripHit* hit,
   litfloat& chiSq)
{
   litfloat xIn[5] = { par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp() };

   std::vector<litfloat> cIn = par->GetCovMatrix();
   std::vector<litfloat> cInInv = par->GetCovMatrix();

   litfloat duu = hit->GetDu() * hit->GetDu();
   litfloat phiCos = hit->GetCosPhi();
   litfloat phiSin = hit->GetSinPhi();

   // Inverse predicted cov matrix
   InvSym15(cInInv);
   // Calculate C1
   std::vector<litfloat> C1 = cInInv;
   C1[0] += phiCos*phiCos / duu;
   C1[1] += phiCos*phiSin / duu;
   C1[5] += phiSin*phiSin / duu;
   // Inverse C1 -> output updated covariance matrix
   InvSym15(C1);

   std::vector<litfloat> t(5);
   t[0] = cInInv[0]*par->GetX() + cInInv[1]*par->GetY() + cInInv[2]*par->GetTx()
          + cInInv[3]*par->GetTy() + cInInv[4]*par->GetQp()
          + phiCos*hit->GetU()/duu;
   t[1] = cInInv[1]*par->GetX() + cInInv[5]*par->GetY() + cInInv[6]*par->GetTx()
          + cInInv[7]*par->GetTy() + cInInv[8]*par->GetQp()
          + phiSin*hit->GetU()/duu;
   t[2] = cInInv[2]*par->GetX() + cInInv[6]*par->GetY() + cInInv[9]*par->GetTx()
          + cInInv[10]*par->GetTy() + cInInv[11]*par->GetQp();
   t[3] = cInInv[3]*par->GetX() + cInInv[7]*par->GetY() + cInInv[10]*par->GetTx()
          + cInInv[12]*par->GetTy() + cInInv[13]*par->GetQp();
   t[4] = cInInv[4]*par->GetX() + cInInv[8]*par->GetY() + cInInv[11]*par->GetTx()
          + cInInv[13]*par->GetTy() + cInInv[14]*par->GetQp();

   std::vector<litfloat> xOut(5);
   Mult15On5(C1, t, xOut);

   // Copy filtered state to output
   par->SetX(xOut[0]);
   par->SetY(xOut[1]);
   par->SetTx(xOut[2]);
   par->SetTy(xOut[3]);
   par->SetQp(xOut[4]);
   par->SetCovMatrix(C1);

   // Calculate chi square
   litfloat zeta = hit->GetU() - phiCos*xOut[0] - phiSin*xOut[1];
   litfloat dx0 = xOut[0] - xIn[0];
   litfloat dx1 = xOut[1] - xIn[1];
   litfloat dx2 = xOut[2] - xIn[2];
   litfloat dx3 = xOut[3] - xIn[3];
   litfloat dx4 = xOut[4] - xIn[4];
   chiSq = zeta * zeta / duu
           + (dx0 * cInInv[0] + dx1 * cInInv[1] + dx2 * cInInv[2 ] + dx3 * cInInv[3 ] + dx4 * cInInv[4 ]) * dx0
           + (dx0 * cInInv[1] + dx1 * cInInv[5] + dx2 * cInInv[6 ] + dx3 * cInInv[7 ] + dx4 * cInInv[8 ]) * dx1
           + (dx0 * cInInv[2] + dx1 * cInInv[6] + dx2 * cInInv[9 ] + dx3 * cInInv[10] + dx4 * cInInv[11]) * dx2
           + (dx0 * cInInv[3] + dx1 * cInInv[7] + dx2 * cInInv[10] + dx3 * cInInv[12] + dx4 * cInInv[13]) * dx3
           + (dx0 * cInInv[4] + dx1 * cInInv[8] + dx2 * cInInv[11] + dx3 * cInInv[13] + dx4 * cInInv[14]) * dx4;

   return kLITSUCCESS;
}
