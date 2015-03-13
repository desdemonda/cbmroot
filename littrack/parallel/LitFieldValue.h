/**
 * \file LitFieldValue.h
 * \brief Magnetic field value at a certain point in the space.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef LITFIELDVALUE_H_
#define LITFIELDVALUE_H_

#include "LitTypes.h"
#include "LitUtils.h"
#include <string>
#include <sstream>
using std::string;
using std::stringstream;
using std::ostream;

namespace lit {
namespace parallel {

/**
 * \class LitFieldValue
 * \brief Magnetic field value at a certain point in the space.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitFieldValue
{
public:

   /**
    * \brief Constructor
    */
   LitFieldValue() :
      Bx(0.),
      By(0.),
      Bz(0.) { }

   /**
    * \brief Return string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitFieldValue: Bx=" << Bx << ", By=" << By << ", Bz=" << Bz << "\n";
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitFieldValue& v) {
      strm << v.ToString();
      return strm;
   }

public:
   T Bx, By, Bz; // components of the magnetic field
} _fvecalignment;

/**
 * \typedef LitFieldValue<fscal> LitFieldValueScal
 * \brief Scalar version of LitFieldValue.
 */
typedef LitFieldValue<fscal> LitFieldValueScal;

/**
 * \typedef LitFieldValue<fvec> LitFieldValueVec
 * \brief Vector version of LitFieldValue.
 */
typedef LitFieldValue<fvec> LitFieldValueVec;

} // namespace parallel
} // namespace lit

#endif /* LITFIELDVALUE_H_ */
