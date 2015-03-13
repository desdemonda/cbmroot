/**
 * \file LitMath.h
 *
 * \brief Useful math functions.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */
#ifndef LITMATH_H_
#define LITMATH_H_

#include "LitTypes.h"
#include "LitScalTrack.h"

namespace lit {
namespace parallel {

/**
 * \fn inline fscal rcp(const fscal& a)
 * \brief Returns reciprocal
 *
 * \param[in] a Input number.
 * \return Reciprocal of a.
 */
inline fscal rcp(const fscal& a) {return 1./a;}

/**
 * \fn inline fscal sgn(const fscal& a) {return a<0 ?-1 :(a>0 ?1 :0);}
 * \brief Returns sign of the input number.
 *
 * If input number is negative return -1,
 * if positive return 1 and if 0 return 0.
 *
 * \param[in] a Input number.
 * \return Sign of the input number.
 */
inline fscal sgn(const fscal& a) {return a<0 ?-1 :(a>0 ?1 :0);}

/**
 * \fn inline unsigned short NDF(const LitScalTrack& track)
 * \brief Returns number of degrees of freedom for the track.
 *
 * Note currently this function works only for pixel hits.
 * TODO This has to be corrected later.
 *
 * \param[in] track Reference to track.
 * \return Number of degrees of freedom for track.
 */
inline unsigned short NDF(
   const LitScalTrack& track)
{
   short ndf =  2 * track.GetNofHits() - 5;
   if (ndf > 0) { return ndf; }
   else { return 1; }

// int ndf = 0;
// for (int i = 0; i < track->GetNofHits(); i++){
//    if (track->GetHit(i)->GetType() == kLITPIXELHIT) ndf += 2;
//    else if (track->GetHit(i)->GetType() == kLITSTRIPHIT) ndf++;
// }
// ndf -= 5;
// if (ndf > 0) return ndf; else return 1;
}

} // namespace parallel
} // namespace lit
#endif /* LITMATH_H_ */
