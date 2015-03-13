/**
 * \file LitScalStripHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Base class for scalar strip hits.
 */

#ifndef LITSCALSTRIPHIT_H_
#define LITSCALSTRIPHIT_H_

#include <sstream>
#include <string>
using std::stringstream;
using std::ostream;
using std::string;

/**
 * \class LitScalStripHit
 * \brief Base class for scalar strip hits.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Contains more information in comparison with \c LitStripHit.
 * Used for input scalar strip hit data to tracking.
 */
class LitScalStripHit
{
public:
   /**
    * \brief Constructor.
    */
   LitScalStripHit():
      phiCos(0.),
      phiSin(0.),
      U(0.),
      Du(0.),
      stationId(0),
      refId(0),
      Z(0.) {}

   /**
    * \brief Destructor.
    */
   virtual ~LitScalStripHit() {}

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitScalStripHit: phiCos=" << phiCos << " phiSin=" << phiSin << " U=" << U
         << " Du=" << Du << " planeId=" << (int)stationId << " refId=" << refId << " Z=" << Z << "\n";
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitScalStripHit& hit) {
      strm << hit.ToString();
      return strm;
   }

public:
   fscal phiCos; // Cosine of strip rotation angle
   fscal phiSin; // Sine of strip rotation angle
   fscal U; // U measurement [cm]
   fscal Du; // U measurement error [cm]
   unsigned char stationId; // Detector station ID
   unsigned short refId; // Reference id to smth
   fscal Z; // Z position [cm]
};

#endif /* LITSCALSTRIPHIT_H_ */
