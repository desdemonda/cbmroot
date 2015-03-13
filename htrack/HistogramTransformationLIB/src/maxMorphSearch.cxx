/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - class for evaluating the histogram
//     - class computes the tracks
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:35:35 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/filterDef.h"
#include "../include/histogramTransformationError.h"
#include "../include/maxMorphSearch.h"
#include "../include/firstFilterFinal.h"
#include "../include/firstFilterFinalMod.h"
#include "../include/filterDim1.h"
#include "../include/filterDim1Dim2.h"
#include "../include/filterDim1Dim2Mod.h"
#include "../include/filterDim12Dim1.h"
#include "../include/filterDim12.h"
#include "../include/filterDim2.h"
#include "../include/filterDim3.h"
#include "../include/filterDim3Mod.h"
#include "../include/secondFilterFinal.h"
#include "../include/secondFilterFinalMod.h"


/****************************************************************
 * This method filters the peakclasses in a speciallized manner.*
 ****************************************************************/

void maxMorphSearch::filteringHistogramLayer() {

	if (histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);

	if (firstFilter != NULL)
		firstFilter->filter();
	if (secondFilter != NULL)
		secondFilter->filter();
	if (combiFilter != NULL)
		combiFilter->filter();

}

/****************************************************************
 * This method filters the tracks in a speciallized manner		*
 * again.														*
 ****************************************************************/

void maxMorphSearch::filteringHistogram(std::streambuf* terminal) {

	if (tracks == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);
	if (*tracks == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);

	if (thirdFilter != NULL)
		thirdFilter->filter(terminal);

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

maxMorphSearch::maxMorphSearch() 
  : histogramTransformation(),
    firstFilter(NULL),
    secondFilter(NULL),
    combiFilter(NULL),
    thirdFilter(NULL)
{
  /*
	firstFilter  = NULL;
	secondFilter = NULL;
	combiFilter  = NULL;
	thirdFilter  = NULL;
*/
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

maxMorphSearch::maxMorphSearch(histogramData** _histogram, trackData** _tracks, tables** _ratings,
							   unsigned short firstFilterGeometry, unsigned short firstFilterArithmetic,
							   unsigned short secondFilterGeometry, unsigned short secondFilterArithmetic,
							   unsigned short firstFilterDim1ClearRadius,
							   unsigned short firstFilterDim2ClearRadius,
							   unsigned short secondFilterDim1ClearRadius,
							   unsigned short secondFilterDim2ClearRadius,
							   unsigned short secondFilterDim3ClearRadius) 
  : histogramTransformation(_histogram, _tracks, _ratings),
    firstFilter(NULL),
    secondFilter(NULL),
    combiFilter(NULL),
    thirdFilter(NULL)
{

	unsigned short firstFilterNeighborhoodDim1ClearRadius;
	unsigned short firstFilterNeighborhoodDim1LocalRadius;
	unsigned short firstFilterNeighborhoodDim2ClearRadius;
	unsigned short firstFilterNeighborhoodDim2LocalRadius;
	unsigned short secondFilterNeighborhoodDim1ClearRadius;
	unsigned short secondFilterNeighborhoodDim1LocalRadius;
	unsigned short secondFilterNeighborhoodDim2ClearRadius;
	unsigned short secondFilterNeighborhoodDim2LocalRadius;
	unsigned short secondFilterNeighborhoodDim3ClearRadius;
	unsigned short secondFilterNeighborhoodDim3LocalRadius;

	firstFilterNeighborhoodDim1ClearRadius  = 2 * firstFilterDim1ClearRadius + 1;
	firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	firstFilterNeighborhoodDim2ClearRadius  = 2 * firstFilterDim2ClearRadius + 1;
	firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	secondFilterNeighborhoodDim1ClearRadius = 2 * secondFilterDim1ClearRadius + 1;
	secondFilterNeighborhoodDim1LocalRadius = 2 * (secondFilterDim1ClearRadius / 2) + 1;
	secondFilterNeighborhoodDim2ClearRadius = 2 * secondFilterDim2ClearRadius + 1;
	secondFilterNeighborhoodDim2LocalRadius = 2 * (secondFilterDim2ClearRadius / 2) + 1;
	secondFilterNeighborhoodDim3ClearRadius = 2 * secondFilterDim3ClearRadius + 1;
	secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;

	if (_ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*_ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	switch(firstFilterGeometry) {

		case FIRST21GEOMETRY:
			firstFilter  = new filterDim2( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			secondFilter = new filterDim1( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			combiFilter  = NULL;
			break;

		case FIRST122GEOMETRY:
			firstFilter  = new filterDim12( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			secondFilter = new filterDim2( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			combiFilter  = NULL;
			break;

		case FIRST12GEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new filterDim1Dim2( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case FIRST12MODGEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new filterDim1Dim2Mod( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case FIRST121GEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new filterDim12Dim1( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case FIRSTFINALGEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new firstFilterFinal( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case FIRSTFINALMODGEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new firstFilterFinalMod( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		default:
            firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = NULL;
			break;

	}

	switch(secondFilterGeometry) {

		case SECOND3MODGEOMETRY:
			thirdFilter  = new filterDim3Mod( _tracks,
						   secondFilterArithmetic,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case SECONDFINALGEOMETRY:
			thirdFilter = new secondFilterFinal( _tracks,
						   secondFilterArithmetic,
						   secondFilterNeighborhoodDim1ClearRadius,
						   secondFilterNeighborhoodDim2ClearRadius,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim1LocalRadius,
						   secondFilterNeighborhoodDim2LocalRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case SECONDFINALMODGEOMETRY:
			thirdFilter = new secondFilterFinalMod( _tracks,
						   secondFilterArithmetic,
						   secondFilterNeighborhoodDim1ClearRadius,
						   secondFilterNeighborhoodDim2ClearRadius,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim1LocalRadius,
						   secondFilterNeighborhoodDim2LocalRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case SECOND3GEOMETRY:
			thirdFilter = new filterDim3( _tracks,
						   secondFilterArithmetic,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		default:
			thirdFilter  = NULL;
			break;

	}

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

maxMorphSearch::~maxMorphSearch() {

	if (firstFilter != NULL) {
		delete firstFilter;
		firstFilter = NULL;
	}
	if (secondFilter != NULL) {
		delete secondFilter;
		secondFilter = NULL;
	}
	if (combiFilter != NULL) {
		delete combiFilter;
		combiFilter = NULL;
	}
	if (thirdFilter != NULL) {
		delete thirdFilter;
		thirdFilter = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void maxMorphSearch::init( histogramData** _histogram, trackData** _tracks,
						   tables** _ratings,
						   unsigned short firstFilterGeometry, unsigned short firstFilterArithmetic,
						   unsigned short secondFilterGeometry, unsigned short secondFilterArithmetic,
						   unsigned short firstFilterDim1ClearRadius,
						   unsigned short firstFilterDim2ClearRadius,
						   unsigned short secondFilterDim1ClearRadius,
						   unsigned short secondFilterDim2ClearRadius,
						   unsigned short secondFilterDim3ClearRadius) {

	unsigned short firstFilterNeighborhoodDim1ClearRadius;
	unsigned short firstFilterNeighborhoodDim1LocalRadius;
	unsigned short firstFilterNeighborhoodDim2ClearRadius;
	unsigned short firstFilterNeighborhoodDim2LocalRadius;
	unsigned short secondFilterNeighborhoodDim1ClearRadius;
	unsigned short secondFilterNeighborhoodDim1LocalRadius;
	unsigned short secondFilterNeighborhoodDim2ClearRadius;
	unsigned short secondFilterNeighborhoodDim2LocalRadius;
	unsigned short secondFilterNeighborhoodDim3ClearRadius;
	unsigned short secondFilterNeighborhoodDim3LocalRadius;

	histogramTransformation::init(_histogram, _tracks, _ratings);

	firstFilterNeighborhoodDim1ClearRadius  = 2 * firstFilterDim1ClearRadius + 1;
	firstFilterNeighborhoodDim1LocalRadius  = 2 * (firstFilterDim1ClearRadius / 2) + 1;
	firstFilterNeighborhoodDim2ClearRadius  = 2 * firstFilterDim2ClearRadius + 1;
	firstFilterNeighborhoodDim2LocalRadius  = 2 * (firstFilterDim2ClearRadius / 2) + 1;
	secondFilterNeighborhoodDim1ClearRadius = 2 * secondFilterDim1ClearRadius + 1;
	secondFilterNeighborhoodDim1LocalRadius = 2 * (secondFilterDim1ClearRadius / 2) + 1;
	secondFilterNeighborhoodDim2ClearRadius = 2 * secondFilterDim2ClearRadius + 1;
	secondFilterNeighborhoodDim2LocalRadius = 2 * (secondFilterDim2ClearRadius / 2) + 1;
	secondFilterNeighborhoodDim3ClearRadius = 2 * secondFilterDim3ClearRadius + 1;
	secondFilterNeighborhoodDim3LocalRadius = 2 * (secondFilterDim3ClearRadius / 2) + 1;

	if (_ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*_ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	switch(firstFilterGeometry) {

		case FIRST21GEOMETRY:
			firstFilter  = new filterDim2( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			secondFilter = new filterDim1( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			combiFilter  = NULL;
			break;

		case FIRST122GEOMETRY:
			firstFilter  = new filterDim12( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			secondFilter = new filterDim2( histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			combiFilter  = NULL;
			break;

		case FIRST12GEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new filterDim1Dim2( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case FIRST12MODGEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new filterDim1Dim2Mod( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case FIRST121GEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new filterDim12Dim1( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case FIRSTFINALGEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new firstFilterFinal( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case FIRSTFINALMODGEOMETRY:
			firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = new firstFilterFinalMod( _histogram,
						   firstFilterArithmetic,
						   firstFilterNeighborhoodDim1ClearRadius,
						   firstFilterNeighborhoodDim2ClearRadius,
						   firstFilterNeighborhoodDim1LocalRadius,
						   firstFilterNeighborhoodDim2LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		default:
            firstFilter  = NULL;
			secondFilter = NULL;
			combiFilter  = NULL;
			break;

	}

	switch(secondFilterGeometry) {

		case SECOND3MODGEOMETRY:
			thirdFilter  = new filterDim3Mod( _tracks,
						   secondFilterArithmetic,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case SECONDFINALGEOMETRY:
			thirdFilter = new secondFilterFinal( _tracks,
						   secondFilterArithmetic,
						   secondFilterNeighborhoodDim1ClearRadius,
						   secondFilterNeighborhoodDim2ClearRadius,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim1LocalRadius,
						   secondFilterNeighborhoodDim2LocalRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case SECONDFINALMODGEOMETRY:
			thirdFilter = new secondFilterFinalMod( _tracks,
						   secondFilterArithmetic,
						   secondFilterNeighborhoodDim1ClearRadius,
						   secondFilterNeighborhoodDim2ClearRadius,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim1LocalRadius,
						   secondFilterNeighborhoodDim2LocalRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		case SECOND3GEOMETRY:
			thirdFilter = new filterDim3( _tracks,
						   secondFilterArithmetic,
						   secondFilterNeighborhoodDim3ClearRadius,
						   secondFilterNeighborhoodDim3LocalRadius,
						   (*_ratings)->getCodingTableMaximumClassification());
			break;

		default:
			thirdFilter  = NULL;
			break;

	}

}
