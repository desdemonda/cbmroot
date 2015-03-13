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
//     - implements methods to show analysis
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:29:27 $
// $Revision: 1.16 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisWarningMsg.h"
#include "../include/showAnalysis.h"
#include "TStyle.h"
#include "TAxis.h"


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 


/****************************************************************
 * This method initializes the global style for each display.	*
 ****************************************************************/

void showAnalysis::initWindowStyle() {

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

		gStyleNotFoundWarningMsg* gStyleNotFound = new gStyleNotFoundWarningMsg(ANALYSISLIB);
		gStyleNotFound->warningMsg();
		if(gStyleNotFound != NULL) {
			delete gStyleNotFound;
			gStyleNotFound = NULL;
		}

	}

}

/****************************************************************
 * This method initializes the window with a name and a title.	*
 ****************************************************************/

TCanvas* showAnalysis::addWindow(std::string name, std::string title) {

	numberOfWindows++;
	if (window == NULL)
		window = (TCanvas**)calloc(numberOfWindows, sizeof(TCanvas*));
	else
		window = (TCanvas**)realloc(window, numberOfWindows * sizeof(TCanvas*));

	initWindowStyle();
	window[numberOfWindows - 1] = new TCanvas(name.c_str(), title.c_str(), 600, 400);

	return window[numberOfWindows - 1];

}

/****************************************************************
 * This method initializes the display.							*
 ****************************************************************/

TGraph* showAnalysis::addDisplay(int numberOfEntries, double* arrayX, double* arrayY) {

	TGraph* returnValue;

	if ((arrayX != NULL) && (arrayY != NULL)) {

		numberOfDisplays++;
		if (display == NULL)
			display = (TGraph**)calloc(numberOfDisplays, sizeof(TGraph*));
		else
			display = (TGraph**)realloc(display, numberOfDisplays * sizeof(TGraph*));

		display[numberOfDisplays - 1] = new TGraph(numberOfEntries, arrayX, arrayY);

		returnValue = display[numberOfDisplays - 1];

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

		returnValue = NULL;

	}

	return returnValue;
}

/****************************************************************
 * method updates the corresponding window- and display-object.	*
 ****************************************************************/

void showAnalysis::updateWindowAndDisplay(TCanvas* windowToUpdate, TGraph* displayToUpdate, unsigned short numberOfAdditions, double* arrayY) {

	double* xValues;
	double* yValues;
	double  xMin;
	double  xMax;
	double  yMin;
	double  yMax;

	if (windowToUpdate != NULL) {

		if (displayToUpdate != NULL) {

			xValues = displayToUpdate->GetX();
			yValues = displayToUpdate->GetY();

			if (arrayY != NULL) {

				for (int i = 0; i < displayToUpdate->GetN(); i++) {

					yValues[i] = ((double)numberOfAdditions / (double)(numberOfAdditions + 1)) * yValues[i] + arrayY[i] / (double)(numberOfAdditions + 1);

				}

			}
			else {

				arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
				arrayToDisplayIsNotAccessible->warningMsg();
				if(arrayToDisplayIsNotAccessible != NULL) {
					delete arrayToDisplayIsNotAccessible;
					arrayToDisplayIsNotAccessible = NULL;
				}

			}

			xMin = xValues[0];
			xMax = xValues[0];
			yMin = yValues[0];
			yMax = yValues[0];

			for (int j = 1; j < displayToUpdate->GetN(); j++) {

				if (xValues[j] < xMin)
					xMin = xValues[j];
				if (xValues[j] > xMax)
					xMax = xValues[j];
				if (yValues[j] < yMin)
					yMin = yValues[j];
				if (yValues[j] > yMax)
					yMax = yValues[j];

			}

			/*
			 * SetLimits sets the minimal and maximal values for the axis.
			 * Smaller values are in the underflow bin and higher values
			 * in the overflow bin.
			 *
			 * SetRangeUser sets the zoom of the axis. Here the zoom is
			 * set to one.
			 */
			displayToUpdate->GetXaxis()->SetLimits(0.95 * xMin, 1.05 * xMax);
			displayToUpdate->GetXaxis()->SetRangeUser(0.95 * xMin, 1.05 * xMax);
			displayToUpdate->GetYaxis()->SetLimits(0.95 * yMin, 1.05 * yMax);
			displayToUpdate->GetYaxis()->SetRangeUser(0.95 * yMin, 1.05 * yMax);

		}
		else {

			displayNotFoundWarningMsg* displayNotFound = new displayNotFoundWarningMsg();
			displayNotFound->warningMsg();
			if(displayNotFound != NULL) {
				delete displayNotFound;
				displayNotFound = NULL;
			}

		}

		windowToUpdate->Update();
		windowToUpdate->Paint();
		/* 
		 * Because of double buffering the window must be updated two times
		 * to show the actual update of the data.
		 * The method window->setDoubleBuffer(0) is not working until now.
		 */
		windowToUpdate->Update();
		windowToUpdate->Paint();

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

showAnalysis::showAnalysis() 
  : window(NULL),
    numberOfWindows(0),
    display(NULL),
    numberOfDisplays(0),
    correctLayerWindow(NULL),
    correctLayerDisplay(NULL),
    correctLayerAdditions(0),
    correctSlopeWindow(NULL),
    correctSlopeDisplay(NULL),
    correctSlopeAdditions(0),
    layerWindow(NULL),
    layerDisplay(NULL),
    layerAdditions(0),
    signatureWindow(NULL),
    signatureDisplay(NULL),
    signatureAdditions(0),
    correctPrelutCellWindow(NULL),
    correctPrelutCellDisplay(NULL),
    correctPrelutCellAdditions(0),
    prelutCellWindow(NULL),
    prelutCellDisplay(NULL),
    prelutCellAdditions(0),
    correctLutCellWindow(NULL),
    correctLutCellDisplay(NULL),
    correctLutCellAdditions(0),
    lutCellWindow(NULL),
    lutCellDisplay(NULL),
    lutCellAdditions(0),
    momentaErrorWindow(NULL),
    momentaErrorDisplay(NULL),
    momentaErrorAdditions(0),
    momentaXWindow(NULL),
    momentaXDisplay(NULL),
    momentaXAdditions(0),
    momentaYWindow(NULL),
    momentaYDisplay(NULL),
    momentaYAdditions(0),
    momentaZWindow(NULL),
    momentaZDisplay(NULL),
    momentaZAdditions(0),
    dim1Window(NULL),
    dim1Display(NULL),
    dim1Additions(0),
    dim2Window(NULL),
    dim2Display(NULL),
    dim2Additions(0),
    dim3Window(NULL),
    dim3Display(NULL),
    dim3Additions(0),
    momentaWindow(NULL),
    momentaDisplay(NULL),
    momentaAdditions(0),
    coordinateWindow(NULL),
    coordinateDisplay(NULL),
    coordinateAdditions(0),
    dim1PeakDistanceWindow(NULL),
    dim1PeakDistanceDisplay(NULL),
    dim1PeakDistanceAdditions(0),
    dim2PeakDistanceWindow(NULL),
    dim2PeakDistanceDisplay(NULL),
    dim2PeakDistanceAdditions(0),
    dim3PeakDistanceWindow(NULL),
    dim3PeakDistanceDisplay(NULL),
    dim3PeakDistanceAdditions(0),
    accumulatedPeakDistanceWindow(NULL),
    accumulatedPeakDistanceDisplay(NULL),
    accumulatedPeakDistanceAdditions(0),
    minimalTrackColumnWindow(NULL),
    minimalTrackColumnDisplay(NULL),
    minimalTrackColumnAdditions(0),
    averageTrackColumnWindow(NULL),
    averageTrackColumnDisplay(NULL),
    averageTrackColumnAdditions(0),
    maximalTrackColumnWindow(NULL),
    maximalTrackColumnDisplay(NULL),
    maximalTrackColumnAdditions(0),
    fifoForColumnWindow(NULL),
    fifoForColumnDisplay(NULL),
    fifoForColumnAdditions(0),
    minimalTrackRowWindow(NULL),
    minimalTrackRowDisplay(NULL),
    minimalTrackRowAdditions(0),
    averageTrackRowWindow(NULL),
    averageTrackRowDisplay(NULL),
    averageTrackRowAdditions(0),
    maximalTrackRowWindow(NULL),
    maximalTrackRowDisplay(NULL),
    maximalTrackRowAdditions(0),
    fifoForRowWindow(NULL),
    fifoForRowDisplay(NULL),
    fifoForRowAdditions(0),
    trackLayerWindow(NULL),
    trackLayerDisplay(NULL),
    trackLayerAdditions(0),
    trackDensityLayerWindow(NULL),
    trackDensityLayerDisplay(NULL),
    trackDensityLayerAdditions(0),
    hitReadoutDistributionWindow(NULL),
    hitReadoutDistributionDisplay(NULL),
    hitReadoutDistributionAdditions(0),
    hitReadoutMeanDistributionDistributionWindow(NULL),
    hitReadoutMeanDistributionDistributionDisplay(NULL),
    hitReadoutMeanDistributionDistributionAdditions(0),
    fpgaHistogramProcessingTimeDistributionDistributionWindow(NULL),
    fpgaHistogramProcessingTimeDistributionDistributionDisplay(NULL),
    fpgaHistogramProcessingTimeDistributionDistributionAdditions(0)
{
  /*
	window                                                       = NULL;
	numberOfWindows                                              = 0;
	display                                                      = NULL;
	numberOfDisplays                                             = 0;
	correctLayerWindow                                           = NULL;
	correctLayerDisplay                                          = NULL;
	correctLayerAdditions                                        = 0;
	correctSlopeWindow                                           = NULL;
	correctSlopeDisplay                                          = NULL;
	correctSlopeAdditions                                        = 0;
	layerWindow                                                  = NULL;
	layerDisplay                                                 = NULL;
	layerAdditions                                               = 0;
	signatureWindow                                              = NULL;
	signatureDisplay                                             = NULL;
	signatureAdditions                                           = 0;
	correctPrelutCellWindow                                      = NULL;
	correctPrelutCellDisplay                                     = NULL;
	correctPrelutCellAdditions                                   = 0;
	prelutCellWindow                                             = NULL;
	prelutCellDisplay                                            = NULL;
	prelutCellAdditions                                          = 0;
	correctLutCellWindow                                         = NULL;
	correctLutCellDisplay                                        = NULL;
	correctLutCellAdditions                                      = 0;
	lutCellWindow                                                = NULL;
	lutCellDisplay                                               = NULL;
	lutCellAdditions                                             = 0;
	momentaErrorWindow                                           = NULL;
	momentaErrorDisplay                                          = NULL;
	momentaErrorAdditions                                        = 0;
	momentaXWindow                                               = NULL;
	momentaXDisplay                                              = NULL;
	momentaXAdditions                                            = 0;
	momentaYWindow                                               = NULL;
	momentaYDisplay                                              = NULL;
	momentaYAdditions                                            = 0;
	momentaZWindow                                               = NULL;
	momentaZDisplay                                              = NULL;
	momentaZAdditions                                            = 0;
	dim1Window                                                   = NULL;
	dim1Display                                                  = NULL;
	dim1Additions                                                = 0;
	dim2Window                                                   = NULL;
	dim2Display                                                  = NULL;
	dim2Additions                                                = 0;
	dim3Window                                                   = NULL;
	dim3Display                                                  = NULL;
	dim3Additions                                                = 0;
	momentaWindow                                                = NULL;
	momentaDisplay                                               = NULL;
	momentaAdditions                                             = 0;
	coordinateWindow                                             = NULL;
	coordinateDisplay                                            = NULL;
	coordinateAdditions                                          = 0;
	dim1PeakDistanceWindow                                       = NULL;
	dim1PeakDistanceDisplay                                      = NULL;
	dim1PeakDistanceAdditions                                    = 0;
	dim2PeakDistanceWindow                                       = NULL;
	dim2PeakDistanceDisplay                                      = NULL;
	dim2PeakDistanceAdditions                                    = 0;
	dim3PeakDistanceWindow                                       = NULL;
	dim3PeakDistanceDisplay                                      = NULL;
	dim3PeakDistanceAdditions                                    = 0;
	accumulatedPeakDistanceWindow                                = NULL;
	accumulatedPeakDistanceDisplay                               = NULL;
	accumulatedPeakDistanceAdditions                             = 0;
	minimalTrackColumnWindow                                     = NULL;
	minimalTrackColumnDisplay                                    = NULL;
	minimalTrackColumnAdditions                                  = 0;
	averageTrackColumnWindow                                     = NULL;
	averageTrackColumnDisplay                                    = NULL;
	averageTrackColumnAdditions                                  = 0;
	maximalTrackColumnWindow                                     = NULL;
	maximalTrackColumnDisplay                                    = NULL;
	maximalTrackColumnAdditions                                  = 0;
	fifoForColumnWindow                                          = NULL;
	fifoForColumnDisplay                                         = NULL;
	fifoForColumnAdditions                                       = 0;
	minimalTrackRowWindow                                        = NULL;
	minimalTrackRowDisplay                                       = NULL;
	minimalTrackRowAdditions                                     = 0;
	averageTrackRowWindow                                        = NULL;
	averageTrackRowDisplay                                       = NULL;
	averageTrackRowAdditions                                     = 0;
	maximalTrackRowWindow                                        = NULL;
	maximalTrackRowDisplay                                       = NULL;
	maximalTrackRowAdditions                                     = 0;
	fifoForRowWindow                                             = NULL;
	fifoForRowDisplay                                            = NULL;
	fifoForRowAdditions                                          = 0;
	trackLayerWindow                                             = NULL;
	trackLayerDisplay                                            = NULL;
	trackLayerAdditions                                          = 0;
	trackDensityLayerWindow                                      = NULL;
	trackDensityLayerDisplay                                     = NULL;
	trackDensityLayerAdditions                                   = 0;;
	hitReadoutDistributionWindow                                 = NULL;
	hitReadoutDistributionDisplay                                = NULL;
	hitReadoutDistributionAdditions                              = 0;
	hitReadoutMeanDistributionDistributionWindow                 = NULL;
	hitReadoutMeanDistributionDistributionDisplay                = NULL;
	hitReadoutMeanDistributionDistributionAdditions              = 0;
	fpgaHistogramProcessingTimeDistributionDistributionWindow    = NULL;
	fpgaHistogramProcessingTimeDistributionDistributionDisplay   = NULL;
	fpgaHistogramProcessingTimeDistributionDistributionAdditions = 0;
*/
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

showAnalysis::~showAnalysis() {

	if (window != NULL) {

		for (unsigned short i = 0; i < numberOfWindows; i++) {

			if (window[i] != NULL) {

				delete window[i];
				window[i] = NULL;

			}

		}

		delete window;
		window = NULL;

	}

	if (display != NULL) {

		for (unsigned short i = 0; i < numberOfDisplays; i++) {

			if (display[i] != NULL) {

				delete display[i];
				display[i] = NULL;

			}

		}

		delete display;
		display = NULL;

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * correct layers for the hits									*
 ****************************************************************/

void showAnalysis::addCorrectLayerDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = ((double)-1 * 100) / (double)(numberOfEntries - 1);
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = ((double)i * 100) / (double)(numberOfEntries - 1);
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = ((double)numberOfEntries * 100) / (double)(numberOfEntries - 1);
		arrayY[numberOfEntries + 1] = 0.0;

		if (correctLayerWindow == NULL) {

			correctLayerWindow    = addWindow("CLDistribution", "Correct Layer Distribution");

			correctLayerDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			correctLayerAdditions = 0;

			correctLayerDisplay->SetName("CLDistribution");
			correctLayerDisplay->SetTitle("Correct Layer Distribution");
			correctLayerDisplay->GetXaxis()->SetTitle("percentage of hits of one track in correct layer [%]");
			correctLayerDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			correctLayerDisplay->Draw("AB");

		}
		else {

			correctLayerAdditions++;
			updateWindowAndDisplay(correctLayerWindow, correctLayerDisplay, correctLayerAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * correct slopes for the hits									*
 ****************************************************************/

void showAnalysis::addCorrectSlopeDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = ((double)-1 * 100) / (double)(numberOfEntries - 1);
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = ((double)i * 100) / (double)(numberOfEntries - 1);
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = ((double)numberOfEntries * 100) / (double)(numberOfEntries - 1);
		arrayY[numberOfEntries + 1] = 0.0;

		if (correctSlopeWindow == NULL) {

			correctSlopeWindow    = addWindow("CSDistribution", "Correct Slope Distribution");

			correctSlopeDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			correctSlopeAdditions = 0;

			correctSlopeDisplay->SetName("CSDistribution");
			correctSlopeDisplay->SetTitle("Correct Slope Distribution");
			correctSlopeDisplay->GetXaxis()->SetTitle("percentage of hits of one track with correct slope [%]");
			correctSlopeDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			correctSlopeDisplay->Draw("AB");

		}
		else {

			correctSlopeAdditions++;
			updateWindowAndDisplay(correctSlopeWindow, correctSlopeDisplay, correctSlopeAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * layers for the hits											*
 ****************************************************************/

void showAnalysis::addLayerDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = ((double)-1 * 100) / (double)(numberOfEntries - 1);
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = ((double)i * 100) / (double)(numberOfEntries - 1);
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = ((double)numberOfEntries * 100) / (double)(numberOfEntries - 1);
		arrayY[numberOfEntries + 1] = 0.0;

		if (layerWindow == NULL) {

			layerWindow    = addWindow("LDistribution", "Layer Distribution");

			layerDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			layerAdditions = 0;

			layerDisplay->SetName("LDistribution");
			layerDisplay->SetTitle("Layer Distribution");
			layerDisplay->GetXaxis()->SetTitle("percentage of hits of one track in the same layer [%]");
			layerDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			layerDisplay->Draw("AB");

		}
		else {

			layerAdditions++;
			updateWindowAndDisplay(layerWindow, layerDisplay, layerAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the yz-projection of one track	*
 ****************************************************************/

void showAnalysis::addYZProjection(double* arrayY, double* arrayZ, unsigned short numberOfEntries) {

	TCanvas* actualWindow;
	TGraph*  actualDisplay;

	if ((arrayY != NULL) && (arrayZ != NULL) && (numberOfEntries > 0)) {

		actualWindow  = addWindow("YZProjection", "YZ-projection of one track");

		actualDisplay = addDisplay((int)numberOfEntries, arrayZ, arrayY);

		actualDisplay->SetName("YZProjection");
		actualDisplay->SetTitle("YZ-projection of one track");
		actualDisplay->GetXaxis()->SetTitle("z-position [cm]");
		actualDisplay->GetYaxis()->SetTitle("y-position [cm]");

		actualDisplay->Draw("AC*");

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * tracks' hit signature										*
 ****************************************************************/

void showAnalysis::addSignatureDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (signatureWindow == NULL) {

			signatureWindow    = addWindow("TSDistribution", "Track Signature Distribution");

			signatureDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			signatureAdditions = 0;

			signatureDisplay->SetName("TSDistribution");
			signatureDisplay->SetTitle("Track Signature Distribution");
			signatureDisplay->GetXaxis()->SetTitle("signature of hits of one track");
			signatureDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			signatureDisplay->Draw("AB");

		}
		else {

			signatureAdditions++;
			updateWindowAndDisplay(signatureWindow, signatureDisplay, signatureAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * correct cells for the hits based on the prelut				*
 ****************************************************************/

void showAnalysis::addCorrectPrelutCellDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (correctPrelutCellWindow == NULL) {

			correctPrelutCellWindow    = addWindow("CPCDistribution", "Correct Prelut Cell Distribution");

			correctPrelutCellDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			correctPrelutCellAdditions = 0;

			correctPrelutCellDisplay->SetName("CPCDistribution");
			correctPrelutCellDisplay->SetTitle("Correct Prelut Cell Distribution");
			correctPrelutCellDisplay->GetXaxis()->SetTitle("signature of hits of one track in the correct cell");
			correctPrelutCellDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			correctPrelutCellDisplay->Draw("AB");

		}
		else {

			correctPrelutCellAdditions++;
			updateWindowAndDisplay(correctPrelutCellWindow, correctPrelutCellDisplay, correctPrelutCellAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the cells	*
 * for the hits based on the prelut								*
 ****************************************************************/

void showAnalysis::addPrelutCellDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double) numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (prelutCellWindow == NULL) {

			prelutCellWindow    = addWindow("PCDistribution", "Prelut Cell Distribution");

			prelutCellDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			prelutCellAdditions = 0;

			prelutCellDisplay->SetName("PCDistribution");
			prelutCellDisplay->SetTitle("Prelut cell Distribution");
			prelutCellDisplay->GetXaxis()->SetTitle("signature of hits of one track in the same cell");
			prelutCellDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			prelutCellDisplay->Draw("AB");

		}
		else {

			lutCellAdditions++;
			updateWindowAndDisplay(prelutCellWindow, prelutCellDisplay, prelutCellAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * correct cells for the hits based on the lut					*
 ****************************************************************/

void showAnalysis::addCorrectLutCellDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (correctLutCellWindow == NULL) {

			correctLutCellWindow    = addWindow("CLCDistribution", "Correct Lut Cell Distribution");

			correctLutCellDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			correctLutCellAdditions = 0;

			correctLutCellDisplay->SetName("CLCDistribution");
			correctLutCellDisplay->SetTitle("Correct Lut Cell Distribution");
			correctLutCellDisplay->GetXaxis()->SetTitle("signature of hits of one track in the correct cell");
			correctLutCellDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			correctLutCellDisplay->Draw("AB");

		}
		else {

			correctLutCellAdditions++;
			updateWindowAndDisplay(correctLutCellWindow, correctLutCellDisplay, correctLutCellAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the cells	*
 * for the hits	based on the lut								*
 ****************************************************************/

void showAnalysis::addLutCellDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double) numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (lutCellWindow == NULL) {

			lutCellWindow    = addWindow("LCDistribution", "Lut Cell Distribution");

			lutCellDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			lutCellAdditions = 0;

			lutCellDisplay->SetName("LCDistribution");
			lutCellDisplay->SetTitle("Lut Cell Distribution");
			lutCellDisplay->GetXaxis()->SetTitle("signature of hits of one track in the same cell");
			lutCellDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			lutCellDisplay->Draw("AB");

		}
		else {

			lutCellAdditions++;
			updateWindowAndDisplay(lutCellWindow, lutCellDisplay, lutCellAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the magnetfield factors dependant of z
 ****************************************************************/

void showAnalysis::addMagnetfieldFactorsZ(double* arrayZ, double* arrayM, unsigned short numberOfEntries) {

	TCanvas* actualWindow;
	TGraph*  actualDisplay;

	if ((arrayZ != NULL) && (arrayM != NULL) && (numberOfEntries > 0)) {

		actualWindow  = addWindow("MagnetfieldFactorsZ", "Magnetfield factors dependant of z");

		actualDisplay = addDisplay((int)numberOfEntries, arrayZ, arrayM);

		actualDisplay->SetName("MagnetfieldFactorsZ");
		actualDisplay->SetTitle("Magnetfield factors dependant of z");
		actualDisplay->GetXaxis()->SetTitle("z-position [cm]");
		actualDisplay->GetYaxis()->SetTitle("B [T]");

		actualDisplay->Draw("AC*");

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the magnetfield values			*
 * dependant of z												*
 ****************************************************************/

void showAnalysis::addMagnetfieldValuesZ(double* arrayZ, magneticFieldValue* arrayM, unsigned short numberOfEntries) {

	double*        array;
	unsigned short i;
	TCanvas*       actualWindow;
	TGraph*        actualDisplay;

	if ((arrayZ != NULL) && (arrayM != NULL) && (numberOfEntries > 0)) {

		array = new double[numberOfEntries];
		if (array == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		for (i = 0; i < numberOfEntries; i++)
			array[i] = arrayM[i].getFieldX();

		actualWindow  = addWindow("MagnetfieldXValuesZ", "Magnetfield values for B_x dependant of z");

		actualDisplay = addDisplay((int)numberOfEntries, arrayZ, array);

		actualDisplay->SetName("MagnetfieldXValuesZ");
		actualDisplay->SetTitle("Magnetfield values for B_x dependant of z");
		actualDisplay->GetXaxis()->SetTitle("z-position [cm]");
		actualDisplay->GetYaxis()->SetTitle("B_x [T]");

		actualDisplay->Draw("AC*");

		for (i = 0; i < numberOfEntries; i++)
			array[i] = arrayM[i].getFieldY();

		actualWindow  = addWindow("MagnetfieldYValuesZ", "Magnetfield values for B_y dependant of z");

		actualDisplay = addDisplay((int)numberOfEntries, arrayZ, array);

		actualDisplay->SetName("MagnetfieldYValuesZ");
		actualDisplay->SetTitle("Magnetfield values for B_y dependant of z");
		actualDisplay->GetXaxis()->SetTitle("z-position [cm]");
		actualDisplay->GetYaxis()->SetTitle("B_y [T]");

		actualDisplay->Draw("AC*");

		for (i = 0; i < numberOfEntries; i++)
			array[i] = arrayM[i].getFieldZ();

		actualWindow  = addWindow("MagnetfieldZValuesZ", "Magnetfield values for B_z dependant of z");

		actualDisplay = addDisplay((int)numberOfEntries, arrayZ, array);

		actualDisplay->SetName("MagnetfieldZValuesZ");
		actualDisplay->SetTitle("Magnetfield values for B_z dependant of z");
		actualDisplay->GetXaxis()->SetTitle("z-position [cm]");
		actualDisplay->GetYaxis()->SetTitle("B_z [T]");

		actualDisplay->Draw("AC*");

		if (array != NULL) {
			delete [] array;
			array = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the integrated magnetfield		*
 * values dependant of z										*
 ****************************************************************/

void showAnalysis::addIntegratedMagnetfieldValuesZ(double* arrayZ, magneticFieldValue* arrayM, unsigned short numberOfEntries) {

	double*        array;
	unsigned short i;
	TCanvas*       actualWindow;
	TGraph*        actualDisplay;

	if ((arrayZ != NULL) && (arrayM != NULL) && (numberOfEntries > 0)) {

		array = new double[numberOfEntries];
		if (array == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		for (i = 0; i < numberOfEntries; i++)
			array[i] = arrayM[i].getFieldX();

		actualWindow  = addWindow("IntegratedMagnetfieldXValuesZ", "Integrated magnetfield values for B_x dependant of z");

		actualDisplay = addDisplay((int)numberOfEntries, arrayZ, array);

		actualDisplay->SetName("IntegratedMagnetfieldXValuesZ");
		actualDisplay->SetTitle("Integrated magnetfield values for B_x dependant of z");
		actualDisplay->GetXaxis()->SetTitle("z-position [cm]");
		actualDisplay->GetYaxis()->SetTitle("B_x [T]");

		actualDisplay->Draw("AC*");

		for (i = 0; i < numberOfEntries; i++)
			array[i] = arrayM[i].getFieldY();

		actualWindow  = addWindow("IntegratedMagnetfieldYValuesZ", "Integrated magnetfield values for B_y dependant of z");

		actualDisplay = addDisplay((int)numberOfEntries, arrayZ, array);

		actualDisplay->SetName("IntegratedMagnetfieldYValuesZ");
		actualDisplay->SetTitle("Integrated magnetfield values for B_y dependant of z");
		actualDisplay->GetXaxis()->SetTitle("z-position [cm]");
		actualDisplay->GetYaxis()->SetTitle("B_y [T]");

		actualDisplay->Draw("AC*");

		for (i = 0; i < numberOfEntries; i++)
			array[i] = arrayM[i].getFieldZ();

		actualWindow  = addWindow("IntegratedMagnetfieldZValuesZ", "Integrated magnetfield values for B_z dependant of z");

		actualDisplay = addDisplay((int)numberOfEntries, arrayZ, array);

		actualDisplay->SetName("IntegratedMagnetfieldZValuesZ");
		actualDisplay->SetTitle("Integrated magnetfield values for B_z dependant of z");
		actualDisplay->GetXaxis()->SetTitle("z-position [cm]");
		actualDisplay->GetYaxis()->SetTitle("B_z [T]");

		actualDisplay->Draw("AC*");

		if (array != NULL) {
			delete [] array;
			array = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * percentage of errors for the momentaError					*
 ****************************************************************/

void showAnalysis::addMomentaErrorDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 1)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = ((double)i * 100) / (double)(numberOfEntries - 1);
			arrayY[i + 1] = (double)(array[i] * 100)  / (double) numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = ((double)numberOfEntries * 100) / (double)(numberOfEntries - 1);
		arrayY[numberOfEntries + 1] = 0.0;

		if (momentaErrorWindow == NULL) {

			momentaErrorWindow    = addWindow("MEDistribution", "Momenta Error Distribution");

			momentaErrorDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			momentaErrorAdditions = 0;

			momentaErrorDisplay->SetName("MEDistribution");
			momentaErrorDisplay->SetTitle("Momenta Error Distribution");
			momentaErrorDisplay->GetXaxis()->SetTitle("percentage of error [%]");
			momentaErrorDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			momentaErrorDisplay->Draw("AB");

		}
		else {

			momentaErrorAdditions++;
			updateWindowAndDisplay(momentaErrorWindow, momentaErrorDisplay, momentaErrorAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the momenta*
 ****************************************************************/

void showAnalysis::addMomentaXDistribution(unsigned int* array, unsigned short numberOfEntries, double min, double max) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 1)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = ((double)i * (max - min)) / (double)(numberOfEntries - 1) + min;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double) numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = ((double)numberOfEntries * (max - min)) / (double)(numberOfEntries - 1) + min;;
		arrayY[numberOfEntries + 1] = 0.0;

		if (momentaXWindow == NULL) {

			momentaXWindow    = addWindow("PXDistribution", "PX Distribution");

			momentaXDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			momentaXAdditions = 0;

			momentaXDisplay->SetName("PXDistribution");
			momentaXDisplay->SetTitle("PX Distribution");
			momentaXDisplay->GetXaxis()->SetTitle("quantized values [GeV/c]");
			momentaXDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			momentaXDisplay->Draw("AB");

		}
		else {

			momentaXAdditions++;
			updateWindowAndDisplay(momentaXWindow, momentaXDisplay, momentaXAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addMomentaYDistribution(unsigned int* array, unsigned short numberOfEntries, double min, double max) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 1)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = ((double)i * (max - min)) / (double)(numberOfEntries - 1) + min;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double) numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = ((double)numberOfEntries * (max - min)) / (double)(numberOfEntries - 1) + min;;
		arrayY[numberOfEntries + 1] = 0.0;

		if (momentaYWindow == NULL) {

			momentaYWindow    = addWindow("PYDistribution", "PY Distribution");

			momentaYDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			momentaYAdditions = 0;

			momentaYDisplay->SetName("PYDistribution");
			momentaYDisplay->SetTitle("PY Distribution");
			momentaYDisplay->GetXaxis()->SetTitle("quantized values [GeV/c]");
			momentaYDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			momentaYDisplay->Draw("AB");

		}
		else {

			momentaYAdditions++;
			updateWindowAndDisplay(momentaYWindow, momentaYDisplay, momentaYAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addMomentaZDistribution(unsigned int* array, unsigned short numberOfEntries, double min, double max) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 1)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = ((double)i * (max - min)) / (double)(numberOfEntries - 1) + min;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double) numberOfTracks;
		
		}
		arrayX[numberOfEntries + 1] = ((double)numberOfEntries * (max - min)) / (double)(numberOfEntries - 1) + min;;
		arrayY[numberOfEntries + 1] = 0.0;

		if (momentaZWindow == NULL) {

			momentaZWindow    = addWindow("PZDistribution", "PZ Distribution");

			momentaZDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			momentaZAdditions = 0;

			momentaZDisplay->SetName("PZDistribution");
			momentaZDisplay->SetTitle("PZ Distribution");
			momentaZDisplay->GetXaxis()->SetTitle("quantized values [GeV/c]");
			momentaZDisplay->GetYaxis()->SetTitle("percentage of tracks [%]");

			momentaZDisplay->Draw("AB");

		}
		else {

			momentaZAdditions++;
			updateWindowAndDisplay(momentaZWindow, momentaZDisplay, momentaZAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * coordinates													*
 ****************************************************************/

void showAnalysis::addHoughspaceDim1Distribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (dim1Window == NULL) {

			dim1Window    = addWindow("HD1Distribution", "Hough Dimension 1 Distribution");

			dim1Display   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			dim1Additions = 0;

			dim1Display->SetName("HD1Distribution");
			dim1Display->SetTitle("Hough Dimension 1 Distribution");
			dim1Display->GetXaxis()->SetTitle("cell [index]");
			dim1Display->GetYaxis()->SetTitle("percentage of tracks [%]");

			dim1Display->Draw("AB");

		}
		else {

			dim1Additions++;
			updateWindowAndDisplay(dim1Window, dim1Display, dim1Additions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addHoughspaceDim2Distribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (dim2Window == NULL) {

			dim2Window    = addWindow("HD2Distribution", "Hough Dimension 2 Distribution");

			dim2Display   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			dim2Additions = 0;

			dim2Display->SetName("HD2Distribution");
			dim2Display->SetTitle("Hough Dimension 2 Distribution");
			dim2Display->GetXaxis()->SetTitle("cell [index]");
			dim2Display->GetYaxis()->SetTitle("percentage of tracks [%]");

			dim2Display->Draw("AB");

		}
		else {

			dim2Additions++;
			updateWindowAndDisplay(dim2Window, dim2Display, dim2Additions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addHoughspaceDim3Distribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (dim3Window == NULL) {

			dim3Window    = addWindow("HD3Distribution", "Hough Dimension 3 Distribution");

			dim3Display   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			dim3Additions = 0;

			dim3Display->SetName("HD3Distribution");
			dim3Display->SetTitle("Hough Dimension 3 Distribution");
			dim3Display->GetXaxis()->SetTitle("cell [index]");
			dim3Display->GetYaxis()->SetTitle("percentage of tracks [%]");

			dim3Display->Draw("AB");

		}
		else {

			dim3Additions++;
			updateWindowAndDisplay(dim3Window, dim3Display, dim3Additions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addMomentaDistribution(unsigned int* array, unsigned int numberOfEntries) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)array[i];

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (momentaWindow == NULL) {

			momentaWindow        = addWindow("MDistribution", "Momenta Distribution");

			momentaDisplay       = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			momentaAdditions     = 0;

			momentaDisplay->SetName("MDistribution");
			momentaDisplay->SetTitle("Momenta Distribution");
			momentaDisplay->GetXaxis()->SetTitle("quantization [index]");
			momentaDisplay->GetYaxis()->SetTitle("number of tracks");

			momentaDisplay->Draw("AL");

		}
		else {

			momentaAdditions++;
			updateWindowAndDisplay(momentaWindow, momentaDisplay, momentaAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addCoordinateDistribution(unsigned int* array, unsigned int numberOfEntries) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)array[i];

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (coordinateWindow == NULL) {

			coordinateWindow     = addWindow("CDistribution", "Coordinate Distribution");

			coordinateDisplay    = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			coordinateAdditions  = 0;

			coordinateDisplay->SetName("CDistribution");
			coordinateDisplay->SetTitle("Coordinate Distribution");
			coordinateDisplay->GetXaxis()->SetTitle("cell [index]");
			coordinateDisplay->GetYaxis()->SetTitle("number of tracks");

			coordinateDisplay->Draw("AL");

		}
		else {

			coordinateAdditions++;
			updateWindowAndDisplay(coordinateWindow, coordinateDisplay, coordinateAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the peak	*
 * distances													*
 ****************************************************************/

void showAnalysis::addDim1PeakDistanceDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double       mean;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		mean   = 0;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {

			mean += (double)(i * array[i]) / (double)numberOfTracks;
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		meanValueWarningMsg* meanValue = new meanValueWarningMsg(mean, "Mean peak distance in dim1");
		meanValue->warningMsg();
		if(meanValue != NULL) {
			delete meanValue;
			meanValue = NULL;
		}

		if (dim1PeakDistanceWindow == NULL) {

			dim1PeakDistanceWindow     = addWindow("D1PDDistribution", "Dim1 Peak Distance Distribution");

			dim1PeakDistanceDisplay    = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			dim1PeakDistanceAdditions  = 0;

			dim1PeakDistanceDisplay->SetName("D1PDDistribution");
			dim1PeakDistanceDisplay->SetTitle("Dim1 Peak Distance Distribution");
			dim1PeakDistanceDisplay->GetXaxis()->SetTitle("distance [cells]");
			dim1PeakDistanceDisplay->GetYaxis()->SetTitle("number of tracks");

			dim1PeakDistanceDisplay->Draw("AL");

		}
		else {

			dim1PeakDistanceAdditions++;
			updateWindowAndDisplay(dim1PeakDistanceWindow, dim1PeakDistanceDisplay, dim1PeakDistanceAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addDim2PeakDistanceDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double       mean;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		mean   = 0;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {

			mean += (double)(i * array[i]) / (double)numberOfTracks;
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		meanValueWarningMsg* meanValue = new meanValueWarningMsg(mean, "Mean peak distance in dim2");
		meanValue->warningMsg();
		if(meanValue != NULL) {
			delete meanValue;
			meanValue = NULL;
		}

		if (dim2PeakDistanceWindow == NULL) {

			dim2PeakDistanceWindow     = addWindow("D2PDDistribution", "Dim2 Peak Distance Distribution");

			dim2PeakDistanceDisplay    = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			dim2PeakDistanceAdditions  = 0;

			dim2PeakDistanceDisplay->SetName("D2PDDistribution");
			dim2PeakDistanceDisplay->SetTitle("Dim2 Peak Distance Distribution");
			dim2PeakDistanceDisplay->GetXaxis()->SetTitle("distance [cells]");
			dim2PeakDistanceDisplay->GetYaxis()->SetTitle("number of tracks");

			dim2PeakDistanceDisplay->Draw("AL");

		}
		else {

			dim2PeakDistanceAdditions++;
			updateWindowAndDisplay(dim2PeakDistanceWindow, dim2PeakDistanceDisplay, dim2PeakDistanceAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addDim3PeakDistanceDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double       mean;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		mean   = 0;

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {

			mean += (double)(i * array[i]) / (double)numberOfTracks;
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		meanValueWarningMsg* meanValue = new meanValueWarningMsg(mean, "Mean peak distance in dim3");
		meanValue->warningMsg();
		if(meanValue != NULL) {
			delete meanValue;
			meanValue = NULL;
		}

		if (dim3PeakDistanceWindow == NULL) {

			dim3PeakDistanceWindow     = addWindow("D3PDDistribution", "Dim3 Peak Distance Distribution");

			dim3PeakDistanceDisplay    = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			dim3PeakDistanceAdditions  = 0;

			dim3PeakDistanceDisplay->SetName("D3PDDistribution");
			dim3PeakDistanceDisplay->SetTitle("Dim3 Peak Distance Distribution");
			dim3PeakDistanceDisplay->GetXaxis()->SetTitle("distance [cells]");
			dim3PeakDistanceDisplay->GetYaxis()->SetTitle("number of tracks");

			dim3PeakDistanceDisplay->Draw("AL");

		}
		else {

			dim3PeakDistanceAdditions++;
			updateWindowAndDisplay(dim3PeakDistanceWindow, dim3PeakDistanceDisplay, dim3PeakDistanceAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}
void showAnalysis::addAccumulatedPeakDistanceDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int numberOfTracks;
	double       mean;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += array[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		mean   = 0;

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {

			mean += (double)(i * array[i]) / (double)numberOfTracks;
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)(array[i] * 100)  / (double)numberOfTracks;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		meanValueWarningMsg* meanValue = new meanValueWarningMsg(mean, "Accumulated mean peak distance");
		meanValue->warningMsg();
		if(meanValue != NULL) {
			delete meanValue;
			meanValue = NULL;
		}

		if (accumulatedPeakDistanceWindow == NULL) {

			accumulatedPeakDistanceWindow     = addWindow("APDDistribution", "Accumulated Peak Distance Distribution");

			accumulatedPeakDistanceDisplay    = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			accumulatedPeakDistanceAdditions  = 0;

			accumulatedPeakDistanceDisplay->SetName("APDDistribution");
			accumulatedPeakDistanceDisplay->SetTitle("Accumulated Peak Distance Distribution");
			accumulatedPeakDistanceDisplay->GetXaxis()->SetTitle("distance [cells]");
			accumulatedPeakDistanceDisplay->GetYaxis()->SetTitle("number of tracks");

			accumulatedPeakDistanceDisplay->Draw("AL");

		}
		else {

			accumulatedPeakDistanceAdditions++;
			updateWindowAndDisplay(accumulatedPeakDistanceWindow, accumulatedPeakDistanceDisplay, accumulatedPeakDistanceAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * minimal found tracks' column									*
 ****************************************************************/

void showAnalysis::addMinimalTrackColumnDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)array[i];

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (minimalTrackColumnWindow == NULL) {

			minimalTrackColumnWindow    = addWindow("MiniTCDistribution", "Minimal Track Column Distribution");

			minimalTrackColumnDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			minimalTrackColumnAdditions = 0;

			minimalTrackColumnDisplay->SetName("MiniTCDistribution");
			minimalTrackColumnDisplay->SetTitle("Minimal Track Column Distribution");
			minimalTrackColumnDisplay->GetXaxis()->SetTitle("column");
			minimalTrackColumnDisplay->GetYaxis()->SetTitle("number of tracks");

			minimalTrackColumnDisplay->Draw("AB");

		}
		else {

			minimalTrackColumnAdditions++;
			updateWindowAndDisplay(minimalTrackColumnWindow, minimalTrackColumnDisplay, minimalTrackColumnAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * average found tracks' column									*
 ****************************************************************/

void showAnalysis::addAverageTrackColumnDistribution(double* array, unsigned short numberOfEntries, unsigned int numberOfAnalysis) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = array[i] / (double)numberOfAnalysis;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (averageTrackColumnWindow == NULL) {

			averageTrackColumnWindow     = addWindow("ATCDistribution", "Average Track Column Distribution");

			averageTrackColumnDisplay    = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			averageTrackColumnAdditions  = 0;

			averageTrackColumnDisplay->SetName("ATCDistribution");
			averageTrackColumnDisplay->SetTitle("Average Track Column Distribution");
			averageTrackColumnDisplay->GetXaxis()->SetTitle("column");
			averageTrackColumnDisplay->GetYaxis()->SetTitle("number of tracks");

			averageTrackColumnDisplay->Draw("AB");

		}
		else {

			averageTrackColumnAdditions++;
			updateWindowAndDisplay(averageTrackColumnWindow, averageTrackColumnDisplay, averageTrackColumnAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * maximal found tracks' column									*
 ****************************************************************/

void showAnalysis::addMaximalTrackColumnDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)array[i];

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (maximalTrackColumnWindow == NULL) {

			maximalTrackColumnWindow    = addWindow("MaxiTCDistribution", "Maximal Track Column Distribution");

			maximalTrackColumnDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			maximalTrackColumnAdditions = 0;

			maximalTrackColumnDisplay->SetName("MaxiTCDistribution");
			maximalTrackColumnDisplay->SetTitle("Maximal Track Column Distribution");
			maximalTrackColumnDisplay->GetXaxis()->SetTitle("column");
			maximalTrackColumnDisplay->GetYaxis()->SetTitle("number of tracks");

			maximalTrackColumnDisplay->Draw("AB");

		}
		else {

			maximalTrackColumnAdditions++;
			updateWindowAndDisplay(maximalTrackColumnWindow, maximalTrackColumnDisplay, maximalTrackColumnAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * fifos for the found tracks' column							*
 ****************************************************************/

void showAnalysis::addFifoForColumnDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int value;
	unsigned int counter;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		value     = 0;
		counter   = 0;
		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {

			value    = min(counter, array[i]);

			counter += array[i];
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)value;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (fifoForColumnWindow == NULL) {

			fifoForColumnWindow    = addWindow("FFCDistribution", "FIFOs For Column Distribution");

			fifoForColumnDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			fifoForColumnAdditions = 0;

			fifoForColumnDisplay->SetName("FFCDistribution");
			fifoForColumnDisplay->SetTitle("FIFOs For Column Distribution");
			fifoForColumnDisplay->GetXaxis()->SetTitle("column");
			fifoForColumnDisplay->GetYaxis()->SetTitle("fifo size");

			fifoForColumnDisplay->Draw("AB");

		}
		else {

			fifoForColumnAdditions++;
			updateWindowAndDisplay(fifoForColumnWindow, fifoForColumnDisplay, fifoForColumnAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * minimal found tracks' row									*
 ****************************************************************/

void showAnalysis::addMinimalTrackRowDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)array[i];

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (minimalTrackRowWindow == NULL) {

			minimalTrackRowWindow    = addWindow("MiniTRDistribution", "Minimal Track Row Distribution");

			minimalTrackRowDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			minimalTrackRowAdditions = 0;

			minimalTrackRowDisplay->SetName("MiniTRDistribution");
			minimalTrackRowDisplay->SetTitle("Minimal Track Row Distribution");
			minimalTrackRowDisplay->GetXaxis()->SetTitle("row");
			minimalTrackRowDisplay->GetYaxis()->SetTitle("number of tracks");

			minimalTrackRowDisplay->Draw("AB");

		}
		else {

			minimalTrackRowAdditions++;
			updateWindowAndDisplay(minimalTrackRowWindow, minimalTrackRowDisplay, minimalTrackRowAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * average found tracks' row									*
 ****************************************************************/

void showAnalysis::addAverageTrackRowDistribution(double* array, unsigned short numberOfEntries, unsigned int numberOfAnalysis) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = array[i] / (double)numberOfAnalysis;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (averageTrackRowWindow == NULL) {

			averageTrackRowWindow     = addWindow("ATRDistribution", "Average Track Row Distribution");

			averageTrackRowDisplay    = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			averageTrackRowAdditions  = 0;

			averageTrackRowDisplay->SetName("ATRDistribution");
			averageTrackRowDisplay->SetTitle("Average Track Row Distribution");
			averageTrackRowDisplay->GetXaxis()->SetTitle("row");
			averageTrackRowDisplay->GetYaxis()->SetTitle("number of tracks");

			averageTrackRowDisplay->Draw("AB");

		}
		else {

			averageTrackRowAdditions++;
			updateWindowAndDisplay(averageTrackRowWindow, averageTrackRowDisplay, averageTrackRowAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * maximal found tracks' row									*
 ****************************************************************/

void showAnalysis::addMaximalTrackRowDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)array[i];

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (maximalTrackRowWindow == NULL) {

			maximalTrackRowWindow    = addWindow("MaxiTRDistribution", "Maximal Track Row Distribution");

			maximalTrackRowDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			maximalTrackRowAdditions = 0;

			maximalTrackRowDisplay->SetName("MaxiTRDistribution");
			maximalTrackRowDisplay->SetTitle("Maximal Track Row Distribution");
			maximalTrackRowDisplay->GetXaxis()->SetTitle("row");
			maximalTrackRowDisplay->GetYaxis()->SetTitle("number of tracks");

			maximalTrackRowDisplay->Draw("AB");

		}
		else {

			maximalTrackRowAdditions++;
			updateWindowAndDisplay(maximalTrackRowWindow, maximalTrackRowDisplay, maximalTrackRowAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * fifos for the found tracks' row								*
 ****************************************************************/

void showAnalysis::addFifoForRowDistribution(unsigned int* array, unsigned short numberOfEntries) {

	unsigned int i;
	unsigned int value;
	unsigned int counter;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		value     = 0;
		counter   = 0;
		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {

			value    = min(counter, array[i]);

			counter += array[i];
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)value;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (fifoForRowWindow == NULL) {

			fifoForRowWindow    = addWindow("FFRDistribution", "FIFOs For Row Distribution");

			fifoForRowDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			fifoForRowAdditions = 0;

			fifoForRowDisplay->SetName("FFRDistribution");
			fifoForRowDisplay->SetTitle("FIFOs For Row Distribution");
			fifoForRowDisplay->GetXaxis()->SetTitle("row");
			fifoForRowDisplay->GetYaxis()->SetTitle("fifo size");

			fifoForRowDisplay->Draw("AB");

		}
		else {

			fifoForRowAdditions++;
			updateWindowAndDisplay(fifoForRowWindow, fifoForRowDisplay, fifoForRowAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * maximal found tracks' layer									*
 ****************************************************************/

void showAnalysis::addTrackLayerDistribution(unsigned int* array, unsigned short numberOfEntries, unsigned int numberOfAnalysis) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)array[i] / (double)numberOfAnalysis;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (trackLayerWindow == NULL) {

			trackLayerWindow    = addWindow("TLDistribution", "Track Layer Distribution");

			trackLayerDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			trackLayerAdditions = 0;

			trackLayerDisplay->SetName("TLDistribution");
			trackLayerDisplay->SetTitle("Track Layer Distribution");
			trackLayerDisplay->GetXaxis()->SetTitle("layer");
			trackLayerDisplay->GetYaxis()->SetTitle("number of tracks");

			trackLayerDisplay->Draw("AB");

		}
		else {

			trackLayerAdditions++;
			updateWindowAndDisplay(trackLayerWindow, trackLayerDisplay, trackLayerAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * maximal found tracks' density layer							*
 ****************************************************************/

void showAnalysis::addTrackDensityLayerDistribution(int* array, unsigned short numberOfEntries, unsigned int numberOfAnalysis) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] = -1.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i;
			arrayY[i + 1] = (double)array[i] / (double)numberOfAnalysis;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (trackDensityLayerWindow == NULL) {

			trackDensityLayerWindow    = addWindow("TCDLDistribution", "Track Candidate Difference Layer Distribution");

			trackDensityLayerDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			trackDensityLayerAdditions = 0;

			trackDensityLayerDisplay->SetName("TCDLDistribution");
			trackDensityLayerDisplay->SetTitle("Track Candidate Difference Layer Distribution");
			trackDensityLayerDisplay->GetXaxis()->SetTitle("layer");
			trackDensityLayerDisplay->GetYaxis()->SetTitle("number of tracks");

			trackDensityLayerDisplay->Draw("AB");

		}
		else {

			trackDensityLayerAdditions++;
			updateWindowAndDisplay(trackDensityLayerWindow, trackDensityLayerDisplay, trackDensityLayerAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * hit readouts while histogramming								*
 ****************************************************************/

void showAnalysis::addHitReadoutDistribution(unsigned int* array, unsigned int numberOfEntries, unsigned int numberOfAnalysis) {

	unsigned int i;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] =  0.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			arrayX[i + 1] = (double)i + 1;
			arrayY[i + 1] = (double)array[i] / (double)numberOfAnalysis;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (hitReadoutDistributionWindow == NULL) {

			hitReadoutDistributionWindow    = addWindow("HRDistribution", "Hit Readout Distribution");

			hitReadoutDistributionDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			hitReadoutDistributionAdditions = 0;

			hitReadoutDistributionDisplay->SetName("HRDistribution");
			hitReadoutDistributionDisplay->SetTitle("Hit Readout Distribution");
			hitReadoutDistributionDisplay->GetXaxis()->SetTitle("number of readouts");
			hitReadoutDistributionDisplay->GetYaxis()->SetTitle("number of hits");

			hitReadoutDistributionDisplay->Draw("AB");

		}
		else {

			hitReadoutDistributionAdditions++;
			updateWindowAndDisplay(hitReadoutDistributionWindow, hitReadoutDistributionDisplay, hitReadoutDistributionAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the mean of the				*
 * hit readouts with different parallely implemented			*
 * histogram layers while histogramming							*
 ****************************************************************/

void showAnalysis::addHitReadoutMeanDistribution(unsigned int* array, unsigned int numberOfEntries, unsigned int numberOfAnalysis) {

	unsigned int i;
	unsigned int j;
	double       sum;
	double       meanValue;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		sum      = 0;
		for (j = 0; j < numberOfEntries; j++)
			sum += (double)array[j] / (double)numberOfAnalysis;

		arrayX[0] =  0.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			meanValue          = 0;
			for (j = 0; j < numberOfEntries; j++) {

				if (i + 1 < numberOfEntries)
					meanValue += (1 + ((double)j / (double)(i + 1))) * ((double)array[j] / (double)numberOfAnalysis);
				else
					meanValue += (double)array[j] / (double)numberOfAnalysis;

			}

			arrayX[i + 1] = (double)(i + 1);
			arrayY[i + 1] = meanValue / sum;

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (hitReadoutMeanDistributionDistributionWindow == NULL) {

			hitReadoutMeanDistributionDistributionWindow    = addWindow("HRMDistribution", "Hit Readout Mean Distribution");

			hitReadoutMeanDistributionDistributionDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			hitReadoutMeanDistributionDistributionAdditions = 0;

			hitReadoutMeanDistributionDistributionDisplay->SetName("HRMDistribution");
			hitReadoutMeanDistributionDistributionDisplay->SetTitle("Hit Readout Mean Distribution");
			hitReadoutMeanDistributionDistributionDisplay->GetXaxis()->SetTitle("mean number of readouts");
			hitReadoutMeanDistributionDistributionDisplay->GetYaxis()->SetTitle("number of parallely implemented layers");

			hitReadoutMeanDistributionDistributionDisplay->Draw("AB");

		}
		else {

			hitReadoutMeanDistributionDistributionAdditions++;
			updateWindowAndDisplay(hitReadoutMeanDistributionDistributionWindow, hitReadoutMeanDistributionDistributionDisplay, hitReadoutMeanDistributionDistributionAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the time						*
 * consumption of the histogram processing with					*
 * different parallely implementedhistogram layers				*
 ****************************************************************/

void showAnalysis::addFpgaHistogramProcessingTimeDistribution(unsigned int* array, unsigned int numberOfEntries, unsigned int numberOfAnalysis, bool readoutColumnsInParallel, unsigned short histogramDim1, unsigned short histogramDim2) {

	unsigned int i;
	unsigned int j;
	unsigned int timeHistogramReadout;
	double       timeHistogramming;
	double*      arrayX;
	double*      arrayY;

	if ((array != NULL) && (numberOfEntries > 0)) {

		arrayX = new double[numberOfEntries + 2];
		if (arrayX == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayY = new double[numberOfEntries + 2];
		if (arrayY == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		arrayX[0] =  0.0;
		arrayY[0] =  0.0;
		for (i = 0; i < numberOfEntries; i++) {
		
			if (readoutColumnsInParallel)
				timeHistogramReadout = histogramDim2 * ((numberOfEntries + i) / (i + 1));
			else
				timeHistogramReadout = histogramDim1 * ((numberOfEntries + i) / (i + 1));

			timeHistogramming    = 0;
			for (j = 0; j < numberOfEntries; j++) {

				if (i + 1 < numberOfEntries)
					timeHistogramming += (1 + ((double)j / (double)(i + 1))) * ((double)array[j] / (double)numberOfAnalysis);
				else
					timeHistogramming += (double)array[j] / (double)numberOfAnalysis;

			}

			arrayX[i + 1] = (double)(i + 1);
			arrayY[i + 1] = (double)((unsigned int)ceil(timeHistogramming) + timeHistogramReadout);

		}
		arrayX[numberOfEntries + 1] = numberOfEntries;
		arrayY[numberOfEntries + 1] = 0.0;

		if (fpgaHistogramProcessingTimeDistributionDistributionWindow == NULL) {

			fpgaHistogramProcessingTimeDistributionDistributionWindow    = addWindow("FPGAHPTDistribution", "FPGA Histogram Processing Time Distribution");

			fpgaHistogramProcessingTimeDistributionDistributionDisplay   = addDisplay((int)(numberOfEntries + 2), arrayX, arrayY);

			fpgaHistogramProcessingTimeDistributionDistributionAdditions = 0;

			fpgaHistogramProcessingTimeDistributionDistributionDisplay->SetName("FPGAHPTDistribution");
			fpgaHistogramProcessingTimeDistributionDistributionDisplay->SetTitle("FPGA Histogram Processing Time Distribution");
			fpgaHistogramProcessingTimeDistributionDistributionDisplay->GetXaxis()->SetTitle("parallely implemented layers");
			fpgaHistogramProcessingTimeDistributionDistributionDisplay->GetYaxis()->SetTitle("time [clock cycles]");

			fpgaHistogramProcessingTimeDistributionDistributionDisplay->Draw("AB");

		}
		else {

			fpgaHistogramProcessingTimeDistributionDistributionAdditions++;
			updateWindowAndDisplay(fpgaHistogramProcessingTimeDistributionDistributionWindow, fpgaHistogramProcessingTimeDistributionDistributionDisplay, fpgaHistogramProcessingTimeDistributionDistributionAdditions, arrayY);

		}

		if (arrayX != NULL) {
			delete arrayX;
			arrayX = NULL;
		}
		if (arrayY != NULL) {
			delete arrayY;
			arrayY = NULL;
		}

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * two given arrays												*
 ****************************************************************/

void showAnalysis::addDistribution(double* arrayX, double* arrayY, unsigned short numberOfEntries, std::string name, std::string title, std::string xAxiTitle, std::string yAxiTitle) {

	TGraph* actualGraph;

	if ((arrayX != NULL) && (arrayY != NULL) && (numberOfEntries > 0)) {

		addWindow(name.c_str(), title.c_str());

		actualGraph = addDisplay((int)numberOfEntries, arrayX, arrayY);

		actualGraph->SetName(name.c_str());
		actualGraph->SetTitle(title.c_str());
		actualGraph->GetXaxis()->SetTitle(xAxiTitle.c_str());
		actualGraph->GetYaxis()->SetTitle(yAxiTitle.c_str());

		actualGraph->Draw("AB");

	}
	else {

		arrayToDisplayIsNotAccessibleWarningMsg* arrayToDisplayIsNotAccessible = new arrayToDisplayIsNotAccessibleWarningMsg();
		arrayToDisplayIsNotAccessible->warningMsg();
		if(arrayToDisplayIsNotAccessible != NULL) {
			delete arrayToDisplayIsNotAccessible;
			arrayToDisplayIsNotAccessible = NULL;
		}

	}

}

/****************************************************************
 * method adds a display to show the distribution of the		*
 * given table													*
 ****************************************************************/

void showAnalysis::addTable(table& source, std::string name, std::string title, std::string xAxiTitle, std::string yAxiTitle) {

	int      numberOfEntries;
	double*  arrayX;
	double*  arrayY;
	bitArray signature;
	TGraph*  actualGraph;

	numberOfEntries = (1 << signature.length());

	arrayX = new double[numberOfEntries];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayY = new double[numberOfEntries];
	if (arrayY == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (int i = 0; i < numberOfEntries; i++) {
		
		signature = bitArray(i);
		arrayX[i] = (double)i;
		arrayY[i] = (double)source.getClassification(signature).toULong();

	}

	addWindow(name.c_str(), title.c_str());

	actualGraph = addDisplay(numberOfEntries, arrayX, arrayY);

	actualGraph->SetName(name.c_str());
	actualGraph->SetTitle(title.c_str());
	actualGraph->GetXaxis()->SetTitle(xAxiTitle.c_str());
	actualGraph->GetYaxis()->SetTitle(yAxiTitle.c_str());

	actualGraph->Draw("AB");

	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}
	if (arrayY != NULL) {
		delete arrayY;
		arrayY = NULL;
	}

}
