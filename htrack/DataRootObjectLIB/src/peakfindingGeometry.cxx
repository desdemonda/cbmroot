//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
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
///     - class for holding the information of the automatic
///       generated peakfinding geometry
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:59:23 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../DataObjectLIB/include/peakfindingGeometryFile.h"
#include "../include/dataRootObjectError.h"
#include "../include/dataRootObjectWarningMsg.h"
#include "../include/peakfindingGeometry.h"
#include <list>
#include <cmath>

#define RELATIVERANGE 2


/* **************************************************************
 * creates a string representation for the object				*
 * **************************************************************/

std::string peakfindingGeometry::createString(bool projection, bool coverage) {

	std::string                                returnValue;
	char                                       buffer[shortConversionDigits+1];
	specialListMem<peakfindingGeometryElement> actualGeometry;
	peakfindingGeometryElement                 actualGeometryElement;

	returnValue.clear();

	if (coverage) {

		if (projection)
			returnValue += "Covered Projected Peakfinding Geometry:\n";
		else
			returnValue += "Covered Peakfinding Geometry:\n";

	}
	else {

		if (projection){

			returnValue += "Projected Peakfinding Geometry:\n";
			returnValue += "Percentage of coverage: ";
			ustos(percentageOfCoverage, buffer, 10, shortConversionDigits);
			returnValue += buffer;
			returnValue += "\n";

		}		
		else {

			returnValue += "Peakfinding Geometry:\n";
			returnValue += "Percentage of coverage: ";
			ustos(percentageOfCoverage, buffer, 10, shortConversionDigits);
			returnValue += buffer;
			returnValue += "\n";

		}

	}

	if (coverage) {

		if (projection)
			actualGeometry = getCoveredProjectedGeometryElements();
		else
			actualGeometry = getCoveredGeometryElements();

	}
	else {

		if (projection)
			actualGeometry = getProjectedGeometryElements();
		else
			actualGeometry = getGeometryElements();

	}

	actualGeometry.resetActiveObject();
	for (unsigned long i = 0; i < actualGeometry.getNumberOfEntries(); i++) {

		actualGeometryElement = actualGeometry.readActiveObjectAndMakeNextOneActive();

		stos(actualGeometryElement.get(DIM1), buffer, 10, shortConversionDigits);
		returnValue += buffer;
		returnValue += ", ";
		stos(actualGeometryElement.get(DIM2), buffer, 10, shortConversionDigits);
		returnValue += buffer;
		if (!projection) {

			returnValue += ", ";
			stos(actualGeometryElement.get(DIM3), buffer, 10, shortConversionDigits);
			returnValue += buffer;

		}
		returnValue += ": ";
		ustos(actualGeometryElement.getValue(), buffer, 10, shortConversionDigits);
		returnValue += buffer;
		returnValue += "\n";

	}

	return returnValue;

}

/* **************************************************************
 * creates all displays for a normal drawing					*
 * **************************************************************/

void peakfindingGeometry::createDisplay(TH2S*** indicators, unsigned short* numberOfIndicators, std::string name, std::string title, bool projection, bool coverage) {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	peakfindingGeometryElement                 actualGeometryElement;
	std::list<short>                           layers;
	short                                      xMin;
	short                                      xMax;
	short                                      yMin;
	short                                      yMax;
	unsigned long                              i;
	char                                       buffer[shortConversionDigits+1];
	std::string                                actualName;
	std::string                                actualTitle;

	if (indicators == NULL)
		throw cannotAccessIndicatorsError();

	if (numberOfIndicators == NULL)
		throw cannotAccessNumberOfIndicatorsError();

	if (*numberOfIndicators > 0)
		deleteDisplayMemory(indicators, *numberOfIndicators);


	if (coverage) {

		if (projection)
			actualGeometry = getCoveredProjectedGeometryElements();
		else
			actualGeometry = getCoveredGeometryElements();

	}
	else {

		if (projection)
			actualGeometry = getProjectedGeometryElements();
		else
			actualGeometry = getGeometryElements();

	}

	layers.clear();
	actualGeometry.resetActiveObject();
	for (i = 0; i < actualGeometry.getNumberOfEntries(); i++) {

		actualGeometryElement = actualGeometry.readActiveObjectAndMakeNextOneActive();

		layers.push_back(actualGeometryElement.get(DIM3));

	}

	layers.sort();
	layers.unique();

	*numberOfIndicators = layers.size();

	if (*numberOfIndicators > 0) {

		(*indicators) = new TH2S*[*numberOfIndicators];
		if ((*indicators) == NULL)
			throw memoryAllocationError(DATAROOTOBJECTLIB);

		getMinMaxDimensions(&xMin, &xMax, &yMin, &yMax, NULL, NULL);

		for (unsigned short j = 0; j < *numberOfIndicators; j++) {

			short& layer = layers.front();

			if (projection) {

				actualName   = name;
				actualTitle  = title;

			}
			else {

				stos(layer, buffer, 10, shortConversionDigits);

				actualName   = name;
				actualName  += "_layer_";
				actualName  += buffer;
				actualTitle  = title;
				actualTitle += " (layer:";
				actualTitle += buffer;
				actualTitle += ")";

			}

			(*indicators)[j]  = new TH2S(actualName.c_str(), actualTitle.c_str(), xMax - xMin + 3, xMin - 1, xMax + 1, yMax - yMin + 3, yMin - 1, yMax + 1);
			if ((*indicators)[j] == NULL)
				throw memoryAllocationError(DATAROOTOBJECTLIB);

			(*indicators)[j]->SetXTitle("Dim1");
			(*indicators)[j]->SetYTitle("Dim2");

			actualGeometry.resetActiveObject();
			for (i = 0; i < actualGeometry.getNumberOfEntries(); i++) {

				actualGeometryElement = actualGeometry.readActiveObjectAndMakeNextOneActive();

				if (actualGeometryElement.get(DIM3) == layer)
					(*indicators)[j]->Fill(actualGeometryElement.get(DIM1), actualGeometryElement.get(DIM2), actualGeometryElement.getValue());

			}

			layers.pop_front();

		}

	}

}

/* **************************************************************
 * deletes allocated memory										*
 * **************************************************************/

void peakfindingGeometry::deleteDisplayMemory(TH2S*** memory, unsigned short numberOfMemoryElements) {

	if (memory != NULL) {
		if ((*memory) != NULL)
		for (unsigned short i = 0; i < numberOfMemoryElements; i++) {
			if ((*memory)[i] != NULL) {
				delete (*memory)[i];
				(*memory)[i] = NULL;
			}
		}
		delete [] (*memory);
		(*memory) = NULL;
	}

}

/****************************************************************
 * This method initializes the global style for each display.	*
 ****************************************************************/

void peakfindingGeometry::initDisplayStyle() {

	/* setting some globals for the display */

	if (gStyle != NULL) {

		gStyle->SetCanvasBorderMode(-1);
		gStyle->SetCanvasBorderSize(1);
		gStyle->SetCanvasColor(10);

		gStyle->SetFrameFillColor(10);
		gStyle->SetFrameBorderSize(1);
		gStyle->SetFrameBorderMode(-1);
		gStyle->SetFrameLineWidth(2);
		gStyle->SetFrameLineColor(1);

		gStyle->SetHistFillColor(0);
		gStyle->SetHistLineWidth(2);
		gStyle->SetHistLineColor(1);

		gStyle->SetPadColor(10);
		gStyle->SetPadBorderSize(1);
		gStyle->SetPadBorderMode(-1);

		gStyle->SetStatColor(10);
		gStyle->SetStatFont(22);

		gStyle->SetTitleColor(1, "X");
		gStyle->SetTitleOffset(1.0, "X");
		gStyle->SetTitleFont(22, "X");
		gStyle->SetLabelFont(22, "X");

		gStyle->SetTitleColor(1, "Y");
		gStyle->SetTitleOffset(0.8, "Y");
		gStyle->SetTitleFont(22, "Y");
		gStyle->SetLabelFont(22, "Y");

		gStyle->SetOptDate(0);
		gStyle->SetOptTitle(1);
		gStyle->SetOptStat(0);
		gStyle->SetOptFit(1);

	}
	else {

		gStyleNotFoundWarningMsg* gStyleNotFound = new gStyleNotFoundWarningMsg(DATAROOTOBJECTLIB);
		gStyleNotFound->warningMsg();
		if(gStyleNotFound != NULL) {
			delete gStyleNotFound;
			gStyleNotFound = NULL;
		}

	}

}

/****************************************************************
 * This method shows the created displays.						*
 ****************************************************************/

void peakfindingGeometry::showDisplay(TCanvas* window, unsigned short* numberOfWindowPadColumns, unsigned short* numberOfWindowPadRows, TH2S** indicators, unsigned short numberOfIndicators, std::string name, std::string title) {

	unsigned short numberOfEnabledPadColumns;
	unsigned short numberOfEnabledPadRows;

	if ((numberOfWindowPadColumns == NULL) || (numberOfWindowPadRows == NULL))
		throw windowDivisionError(DATAROOTOBJECTLIB);

	evaluateWindowDivisionFactors(&numberOfEnabledPadColumns, &numberOfEnabledPadRows, numberOfIndicators);

	if ((*numberOfWindowPadColumns != 0) || (*numberOfWindowPadRows != 0)) {

		if ((*numberOfWindowPadColumns != numberOfEnabledPadColumns) || (*numberOfWindowPadRows != numberOfEnabledPadRows)) {

			if (window != NULL) {
				delete window;
				window = NULL;
			}

		}

	}

	if (window == NULL) {

		initDisplayStyle();

		window = new TCanvas(name.c_str(), title.c_str(), 600, 400);

		*numberOfWindowPadColumns = 0;
		*numberOfWindowPadRows    = 0;

	}
	else {

		window->SetName(name.c_str());
		window->SetTitle(title.c_str());

	}

	if ((*numberOfWindowPadColumns == 0) && (*numberOfWindowPadRows == 0)) {

		*numberOfWindowPadColumns = numberOfEnabledPadColumns;
		*numberOfWindowPadRows    = numberOfEnabledPadRows;
		window->Divide(*numberOfWindowPadColumns, *numberOfWindowPadRows, 0, 0, 0);
	
	}

	if (indicators != NULL) {

		for (unsigned short i = 0; i < numberOfIndicators; i++) {

			window->cd(i + 1);
				
			if (indicators[i] != NULL) {

				indicators[i]->Draw("TEXT COL");

			}
			else {

				cannotDrawWarningMsg* cannotDraw = new cannotDrawWarningMsg(DATAROOTOBJECTLIB);
				cannotDraw->warningMsg();
				if(cannotDraw != NULL) {
					delete cannotDraw;
					cannotDraw = NULL;
				}
			
			}

		}

	}
	else {

		cannotDrawWarningMsg* cannotDraw = new cannotDrawWarningMsg(DATAROOTOBJECTLIB);
		cannotDraw->warningMsg();
		if(cannotDraw != NULL) {
			delete cannotDraw;
			cannotDraw = NULL;
		}

	}

	window->cd();

}

/****************************************************************
 * This method evaluate the window division factors.			*
 ****************************************************************/

void peakfindingGeometry::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows, unsigned short numberOfIndicators) {

	double sqrtOfDisplays;

	if ((numberOfEnabledPadColumns == NULL) || (numberOfEnabledPadRows == NULL))
		throw windowDivisionError(DATAROOTOBJECTLIB);

	sqrtOfDisplays = sqrt((double)numberOfIndicators);

	if ((unsigned short)sqrtOfDisplays * (unsigned short)sqrtOfDisplays >= numberOfIndicators) {

		(*numberOfEnabledPadColumns) = (unsigned short)sqrtOfDisplays;
		(*numberOfEnabledPadRows)    = (unsigned short)sqrtOfDisplays;

	}
	else if (((unsigned short)sqrtOfDisplays + 1) * (unsigned short)sqrtOfDisplays >= numberOfIndicators) {

		(*numberOfEnabledPadColumns) = (unsigned short)(sqrtOfDisplays + 1);
		(*numberOfEnabledPadRows)    = (unsigned short)sqrtOfDisplays;

	}
	else {

		(*numberOfEnabledPadColumns) = (unsigned short)(sqrtOfDisplays + 1);
		(*numberOfEnabledPadRows)    = (unsigned short)(sqrtOfDisplays + 1);

	}

}

/****************************************************************
 * build a list containing special peakfinding geometry elements*
 ****************************************************************/

specialListMem<peakfindingGeometryElement> peakfindingGeometry::buildProjectedGeometry(specialListMem<peakfindingGeometryElement>& actualGeometry) {

	specialListMem<peakfindingGeometryElement> returnValue;
	peakfindingGeometryElement                 element;

	returnValue.clear();

	actualGeometry.resetActiveObject();
	for (unsigned long i = 0; i < actualGeometry.getNumberOfEntries(); i++) {

		element = actualGeometry.readActiveObjectAndMakeNextOneActive();

		element.set(0, DIM3);

		if (returnValue.isFound(element, true))		/* element is old element */
			returnValue.getActiveObject()->incrementValue(element.getValue());
		else										/* element not found, so add it */
			returnValue.push(element);

	}

	returnValue.sort();

	return returnValue;

}
specialListMem<peakfindingGeometryElement> peakfindingGeometry::buildCoveredGeometry(specialListMem<peakfindingGeometryElement>& actualGeometry) {

	specialListMem<peakfindingGeometryElement> returnValue;
	unsigned short                             limit;
	peakfindingGeometryElement                 element;

	returnValue.clear();
	limit = 0;

	element.set(0, DIM1);
	element.set(0, DIM2);
	element.set(0, DIM3);

	if (actualGeometry.isFound(element, true)) {

		element = actualGeometry.readActiveObject();
		limit = ((100 - percentageOfCoverage) * element.getValue()) / 100;

	}

	actualGeometry.resetActiveObject();
	for (unsigned long i = 0; i < actualGeometry.getNumberOfEntries(); i++) {

		element = actualGeometry.readActiveObjectAndMakeNextOneActive();

		if (element.getValue() > limit) {

			element.setValue(element.getValue() - limit);
			returnValue.push(element);

		}

	}

	returnValue.sort();

	return returnValue;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

peakfindingGeometry::peakfindingGeometry() 
  : percentageOfCoverage(0),
    geometry(),
    canvas(NULL),
    numberOfCanvasPadColumns(0),
    numberOfCanvasPadRows(0),
    displays(NULL),
    numberOfDisplays(0),
    projectionCanvas(NULL),
    numberOfProjectionCanvasPadColumns(0),
    numberOfProjectionCanvasPadRows(0),
    projectionDisplays(NULL),
    numberOfProjectionDisplays(0),
    coverageCanvas(NULL),
    numberOfCoverageCanvasPadColumns(0),
    numberOfCoverageCanvasPadRows(0),
    coverageDisplays(NULL),
    numberOfCoverageDisplays(0),
    coverageProjectionCanvas(NULL),
    numberOfCoverageProjectionCanvasPadColumns(0),
    numberOfCoverageProjectionCanvasPadRows(0),
    coverageProjectionDisplays(NULL),
    numberOfCoverageProjectionDisplays(0)
{
  reset();
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

peakfindingGeometry::peakfindingGeometry(const peakfindingGeometry& value) 
  : percentageOfCoverage(0),
    geometry(),
    canvas(NULL),
    numberOfCanvasPadColumns(0),
    numberOfCanvasPadRows(0),
    displays(NULL),
    numberOfDisplays(0),
    projectionCanvas(NULL),
    numberOfProjectionCanvasPadColumns(0),
    numberOfProjectionCanvasPadRows(0),
    projectionDisplays(NULL),
    numberOfProjectionDisplays(0),
    coverageCanvas(NULL),
    numberOfCoverageCanvasPadColumns(0),
    numberOfCoverageCanvasPadRows(0),
    coverageDisplays(NULL),
    numberOfCoverageDisplays(0),
    coverageProjectionCanvas(NULL),
    numberOfCoverageProjectionCanvasPadColumns(0),
    numberOfCoverageProjectionCanvasPadRows(0),
    coverageProjectionDisplays(NULL),
    numberOfCoverageProjectionDisplays(0)
{
  reset();

  setCoverage(value.percentageOfCoverage);
  this->geometry = value.geometry;
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

peakfindingGeometry::~peakfindingGeometry() {

	reset();

	deleteDisplayMemory(&displays, numberOfDisplays);
	deleteDisplayMemory(&projectionDisplays, numberOfProjectionDisplays);
	deleteDisplayMemory(&coverageDisplays, numberOfCoverageDisplays);
	deleteDisplayMemory(&coverageProjectionDisplays, numberOfCoverageProjectionDisplays);

	if (canvas != NULL) {
		delete canvas;
		canvas = NULL;
	}
	if (projectionCanvas != NULL) {
		delete projectionCanvas;
		projectionCanvas = NULL;
	}
	if (coverageCanvas != NULL) {
		delete coverageCanvas;
		coverageCanvas = NULL;
	}
	if (coverageProjectionCanvas != NULL) {
		delete coverageProjectionCanvas;
		coverageProjectionCanvas = NULL;
	}

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const peakfindingGeometry& peakfindingGeometry::operator = (const peakfindingGeometry& value) {

	reset();

	canvas                                     = NULL;
	numberOfCanvasPadColumns                   = 0;
	numberOfCanvasPadRows                      = 0;
	displays                                   = NULL;
	numberOfDisplays                           = 0;
	projectionCanvas                           = NULL;
	numberOfProjectionCanvasPadColumns         = 0;
	numberOfProjectionCanvasPadRows            = 0;
	projectionDisplays                         = NULL;
	numberOfProjectionDisplays                 = 0;
	coverageCanvas                             = NULL;
	numberOfCoverageCanvasPadColumns           = 0;
	numberOfCoverageCanvasPadRows              = 0;
	coverageDisplays                           = NULL;
	numberOfCoverageDisplays                   = 0;
	coverageProjectionCanvas                   = NULL;
	numberOfCoverageProjectionCanvasPadColumns = 0;
	numberOfCoverageProjectionCanvasPadRows    = 0;
	coverageProjectionDisplays                 = NULL;
	numberOfCoverageProjectionDisplays         = 0;

	setCoverage(value.percentageOfCoverage);
	this->geometry = value.geometry;

	return *this;

}

/****************************************************************
 * get percentage of coverage
 ****************************************************************/

unsigned short peakfindingGeometry::getCoverage() {

	return percentageOfCoverage;

}

/****************************************************************
 * set percentage of coverage
 ****************************************************************/

void peakfindingGeometry::setCoverage(unsigned short _percentageOfCoverage) {

	if (_percentageOfCoverage > 100)
		this->percentageOfCoverage = 100;
	else
		this->percentageOfCoverage = _percentageOfCoverage;

}

/****************************************************************
 * resets the object											*
 ****************************************************************/

void peakfindingGeometry::reset() {

	setCoverage(0);
	resetElements();

}

/****************************************************************
 * removes all elements											*
 ****************************************************************/

void peakfindingGeometry::resetElements() {

	geometry.clear();

}

/****************************************************************
 * Returns the element in the geometry which is farest away		*
 * from the origin element and smaller than this one.			*
 ****************************************************************/

peakfindingGeometryElement peakfindingGeometry::getCoveredProjectedMinElement() {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	peakfindingGeometryElement                 returnValue;
	peakfindingGeometryElement                 element;

	actualGeometry = getCoveredProjectedGeometryElements();

	actualGeometry.resetActiveObject();
	if (actualGeometry.getNumberOfEntries() > 0) {

		returnValue         = actualGeometry.readActiveObjectAndMakeNextOneActive();
		for (unsigned long i = 1; i < actualGeometry.getNumberOfEntries(); i++) {

			element = actualGeometry.readActiveObjectAndMakeNextOneActive();

			if ((element.get(DIM2) < returnValue.get(DIM2)) || ((element.get(DIM2) == returnValue.get(DIM2)) && (element.get(DIM1) < returnValue.get(DIM1))))
				returnValue = element;

		}

		if ((returnValue.get(DIM2) > 0) || ((returnValue.get(DIM2) == 0) && (returnValue.get(DIM1) > 0))) {

			missingOriginWarningMsg* missingOrigin = new missingOriginWarningMsg();
			missingOrigin->warningMsg();
			if(missingOrigin != NULL) {
				delete missingOrigin;
				missingOrigin = NULL;
			}

		}

	}
	else
		returnValue = peakfindingGeometryElement(0, 0, 0, 0);

	return returnValue;

}

/****************************************************************
 * Returns the element in the geometry which is farest away		*
 * from the origin element and bigger than this one.			*
 ****************************************************************/

peakfindingGeometryElement peakfindingGeometry::getCoveredProjectedMaxElement() {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	peakfindingGeometryElement                 returnValue;
	peakfindingGeometryElement                 element;

	actualGeometry = getCoveredProjectedGeometryElements();

	actualGeometry.resetActiveObject();
	returnValue         = actualGeometry.readActiveObjectAndMakeNextOneActive();
	for (unsigned long i = 1; i < actualGeometry.getNumberOfEntries(); i++) {

		element = actualGeometry.readActiveObjectAndMakeNextOneActive();

		if ((element.get(DIM2) > returnValue.get(DIM2)) || ((element.get(DIM2) == returnValue.get(DIM2)) && (element.get(DIM1) > returnValue.get(DIM1))))
			returnValue = element;

	}

	if ((returnValue.get(DIM2) < 0) || ((returnValue.get(DIM2) == 0) && (returnValue.get(DIM1) < 0))) {

		missingOriginWarningMsg* missingOrigin = new missingOriginWarningMsg();
		missingOrigin->warningMsg();
		if(missingOrigin != NULL) {
			delete missingOrigin;
			missingOrigin = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * Returns the min/max in the geometry which is farest away		*
 * from the origin element.										*
 ****************************************************************/

void peakfindingGeometry::getMinMaxDimensions(short* dim1Min, short* dim1Max, short* dim2Min, short* dim2Max, short* dim3Min, short* dim3Max) {

	peakfindingGeometryElement                 actualGeometryElement;
	bool                                       isFirstElementFound;

	isFirstElementFound = false;
	geometry.resetActiveObject();
	for (unsigned long i = 0; i < geometry.getNumberOfEntries(); i++) {

		actualGeometryElement = geometry.readActiveObjectAndMakeNextOneActive();

		if (!isFirstElementFound) {

			if (dim1Min != NULL)
				*dim1Min        = actualGeometryElement.get(DIM1);
			if (dim1Max != NULL)
				*dim1Max        = actualGeometryElement.get(DIM1);
			if (dim2Min != NULL)
				*dim2Min        = actualGeometryElement.get(DIM2);
			if (dim2Max != NULL)
				*dim2Max        = actualGeometryElement.get(DIM2);
			if (dim3Min != NULL)
				*dim3Min        = actualGeometryElement.get(DIM3);
			if (dim3Max != NULL)
				*dim3Max        = actualGeometryElement.get(DIM3);
			isFirstElementFound = true;

		}
		else {

			if (dim1Min != NULL) {
				if (actualGeometryElement.get(DIM1) < *dim1Min)
					*dim1Min = actualGeometryElement.get(DIM1);
			}
			if (dim1Max != NULL) {
				if (actualGeometryElement.get(DIM1) > *dim1Max)
					*dim1Max = actualGeometryElement.get(DIM1);
			}
			if (dim2Min != NULL) {
				if (actualGeometryElement.get(DIM2) < *dim2Min)
					*dim2Min = actualGeometryElement.get(DIM2);
			}
			if (dim2Max != NULL) {
				if (actualGeometryElement.get(DIM2) > *dim2Max)
					*dim2Max = actualGeometryElement.get(DIM2);
			}
			if (dim3Min != NULL) {
				if (actualGeometryElement.get(DIM3) < *dim3Min)
					*dim3Min = actualGeometryElement.get(DIM3);
			}
			if (dim3Max != NULL) {
				if (actualGeometryElement.get(DIM3) > *dim3Max)
					*dim3Max = actualGeometryElement.get(DIM3);
			}

		}

	}

}

/****************************************************************
 * get a list containing the peakfinding geometry elements		*
 ****************************************************************/

specialListMem<peakfindingGeometryElement> peakfindingGeometry::getGeometryElements() {

	return geometry;

}
specialListMem<peakfindingGeometryElement> peakfindingGeometry::getProjectedGeometryElements() {

	return buildProjectedGeometry(geometry);

}
specialListMem<peakfindingGeometryElement> peakfindingGeometry::getCoveredGeometryElements() {

	return buildCoveredGeometry(geometry);

}
specialListMem<peakfindingGeometryElement> peakfindingGeometry::getCoveredProjectedGeometryElements() {

	specialListMem<peakfindingGeometryElement> temp;

	temp = buildProjectedGeometry(geometry);

	return buildCoveredGeometry(temp);

}

/****************************************************************
 * add element													*
 ****************************************************************/

void peakfindingGeometry::addGeometryElement(peakfindingGeometryElement& element) {

	if (geometry.isFound(element, true))		/* element is old element */
		geometry.getActiveObject()->incrementValue(element.getValue());
	else {										/* element not found, so add it */
		geometry.push(element);
		geometry.sort();
	}

}
void peakfindingGeometry::addGeometryElement(short position1, short position2, short position3, unsigned short value) {

	peakfindingGeometryElement element;

	element.set(position1, DIM1);
	element.set(position2, DIM2);
	element.set(position3, DIM3);
	element.setValue(value);

	addGeometryElement(element);

}

/****************************************************************
 * Method adds whole other peakfinding geometry elements, but	*
 * will not modify the coverage									*
 ****************************************************************/

void peakfindingGeometry::addGeometryElements(specialListMem<peakfindingGeometryElement>& elements) {

	peakfindingGeometryElement temp;

	elements.resetActiveObject();
	for (unsigned long i = 0; i < elements.getNumberOfEntries(); i++) {

		temp = elements.readActiveObjectAndMakeNextOneActive();

		addGeometryElement(temp);

	}

}
void peakfindingGeometry::addGeometryElements(specialListMem<peakfindingGeometryElement>& elements, trackCoordinates& center) {

	peakfindingGeometryElement element;
	bool                       isElementFound;
	unsigned long              i;
	short                      position1;
	short                      position2;
	short                      position3;

	element.set((short)center.get(DIM1), DIM1);
	element.set((short)center.get(DIM2), DIM2);
	element.set((short)center.get(DIM3), DIM3);

	isElementFound = elements.isFound(element, false);

	if (isElementFound) {

		elements.resetActiveObject();

		for (i = 0; i < elements.getNumberOfEntries(); i++) {

			element   = elements.readActiveObjectAndMakeNextOneActive();
		
			position1 = element.get(DIM1) - (short)center.get(DIM1);
			position2 = element.get(DIM2) - (short)center.get(DIM2);
			position3 = element.get(DIM3) - (short)center.get(DIM3);

			addGeometryElement(position1, position2, position3, element.getValue());

		}

	}
	else {
/*
		std::cout << std::endl;
		std::cout << "Center: " << std::endl;
		std::cout << " dim1 = " << center.get(DIM1) << ", dim2 = " << center.get(DIM2) << ", dim3 = " << center.get(DIM3) << std::endl;
		std::cout << "Elements: " << std::endl;
		elements.resetActiveObject();
		for (unsigned long a = 0; a < elements.getNumberOfEntries(); a++) {
			element = elements.readActiveObjectAndMakeNextOneActive();
			std::cout << " dim1 = " << element.get(DIM1) << ", dim2 = " << element.get(DIM2) << ", dim3 = " << element.get(DIM3) << std::endl;
		}
*/
		for (short j = -RELATIVERANGE; j <= RELATIVERANGE; j++) {

			for (short k = -RELATIVERANGE; k <= RELATIVERANGE; k++) {

				for (short l = -RELATIVERANGE; l <= RELATIVERANGE; l++) {

					element.set((short)center.get(DIM1) + j, DIM1);
					element.set((short)center.get(DIM2) + k, DIM2);
					element.set((short)center.get(DIM3) + l, DIM3);

//					std::cout << "  dim1 = " << element.get(DIM1) << ", dim2 = " << element.get(DIM2) << ", dim3 = " << element.get(DIM3);
//					std::cout << std::endl;

					if (elements.isFound(element, false)) {

						isElementFound = true;
						break;

					}

				}

				if (isElementFound)
					break;

			}

			if (isElementFound)
				break;

		}

		if (isElementFound) {

			addGeometryElement(0, 0, 0, 1);

			elements.resetActiveObject();

			for (i = 0; i < elements.getNumberOfEntries(); i++) {

				element   = elements.readActiveObjectAndMakeNextOneActive();
		
				position1 = element.get(DIM1) - (short)center.get(DIM1);
				position2 = element.get(DIM2) - (short)center.get(DIM2);
				position3 = element.get(DIM3) - (short)center.get(DIM3);

				addGeometryElement(position1, position2, position3, element.getValue());

			}

		}

	}

}

/****************************************************************
 * set whole other peakfinding geometry							*
 ****************************************************************/

void peakfindingGeometry::setGeometry(peakfindingGeometry& actualPeakfindingGeometry) {

	reset();
	setCoverage(actualPeakfindingGeometry.getCoverage());
	addGeometryElements(actualPeakfindingGeometry.geometry);

}
void peakfindingGeometry::setGeometry(peakfindingGeometry& actualPeakfindingGeometry, trackCoordinates& center) {

	reset();
	setCoverage(actualPeakfindingGeometry.getCoverage());
	addGeometryElements(actualPeakfindingGeometry.geometry, center);

}

/****************************************************************
 * method returns a string representation of the peakfinding	*
 * geometry														*
 ****************************************************************/

std::string peakfindingGeometry::toString() {

	return createString(false, false);

}
std::string peakfindingGeometry::toProjectedString() {

	return createString(true, false);

}
std::string peakfindingGeometry::toCoveredString() {

	return createString(false, true);

}
std::string peakfindingGeometry::toCoveredProjectedString() {

	return createString(true, true);

}

/****************************************************************
 * method draws the peakfinding geometry						*
 ****************************************************************/

void peakfindingGeometry::draw() {

	createDisplay(&displays, &numberOfDisplays, "PGVDisplay", "Peakfinding Geometry Display", false, false);

	showDisplay(canvas, &numberOfCanvasPadColumns, &numberOfCanvasPadRows, displays, numberOfDisplays, "PGVWindow", "Peakfinding Geometry Visualizer");

}
void peakfindingGeometry::drawProjection() {

	createDisplay(&projectionDisplays, &numberOfProjectionDisplays, "PGPVDisplay", "Peakfinding Geometry Projection Display", true, false);

	showDisplay(projectionCanvas, &numberOfProjectionCanvasPadColumns, &numberOfProjectionCanvasPadRows, projectionDisplays, numberOfProjectionDisplays, "PGPVWindow", "Peakfinding Geometry Projection Visualizer");

}
void peakfindingGeometry::drawCoverage() {

	createDisplay(&coverageDisplays, &numberOfCoverageDisplays, "PGCVDisplay", "Peakfinding Geometry Covered Display", false, true);

	showDisplay(coverageCanvas, &numberOfCoverageCanvasPadColumns, &numberOfCoverageCanvasPadRows, coverageDisplays, numberOfCoverageDisplays, "PGCVWindow", "Peakfinding Geometry Covered Visualizer");

}
void peakfindingGeometry::drawCoverageProjection() {

	createDisplay(&coverageProjectionDisplays, &numberOfCoverageProjectionDisplays, "PGCPVDisplay", "Peakfinding Geometry Covered Projection Display", true, true);

	showDisplay(coverageProjectionCanvas, &numberOfCoverageProjectionCanvasPadColumns, &numberOfCoverageProjectionCanvasPadRows, coverageProjectionDisplays, numberOfCoverageProjectionDisplays, "PGCPVWindow", "Peakfinding Geometry Covered Projection Visualizer");

}

/****************************************************************
 * method reads a file to get the peakfinding geometry			*
 ****************************************************************/

void peakfindingGeometry::read(std::string fileName, std::streambuf* terminal) {

	peakfindingGeometryFile     readFile;
	peakfindingGeometryElement* readData;

	readFile.setFileName(fileName);

	readFile.setDataNum(0);

	readFile.readFile(terminal);

	peakfindingGeometryFileHeader& fileHeader = readFile.getHeaderReference();

	readData                                  = (peakfindingGeometryElement*)readFile.getDataPtr();

	reset();

	setCoverage(fileHeader.percentageOfCoverage);
	for (unsigned long i = 0; i < readFile.getDataNum(); i++)
		addGeometryElement(readData[i]);

}

/****************************************************************
 * method writes a file representing the peakfinding geometry	*
 ****************************************************************/

void peakfindingGeometry::write(std::string fileName, std::string name, std::streambuf* terminal) {

	peakfindingGeometryElement*   writeData;
	peakfindingGeometryFileHeader fileHeader;
	peakfindingGeometryFile       writeFile;

	writeData = new peakfindingGeometryElement[geometry.getNumberOfEntries()];

	geometry.resetActiveObject();
	for (unsigned long i = 0; i < geometry.getNumberOfEntries(); i++)
		writeData[i] = geometry.readActiveObjectAndMakeNextOneActive();

	fileHeader.name                 = name;
	fileHeader.numberOfEntries      = (unsigned int)geometry.getNumberOfEntries();
	fileHeader.percentageOfCoverage = getCoverage();

	writeFile.setFileName(fileName);

	writeFile.setDataNum(fileHeader.numberOfEntries);

	writeFile.setDataPtr(writeData);

	writeFile.setHeader(fileHeader);

	writeFile.writeFile(terminal);

	if (writeData != NULL) {
		delete [] writeData;
		writeData = NULL;
	}

}
