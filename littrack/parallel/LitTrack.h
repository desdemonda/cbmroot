/**
 * \file LitTrack.h
 * \brief Track data class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
#ifndef LITTRACK_H_
#define LITTRACK_H_

#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitPixelHit.h"

#include <vector>
#include <string>
#include <sstream>
using std::stringstream;
using std::string;
using std::vector;

namespace lit {
namespace parallel {

/**
 * \class LitTrack
 * \brief Base track data class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitTrack
{
public:
   /**
    * \brief Constructor.
    */
   LitTrack():
      paramLast(),
      chiSq(0.) {
      hits.reserve(30);
   }

   /**
    * \brief Destructor.
    */
   virtual ~LitTrack(){}

   /**
    * \brief Adds hit to track.
    * \param[in] hit Pointer to hit to be added.
    */
   void AddHit(LitPixelHit<T>* hit) {
      hits.push_back(hit);
   }

   /**
    * \brief Return number of hits in track.
    * \return Number of hits in track.
    */
   unsigned short GetNofHits() const {
      return hits.size();
   }

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitTrack: nofHits=" << GetNofHits() << " chiSq=" << chiSq
         << " paramLast=" << paramLast.ToString() << "\n";
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitTrack& track) {
      strm << track.ToString();
      return strm;
   }

public:
   LitTrackParam<T> paramLast; // Last parameter of the track
   vector<LitPixelHit<T>*> hits; // Array of hits
   fscal chiSq; // chi-square of the track
} _fvecalignment;

/**
 * \typedef LitTrack<fvec> LitTrackVec
 * \brief Vector version of the \c LitTrack class.
 */
typedef LitTrack<fvec> LitTrackVec;

/**
 * \typedef LitTrack<fscal> LitTrackScal
 * \brief Scalar version of the \c LitTrack class.
 */
typedef LitTrack<fscal> LitTrackScal;

} // namespace parallel
} // namespace lit
#endif /* LITTRACK_H_ */
