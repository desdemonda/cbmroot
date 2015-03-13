/**
 * \file LitEnums.h
 *
 * \brief Enumerations used in the parallel track reconstruction.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/

#ifndef LITENUMS_H_
#define LITENUMS_H_

namespace lit {
namespace parallel {

/**
 * \enum LitHitType
 * \brief Type of the hit and station.
 */
enum LitHitType {
	kLITSTRIPHIT = 0, /**< Strip like hit (u, phi). */
	kLITPIXELHIT = 1, /**< Pixel like hit (x, y). */
	kLITMIXHIT = 2 /**< Used for stations which produce both strip and pixel hits */
};

} // namespace parallel
} // namespace lit
#endif /*LITENUMS_H_*/
