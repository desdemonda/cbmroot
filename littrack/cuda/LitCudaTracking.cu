#ifndef LITCUDATRACKING_H_
#define LITCUDATRACKING_H_

#include <cuda.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "LitCudaHit.h"
#include "LitCudaTrack.h"
#include "LitCudaMaterialInfo.h"
#include "LitCudaField.cu"


//#ifndef LITCUDAADDMATERIAL_H_
//#define LITCUDAADDMATERIAL_H_

//#include "LitCudaTrackParam.h"
//#include "LitCudaMaterialInfo.h"

/* Function calculates effects from the material on the
 * track parameters and its covariance matrix.
 * @param par Pointer to INPUT/OUTPUT track parameters.
 * @param mat Pointer to material
 */
__device__ void LitCudaAddMaterial(
   LitCudaTrackParam* par,
   const LitCudaMaterialInfo* mat)
{
   const float ZERO = 0.0, ONE = 1., TWO = 2.;
   const float mass = 0.105658369; // muon mass [GeV/c]
   const float massSq = 0.105658369 * 0.105658369; // muon mass squared
   const float C1_2 = 0.5, C1_3 = 1./3.;
   const float me = 0.000511; // Electron mass [GeV/c]
   const float ratio = me / mass;

   float p = copysignf(1./par->Qp, par->Qp); // Momentum [GeV/c]
   float E = sqrtf(massSq + p * p);
   float beta = p / E;
   float betaSq = beta * beta;
   float gamma = E / mass;
   float gammaSq = gamma * gamma;

   //scale material thickness
   float norm = sqrtf(ONE + par->Tx * par->Tx + par->Ty * par->Ty);
   float thickness = norm * mat->Thickness;
   float radThick = thickness / mat->X0;
   float sqrtRadThick = sqrtf(radThick);//mat.SqrtRadThick;
   float logRadThick = logf(radThick);//mat.LogRadThick;

   /*
    * Energy loss corrections
    */

   // Bethe-Block
   const float K = 0.000307075; // GeV * g^-1 * cm^2
   float Tmax = (2 * me * betaSq * gammaSq) / (ONE + TWO * gamma * ratio + ratio * ratio);

   // density correction
   float dc = ZERO;
// if (p > 0.5) { // for particles above 1 Gev
   const float c7 = 28.816;
   const float c8 = 1e-9;
   float hwp = c7 * sqrtf(mat->Rho * mat->Z / mat->A) * c8; // GeV
   dc = logf(hwp/mat->I) + logf(beta*gamma) - C1_2;
// }

   float bbLoss = K * (mat->Z / mat->A) * (1./betaSq) *
                  (C1_2 * logf(TWO * me * betaSq * gammaSq * Tmax / (mat->I * mat->I)) - betaSq - dc);

   // Bethe-Heitler
// float bhLoss = (E * ratio * ratio)/(mat.X0 * mat.Rho);
   float bhLoss = ZERO;

   // Pair production approximation
// static const float c3 = 7e-5;
// float ppLoss = c3 * E / (mat.X0 * mat.Rho);
   float ppLoss = ZERO;

   // Integrated value of the energy loss
   float energyLoss = (bbLoss + bhLoss + ppLoss) * mat->Rho * thickness;

   // Correct Q/p value due to energy loss
   float Enew = E - energyLoss;
   float pnew = sqrtf(Enew * Enew - massSq);
   par->Qp = copysignf(1./pnew, par->Qp);

   // Calculate Q/p correction in the covariance matrix
   float betanew = pnew / Enew;
   float betaSqnew = betanew * betanew;
   float gammanew = Enew / mass;
// float gammaSqnew = gammanew * gammanew;

   // Calculate xi factor (KeV).
   const float c4 = 153.5;
   float XI = (c4 * mat->Z * thickness * mat->Rho)/(mat->A * betaSqnew);

   // Maximum energy transfer to atomic electron (KeV).
   float etanew = betanew * gammanew;
   float etaSqnew = etanew * etanew;
   float F1 = TWO * me * etaSqnew;
   float F2 = ONE + TWO * ratio * gammanew + ratio * ratio;
   const float c5 = 1e6;
   float emax = c5 * F1/F2;

   const float c6 = 1e-12;
   float dedxSq = XI * emax * (ONE - C1_2 * betaSqnew) * c6;

   float p2 = pnew * pnew;
   float p6 = p2 * p2 * p2;
   float qpCorr = (Enew * Enew * dedxSq) / p6;
   par->C14 += qpCorr;
   // end calculate Q/p correction in the covariance matrix

   /*
    * End energy loss corrections
    */

   /*
    * Multiple scattering corrections
    */
   float tx = par->Tx;
   float ty = par->Ty;
   float bcp = betanew * pnew;
   const float c1 = 0.0136, c2 = 0.038;
   float theta = c1 * (1./bcp) * sqrtRadThick * (ONE + c2 * logRadThick);
   float thetaSq = theta * theta;

   float t = ONE + tx * tx + ty * ty;

   float Q33 = (1 + tx * tx) * t * thetaSq;
   float Q44 = (1 + ty * ty) * t * thetaSq;
   float Q34 = tx * ty * t * thetaSq;

   float T23 = thickness * thickness * C1_3;
   float T2 = thickness * C1_2;

   par->C0 += Q33 * T23;
   par->C1 += Q34 * T23;
   par->C2 += Q33 * T2;
   par->C3 += Q34 * T2;

   par->C5 += Q44 * T23;
   par->C6 += Q34 * T2;
   par->C7 += Q44 * T2;

   par->C9 += Q33;
   par->C10 += Q34;

   par->C12 += Q44;

   /*
    * End multiple scattering corrections
    */
}

//#endif /* LITCUDAADDMATERIAL_H_ */




//#ifndef LITCUDAEXTRAPOLATION_H_
//#define LITCUDAEXTRAPOLATION_H_

//#include "LitCudaTrackParam.h"
//#include "LitCudaField.h"

/* Line track extrapolation for CUDA version of the tracking.
 *@param par Input/Output track parameters.
 *@param zOut Z position to extrapolate to.
 */
__device__ void LitCudaLineExtrapolation(
   LitCudaTrackParam* par,
   float zOut)
{
   float dz = zOut - par->Z;

   // transport state vector F*X*F.T()
   par->X += dz * par->Tx;
   par->Y += dz * par->Ty;

   // transport covariance matrix F*C*F.T()
   float t3 = par->C2 + dz * par->C9;
   float t7 = dz * par->C10;
   float t8 = par->C3 + t7;
   float t19 = par->C7 + dz * par->C12;
   par->C0 += dz * par->C2 + t3 * dz;
   par->C1 += dz * par->C6 + t8 * dz;
   par->C2 = t3;
   par->C3 = t8;
   par->C4 += dz * par->C11;
   par->C5 += dz * par->C7 + t19 * dz;
   par->C6 += t7;
   par->C7 = t19;
   par->C8 += dz * par->C13;

   par->Z = zOut;
}

/* Runge-Kutta track extrapolation for parallel version of the tracking.
 *@param par Input/Output track parameters.
 *@param zOut Z position to extrapolate to.
 */
__device__ void LitCudaRK4Extrapolation(
   LitCudaTrackParam* par,
   float zOut,
   const LitCudaFieldRegion* field)
{
   const float fC = 0.000299792458;
   const float ZERO = 0., ONE = 1., TWO = 2., C1_3 = 1./3., C1_6 = 1./6.;

   float coef[4] = {0.0, 0.5, 0.5, 1.0};

   float Ax[4], Ay[4];
   float dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
   float kx[4];
   float ky[4];
   float ktx[4];
   float kty[4];

   float zIn = par->Z;
   float h = zOut - zIn;
   float hC   = h * fC;
   float hCqp = h * fC * par->Qp;
   float x0[4];

   float x[4] = {par->X, par->Y, par->Tx, par->Ty};

   float F[25]; // derivatives, transport matrix

   // Get field values
   LitCudaFieldValue B[4];
   GetFieldValue(zIn + coef[0] * h, field, &B[0]);
   GetFieldValue(zIn + coef[1] * h, field, &B[1]);
   B[2] = B[1];
   GetFieldValue(zIn + coef[3] * h, field, &B[3]);

   // Calculation for zero step
   {
      float Bx = B[0].Bx;
      float By = B[0].By;
      float Bz = B[0].Bz;

      float tx = x[2];
      float ty = x[3];
      float txtx = tx * tx;
      float tyty = ty * ty;
      float txty = tx * ty;
      float txtxtyty1 = ONE + txtx + tyty;
      float t1 = sqrtf(txtxtyty1);
      float t2 = (1./txtxtyty1); //1.0 / txtxtyty1;

      Ax[0] = ( txty * Bx + ty * Bz - ( ONE + txtx ) * By ) * t1;
      Ay[0] = (-txty * By - tx * Bz + ( ONE + tyty ) * Bx ) * t1;

      dAx_dtx[0] = Ax[0] * tx * t2 + ( ty * Bx - TWO * tx * By ) * t1;
      dAx_dty[0] = Ax[0] * ty * t2 + ( tx * Bx + Bz ) * t1;
      dAy_dtx[0] = Ay[0] * tx * t2 + (-ty * By - Bz ) * t1;
      dAy_dty[0] = Ay[0] * ty * t2 + (-tx * By + TWO * ty * Bx ) * t1;

      kx[0] = tx * h;
      ky[0] = ty * h;
      ktx[0] = Ax[0] * hCqp;
      kty[0] = Ay[0] * hCqp;
   }
   // end calculation for zero step

   // Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 1
      x[0] = par->X  + coef[iStep] * kx[iStep - 1];
      x[1] = par->Y  + coef[iStep] * ky[iStep - 1];
      x[2] = par->Tx + coef[iStep] * ktx[iStep - 1];
      x[3] = par->Ty + coef[iStep] * kty[iStep - 1];

      float Bx = B[iStep].Bx;
      float By = B[iStep].By;
      float Bz = B[iStep].Bz;

      float tx = x[2];
      float ty = x[3];
      float txtx = tx * tx;
      float tyty = ty * ty;
      float txty = tx * ty;
      float txtxtyty1 = ONE + txtx + tyty;
      float t1 = sqrtf(txtxtyty1);
      float t2 = (1./txtxtyty1); //1.0 / txtxtyty1;

      Ax[iStep] = ( txty * Bx + ty * Bz - ( ONE + txtx ) * By ) * t1;
      Ay[iStep] = (-txty * By - tx * Bz + ( ONE + tyty ) * Bx ) * t1;

      dAx_dtx[iStep] = Ax[iStep] * tx * t2 + ( ty * Bx - TWO * tx * By ) * t1;
      dAx_dty[iStep] = Ax[iStep] * ty * t2 + ( tx * Bx + Bz ) * t1;
      dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * By - Bz ) * t1;
      dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * By + TWO * ty * Bx ) * t1;

      kx[iStep] = tx * h;
      ky[iStep] = ty * h;
      ktx[iStep] = Ax[iStep] * hCqp;
      kty[iStep] = Ay[iStep] * hCqp;

   } // 1

   // Calculate output state vector
//   for (unsigned int i = 0; i < 4; i++) xOut[i] = CalcOut(xIn[i], k[i]);
   par->X  += kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   par->Y  += ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   par->Tx += ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
   par->Ty += kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
//   xOut[4] = xIn[4];


   // Calculation of the derivatives

   // derivatives dx/dx and dx/dy
   // dx/dx
   F[0] = ONE;
   F[5] = ZERO;
   F[10] = ZERO;
   F[15] = ZERO;
   F[20] = ZERO;
   // dx/dy
   F[1] = ZERO;
   F[6] = ONE;
   F[11] = ZERO;
   F[16] = ZERO;
   F[21] = ZERO;
   // end of derivatives dx/dx and dx/dy

   // Derivatives dx/tx
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ONE;
   x[3] = x0[3] = ZERO;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   //ktx[0] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
   kty[0] = (dAy_dtx[0] * x[2] + dAy_dty[0] * x[3]) * hCqp;
   // Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 2
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
//         x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
      x[3] = x0[3] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      //ktx[iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      kty[iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   } // 2

   F[2]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   F[7]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   F[12] = ONE;
   F[17] = x0[3] + kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
   F[22] = ZERO;
   // end of derivatives dx/dtx

   // Derivatives    dx/ty
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ZERO;
   x[3] = x0[3] = ONE;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   ktx[0] = (dAx_dtx[0] * x[2] + dAx_dty[0] * x[3]) * hCqp;
   //kty[0] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   //Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 4
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
      x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
//         x[3] = x0[0] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      ktx[iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      //kty[iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   }  // 4

   F[3]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   F[8]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   F[13] = x0[2] + ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
   F[18] = ONE;
   F[23] = ZERO;
   // end of derivatives dx/dty

   // Derivatives dx/dqp
   x[0] = x0[0] = ZERO;
   x[1] = x0[1] = ZERO;
   x[2] = x0[2] = ZERO;
   x[3] = x0[3] = ZERO;

   //Calculate for zero step
   kx[0] = x[2] * h;
   ky[0] = x[3] * h;
   ktx[0] = Ax[0] * hC + hCqp * (dAx_dtx[0] * x[2] + dAx_dty[0] * x[3]);
   kty[0] = Ay[0] * hC + hCqp * (dAy_dtx[0] * x[2] + dAy_dty[0] * x[3]);
   //Calculate for steps starting from 1
   for (unsigned int iStep = 1; iStep < 4; iStep++) { // 4
      x[0] = x0[0] + coef[iStep] * kx[iStep - 1];
      x[1] = x0[1] + coef[iStep] * ky[iStep - 1];
      x[2] = x0[2] + coef[iStep] * ktx[iStep - 1];
      x[3] = x0[3] + coef[iStep] * kty[iStep - 1];

      kx[iStep] = x[2] * h;
      ky[iStep] = x[3] * h;
      ktx[iStep] = Ax[iStep] * hC + hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
      kty[iStep] = Ay[iStep] * hC + hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
   }  // 4

   F[4]  = x0[0] + kx[0] * C1_6 + kx[1] * C1_3 + kx[2] * C1_3 + kx[3] * C1_6;
   F[9]  = x0[1] + ky[0] * C1_6 + ky[1] * C1_3 + ky[2] * C1_3 + ky[3] * C1_6;
   F[14] = x0[2] + ktx[0] * C1_6 + ktx[1] * C1_3 + ktx[2] * C1_3 + ktx[3] * C1_6;
   F[19] = x0[3] + kty[0] * C1_6 + kty[1] * C1_3 + kty[2] * C1_3 + kty[3] * C1_6;
   F[24] = 1.;
   // end of derivatives dx/dqp

   // end calculation of the derivatives


   // Transport C matrix
   {
      float cIn[15] = {par->C0,  par->C1,  par->C2,  par->C3,  par->C4,
                       par->C5,  par->C6,  par->C7,  par->C8,  par->C9,
                       par->C10, par->C11, par->C12, par->C13, par->C14
                      };
      // F*C*Ft
      float A = cIn[2] + F[2] * cIn[9] + F[3] * cIn[10] + F[4] * cIn[11];
      float B = cIn[3] + F[2] * cIn[10] + F[3] * cIn[12] + F[4] * cIn[13];
      float C = cIn[4] + F[2] * cIn[11] + F[3] * cIn[13] + F[4] * cIn[14];

      float D = cIn[6] + F[7] * cIn[9] + F[8] * cIn[10] + F[9] * cIn[11];
      float E = cIn[7] + F[7] * cIn[10] + F[8] * cIn[12] + F[9] * cIn[13];
      float G = cIn[8] + F[7] * cIn[11] + F[8] * cIn[13] + F[9] * cIn[14];

      float H = cIn[9] + F[13] * cIn[10] + F[14] * cIn[11];
      float I = cIn[10] + F[13] * cIn[12] + F[14] * cIn[13];
      float J = cIn[11] + F[13] * cIn[13] + F[14] * cIn[14];

      float K = cIn[13] + F[17] * cIn[11] + F[19] * cIn[14];

      par->C0 = cIn[0] + F[2] * cIn[2] + F[3] * cIn[3] + F[4] * cIn[4] + A * F[2] + B * F[3] + C * F[4];
      par->C1 = cIn[1] + F[2] * cIn[6] + F[3] * cIn[7] + F[4] * cIn[8] + A * F[7] + B * F[8] + C * F[9];
      par->C2 = A + B * F[13] + C * F[14];
      par->C3 = B + A * F[17] + C * F[19];
      par->C4 = C;

      par->C5 = cIn[5] + F[7] * cIn[6] + F[8] * cIn[7] + F[9] * cIn[8] + D * F[7] + E * F[8] + G * F[9];
      par->C6 = D + E * F[13] + G * F[14];
      par->C7 = E + D * F[17] + G * F[19];
      par->C8 = G;

      par->C9 = H + I * F[13] + J * F[14];
      par->C10 = I + H * F[17] + J * F[19];
      par->C11 = J;

      par->C12 = cIn[12] + F[17] * cIn[10] + F[19] * cIn[13] + (F[17] * cIn[9] + cIn[10] + F[19] * cIn[11]) * F[17] + K * F[19];
      par->C13 = K;

      par->C14 = cIn[14];
   }
   //end transport C matrix

   par->Z = zOut;
}

//#undef cnst
//#endif



//#ifndef LITCUDAFILTRATION_H_
//#define LITCUDAFILTRATION_H_

//#include "LitCudaTypes.h"
//#include "LitCudaHit.h"

//#define cnst static const fvec

__device__ void LitCudaFiltration (
   LitCudaTrackParam* par,
   const LitCudaPixelHit* hit)
{
   const float ONE = 1., TWO = 2.;

   float dxx = hit->Dx * hit->Dx;
   float dxy = hit->Dxy;
   float dyy = hit->Dy * hit->Dy;

   // calculate residuals
   float dx = hit->X - par->X;
   float dy = hit->Y - par->Y;

   // Calculate and inverse residual covariance matrix
   float t = ONE / (dxx * dyy + dxx * par->C5 + dyy * par->C0 + par->C0 * par->C5 -
                    dxy * dxy - TWO * dxy * par->C1 - par->C1 * par->C1);
   float R00 = (dyy + par->C5) * t;
   float R01 = -(dxy + par->C1) * t;
   float R11 = (dxx + par->C0) * t;

   // Calculate Kalman gain matrix
   float K00 = par->C0 * R00 + par->C1 * R01;
   float K01 = par->C0 * R01 + par->C1 * R11;
   float K10 = par->C1 * R00 + par->C5 * R01;
   float K11 = par->C1 * R01 + par->C5 * R11;
   float K20 = par->C2 * R00 + par->C6 * R01;
   float K21 = par->C2 * R01 + par->C6 * R11;
   float K30 = par->C3 * R00 + par->C7 * R01;
   float K31 = par->C3 * R01 + par->C7 * R11;
   float K40 = par->C4 * R00 + par->C8 * R01;
   float K41 = par->C4 * R01 + par->C8 * R11;

   // Calculate filtered state vector
   par->X  += K00 * dx + K01 * dy;
   par->Y  += K10 * dx + K11 * dy;
   par->Tx += K20 * dx + K21 * dy;
   par->Ty += K30 * dx + K31 * dy;
   par->Qp += K40 * dx + K41 * dy;

   // Calculate filtered covariance matrix
   float cIn[15] = {par->C0,  par->C1,  par->C2,  par->C3,  par->C4,
                    par->C5,  par->C6,  par->C7,  par->C8,  par->C9,
                    par->C10, par->C11, par->C12, par->C13, par->C14
                   };

   par->C0  += -K00 * cIn[0] - K01 * cIn[1];
   par->C1  += -K00 * cIn[1] - K01 * cIn[5];
   par->C2  += -K00 * cIn[2] - K01 * cIn[6];
   par->C3  += -K00 * cIn[3] - K01 * cIn[7];
   par->C4  += -K00 * cIn[4] - K01 * cIn[8];

   par->C5  += -K11 * cIn[5] - K10 * cIn[1];
   par->C6  += -K11 * cIn[6] - K10 * cIn[2];
   par->C7  += -K11 * cIn[7] - K10 * cIn[3];
   par->C8  += -K11 * cIn[8] - K10 * cIn[4];

   par->C9  += -K20 * cIn[2] - K21 * cIn[6];
   par->C10 += -K20 * cIn[3] - K21 * cIn[7];
   par->C11 += -K20 * cIn[4] - K21 * cIn[8];

   par->C12 += -K30 * cIn[3] - K31 * cIn[7];
   par->C13 += -K30 * cIn[4] - K31 * cIn[8];

   par->C14 += -K40 * cIn[4] - K41 * cIn[8];
}

__device__ void LitFiltration(
   LitCudaTrackParam* par,
   const LitCudaStripHit* hit)
{
   const float ONE = 1., TWO = 2.;

   float duu = hit->Du * hit->Du;
   float phiCosSq = hit->phiCos * hit->phiCos;
   float phiSinSq = hit->phiSin * hit->phiSin;
   float phi2SinCos = TWO * hit->phiCos * hit->phiSin;

   // residual
   float r = hit->U - par->C0 * hit->phiCos - par->C1 * hit->phiSin;
   float norm = duu + par->C0 * phiCosSq + phi2SinCos * par->C1 + par->C5 * phiSinSq;
   //  myf norm = duu + cIn[0] * phiCos + cIn[5] * phiSin;
   float R = (ONE/norm);

   // Calculate Kalman gain matrix
   float K0 = par->C0 * hit->phiCos + par->C1 * hit->phiSin;
   float K1 = par->C1 * hit->phiCos + par->C5 * hit->phiSin;
   float K2 = par->C2 * hit->phiCos + par->C6 * hit->phiSin;
   float K3 = par->C3 * hit->phiCos + par->C7 * hit->phiSin;
   float K4 = par->C4 * hit->phiCos + par->C8 * hit->phiSin;

   float KR0 = K0 * R;
   float KR1 = K1 * R;
   float KR2 = K2 * R;
   float KR3 = K3 * R;
   float KR4 = K4 * R;

   // Calculate filtered state vector
   par->X += KR0 * r;
   par->Y += KR1 * r;
   par->Tx += KR2 * r;
   par->Ty += KR3 * r;
   par->Qp += KR4 * r;

   // Calculate filtered covariance matrix
   par->C0 -= KR0 * K0;
   par->C1 -= KR0 * K1;
   par->C2 -= KR0 * K2;
   par->C3 -= KR0 * K3;
   par->C4 -= KR0 * K4;

   par->C5 -= KR1 * K1;
   par->C6 -= KR1 * K2;
   par->C7 -= KR1 * K3;
   par->C8 -= KR1 * K4;

   par->C9 -= KR2 * K2;
   par->C10 -= KR2 * K3;
   par->C11 -= KR2 * K4;

   par->C12 -= KR3 * K3;
   par->C13 -= KR3 * K4;

   par->C14 -= KR4 * K4;
}

//#undef cnst
//#endif /* LITCUDAFILTRATION_H_ */



#endif /* LITCUDATRACKING_H_ */



