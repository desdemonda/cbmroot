/**
 * \file LitScalPixelHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Base class for scalar pixel hits.
 */

#ifndef LITSCALPIXELHIT_H_
#define LITSCALPIXELHIT_H_

#include <sstream>
#include <string>
using std::stringstream;
using std::ostream;
using std::string;

namespace lit {
namespace parallel {

/**
 * \class LitScalPixelHit
 * \brief Base class for scalar pixel hits.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Contains more information in comparison with \c LitPixelHit.
 * Used for input scalar strip hit data to tracking.
 */
class LitScalPixelHit
{
public:
   /**
    * \brief Constructor.
    */
   LitScalPixelHit():
      X(0.),
      Y(0.),
      Dx(0.),
      Dy(0.),
      Dxy(0.),
      stationId(0),
      refId(0),
      Z(0.) {}

   /**
    * \brief Destructor.
    */
   virtual ~LitScalPixelHit() {}

   /**
    * \brief Returns string representation of the class.
    * \return Class representation as string.
    */
    string ToString() const {
       stringstream ss;
       ss << "LitScalPixelHit: X=" << X << " Y=" << Y << " Dx=" << Dx << " Dy=" << Dy
          << " Dxy=" << Dxy << " planeId=" << (int)stationId << " refId=" << refId
          << " Z=" << Z << "\n";
       return ss.str();
    }

    /**
     * \brief Operator << for convenient output to ostream.
     * \return Insertion stream in order to be able to call a succession of insertion operations.
     */
   friend ostream& operator<<(ostream& strm, const LitScalPixelHit& hit) {
      strm << hit.ToString();
      return strm;
   }

public:
   fscal X, Y; // X and Y position measurements [cm]
   fscal Dx, Dy; // X and Y position error [cm]
   fscal Dxy; // Covariance between X and Y [cm]
   unsigned char stationId; // Detector station ID
   unsigned short refId; // Reference id to smth
   fscal Z; // Z position [cm]
};

} // namespace parallel
} // namespace lit
#endif /* LITSCALPIXELHIT_H_ */
