/** CbmLitRK4TrackExtrapolator.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/

#include "propagation/CbmLitRK4TrackExtrapolator.h"

#include "interface/CbmLitField.h"

#include <cmath>

CbmLitRK4TrackExtrapolator::CbmLitRK4TrackExtrapolator(
   CbmLitField* field):
   fField(field)
{
}

CbmLitRK4TrackExtrapolator::~CbmLitRK4TrackExtrapolator()
{
}

LitStatus CbmLitRK4TrackExtrapolator::Extrapolate(
   const CbmLitTrackParam* parIn,
   CbmLitTrackParam* parOut,
   litfloat zOut,
   std::vector<litfloat>* F)
{
   *parOut = *parIn;
   return Extrapolate(parOut, zOut, F);
}

LitStatus CbmLitRK4TrackExtrapolator::Extrapolate(
   CbmLitTrackParam* par,
   litfloat zOut,
   std::vector<litfloat>* F)
{
   litfloat zIn = par->GetZ();

   std::vector<litfloat> xIn = par->GetStateVector();
   std::vector<litfloat> xOut(5, 0.);
   std::vector<litfloat> F1(25, 0.);

   RK4Order(xIn, zIn, xOut, zOut, F1);

   std::vector<litfloat> cIn = par->GetCovMatrix();
   std::vector<litfloat> cOut(15);
   TransportC(cIn, F1, cOut);

   par->SetStateVector(xOut);
   par->SetCovMatrix(cOut);
   par->SetZ(zOut);

   if (F != NULL) { F->assign(F1.begin(), F1.end()); }

   return kLITSUCCESS;
}

void CbmLitRK4TrackExtrapolator::RK4Order(
   const std::vector<litfloat>& xIn,
   litfloat zIn,
   std::vector<litfloat>& xOut,
   litfloat zOut,
   std::vector<litfloat>& derivs) const
{
   const litfloat fC = 0.000299792458;

   litfloat coef[4] = {0.0, 0.5, 0.5, 1.0};

   litfloat Ax[4], Ay[4];
   litfloat dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
   litfloat k[4][4];

   litfloat h = zOut - zIn;
   litfloat hC   = h * fC;
   litfloat hCqp = h * fC * xIn[4];
   litfloat x0[4];

   litfloat x[4] = {xIn[0], xIn[1], xIn[2], xIn[3]};

   for (unsigned int iStep = 0; iStep < 4; iStep++) { // 1
      if (iStep > 0) {
         for(unsigned int i = 0; i < 4; i++) {
            x[i] = xIn[i] + coef[iStep] * k[i][iStep - 1];
         }
      }

      litfloat Bx, By, Bz;
      fField->GetFieldValue(x[0], x[1], zIn + coef[iStep] * h, Bx, By, Bz);

      litfloat tx = x[2];
      litfloat ty = x[3];
      litfloat txtx = tx * tx;
      litfloat tyty = ty * ty;
      litfloat txty = tx * ty;
      litfloat txtxtyty1= 1.0 + txtx + tyty;
      litfloat t1 = std::sqrt(txtxtyty1);
      litfloat t2 = 1.0 / txtxtyty1;

      Ax[iStep] = ( txty * Bx + ty * Bz - ( 1.0 + txtx ) * By ) * t1;
      Ay[iStep] = (-txty * By - tx * Bz + ( 1.0 + tyty ) * Bx ) * t1;

      dAx_dtx[iStep] = Ax[iStep] * tx * t2 + ( ty * Bx - 2.0 * tx * By ) * t1;
      dAx_dty[iStep] = Ax[iStep] * ty * t2 + ( tx * Bx + Bz ) * t1;
      dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * By - Bz ) * t1;
      dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * By + 2.0 * ty * Bx ) * t1;

      k[0][iStep] = tx * h;
      k[1][iStep] = ty * h;
      k[2][iStep] = Ax[iStep] * hCqp;
      k[3][iStep] = Ay[iStep] * hCqp;

   } // 1

   for (unsigned int i = 0; i < 4; i++) { xOut[i] = CalcOut(xIn[i], k[i]); }
   xOut[4] = xIn[4];

   // Calculation of the derivatives

   // derivatives dx/dx and dx/dy
   // dx/dx
   derivs[0] = 1.;
   derivs[5] = 0.;
   derivs[10] = 0.;
   derivs[15] = 0.;
   derivs[20] = 0.;
   // dx/dy
   derivs[1] = 0.;
   derivs[6] = 1.;
   derivs[11] = 0.;
   derivs[16] = 0.;
   derivs[21] = 0.;
   // end of derivatives dx/dx and dx/dy

   // Derivatives dx/tx
   x[0] = x0[0] = 0.0;
   x[1] = x0[1] = 0.0;
   x[2] = x0[2] = 1.0;
   x[3] = x0[3] = 0.0;
   for (unsigned int iStep = 0; iStep < 4; iStep++) { // 2
      if (iStep > 0) {
         for (unsigned int i = 0; i < 4; i++) {
            if (i != 2) {
               x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
            }
         }
      }

      k[0][iStep] = x[2] * h;
      k[1][iStep] = x[3] * h;
      //k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   } // 2

   derivs[2] = CalcOut(x0[0], k[0]);
   derivs[7] = CalcOut(x0[1], k[1]);
   derivs[12] = 1.0;
   derivs[17] = CalcOut(x0[3], k[3]);
   derivs[22] = 0.0;
   // end of derivatives dx/dtx

   // Derivatives    dx/ty
   x[0] = x0[0] = 0.0;
   x[1] = x0[1] = 0.0;
   x[2] = x0[2] = 0.0;
   x[3] = x0[3] = 1.0;
   for (unsigned int iStep = 0; iStep < 4; iStep++) { // 4
      if (iStep > 0) {
         for(unsigned int i = 0; i < 4; i++) {
            if(i!=3) {
               x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
            }
         }
      }

      k[0][iStep] = x[2] * h;
      k[1][iStep] = x[3] * h;
      k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
      //k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
   }  // 4

   derivs[3] = CalcOut(x0[0], k[0]);
   derivs[8] = CalcOut(x0[1], k[1]);
   derivs[13] = CalcOut(x0[2], k[2]);
   derivs[18] = 1.;
   derivs[23] = 0.;
   // end of derivatives dx/dty

   // Derivatives dx/dqp
   x[0] = x0[0] = 0.0;
   x[1] = x0[1] = 0.0;
   x[2] = x0[2] = 0.0;
   x[3] = x0[3] = 0.0;
   for (unsigned int iStep = 0; iStep < 4; iStep++) { // 4
      if (iStep > 0) {
         for(unsigned int i = 0; i < 4; i++) {
            x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
         }
      }

      k[0][iStep] = x[2] * h;
      k[1][iStep] = x[3] * h;
      k[2][iStep] = Ax[iStep] * hC +
                    hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
      k[3][iStep] = Ay[iStep] * hC +
                    hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
   }  // 4

   derivs[4] = CalcOut(x0[0], k[0]);
   derivs[9] = CalcOut(x0[1], k[1]);
   derivs[14] = CalcOut(x0[2], k[2]);
   derivs[19] = CalcOut(x0[3], k[3]);
   derivs[24] = 1.;
   // end of derivatives dx/dqp

   // end calculation of the derivatives
}

litfloat CbmLitRK4TrackExtrapolator::CalcOut(
   litfloat in,
   const litfloat k[4]) const
{
   return in + k[0]/6. + k[1]/3. + k[2]/3. + k[3]/6.;
}

void CbmLitRK4TrackExtrapolator::TransportC(
   const std::vector<litfloat>& cIn,
   const std::vector<litfloat>& F,
   std::vector<litfloat>& cOut) const
{
   // F*C*Ft
   litfloat A = cIn[2] + F[2] * cIn[9] + F[3] * cIn[10] + F[4] * cIn[11];
   litfloat B = cIn[3] + F[2] * cIn[10] + F[3] * cIn[12] + F[4] * cIn[13];
   litfloat C = cIn[4] + F[2] * cIn[11] + F[3] * cIn[13] + F[4] * cIn[14];

   litfloat D = cIn[6] + F[7] * cIn[9] + F[8] * cIn[10] + F[9] * cIn[11];
   litfloat E = cIn[7] + F[7] * cIn[10] + F[8] * cIn[12] + F[9] * cIn[13];
   litfloat G = cIn[8] + F[7] * cIn[11] + F[8] * cIn[13] + F[9] * cIn[14];

   litfloat H = cIn[9] + F[13] * cIn[10] + F[14] * cIn[11];
   litfloat I = cIn[10] + F[13] * cIn[12] + F[14] * cIn[13];
   litfloat J = cIn[11] + F[13] * cIn[13] + F[14] * cIn[14];

   litfloat K = cIn[13] + F[17] * cIn[11] + F[19] * cIn[14];

   cOut[0] = cIn[0] + F[2] * cIn[2] + F[3] * cIn[3] + F[4] * cIn[4] + A * F[2] + B * F[3] + C * F[4];
   cOut[1] = cIn[1] + F[2] * cIn[6] + F[3] * cIn[7] + F[4] * cIn[8] + A * F[7] + B * F[8] + C * F[9];
   cOut[2] = A + B * F[13] + C * F[14];
   cOut[3] = B + A * F[17] + C * F[19];
   cOut[4] = C;

   cOut[5] = cIn[5] + F[7] * cIn[6] + F[8] * cIn[7] + F[9] * cIn[8] + D * F[7] + E * F[8] + G * F[9];
   cOut[6] = D + E * F[13] + G * F[14];
   cOut[7] = E + D * F[17] + G * F[19];
   cOut[8] = G;

   cOut[9] = H + I * F[13] + J * F[14];
   cOut[10] = I + H * F[17] + J * F[19];
   cOut[11] = J;

   cOut[12] = cIn[12] + F[17] * cIn[10] + F[19] * cIn[13] + (F[17] * cIn[9] + cIn[10] + F[19] * cIn[11]) * F[17] + K * F[19];
   cOut[13] = K;

   cOut[14] = cIn[14];
}
