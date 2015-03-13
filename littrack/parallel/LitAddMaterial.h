/**
 * \file LitAddMaterial.h
 * \brief Functions for calculation of the material effects.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * The influence of the material on the track momentum is taken
 * into account by calculating the expected average energy loss due
 * to ionization (Bethe-Bloch formula) and bremsstrahlung
 * (Bethe-Heitler formula). The influence on the
 * covariance matrix due to multiple scattering is included by adding
 * process noise in the track propagation. Here, a gaussian approximation
 * using the Highland formula is used to estimate the average scattering
 * angle.
 */

#ifndef LITADDMATERIAL_H_
#define LITADDMATERIAL_H_

#include "LitTrackParam.h"
#include "LitMath.h"

namespace lit {
namespace parallel {

/**
 * \fn template<class T> inline void LitAddMaterial(LitTrackParam<T>& par, const LitMaterialInfo<T>& mat)
 * \brief Function calculates material effects and updates track parameters and covariance matrix.
 *
 * \param[in,out] par Reference to track parameters.
 * \param[in] mat Reference to material
 */
template<class T>
inline void LitAddMaterial(
   LitTrackParam<T>& par,
   T siliconThickness)
{
   // Silicon properties
   static const T SILICON_DENSITY = 2.33; // g*cm^-3
//   static const T SILICON_A = 28.08855; // silicon atomic weight
//   static const T SILICON_Z = 14.0; // silicon atomic number
   static const T SILICON_Z_OVER_A = 0.4984;//2.006325; // Z/A ratio for silicon
   static const T SILICON_RAD_LENGTH = 9.365; // cm
   static const T SILICON_I = 173 * 1e-9 ; // mean excitation energy [GeV]
   static const T SILICON_I_SQ = SILICON_I * SILICON_I; // squared mean excitation energy

   static const T ZERO = 0.0, ONE = 1., TWO = 2.;
   static const T mass = 0.105658369; // muon mass [GeV/c]
   static const T massSq = 0.105658369 * 0.105658369; // muon mass squared
   static const T C1_2 = 0.5, C1_3 = 1./3.;
   static const T me = 0.000511; // Electron mass [GeV/c]
   static const T ratio = me / mass;

   T p = sgn(par.Qp) / par.Qp; // Momentum [GeV/c]
   T E = sqrt(massSq + p * p);
   T beta = p / E;
   T betaSq = beta * beta;
   T gamma = E / mass;
   T gammaSq = gamma * gamma;

   // Scale material thickness
   T norm = sqrt(ONE + par.Tx * par.Tx + par.Ty * par.Ty);
   T thickness = norm * siliconThickness;
   T radThick = thickness / SILICON_RAD_LENGTH;
   T sqrtRadThick = sqrt(radThick);
   T logRadThick = log(radThick);

   /*
    * Energy loss corrections
    */

   // Bethe-Block
   static const T K = 0.000307075; // GeV * g^-1 * cm^2
   T Tmax = (2 * me * betaSq * gammaSq) / (ONE + TWO * gamma * ratio + ratio * ratio);

   // density correction
   T dc = ZERO;
   if (p > 0.5) { // for particles above 1 Gev
      static const T c7 = 28.816;
      static const T c8 = 1e-9;
      T hwp = c7 * sqrt(SILICON_DENSITY * SILICON_Z_OVER_A) * c8; // GeV
      dc = log(hwp / SILICON_I) + log(beta * gamma) - C1_2;
   }

   T bbLoss = K * SILICON_Z_OVER_A * rcp(betaSq) *
              (C1_2 * log(TWO * me * betaSq * gammaSq * Tmax / SILICON_I_SQ) - betaSq - dc);

//   static const T bbc = 0.00354;
//   T bbLoss = bbc * SILICON_Z_OVER_A;

   // Bethe-Heitler
// T bhLoss = (E * ratio * ratio)/(mat.X0 * mat.Rho);
   T bhLoss = ZERO;

   // Pair production approximation
// static const T c3 = 7e-5;
// T ppLoss = c3 * E / (mat.X0 * mat.Rho);
   T ppLoss = ZERO;

   // Integrated value of the energy loss
   T energyLoss = (bbLoss + bhLoss + ppLoss) * SILICON_DENSITY * thickness;

   // Correct Q/p value due to energy loss
   T Enew = E - energyLoss;
   T pnew = sqrt(Enew * Enew - massSq);
   par.Qp = sgn(par.Qp) * rcp(pnew);


   // Calculate Q/p correction in the covariance matrix
   T betanew = pnew / Enew;
   T betaSqnew = betanew * betanew;
   T gammanew = Enew / mass;
// T gammaSqnew = gammanew * gammanew;

   // Calculate xi factor (KeV).
   static const T c4 = 153.5;
   T XI = (c4 * SILICON_Z_OVER_A * thickness * SILICON_DENSITY) / betaSqnew;

   // Maximum energy transfer to atomic electron (KeV).
   T etanew = betanew * gammanew;
   T etaSqnew = etanew * etanew;
   T F1 = TWO * me * etaSqnew;
   T F2 = ONE + TWO * ratio * gammanew + ratio * ratio;
   static const T c5 = 1e6;
   T emax = c5 * F1/F2;

   static const T c6 = 1e-12;
   T dedxSq = XI * emax * (ONE - C1_2 * betaSqnew) * c6;

   T p2 = pnew * pnew;
   T p6 = p2 * p2 * p2;
   T qpCorr = (Enew * Enew * dedxSq) / p6;
   par.C14 += qpCorr;
   // end calculate Q/p correction in the covariance matrix

   /*
    * End energy loss corrections
    */

   /*
    * Multiple scattering corrections
    */

   T tx = par.Tx;
   T ty = par.Ty;
   T bcp = betanew * pnew;
   //T bcp = beta * p;
   static const T c1 = 0.0136, c2 = 0.038;
   T theta = c1 * rcp(bcp) * sqrtRadThick * (ONE + c2 * logRadThick);
   T thetaSq = theta * theta;

   T t = ONE + tx * tx + ty * ty;

   T Q33 = (1 + tx * tx) * t * thetaSq;
   T Q44 = (1 + ty * ty) * t * thetaSq;
   T Q34 = tx * ty * t * thetaSq;

   T T23 = thickness * thickness * C1_3;
   T T2 = thickness * C1_2;

   par.C0 += Q33 * T23;
   par.C1 += Q34 * T23;
   par.C2 += Q33 * T2;
   par.C3 += Q34 * T2;

   par.C5 += Q44 * T23;
   par.C6 += Q34 * T2;
   par.C7 += Q44 * T2;

   par.C9 += Q33;
   par.C10 += Q34;

   par.C12 += Q44;

   /*
    * End multiple scattering corrections
    */
}



/**
 * \fn template<class T> inline void LitAddMaterialElectron(LitTrackParam<T>& par, const LitMaterialInfo<T>& mat)
 * \brief Function calculates material effects for electrons and updates track parameters and covariance matrix.
 *
 * \param[in,out] par Reference to track parameters.
 * \param[in] mat Reference to material.
 */
template<class T>
inline void LitAddMaterialElectron(
   LitTrackParam<T>& par,
   T siliconThickness)
{
   // Silicon properties
   static const T SILICON_RAD_LENGTH = 9.365; // cm

   static const T ZERO = 0.0, ONE = 1., TWO = 2., THREE = 3., INF = 1.e5;
   static const T C1_2 = 0.5, C1_3 = 1./3.;

   //scale material thickness
   static const T C_LOG = log(THREE) / log (TWO);
   T norm = sqrt(ONE + par.Tx * par.Tx + par.Ty * par.Ty);
   T thickness = norm * siliconThickness;
   T radThick = thickness / SILICON_RAD_LENGTH;
   T sqrtRadThick = sqrt(radThick);
   T logRadThick = log(radThick);

   // no material thickness scaling
// T thickness = mat.Thickness;
// T radThick = mat.RadThick;
// T sqrtRadThick = mat.SqrtRadThick;
// T logRadThick = mat.LogRadThick;

   /*
    * Energy loss corrections
    */
   par.Qp *= exp(radThick);
//   par.C14 += par.Qp * par.Qp * mat.ElLoss; // no thickness scaling
   par.C14 += (exp(radThick * C_LOG) - exp(-TWO * radThick)); // thickness scaling
   /*
    * End of energy loss corrections
    */

   /*
    * Multiple scattering corrections
    */
   T pnew = sgn(par.Qp) / par.Qp; // Momentum [GeV/c]
   T betanew = ONE;
   T tx = par.Tx;
   T ty = par.Ty;
   T bcp = betanew * pnew;
   static const T c1 = 0.0136, c2 = 0.038;
   T theta = c1 * rcp(bcp) * sqrtRadThick * (ONE + c2 * logRadThick);
   T thetaSq = theta * theta;

   T t = ONE + tx * tx + ty * ty;

   T Q33 = (1 + tx * tx) * t * thetaSq;
   T Q44 = (1 + ty * ty) * t * thetaSq;
   T Q34 = tx * ty * t * thetaSq;

   T T23 = thickness * thickness * C1_3;
   T T2 = thickness * C1_2;

   par.C0 += Q33 * T23;
   par.C1 += Q34 * T23;
   par.C2 += Q33 * T2;
   par.C3 += Q34 * T2;

   par.C5 += Q44 * T23;
   par.C6 += Q34 * T2;
   par.C7 += Q44 * T2;

   par.C9 += Q33;
   par.C10 += Q34;

   par.C12 += Q44;

   /*
    * End multiple scattering corrections
    */
}

} // namespace parallel
} // namespace lit
#endif /* LITADDMATERIAL_H_ */
