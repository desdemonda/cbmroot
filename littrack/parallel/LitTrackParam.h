/**
 * \file LitTrackParam.h
 * \brief Track parameters data class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
#ifndef LITTRACKPARAM_H_
#define LITTRACKPARAM_H_ 1

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
 * \class LitTrackParam
 * \brief Track parameters data class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Track parameters consist of X and Y position of the track,
 * tx and ty slopes, q/p charge (+1 or -1) over momentum and
 * covariance matrix of this elements.
 *
 */
template<class T>
class LitTrackParam
{
public:
   /**
    * \brief Constructor.
    */
   LitTrackParam():
      X(0.), Y(0.), Z(0.), Tx(0.), Ty(0.), Qp(0.),
      C0(0.), C1(0.), C2(0.), C3(0.), C4(0.), C5(0.),
      C6(0.), C7(0.), C8(0.), C9(0.), C10(0.), C11(0.),
      C12(0.), C13(0.), C14(0.) {}

   /**
    * \brief Destructor.
    */
   virtual ~LitTrackParam() {}

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitTrackParam: X=" << X << " Y=" << Y << " Z=" << Z << " Tx=" << Tx
         << " Ty=" << Ty << " Qp=" << Qp << "\n" << " cov matrix[0-14]: "
         << C0 << " " << C1 << " " << C2 << " " << C3 << " " << C4 << " " << C5 << " "
         << C6 << " " << C7 << " " << C8 << " " << C9 << " " << C10 << " " << C11 << " "
         << C12 << " " << C13 << " " << C14 << "\n";
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitTrackParam& par) {
      strm << par.ToString();
      return strm;
   }

public:
   T X; // X position of the track
   T Y; // Y position of the track
   T Z; // Z position of the track
   T Tx; // Slope which equals to dx/dz
   T Ty; // Slope which equals to dy/dz
   T Qp; // Charge (+1 or -1) over momentum

   // Symetric covariance matrix
   T C0,  C1,  C2,  C3,  C4,
     C5,  C6,  C7,  C8,
     C9,  C10, C11,
     C12, C13,
     C14;
} _fvecalignment;

/**
 * \typedef LitTrackParam<fvec> LitTrackParamVec
 * \brief Vector version of \c LitTrackParam.
 */
typedef LitTrackParam<fvec> LitTrackParamVec;

/**
 * \typedef LitTrackParam<fscal> LitTrackParamScal
 * \brief Scalar version of \c LitTrackParam.
 */
typedef LitTrackParam<fscal> LitTrackParamScal;

} // namespace parallel
} // namespace lit
#endif /* LITTRACKPARAM_H_ */
