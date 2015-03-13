/**
 * \file LitAddFiltration.h
 *
 * \brief Functions for Kalman filter update step.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */
#ifndef LITFILTRATION_H_
#define LITFILTRATION_H_

#include "LitPixelHit.h"
#include "LitStripHit.h"
#include "LitScalPixelHit.h"
#include "LitTrackParam.h"

namespace lit {
namespace parallel {

/**
 * \fn template<class T> inline void LitFiltration (LitTrackParam<T>& par, const LitPixelHit<T> &hit, T& chiSq)
 * \brief Function implements Kalman filter update step for pixel hit.
 *
 * It updates track parameters and covariance matrix and
 * calculates contribution to chi-square from this hit.
 *
 * \param[in,out] par Reference to track parameters.
 * \param[in] hit Reference to pixel hit.
 * \param[out] chiSq Contribution to chi-square.
 */
template<class T>
inline void LitFiltration (
   LitTrackParam<T>& par,
   const LitPixelHit<T> &hit,
   T& chiSq)
{
   static const T ONE = 1., TWO = 2.;

   T dxx = hit.Dx * hit.Dx;
   T dxy = hit.Dxy;
   T dyy = hit.Dy * hit.Dy;

   // calculate residuals
   T dx = hit.X - par.X;
   T dy = hit.Y - par.Y;

   // Calculate and inverse residual covariance matrix
   T t = ONE / (dxx * dyy + dxx * par.C5 + dyy * par.C0 + par.C0 * par.C5 -
                dxy * dxy - TWO * dxy * par.C1 - par.C1 * par.C1);
   T R00 = (dyy + par.C5) * t;
   T R01 = -(dxy + par.C1) * t;
   T R11 = (dxx + par.C0) * t;

   // Calculate Kalman gain matrix
   T K00 = par.C0 * R00 + par.C1 * R01;
   T K01 = par.C0 * R01 + par.C1 * R11;
   T K10 = par.C1 * R00 + par.C5 * R01;
   T K11 = par.C1 * R01 + par.C5 * R11;
   T K20 = par.C2 * R00 + par.C6 * R01;
   T K21 = par.C2 * R01 + par.C6 * R11;
   T K30 = par.C3 * R00 + par.C7 * R01;
   T K31 = par.C3 * R01 + par.C7 * R11;
   T K40 = par.C4 * R00 + par.C8 * R01;
   T K41 = par.C4 * R01 + par.C8 * R11;

   // Calculate filtered state vector
   par.X  += K00 * dx + K01 * dy;
   par.Y  += K10 * dx + K11 * dy;
   par.Tx += K20 * dx + K21 * dy;
   par.Ty += K30 * dx + K31 * dy;
   par.Qp += K40 * dx + K41 * dy;

   // Calculate filtered covariance matrix
   T cIn[15] = {par.C0,  par.C1,  par.C2,  par.C3,  par.C4,
                par.C5,  par.C6,  par.C7,  par.C8,  par.C9,
                par.C10, par.C11, par.C12, par.C13, par.C14};

   par.C0  += -K00 * cIn[0] - K01 * cIn[1];
   par.C1  += -K00 * cIn[1] - K01 * cIn[5];
   par.C2  += -K00 * cIn[2] - K01 * cIn[6];
   par.C3  += -K00 * cIn[3] - K01 * cIn[7];
   par.C4  += -K00 * cIn[4] - K01 * cIn[8];

   par.C5  += -K11 * cIn[5] - K10 * cIn[1];
   par.C6  += -K11 * cIn[6] - K10 * cIn[2];
   par.C7  += -K11 * cIn[7] - K10 * cIn[3];
   par.C8  += -K11 * cIn[8] - K10 * cIn[4];

   par.C9  += -K20 * cIn[2] - K21 * cIn[6];
   par.C10 += -K20 * cIn[3] - K21 * cIn[7];
   par.C11 += -K20 * cIn[4] - K21 * cIn[8];

   par.C12 += -K30 * cIn[3] - K31 * cIn[7];
   par.C13 += -K30 * cIn[4] - K31 * cIn[8];

   par.C14 += -K40 * cIn[4] - K41 * cIn[8];

   // Calculate chi-square
   T xmx = hit.X - par.X;
   T ymy = hit.Y - par.Y;
   T norm = dxx * dyy - dxx * par.C5 - dyy * par.C0 + par.C0 * par.C5
              - dxy * dxy + TWO * dxy * par.C1 - par.C1 * par.C1;
   chiSq = ((xmx * (dyy - par.C5) - ymy * (dxy - par.C1)) * xmx
            + (-xmx * (dxy - par.C1) + ymy * (dxx - par.C0)) * ymy) / norm;
}

/**
 * \fn template<class T> inline void LitFiltration(LitTrackParam<T>& par, const LitStripHit<T>& hit, T& chiSq)
 * \brief Function implements Kalman filter update step for strip hit.
 *
 * It updates track parameters and covariance matrix and
 * calculates contribution to chi-square from this hit.
 *
 * \param[in,out] par Reference to track parameters.
 * \param[in] hit Reference to strip hit.
 * \param[out] chiSq Contribution to chi-square.
 */
template<class T>
inline void LitFiltration(
   LitTrackParam<T>& par,
   const LitStripHit<T>& hit,
   T& chiSq)
{
   static const T ONE = 1., TWO = 2.;

   T duu = hit.Du * hit.Du;
   T phiCosSq = hit.phiCos * hit.phiCos;
   T phiSinSq = hit.phiSin * hit.phiSin;
   T phi2SinCos = TWO * hit.phiCos * hit.phiSin;

   // residual
   T r = hit.U - par.C0 * hit.phiCos - par.C1 * hit.phiSin;
   T norm = duu + par.C0 * phiCosSq + phi2SinCos * par.C1 + par.C5 * phiSinSq;
   //  myf norm = duu + cIn[0] * phiCos + cIn[5] * phiSin;
   T R = rcp(norm);

   // Calculate Kalman gain matrix
   T K0 = par.C0 * hit.phiCos + par.C1 * hit.phiSin;
   T K1 = par.C1 * hit.phiCos + par.C5 * hit.phiSin;
   T K2 = par.C2 * hit.phiCos + par.C6 * hit.phiSin;
   T K3 = par.C3 * hit.phiCos + par.C7 * hit.phiSin;
   T K4 = par.C4 * hit.phiCos + par.C8 * hit.phiSin;

   T KR0 = K0 * R;
   T KR1 = K1 * R;
   T KR2 = K2 * R;
   T KR3 = K3 * R;
   T KR4 = K4 * R;

   // Calculate filtered state vector
   par.X += KR0 * r;
   par.Y += KR1 * r;
   par.Tx += KR2 * r;
   par.Ty += KR3 * r;
   par.Qp += KR4 * r;

   // Calculate filtered covariance matrix
   par.C0 -= KR0 * K0;
   par.C1 -= KR0 * K1;
   par.C2 -= KR0 * K2;
   par.C3 -= KR0 * K3;
   par.C4 -= KR0 * K4;

   par.C5 -= KR1 * K1;
   par.C6 -= KR1 * K2;
   par.C7 -= KR1 * K3;
   par.C8 -= KR1 * K4;

   par.C9 -= KR2 * K2;
   par.C10 -= KR2 * K3;
   par.C11 -= KR2 * K4;

   par.C12 -= KR3 * K3;
   par.C13 -= KR3 * K4;

   par.C14 -= KR4 * K4;

   // Calculate chi-square
   T ru = hit.U - par.X * hit.phiCos - par.Y * hit.phiSin;
   chiSq = (ru * ru) / (duu - phiCosSq * par.C0 - phi2SinCos * par.C1 - phiSinSq * par.C5);
}

/**
 * \fn inline void LitFiltration (LitTrackParamScal& par, const LitScalPixelHit &hit, T& chiSq)
 * \brief Function implements Kalman filter update step for scalar pixel hit. *
 * \param[in,out] par Reference to track parameters.
 * \param[in] hit Reference to scalar pixel hit.
 * \param[out] chiSq Contribution to chi-square.
 */
inline void LitFiltration (
   LitTrackParamScal& par,
   const LitScalPixelHit &hit,
   fscal& chiSq)
{
   LitPixelHitScal pixelHit;
   pixelHit.X = hit.X;
   pixelHit.Y = hit.Y;
   pixelHit.Z = hit.Z;
   pixelHit.Dx = hit.Dx;
   pixelHit.Dy = hit.Dy;
   pixelHit.Dxy = hit.Dxy;
   LitFiltration<fscal>(par, pixelHit, chiSq);
}

} // namespace parallel
} // namespace lit
#endif /* LITFILTRATION_H_ */
