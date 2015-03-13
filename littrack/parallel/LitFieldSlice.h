/**
 * \file LitFieldSlice.h
 * \brief Approximated magnetic field slice in XY plane perpendicular to Z.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef LITFIELDSLICE_H_
#define LITFIELDSLICE_H_

#include "LitFieldValue.h"

namespace lit {
namespace parallel {

/**
 * \def LIT_POLYNOM_DEGREE
 * \brief Order of the polynomial function which is used for magnetic field approximation
 */
#define LIT_POLYNOM_DEGREE 7

/**
 * \class LitFieldSlice
 * \brief Approximated magnetic field slice in XY plane perpendicular to Z.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * It stores coefficients of the polynomial function
 * for each field component Bx, By and Bz.
 * Order of the polynom is defined by LIT_POLYNOM_DEGREE.
 */
template<class T>
class LitFieldSlice
{
public:

#if LIT_POLYNOM_DEGREE==3
   static const unsigned int N = 10; // for the 3rd degree polynom
#else
#if LIT_POLYNOM_DEGREE==5
   static const unsigned int N = 21; // for the 5th degree polynom
#else
#if LIT_POLYNOM_DEGREE==7
   static const unsigned int N = 36; // for the 7th degree polynom
#else
#if LIT_POLYNOM_DEGREE==9
   static const unsigned int N = 55; // for the 7th degree polynom
#endif
#endif
#endif
#endif

public:
   /**
    * \brief Constructor.
    */
   LitFieldSlice():fZ(0.) {
      for(unsigned int i=0; i<N; i++) { cx[i] = cy[i] = cz[i] = 0.; }
   }

   /**
    * \brief Returns field value at a certain (X, Y) position.
    *
    * LIT_POLYNOM_DEGREE defines a polynom order
    * which is used for output field value calculation.
    *
    * \param[in] x X position.
    * \param[in] y Y position.
    * \param[out] B Field value.
    */
   void GetFieldValue(
      const T& x,
      const T& y,
      LitFieldValue<T> &B) const {
#if LIT_POLYNOM_DEGREE==3
      T x2 = x*x;
      T y2 = y*y;
      T x2y = x2*y;
      T xy2 = x*y2;
      T x3 = x2*x;
      T y3 = y2*y;

      B.Bx = cx[0]
             + cx[1]*x + cx[2]*y
             + cx[3]*x2 + cx[4]*x*y + cx[5]*y2
             + cx[6]*x3 + cx[7]*x2y + cx[8]*xy2 + cx[9]*y3;

      B.By = cy[0]
             + cy[1]*x + cy[2]*y
             + cy[3]*x2 + cy[4]*x*y + cy[5]*y2
             + cy[6]*x3 + cy[7]*x2y + cy[8]*xy2 + cy[9]*y3;

      B.Bz = cz[0]
             + cz[1]*x + cz[2]*y
             + cz[3]*x2 + cz[4]*x*y + cz[5]*y2
             + cz[6]*x3 + cz[7]*x2y + cz[8]*xy2 + cz[9]*y3;
#else
#if LIT_POLYNOM_DEGREE==5
      T x2 = x*x;
      T y2 = y*y;
      T xy = x*y;

      T x3 = x2*x;
      T y3 = y2*y;
      T xy2 = x*y2;
      T x2y = x2*y;

      T x4 = x3*x;
      T y4 = y3*y;
      T xy3 = x*y3;
      T x2y2 = x2*y2;
      T x3y = x3*y;

      T x5 = x4*x;
      T y5 = y4*y;
      T xy4 = x*y4;
      T x2y3 = x2*y3;
      T x3y2 = x3*y2;
      T x4y = x4*y;

      B.Bx = cx[0]
             +cx[1]*x +cx[2]*y
             +cx[3]*x2 +cx[4]*xy +cx[5]*y2
             +cx[6]*x3 +cx[7]*x2y +cx[8]*xy2 +cx[9]*y3
             +cx[10]*x4 +cx[11]*x3y +cx[12]*x2y2 +cx[13]*xy3 +cx[14]*y4
             +cx[15]*x5 +cx[16]*x4y +cx[17]*x3y2 +cx[18]*x2y3 +cx[19]*xy4 +cx[20]*y5;

      B.By = cy[0]
             +cy[1]*x +cy[2]*y
             +cy[3]*x2 +cy[4]*xy +cy[5]*y2
             +cy[6]*x3 +cy[7]*x2y +cy[8]*xy2 +cy[9]*y3
             +cy[10]*x4 +cy[11]*x3y +cy[12]*x2y2 +cy[13]*xy3 +cy[14]*y4
             +cy[15]*x5 +cy[16]*x4y +cy[17]*x3y2 +cy[18]*x2y3 +cy[19]*xy4 +cy[20]*y5;

      B.Bz = cz[0]
             +cz[1]*x +cz[2]*y
             +cz[3]*x2 +cz[4]*xy +cz[5]*y2
             +cz[6]*x3 +cz[7]*x2y +cz[8]*xy2 +cz[9]*y3
             +cz[10]*x4 +cz[11]*x3y +cz[12]*x2y2 +cz[13]*xy3 +cz[14]*y4
             +cz[15]*x5 +cz[16]*x4y +cz[17]*x3y2 +cz[18]*x2y3 +cz[19]*xy4 +cz[20]*y5;

#else
#if LIT_POLYNOM_DEGREE==7
      T x2 = x*x;
      T y2 = y*y;
      T xy = x*y;

      T x3 = x2*x;
      T y3 = y2*y;
      T xy2 = x*y2;
      T x2y = x2*y;

      T x4 = x3*x;
      T y4 = y3*y;
      T xy3 = x*y3;
      T x2y2 = x2*y2;
      T x3y = x3*y;

      T x5 = x4*x;
      T y5 = y4*y;
      T xy4 = x*y4;
      T x2y3 = x2*y3;
      T x3y2 = x3*y2;
      T x4y = x4*y;

      T x6 = x5*x;
      T y6 = y5*y;
      T xy5 = x*y5;
      T x2y4 = x2*y4;
      T x3y3 = x3*y3;
      T x4y2 = x4*y2;
      T x5y = x5*y;

      T x7 = x6*x;
      T y7 = y6*y;
      T xy6 = x*y6;
      T x2y5 = x2*y5;
      T x3y4 = x3*y4;
      T x4y3 = x4*y3;
      T x5y2 = x5*y2;
      T x6y = x6*y;

      B.Bx = cx[0]
             +cx[1]*x +cx[2]*y
             +cx[3]*x2 +cx[4]*xy +cx[5]*y2
             +cx[6]*x3 +cx[7]*x2y +cx[8]*xy2 +cx[9]*y3
             +cx[10]*x4 +cx[11]*x3y +cx[12]*x2y2 +cx[13]*xy3 +cx[14]*y4
             +cx[15]*x5 +cx[16]*x4y +cx[17]*x3y2 +cx[18]*x2y3 +cx[19]*xy4 +cx[20]*y5
             +cx[21]*x6 +cx[22]*x5y +cx[23]*x4y2 +cx[24]*x3y3 +cx[25]*x2y4 +cx[26]*xy5 +cx[27]*y6
             +cx[28]*x7 +cx[29]*x6y +cx[30]*x5y2 +cx[31]*x4y3 +cx[32]*x3y4 +cx[33]*x2y5 +cx[34]*xy6 +cx[35]*y7;

      B.By = cy[0]
             +cy[1]*x +cy[2]*y
             +cy[3]*x2 +cy[4]*xy +cy[5]*y2
             +cy[6]*x3 +cy[7]*x2y +cy[8]*xy2 +cy[9]*y3
             +cy[10]*x4 +cy[11]*x3y +cy[12]*x2y2 +cy[13]*xy3 +cy[14]*y4
             +cy[15]*x5 +cy[16]*x4y +cy[17]*x3y2 +cy[18]*x2y3 +cy[19]*xy4 +cy[20]*y5
             +cy[21]*x6 +cy[22]*x5y +cy[23]*x4y2 +cy[24]*x3y3 +cy[25]*x2y4 +cy[26]*xy5 +cy[27]*y6
             +cy[28]*x7 +cy[29]*x6y +cy[30]*x5y2 +cy[31]*x4y3 +cy[32]*x3y4 +cy[33]*x2y5 +cy[34]*xy6 +cy[35]*y7;

      B.Bz = cz[0]
             +cz[1]*x +cz[2]*y
             +cz[3]*x2 +cz[4]*xy +cz[5]*y2
             +cz[6]*x3 +cz[7]*x2y +cz[8]*xy2 +cz[9]*y3
             +cz[10]*x4 +cz[11]*x3y +cz[12]*x2y2 +cz[13]*xy3 +cz[14]*y4
             +cz[15]*x5 +cz[16]*x4y +cz[17]*x3y2 +cz[18]*x2y3 +cz[19]*xy4 +cz[20]*y5
             +cz[21]*x6 +cz[22]*x5y +cz[23]*x4y2 +cz[24]*x3y3 +cz[25]*x2y4 +cz[26]*xy5 +cz[27]*y6
             +cz[28]*x7 +cz[29]*x6y +cz[30]*x5y2 +cz[31]*x4y3 +cz[32]*x3y4 +cz[33]*x2y5 +cz[34]*xy6 +cz[35]*y7;
#else
#if LIT_POLYNOM_DEGREE==9
      T x2 = x*x;
      T y2 = y*y;
      T xy = x*y;

      T x3 = x2*x;
      T y3 = y2*y;
      T xy2 = x*y2;
      T x2y = x2*y;

      T x4 = x3*x;
      T y4 = y3*y;
      T xy3 = x*y3;
      T x2y2 = x2*y2;
      T x3y = x3*y;

      T x5 = x4*x;
      T y5 = y4*y;
      T xy4 = x*y4;
      T x2y3 = x2*y3;
      T x3y2 = x3*y2;
      T x4y = x4*y;

      T x6 = x5*x;
      T y6 = y5*y;
      T xy5 = x*y5;
      T x2y4 = x2*y4;
      T x3y3 = x3*y3;
      T x4y2 = x4*y2;
      T x5y = x5*y;

      T x7 = x6*x;
      T y7 = y6*y;
      T xy6 = x*y6;
      T x2y5 = x2*y5;
      T x3y4 = x3*y4;
      T x4y3 = x4*y3;
      T x5y2 = x5*y2;
      T x6y = x6*y;

      T x8 = x7*x;
      T y8 = y7*y;
      T xy7 = x*y7;
      T x2y6 = x2*y6;
      T x3y5 = x3*y5;
      T x4y4 = x4*y4;
      T x5y3 = x5*y3;
      T x6y2 = x6*y2;
      T x7y = x7*y;

      T x9 = x8*x;
      T y9 = y8*y;
      T xy8 = x*y8;
      T x2y7 = x2*y7;
      T x3y6 = x3*y6;
      T x4y5 = x4*y5;
      T x5y4 = x5*y4;
      T x6y3 = x6*y3;
      T x7y2 = x7*y2;
      T x8y = x8*y;

      B.Bx = cx[0]
             +cx[1]*x +cx[2]*y
             +cx[3]*x2 +cx[4]*xy +cx[5]*y2
             +cx[6]*x3 +cx[7]*x2y +cx[8]*xy2 +cx[9]*y3
             +cx[10]*x4 +cx[11]*x3y +cx[12]*x2y2 +cx[13]*xy3 +cx[14]*y4
             +cx[15]*x5 +cx[16]*x4y +cx[17]*x3y2 +cx[18]*x2y3 +cx[19]*xy4 +cx[20]*y5
             +cx[21]*x6 +cx[22]*x5y +cx[23]*x4y2 +cx[24]*x3y3 +cx[25]*x2y4 +cx[26]*xy5 +cx[27]*y6
             +cx[28]*x7 +cx[29]*x6y +cx[30]*x5y2 +cx[31]*x4y3 +cx[32]*x3y4 +cx[33]*x2y5 +cx[34]*xy6 +cx[35]*y7;
      +cx[36]*x8 +cx[37]*x7y +cx[38]*x6y2 +cx[39]*x5y3 +cx[40]*x4y4 +cx[41]*x3y5 +cx[42]*x2y6 +cx[43]*xy7 +cx[44]*y8
      +cx[45]*x9 +cx[46]*x8y +cx[47]*x7y2 +cx[48]*x6y3 +cx[49]*x5y4 +cx[50]*x4y5 +cx[51]*x3y6 +cx[52]*x2y7 +cx[53]*xy8 +cx[54]*y9;

      B.By = cy[0]
             +cy[1]*x +cy[2]*y
             +cy[3]*x2 +cy[4]*xy +cy[5]*y2
             +cy[6]*x3 +cy[7]*x2y +cy[8]*xy2 +cy[9]*y3
             +cy[10]*x4 +cy[11]*x3y +cy[12]*x2y2 +cy[13]*xy3 +cy[14]*y4
             +cy[15]*x5 +cy[16]*x4y +cy[17]*x3y2 +cy[18]*x2y3 +cy[19]*xy4 +cy[20]*y5
             +cy[21]*x6 +cy[22]*x5y +cy[23]*x4y2 +cy[24]*x3y3 +cy[25]*x2y4 +cy[26]*xy5 +cy[27]*y6
             +cy[28]*x7 +cy[29]*x6y +cy[30]*x5y2 +cy[31]*x4y3 +cy[32]*x3y4 +cy[33]*x2y5 +cy[34]*xy6 +cy[35]*y7;
      +cy[36]*x8 +cy[37]*x7y +cy[38]*x6y2 +cy[39]*x5y3 +cy[40]*x4y4 +cy[41]*x3y5 +cy[42]*x2y6 +cy[43]*xy7 +cy[44]*y8
      +cy[45]*x9 +cy[46]*x8y +cy[47]*x7y2 +cy[48]*x6y3 +cy[49]*x5y4 +cy[50]*x4y5 +cy[51]*x3y6 +cy[52]*x2y7 +cy[53]*xy8 +cy[54]*y9;

      B.Bz = cz[0]
             +cz[1]*x +cz[2]*y
             +cz[3]*x2 +cz[4]*xy +cz[5]*y2
             +cz[6]*x3 +cz[7]*x2y +cz[8]*xy2 +cz[9]*y3
             +cz[10]*x4 +cz[11]*x3y +cz[12]*x2y2 +cz[13]*xy3 +cz[14]*y4
             +cz[15]*x5 +cz[16]*x4y +cz[17]*x3y2 +cz[18]*x2y3 +cz[19]*xy4 +cz[20]*y5
             +cz[21]*x6 +cz[22]*x5y +cz[23]*x4y2 +cz[24]*x3y3 +cz[25]*x2y4 +cz[26]*xy5 +cz[27]*y6
             +cz[28]*x7 +cz[29]*x6y +cz[30]*x5y2 +cz[31]*x4y3 +cz[32]*x3y4 +cz[33]*x2y5 +cz[34]*xy6 +cz[35]*y7;
      +cz[36]*x8 +cz[37]*x7y +cz[38]*x6y2 +cz[39]*x5y3 +cz[40]*x4y4 +cz[41]*x3y5 +cz[42]*x2y6 +cz[43]*xy7 +cz[44]*y8
      +cz[45]*x9 +cz[46]*x8y +cz[47]*x7y2 +cz[48]*x6y3 +cz[49]*x5y4 +cz[50]*x4y5 +cz[51]*x3y6 +cz[52]*x2y7 +cz[53]*xy8 +cz[54]*y9;
#endif
#endif
#endif
#endif
   }

   /**
    * \brief Sets polynom coefficients.
    *
    * \param[in] x Coefficients for Bx.
    * \param[in] y Coefficients for By
    * \param[in] z Coefficients for Bz
    */
   void SetCoefficients(
         const std::vector<T>& x,
         const std::vector<T>& y,
         const std::vector<T>& z) {
      for(unsigned int i = 0; i < N; i++) {
         cx[i] = x[i];
         cy[i] = y[i];
         cz[i] = z[i];
      }
   }

   /**
    * \brief Returns Z position of the slice.
    * \return Z position of the slice.
    */
   const T& GetZ() const {
      return fZ;
   }

   /**
    * \brief Sets Z position of the slice.
    * \param[in] Z Value of Z coordinate.
    */
   void SetZ(const T& Z) {
      fZ = Z;
   }

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   std::string ToString() const {
      std::string str = "";
      str = "LitFieldSlice: Z=" + lit::parallel::ToString<T>(fZ) + ", cx=";
      for(unsigned int i = 0; i < N; i++) { str += lit::parallel::ToString<T>(cx[i]) + " "; }
      str += "\n";
      str += "LitFieldSlice: cy=";
      for(unsigned int i = 0; i < N; i++) { str += lit::parallel::ToString<T>(cy[i]) + " "; }
      str += "\n";
      str += "LitFieldSlice: cz=";
      for(unsigned int i = 0; i < N; i++) { str += lit::parallel::ToString<T>(cz[i]) + " "; }
      str += "\n";
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitFieldSlice& slice) {
      strm << slice.ToString();
      return strm;
   }

private:
   T cx[N], cy[N], cz[N]; // polinom coefficients
   T fZ; // Z position of the slice
} _fvecalignment;

/**
 * \typedef LitFieldSlice<fscal> LitFieldSliceScal
 * \brief Scalar version of LitFieldSlice.
 */
typedef LitFieldSlice<fscal> LitFieldSliceScal;

/**
 * \typedef LitFieldSlice<fvec> LitFieldSliceVec
 * \brief Vector version of LitFieldSlice.
 */
typedef LitFieldSlice<fvec> LitFieldSliceVec;

} // namespace parallel
} // namespace lit

#endif /* LITFIELDSLICE_H_ */
