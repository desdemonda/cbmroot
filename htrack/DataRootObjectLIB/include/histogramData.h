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
///   class:
///     - class for handling the hough-transformations memory space
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMDATA_H
#define _HISTOGRAMDATA_H


#include "../../DataObjectLIB/include/histogramSpace.h"
#include "histogramCell.h"
#include "borderCell.h"
#include "lutBorder.h"



//#define CORRECTHISTOGRAMLAYER		/**< This define corrects each cell in the histogram to be comparable with previous versions of the program. */
#undef CORRECTHISTOGRAMLAYER


/* **************************************************************
 * CLASS histogramData							 				*
 * **************************************************************/

class histogramData {

private:

	histogramSpace** space;			/**< Variable to store the size of the histogram in the first three dimensions. */
	unsigned short   valueDimSH;	/**< Variable to store the size of the histogram in the fourth dimension. This dimension is the number of the hit-bits in one cell. So it is equal to the number of detector stations. */
	histogramCell**  cell;			/**< Object to store one layer of the histogram. The size is dependent from valueDim1 and valueDim2. This object is used valueDim3 times serially. */
	borderCell*      hitBorders;	/**< Memory to store the borders for each layer of the histogram. */

/**
 * allocates memory
 * @param terminal is a buffer to place the process information
 */

	void allocateMemory(std::streambuf* terminal = NULL);

/**
 * deletes allocated memory
 * @param terminal is a buffer to place the process information
 */

	void deleteMemory(std::streambuf* terminal = NULL);

public:

/**
 * Default constructor
 */

	histogramData();

/**
 * Constructor
 * @param space is the size of the histogram in the first three dimensions
 * @param stationStep is the size of the histogram in the fourth dimension
 */

	histogramData(const histogramData& value);
	histogramData(histogramSpace** space);
	histogramData(histogramSpace** space, unsigned short stationStep);

/**
 * Destructor
 */

	virtual ~histogramData();

/**
 * operator = ()
 */

	const histogramData& operator = (const histogramData& value);

/**
 * This method initializes the object.
 * @param terminal is a buffer to place the process information
 * @param space is the size of the histogram in the first three dimensions
 * @param stationStep is the size of the histogram in the fourth dimension
 */

	void init(std::streambuf* terminal = NULL);
	void init(unsigned short stationStep, std::streambuf* terminal = NULL);
	void init(histogramSpace** space, std::streambuf* terminal = NULL);
	void init(histogramSpace** space, unsigned short stationStep, std::streambuf* terminal = NULL);

/**
 * This method resets the object.
 */

	void reset();

/**
 * This method resets the layer of the object.
 */

	void resetLayer();

/**
 * This method returns the size of the dimension 1 for the
 * histogram.
 */

	unsigned short getValueDim1();

/**
 * This method returns the size of the dimension 2 for the
 * histogram.
 */

	unsigned short getValueDim2();

/**
 * This method returns the size of the dimension 3 for the
 * histogram.
 */

	unsigned short getValueDim3();

/**
 * This method returns the size of the dimension SH for the
 * histogram.
 */

	unsigned short getValueDimSH();

/**
 * This method returns the cell at position (index1, index2)
 * from the histogram.
 */

	histogramCell* getCell(unsigned short index1, unsigned short index2);

/**
 * This method returns an iterator for the borders at position
 * (index) from the histogram.
 */

	std::vector<lutBorder>::iterator getBorderBeginIterator(unsigned short index);
	std::vector<lutBorder>::iterator getBorderEndIterator(unsigned short index);

/**
 * This method returns the number of borders at position (index) from the histogram.
 */

	unsigned long getNumberOfBorders(unsigned short index);

/**
 * This method sets the hit into the cell at position
 * (cellIndex1,	cellIndex2) for the detectorstation
 * (indexStation) of the histogram.
 */

	void setStationHit(unsigned short cellIndex1, unsigned short cellIndex2, unsigned short stationIndex, bool hit = true);

/**
 * This method adds the hit itself into the cell at position
 * (cellIndex1,	cellIndex2) for the detectorstation
 * (indexStation) of the histogram.
 */

	bool addStationHit(unsigned short cellIndex1, unsigned short cellIndex2, unsigned short stationIndex, trackfinderInputHit* hit);

/**
 * This method adds the border at position (index) to the
 * histogram.
 */

	void addBorder(lutBorder& border);

/**
 * This method prints a layer of the histogram into a file.
 * @param maximumClass is the maximal value for a class. This is printed just in the comptability mode instead of the correct value.
 * @param dim1Start is the start index in the first dimension of the histogram
 * @param dim1Stop is the stop index in the first dimension of the histogram
 * @param dim2Start is the start index in the second dimension of the histogram
 * @param dim2Stop is the stop index in the second dimension of the histogram
 * @param dimStationStart is the start index in the fourth dimension of the histogram
 * @param dimStationStop is the stop index in the fourth dimension of the histogram
 * @param hits should be true if the ids of the hits of each cell have to be printed also
 * @param name is the name of the file to print
 */

	void printLayer(unsigned int maximumClass, unsigned int dim1Start, unsigned int dim1Stop, unsigned int dim2Start, unsigned int dim2Stop, unsigned int dimStationStart, unsigned int dimStationStop, bool hits, char* name);
	void printLayer(unsigned int dim1Start, unsigned int dim1Stop, unsigned int dim2Start, unsigned int dim2Stop, unsigned int dimStationStart, unsigned int dimStationStop, bool hits, char* name);

/**
 * This method prints the histogram borders of the layers into a file.
 * @param dim3Start is the start index in the third dimension of the histogram
 * @param dim3Stop is the stop index in the third dimension of the histogram
 * @param name is the name of the file to print
 */

	void printBorder(unsigned int dim3Start, unsigned int dim3Stop, char* name);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHBufferData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHBufferData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHBufferData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHBufferPrelutData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHBufferPrelutData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHBufferPrelutData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHBufferLutData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHBufferLutData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHBufferLutData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHBufferHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHBufferHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHBufferHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHBufferAddOnData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHistogramData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHistogramData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHistogramData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHistogramSignatureData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHistogramSignatureData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHistogramSignatureData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHistogramHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHistogramHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHistogramHitData(unsigned short dimension = 0);

};

#endif
