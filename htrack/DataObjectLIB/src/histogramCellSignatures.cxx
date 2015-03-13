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
//     - class for holding the histogram cell signatures' analysis
//       information
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:43 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/histogramCellSignatures.h"
#include <math.h>
#include <cstdlib>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#define sqr(a)  ((a) * (a)) 


/****************************************************************
 * Default constructor											*
 ****************************************************************/

histogramCellSignatures::histogramCellSignatures() : numberOfSignatures(0), signatures(NULL), just2DFlag(false) {

  //	numberOfSignatures = 0;
  //	signatures         = NULL;
  //	just2DFlag         = false;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

histogramCellSignatures::histogramCellSignatures(const histogramCellSignatures& value) : numberOfSignatures(0), signatures(NULL), just2DFlag(false) {

	if (value.numberOfSignatures > 0) {

		numberOfSignatures = value.numberOfSignatures;
		signatures         = (histogramCellSignature*)calloc(numberOfSignatures, sizeof(histogramCellSignature));
		for (unsigned short i = 0; i < numberOfSignatures; i++)
			signatures[i]  = value.signatures[i];
		just2DFlag         = value.just2DFlag;

	}

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

histogramCellSignatures::~histogramCellSignatures() {

	reset();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const histogramCellSignatures& histogramCellSignatures::operator = (const histogramCellSignatures& value) {

	reset();

	if (value.numberOfSignatures > 0) {

		numberOfSignatures = value.numberOfSignatures;
		signatures         = (histogramCellSignature*)calloc(numberOfSignatures, sizeof(histogramCellSignature));
		for (unsigned short i = 0; i < numberOfSignatures; i++)
			signatures[i]  = value.signatures[i];
		just2DFlag         = value.just2DFlag;

	}

	return *this;

}

/****************************************************************
 * method resets all signatures									*
 ****************************************************************/

void histogramCellSignatures::reset() {

	numberOfSignatures = 0;

	if (signatures != NULL) {

		free(signatures);
		signatures    = NULL;

	}

	just2DFlag        = false;

}

/****************************************************************
 * method sets the 2D-flag which is used to find the best		*
 * signature. If it is true, the comparisson is jused done in	*
 * the first and the second dimension. If it is false, the		*
 * third dimension is also taken into account.					*
 ****************************************************************/

void histogramCellSignatures::set2DFlag(bool flag) {

	just2DFlag = flag;

}

/****************************************************************
 * method for adding a signature								*
 ****************************************************************/

void histogramCellSignatures::add(houghBorderPosition& pos, unsigned short stationIndex) {

	just2DFlag = true;

	add(pos, 0, stationIndex);

}
void histogramCellSignatures::add(houghBorderPosition& pos, unsigned short pos3, unsigned short stationIndex) {

	unsigned short signatureIndex;

	/* insert the first entry */
	if (numberOfSignatures == 0) {

		numberOfSignatures                = 1;
		signatures                        = (histogramCellSignature*)calloc(numberOfSignatures, sizeof(histogramCellSignature));
		signatures[0].pos.set(pos.pos1, DIM1);
		signatures[0].pos.set(pos.pos2, DIM2);
		signatures[0].pos.set(pos3, DIM3);
		signatures[0].value               = bitArray(0);
		signatures[0].value[stationIndex] = true;
		signatures[0].distance            = 0;

	}
	/* handle all other entries */
	else {
					
		/* check if this border has occured until now */
		signatureIndex = numberOfSignatures;
		for (unsigned short i = 0; i < numberOfSignatures; i++) {

			if ((signatures[i].pos.get(DIM1) == pos.pos1) && (signatures[i].pos.get(DIM2) == pos.pos2) && (signatures[i].pos.get(DIM3) == pos3)) {

				signatureIndex = i;
				break;

			}

		}
					
		/* if the border has not occured */
		if (signatureIndex == numberOfSignatures) {

			numberOfSignatures++;
			signatures                                     = (histogramCellSignature*)realloc(signatures, numberOfSignatures * sizeof(histogramCellSignature));
			signatures[signatureIndex].pos.set(pos.pos1, DIM1);
			signatures[signatureIndex].pos.set(pos.pos2, DIM2);
			signatures[signatureIndex].pos.set(pos3, DIM3);
			signatures[signatureIndex].value               = bitArray(0);
			signatures[signatureIndex].value[stationIndex] = true;
			signatures[signatureIndex].distance            = 0;

		}
		/* if the border has occured */
		else
			signatures[signatureIndex].value[stationIndex] = true;

	}

}

/****************************************************************
 * method returns the 2D-flag which is used to find the best	*
 * signature.													*
 ****************************************************************/

bool histogramCellSignatures::get2DFlag() {

	return just2DFlag;

}

/****************************************************************
 * method for getting the best signature						*
 ****************************************************************/

histogramCellSignature histogramCellSignatures::getBest(trackCoordinates& pos) {

	histogramCellSignature bestHistogramCellSignature;

	bestHistogramCellSignature.value    = bitArray(0);
	bestHistogramCellSignature.distance = 0;
	for (unsigned short i = 0; i < numberOfSignatures; i++) {

		if (just2DFlag)
			signatures[i].distance = (double)sqrt((double)sqr(signatures[i].pos.get(DIM1) - pos.get(DIM1)) + (double)sqr(signatures[i].pos.get(DIM2) - pos.get(DIM2)));
		else
			signatures[i].distance = (double)sqrt((double)sqr(signatures[i].pos.get(DIM1) - pos.get(DIM1)) + (double)sqr(signatures[i].pos.get(DIM2) - pos.get(DIM2)) + (double)sqr(signatures[i].pos.get(DIM3) - pos.get(DIM3)));

		if (signatures[i].value.count() > bestHistogramCellSignature.value.count())
			bestHistogramCellSignature = signatures[i];
		/* This is added because if there is a equal found clone peak which is nearer to the
		 * correct cell, this peak should be used because of a better quality approximation */
		else if ((signatures[i].value.count() == bestHistogramCellSignature.value.count()) && (signatures[i].distance < bestHistogramCellSignature.distance))
			bestHistogramCellSignature = signatures[i];

	}

	return bestHistogramCellSignature;

}
