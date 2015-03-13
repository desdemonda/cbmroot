//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   file:
///     - global definition of the transformation for the coordinate systems
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:55 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _COORDINATESYSTEM_H
#define _COORDINATESYSTEM_H


#include "memoryDef.h"

/*
 * Defines the histogram coordinates:
 * enum {DIM1, DIM2, DIM3, DIMENSIONS}
 * The enum cannot be used because of the
 * definition of maxDim1, maxDim2, maxDim3.
 * The exact reason is that the enum is
 * evaluated after the #define statement.
 * So the #define statements would be 
 * always NULL. That is why the #define
 * statement must be used here too.
 */

#define DIM1       0
#define DIM2       1
#define DIM3       2
#define DIMENSIONS 3


#define HTHETA     DIM1		/* This defines the histogram-axi in THETA-dimension to the dim1-coordinate. */
#define HRADIUS    DIM2		/* This defines the histogram-axi in RADIUS-dimension to the dim2-coordinate. */
#define HGAMMA     DIM3		/* This defines the histogram-axi in GAMMA-dimension to the dim3-coordinate. */

#define MTHETA     DIM1		/* This defines the momentum in THETA-dimension to the dim1-coordinate. */
#define MGAMMA     DIM2		/* This defines the momentum in GAMMA-dimension to the dim2-coordinate. */
#define MRADIUS    DIM3		/* This defines the momentum in RADIUS-dimension to the dim3-coordinate. */

#define PX         MTHETA	/* This defines the momentum in the x-dimension to the THETA-dimension. */
#define PY         MGAMMA	/* This defines the momentum in the y-dimension to the GAMMA-dimension. */
#define PZ         MRADIUS	/* This defines the momentum in the z-dimension to the RADIUS-dimension. */

/* Explanation:
 * The Histogram is programmed in the way:
 *
 *                                     ^
 *                                     |    /
 *                                     |   /
 *      1 / p_z = Radius = Dim2        |  /  p_y / p_z = Gamma = Dim3
 *                                     | /
 *                                     |/
 *                                      ------------------>
 *                                      p_x / p_z = Theta = Dim1
 *
 * The algorithms of the histogram, espescially the filters,
 * use always the DIMx way to get access to the values. The input of
 * the minimal and maximal sizes of the histogram is given in the
 * (Radius, Theta, Gamma) - Notation. This is done because the third
 * dimension is computed serially and one can imagine the first two
 * dimensions better. So the prelut computes the third dimension, while
 * the lut calculates the first and the second dimension.
 *
 * The momentum is also in a different coordinate system:
 *
 *                                     ^
 *                                     |    /
 *                                     |   /
 *    p_y = Gamma = Dim2               |  /  p_z = Radius = Dim3
 *                                     | /
 *                                     |/
 *                                      ------------------>
 *                                      p_x = Theta = Dim1
 *
 * while the momentum in the detector coordinate system is:
 *
 *                                     ^
 *                                     |    /
 *                                     |   /
 *                              p_x    |  /  p_y
 *                                     | /
 *                                     |/
 *                                      ------------------>
 *                                      p_z
 *
 */

/* Defines the correct maximal dimensions for the histogram
 * according to the maximal values corresponding to the
 * transformation */
#if ((HRADIUS == DIM1) && (HTHETA == DIM2) && (HGAMMA == DIM3))
	#define maxDim1 maxDimRadius
	#define maxDim2 maxDimTheta
	#define maxDim3 maxDimGamma
#elif ((HRADIUS == DIM1) && (HTHETA == DIM3) && (HGAMMA == DIM2))
	#define maxDim1 maxDimRadius
	#define maxDim2 maxDimGamma
	#define maxDim3 maxDimTheta
#elif ((HRADIUS == DIM2) && (HTHETA == DIM1) && (HGAMMA == DIM3))
	#define maxDim1 maxDimTheta
	#define maxDim2 maxDimRadius
	#define maxDim3 maxDimGamma
#elif ((HRADIUS == DIM2) && (HTHETA == DIM3) && (HGAMMA == DIM1))
	#define maxDim1 maxDimGamma
	#define maxDim2 maxDimRadius
	#define maxDim3 maxDimTheta
#elif ((HRADIUS == DIM3) && (HTHETA == DIM1) && (HGAMMA == DIM2))
	#define maxDim1 maxDimTheta
	#define maxDim2 maxDimGamma
	#define maxDim3 maxDimRadius
#elif ((HRADIUS == DIM3) && (HTHETA == DIM2) && (HGAMMA == DIM1))
	#define maxDim1 maxDimGamma
	#define maxDim2 maxDimTheta
	#define maxDim3 maxDimRadius
#endif


#endif
