/**
 * \file LitStripHit.h
 * \brief Base class for strip hits.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef LITSTRIPHIT_H_
#define LITSTRIPHIT_H_

#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::ostream;

/**
 * \class LitStripHit
 * \brief Base class for strip hits.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Each hit contains U measurement and its error
 * and sine and cosine of strip rotation angle phi.
 */
template<class T>
class LitStripHit
{
public:
   /**
    * \brief Constructor.
    */
   LitStripHit():
      phiCos(0.),
      phiSin(0.),
      U(0.),
      Du(0.),
      Z(0.) {}

   /**
    * \brief Destructor.
    */
   virtual ~LitStripHit() {}

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitStripHit: phiCos=" << phiCos << " phiSin=" << phiSin
         << " U=" << U << " Du=" << Du << " Z=" << Z << "\n";
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitStripHit& hit) {
      strm << hit.ToString();
      return strm;
   }

public:
   T phiCos; // Cosine of strip rotation angle
   T phiSin; // Sine of strip rotation angle
   T U; // U measurement [cm]
   T Du; // U measurement error [cm]
   T Z; // Z position of the hit
} _fvecalignment;

/**
 * \typedef LitStripHit<fscal> LitStripHitScal
 * \brief Scalar version of LitStripHit.
 */
typedef LitStripHit<fscal> LitStripHitScal;

/**
 * \typedef LitStripHit<fvec> LitStripHitVec
 * \brief Vector version of LitStripHit.
 */
typedef LitStripHit<fvec> LitStripHitVec;

#endif /* LITSTRIPHIT_H_ */
