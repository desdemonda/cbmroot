/**
 * \class LitFieldRegion.h
 * \brief Storage for field approximation along Z.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef LITFIELDREGION_H_
#define LITFIELDREGION_H_

#include "LitMath.h"

namespace lit {
namespace parallel {

/**
 * \class LitFieldRegion
 * \brief Storage for field approximation along Z.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Field is approximated along Z coordinate using
 * straight line or parabola.
 */
template<class T>
class LitFieldRegion
{
public:
   /**
    * \brief Sets field region using three field values.
    *
    * In this case parabolic approximation is used.
    *
    * \param[in] B0 First field value.
    * \param[in] B0z Z position of the first field value.
    * \param[in] B1 Second field value.
    * \param[in] B1z Z position of the second field value.
    * \param[in] B2 Third field value.
    * \param[in] B2z Z position of the third field value.
    */
   void Set(
      const LitFieldValue<T> &B0,
      const T B0z,
      const LitFieldValue<T> &B1,
      const T B1z,
      const LitFieldValue<T> &B2,
      const T B2z ) {
      z0 = B0z;
      T dz1 = B1z - B0z, dz2 = B2z - B0z;
      T det = rcp(T(dz1 * dz2 * (dz2 - dz1)));
      T w21 = -dz2 * det;
      T w22 = dz1 * det;
      T w11 = -dz2 * w21;
      T w12 = -dz1 * w22;

      T dB1 = B1.Bx - B0.Bx;
      T dB2 = B2.Bx - B0.Bx;
      cx0 = B0.Bx;
      cx1 = dB1 * w11 + dB2 * w12;
      cx2 = dB1 * w21 + dB2 * w22;

      dB1 = B1.By - B0.By;
      dB2 = B2.By - B0.By;
      cy0 = B0.By;
      cy1 = dB1 * w11 + dB2 * w12;
      cy2 = dB1 * w21 + dB2 * w22;

      dB1 = B1.Bz - B0.Bz;
      dB2 = B2.Bz - B0.Bz;
      cz0 = B0.Bz;
      cz1 = dB1 * w11 + dB2 * w12;
      cz2 = dB1 * w21 + dB2 * w22;
   }

   /**
    * \brief Sets field region using two field values.
    *
    * In this case straight line approximation is used.
    *
    * \param[in] B0 First field value.
    * \param[in] B0z Z position of the first field value.
    * \param[in] B1 Second field value.
    * \param[in] B1z Z position of the second field value.
    */
   void Set(
      const LitFieldValue<T>& B0,
      const T B0z,
      const LitFieldValue<T>& B1,
      const T B1z) {
      z0 = B0z;
      T dzi = rcp(T(B1z - B0z));
      cx0 = B0.Bx;
      cy0 = B0.By;
      cz0 = B0.Bz;
      cx1 = (B1.Bx - B0.Bx) * dzi;
      cy1 = (B1.By - B0.By) * dzi;
      cz1 = (B1.Bz - B0.Bz) * dzi;
      cx2 = cy2 = cz2 = 0;
   }

   /**
    *
    */
   void Shift(
      T z) {
      T dz = z-z0;
      T cx2dz = cx2*dz;
      T cy2dz = cy2*dz;
      T cz2dz = cz2*dz;
      z0 = z[0];
      cx0+= ( cx1 + cx2dz )*dz;
      cy0+= ( cy1 + cy2dz )*dz;
      cz0+= ( cz1 + cz2dz )*dz;
      cx1+= cx2dz + cx2dz;
      cy1+= cy2dz + cy2dz;
      cz1+= cz2dz + cz2dz;
   }

   /**
    * \brief Returns field value at a certain Z position.
    *
    * \param[in] z Z position of the field value.
    * \param[out] B Field value.
    */
   void GetFieldValue(
      const T& z,
      LitFieldValue<T> &B) const {
      T dz = z - z0;
      T dzdz = dz * dz;
      B.Bx = cx0 + cx1 * dz + cx2 * dzdz;
      B.By = cy0 + cy1 * dz + cy2 * dzdz;
      B.Bz = cz0 + cz1 * dz + cz2 * dzdz;
   }

private:
   T cx0, cx1, cx2 ; // Bx(z) = cx0 + cx1*(z-z0) + cx2*(z-z0)^2
   T cy0, cy1, cy2 ; // By(z) = cy0 + cy1*(z-z0) + cy2*(z-z0)^2
   T cz0, cz1, cz2 ; // Bz(z) = cz0 + cz1*(z-z0) + cz2*(z-z0)^2
   T z0;
} _fvecalignment;

} // namespace parallel
} // namespace lit

#endif /* LITFIELDREGION_H_ */
