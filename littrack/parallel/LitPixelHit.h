/**
 * \file LitPixelHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Base class for pixel hits.
 */

#ifndef LITPIXELHIT_H_
#define LITPIXELHIT_H_

#include <sstream>
#include <string>
using std::stringstream;
using std::string;
using std::ostream;

namespace lit {
namespace parallel {
/**
 * \class LitPixelHit
 * \brief Base class for pixel hits.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Each hit contains X, Y, Z position measurements and
 * corresponding errors and covariance between X and Y.
 */
template<class T>
class LitPixelHit
{
public:
   /**
    * \brief Constructor.
    */
   LitPixelHit():
      X(0.),
      Y(0.),
      Z(0.),
      Dx(0.),
      Dy(0.),
      Dxy(0.) {}

   /**
    * Destructor.
    */
   virtual ~LitPixelHit() {}

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitPixelHit: XYZ=(" << X << "," << Y << "," << Z << ") "
         << " Dx=" << Dx << " Dy=" << Dy << " Dxy=" << Dxy << "\n";
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitPixelHit& hit) {
      strm << hit.ToString();
      return strm;
   }

public:
   T X, Y, Z; // X, Y, Z position measurements [cm]
   T Dx, Dy; // X and Y position error [cm]
   T Dxy; // Covariance between X and Y [cm]
} _fvecalignment;

/**
 * \typedef LitPixelHit<fscal> LitPixelHitScal
 * \brief Scalar version of LitPixelHit.
 */
typedef LitPixelHit<fscal> LitPixelHitScal;

/**
 * \typedef LitPixelHit<fvec> LitPixelHitVec
 * \brief Vector version of LitPixelHit.
 */
typedef LitPixelHit<fvec> LitPixelHitVec;

} // namespace parallel
} // namespace lit
#endif /* LITPIXELHIT_H_ */
