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
//     - draws pictures to show different Hough transforms
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:29:26 $
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../DataObjectLIB/include/analyticFormula.h"
#include "../include/analysisWarningMsg.h"
#include "../include/houghPictures.h"
#include "TStyle.h"
#include "TAxis.h"
#include <math.h>


#define sqr(a)  ((a) * (a)) 
#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


#define PI (double)3.141592653589793238462643383279502884197169399375


#define WINDOWSIZE                      600
#define QUANTIZATION                    WINDOWSIZE
#define MARKERTYPE                      29

#define RELATIVEDISTANCE                (double)0.03
#define AXISTEXTSIZE                    (double)0.03
#define TEXTSIZE                        (double)0.03
#define FUNCTIONTEXTSIZE                (double)0.04
#define FUNCTIONTEXTXPOSITION           (double)0.1
#define FUNCTIONTEXTYPOSITION           (double)0.8

#define COORDINATESTRAIGHTLINERANGEX    (double)10
#define COORDINATESTRAIGHTLINERANGEY    COORDINATESTRAIGHTLINERANGEX

#define COORDINATEANALYTICFORMULARANGEX (double)100
#define COORDINATEANALYTICFORMULARANGEY (double)100
#define COORDINATEANALYTICFORMULARANGEZ (double)100

#define COORDINATESTRAIGHTLINEM         (double)(-0.6  * (COORDINATESTRAIGHTLINERANGEY / COORDINATESTRAIGHTLINERANGEX))
#define COORDINATESTRAIGHTLINEC         (double)(0.3   * COORDINATESTRAIGHTLINERANGEY)
#define COORDINATESTRAIGHTLINEMSTART    (int)(0.55     * QUANTIZATION)
#define COORDINATESTRAIGHTLINEMLENGTH   (int)(0.15     * QUANTIZATION)
#define COORDINATELENGTH                (double)(0.3   * (COORDINATESTRAIGHTLINERANGEY / COORDINATESTRAIGHTLINERANGEX))
#define COORDINATENORMANGLE             (double)(1.5   * (COORDINATESTRAIGHTLINERANGEY / COORDINATESTRAIGHTLINERANGEX))

#define COORDINATEANALYTICFORMULAQ      (double)1
#define COORDINATEANALYTICFORMULAPX     (double)1
#define COORDINATEANALYTICFORMULAPY     (double)2
#define COORDINATEANALYTICFORMULAPZ     (double)8
#define COORDINATEANALYTICFORMULAM      (double)0.0125
#define COORDINATEANALYTICFORMULAB      (double)100
#define HOUGHANALYTICFORMULAB           (double)10

#define HOUGHLINERANGEM                 (double)(4 * COORDINATESTRAIGHTLINEM)
#define HOUGHLINERANGEC                 (double)(4 * COORDINATESTRAIGHTLINEC)

#define HOUGHHESSERANGEPHI              (double)(PI / 2)
#define HOUGHHESSERANGEPPHI             (double)(4 * cos(-atan(COORDINATESTRAIGHTLINEM)) * COORDINATESTRAIGHTLINEC)


#define HOUGHHESSERANGETHETA            (double)(PI / 2)
#define HOUGHHESSERANGEPTHETA           (double)(4 * cos(-atan(COORDINATESTRAIGHTLINEM)) * COORDINATESTRAIGHTLINEC)

#define HOUGHANALYTICFORMULARANGETHETA  (double)0.46764		// [rad]: atan((planeMaxX + planeMaxX / 100) / planeZ)
#define HOUGHANALYTICFORMULARANGEQPXZ   (double)1.1

#define HOUGHANALYTICFORMULARANGEGAMMA  (double)0.46764		// [rad]: atan((planeMaxY + planeMaxY / 100) / planeZ)
#define HOUGHANALYTICFORMULARANGEM      (double)0.015

#define NUMBEROFCOORDINATEPOINTS        5
const int COORDINATEPOINTS[NUMBEROFCOORDINATEPOINTS] = {(int)(0.15 * QUANTIZATION), (int)(0.35 * QUANTIZATION), (int)(0.52 * QUANTIZATION), (int)(0.8 * QUANTIZATION), (int)(0.9 * QUANTIZATION)};

#define STARTHOUGHCOORDINATEPOINT       0
#define STOPHOUGHCOORDINATEPOINT        NUMBEROFCOORDINATEPOINTS


/* **************************************************************
 * CLASS houghPictures					 						*
 * **************************************************************/

/****************************************************************
 * This method initializes the global style for each display.	*
 ****************************************************************/

void houghPictures::initWindowStyle() {

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

TCanvas* houghPictures::addWindow(std::string name, std::string title, double xRangeStart, double xRangeStop, double yRangeStart, double yRangeStop, int xSize, int ySize) {

	numberOfWindows++;
	if (window == NULL)
		window = (TCanvas**)calloc(numberOfWindows, sizeof(TCanvas*));
	else
		window = (TCanvas**)realloc(window, numberOfWindows * sizeof(TCanvas*));

	initWindowStyle();
	window[numberOfWindows - 1] = new TCanvas(name.c_str(), title.c_str(), xSize, ySize);

	/* set the range of the display. Must be quadratic to see 90 degree in cartesian system */
	window[numberOfWindows - 1]->Range(xRangeStart, yRangeStart, xRangeStop, yRangeStop);

	return window[numberOfWindows - 1];

}

/****************************************************************
 * This method initializes the display.							*
 ****************************************************************/

TGraph* houghPictures::addDisplay(int numberOfEntries, double* arrayX, double* arrayY, double xRangeStart, double xRangeStop, double yRangeStart, double yRangeStop, double rangeFactor, std::string xAxiTitle, std::string yAxiTitle) {

	double  xRange;
	double  yRange;
	double  xRangePart;
	double  yRangePart;
	TGraph* returnValue;
	TArrow* actualArrow;
	TLatex* actualText;

	if ((arrayX != NULL) && (arrayY != NULL)) {

		numberOfDisplays++;
		if (display == NULL)
			display = (TGraph**)calloc(numberOfDisplays, sizeof(TGraph*));
		else
			display = (TGraph**)realloc(display, numberOfDisplays * sizeof(TGraph*));

		display[numberOfDisplays - 1] = new TGraph(numberOfEntries, arrayX, arrayY);

		xRange     = xRangeStop - xRangeStart;
		yRange     = yRangeStop - yRangeStart;
		xRangePart = RELATIVEDISTANCE * xRange;
		yRangePart = RELATIVEDISTANCE * yRange;

		/* display the axis */
		actualArrow = addArrow(xRangeStart + xRangePart, 0, xRangeStop - xRangePart, 0, "|>");
		actualArrow->Draw();
		actualArrow = addArrow(0, yRangeStart + yRangePart, 0, yRangeStop - yRangePart, "|>");
		actualArrow->Draw();

		/* display the axis description */
		actualText = addText(xRangeStop - rangeFactor * xRangePart, -yRangePart, AXISTEXTSIZE, xAxiTitle.c_str());
		actualText->Draw();
		actualText = addText(rangeFactor * (-xRangePart), yRangeStop - rangeFactor * yRangePart, AXISTEXTSIZE, yAxiTitle.c_str());
		actualText->Draw();

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
 * This method initializes a marker.							*
 ****************************************************************/

TMarker* houghPictures::addMarker(double xPos, double yPos, int type) {

	numberOfMarkers++;
	if (marker == NULL)
		marker = (TMarker**)calloc(numberOfMarkers, sizeof(TMarker*));
	else
		marker = (TMarker**)realloc(marker, numberOfMarkers * sizeof(TMarker*));

	marker[numberOfMarkers - 1] = new TMarker(xPos, yPos, type);

	return marker[numberOfMarkers - 1];

}

/****************************************************************
 * This method initializes an arrow.							*
 ****************************************************************/

TArrow* houghPictures::addArrow(double xPosStart, double yPosStart, double xPosStop, double yPosStop, std::string option) {

	numberOfArrows++;
	if (arrow == NULL)
		arrow = (TArrow**)calloc(numberOfArrows, sizeof(TArrow*));
	else
		arrow = (TArrow**)realloc(arrow, numberOfArrows * sizeof(TArrow*));

	arrow[numberOfArrows - 1] = new TArrow(xPosStart, yPosStart, xPosStop, yPosStop, 0.025, option.c_str());
	arrow[numberOfArrows - 1]->SetAngle(30);

	return arrow[numberOfArrows - 1];

}

/****************************************************************
 * This method initializes an ellipse.							*
 ****************************************************************/

TEllipse* houghPictures::addEllipse(double xCenter, double yCenter, double xRadius, double yRadius, double angleMin, double angleMax, double rotationAngle) {

	numberOfEllipses++;
	if (ellipse == NULL)
		ellipse = (TEllipse**)calloc(numberOfEllipses, sizeof(TEllipse*));
	else
		ellipse = (TEllipse**)realloc(ellipse, numberOfEllipses * sizeof(TEllipse*));

	ellipse[numberOfEllipses - 1] = new TEllipse(xCenter, yCenter, xRadius, yRadius, angleMin, angleMax, rotationAngle);

	return ellipse[numberOfEllipses - 1];

}

/****************************************************************
 * This method initializes a TLatex.							*
 ****************************************************************/

TLatex* houghPictures::addText(double xPos, double yPos, double textSize, std::string value) {

	numberOfTexts++;
	if (text == NULL)
		text = (TLatex**)calloc(numberOfTexts, sizeof(TLatex*));
	else
		text = (TLatex**)realloc(text, numberOfTexts * sizeof(TLatex*));

	text[numberOfTexts - 1] = new TLatex(xPos, yPos, value.c_str());
	text[numberOfTexts - 1]->SetTextAlign(12);
	text[numberOfTexts - 1]->SetTextSize(textSize);

	return text[numberOfTexts - 1];

}

/****************************************************************
 * This method creates the straight line in the coordinate		*
 * space.														*
 ****************************************************************/

void houghPictures::createCoordinateStraightLine() {

	double*  arrayX;
	double*  arrayY;
	TMarker* actualMarker;
	TArrow*  actualArrow;
	TLatex*  actualText;

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayY = new double[QUANTIZATION];
	if (arrayY == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayX[i] = (double)i * ((double)(2 * COORDINATESTRAIGHTLINERANGEX) / (double)(QUANTIZATION-1)) - (double)COORDINATESTRAIGHTLINERANGEX;
		arrayY[i] = (double)(COORDINATESTRAIGHTLINEM * arrayX[i] + COORDINATESTRAIGHTLINEC);

	}

	if (coordinateStraightLineWindow == NULL) {

		coordinateStraightLineWindow    = addWindow("CoordinateStraightLine", "Coordinate Space for the Straight Line", -COORDINATESTRAIGHTLINERANGEX, COORDINATESTRAIGHTLINERANGEX, - COORDINATESTRAIGHTLINERANGEY, COORDINATESTRAIGHTLINERANGEY, WINDOWSIZE, WINDOWSIZE);

		coordinateStraightLineDisplay   = addDisplay(QUANTIZATION, arrayX, arrayY, -COORDINATESTRAIGHTLINERANGEX, COORDINATESTRAIGHTLINERANGEX, -COORDINATESTRAIGHTLINERANGEY, COORDINATESTRAIGHTLINERANGEY, 1, "x", "y");

		coordinateStraightLineDisplay->SetName("CoordinateStraightLine");
		coordinateStraightLineDisplay->SetTitle("Coordinate Space for the Straight Line");
		coordinateStraightLineDisplay->GetXaxis()->SetTitle("x");
		coordinateStraightLineDisplay->GetYaxis()->SetTitle("y");

		coordinateStraightLineDisplay->Draw("C");

		/* draw the transforming points */
		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			actualMarker = addMarker(arrayX[COORDINATEPOINTS[j]], arrayY[COORDINATEPOINTS[j]], MARKERTYPE);
			actualMarker->Draw();

		}

		/* draw the y-axis part */
		actualArrow = addArrow(-1.5 * COORDINATELENGTH, COORDINATESTRAIGHTLINEC, 0.5 * COORDINATELENGTH, COORDINATESTRAIGHTLINEC,"");
		actualArrow->Draw();
		actualArrow = addArrow(-COORDINATELENGTH, 0, -COORDINATELENGTH, COORDINATESTRAIGHTLINEC,"<|>");
		actualArrow->Draw();

		/* draw the slope */
		actualArrow = addArrow(arrayX[COORDINATESTRAIGHTLINEMSTART], arrayY[COORDINATESTRAIGHTLINEMSTART], arrayX[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH], arrayY[COORDINATESTRAIGHTLINEMSTART],"<|>");
		actualArrow->Draw();
		actualArrow = addArrow(arrayX[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH], arrayY[COORDINATESTRAIGHTLINEMSTART], arrayX[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH], arrayY[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH],"<|>");
		actualArrow->Draw();

		/* draw the function description */
		actualText = addText(FUNCTIONTEXTXPOSITION * COORDINATESTRAIGHTLINERANGEX, FUNCTIONTEXTYPOSITION * COORDINATESTRAIGHTLINERANGEY, FUNCTIONTEXTSIZE, "y = mx + c");
		actualText->Draw();

		/* draw the slope description */
		if (arrayY[COORDINATESTRAIGHTLINEMSTART] < arrayY[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH])
			actualText = addText(arrayX[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH / 2], arrayY[COORDINATESTRAIGHTLINEMSTART] - RELATIVEDISTANCE * COORDINATESTRAIGHTLINERANGEY, TEXTSIZE, "1");
		else
			actualText = addText(arrayX[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH / 2], arrayY[COORDINATESTRAIGHTLINEMSTART] + RELATIVEDISTANCE * COORDINATESTRAIGHTLINERANGEY, TEXTSIZE, "1");
		actualText->Draw();
		actualText = addText(arrayX[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH] + RELATIVEDISTANCE * COORDINATESTRAIGHTLINERANGEX, arrayY[COORDINATESTRAIGHTLINEMSTART] + (arrayY[COORDINATESTRAIGHTLINEMSTART + COORDINATESTRAIGHTLINEMLENGTH] - arrayY[COORDINATESTRAIGHTLINEMSTART]) / 2, TEXTSIZE, "m");
		actualText->Draw();

		/* draw the y-axis part description */
		actualText = addText(-COORDINATELENGTH - RELATIVEDISTANCE * COORDINATESTRAIGHTLINERANGEX, COORDINATESTRAIGHTLINEC / 2, TEXTSIZE, "c");
		actualText->Draw();

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

/****************************************************************
 * This method creates the straight line in the Hough space.	*
 ****************************************************************/

void houghPictures::createHoughStraightLine() {

	double*     arrayX;
	double*     arrayY;
	double*     arrayM;
	double*     arrayC;
	double      houghRangeM;
	double      houghRangeC;
	char        buffer[intConversionDigits + 1];
	std::string displayName;
	TMarker*    actualMarker;
	TLatex*     actualText;

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayY = new double[QUANTIZATION];
	if (arrayY == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayM = new double[QUANTIZATION];
	if (arrayM == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayC = new double[QUANTIZATION];
	if (arrayC == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	if (HOUGHLINERANGEM < 0)
		houghRangeM = - HOUGHLINERANGEM;
	else
		houghRangeM = HOUGHLINERANGEM;

	if (HOUGHLINERANGEC < 0)
		houghRangeC = - HOUGHLINERANGEC;
	else
		houghRangeC = HOUGHLINERANGEC;

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayX[i] = (double)i * ((double)(2 * COORDINATESTRAIGHTLINERANGEX) / (double)(QUANTIZATION-1)) - (double)COORDINATESTRAIGHTLINERANGEX;
		arrayY[i] = (double)(COORDINATESTRAIGHTLINEM * arrayX[i] + COORDINATESTRAIGHTLINEC);

	}

	if (houghStraightLineWindow == NULL) {

		houghStraightLineWindow = addWindow("HoughStraightLine", "Hough Space for the Straight Line", -houghRangeM, houghRangeM, -houghRangeC, houghRangeC, WINDOWSIZE, WINDOWSIZE);

		houghStraightLineDisplay = (TGraph**)calloc(NUMBEROFCOORDINATEPOINTS, sizeof(TGraph*));

		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			for (unsigned int k = 0; k < QUANTIZATION; k++) {

				arrayM[k] = (double)k * ((double)(2 * houghRangeM) / (double)(QUANTIZATION-1)) - (double)houghRangeM;
				arrayC[k] = (double)(-arrayM[k] * arrayX[COORDINATEPOINTS[j]] + arrayY[COORDINATEPOINTS[j]]);

			}

			houghStraightLineDisplay[j]   = addDisplay(QUANTIZATION, arrayM, arrayC, -houghRangeM, houghRangeM, -houghRangeC, houghRangeC, 1, "m", "c");

			displayName                   = "HoughStraightLine_TGraph_";
			displayName                  += uitos(j, buffer, 10, intConversionDigits);
			houghStraightLineDisplay[j]->SetName(displayName.c_str());
			houghStraightLineDisplay[j]->SetTitle("Hough Space for the Straight Line");
			houghStraightLineDisplay[j]->GetXaxis()->SetTitle("m");
			houghStraightLineDisplay[j]->GetYaxis()->SetTitle("c");

			houghStraightLineDisplay[j]->Draw("SAME C");

		}

		/* draw the point of the found straight line */
		actualMarker = addMarker(COORDINATESTRAIGHTLINEM, COORDINATESTRAIGHTLINEC, MARKERTYPE);
		actualMarker->Draw();

		/* draw the function description */
		actualText = addText(FUNCTIONTEXTXPOSITION * houghRangeM, FUNCTIONTEXTYPOSITION * houghRangeC, FUNCTIONTEXTSIZE, "c = -mx + y");
		actualText->Draw();

	}

	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}
	if (arrayY != NULL) {
		delete arrayY;
		arrayY = NULL;
	}
	if (arrayM != NULL) {
		delete arrayM;
		arrayM = NULL;
	}
	if (arrayC != NULL) {
		delete arrayC;
		arrayC = NULL;
	}

}

/****************************************************************
 * This method creates the Hesse line in the coordinate space.	*
 ****************************************************************/

void houghPictures::createCoordinateHesseLine() {

	double*   arrayX;
	double*   arrayY;
	double    mN;
	double    nStartX;
	double    nStopX;
	double    nStartY;
	double    nStopY;
	double    normAngleMax;
	double    inverseAngleMax;
	TMarker*  actualMarker;
	TArrow*   actualArrow;
	TEllipse* actualEllipse;
	TLatex*   actualText;

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayY = new double[QUANTIZATION];
	if (arrayY == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned int i = 0; i < QUANTIZATION; i++) {
		
		arrayX[i] = (double)i * ((double)(2 * COORDINATESTRAIGHTLINERANGEX) / (double)(QUANTIZATION-1)) - (double)COORDINATESTRAIGHTLINERANGEX;
		arrayY[i] = (double)(COORDINATESTRAIGHTLINEM * arrayX[i] + COORDINATESTRAIGHTLINEC);

	}

	if (coordinateHesseLineWindow == NULL) {

		coordinateHesseLineWindow    = addWindow("CoordinateHesseLine", "Coordinate Space for the Hesse Line", -COORDINATESTRAIGHTLINERANGEX, COORDINATESTRAIGHTLINERANGEX, -COORDINATESTRAIGHTLINERANGEY, COORDINATESTRAIGHTLINERANGEY, WINDOWSIZE, WINDOWSIZE);

		coordinateHesseLineDisplay   = addDisplay(QUANTIZATION, arrayX, arrayY, -COORDINATESTRAIGHTLINERANGEX, COORDINATESTRAIGHTLINERANGEX, -COORDINATESTRAIGHTLINERANGEY, COORDINATESTRAIGHTLINERANGEY, 1, "x", "y");

		coordinateHesseLineDisplay->SetName("CoordinateHesseLine");
		coordinateHesseLineDisplay->SetTitle("Coordinate Space for the Hesse Line");
		coordinateHesseLineDisplay->GetXaxis()->SetTitle("x");
		coordinateHesseLineDisplay->GetYaxis()->SetTitle("y");

		coordinateHesseLineDisplay->Draw("C");

		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			actualMarker = addMarker(arrayX[COORDINATEPOINTS[j]], arrayY[COORDINATEPOINTS[j]],MARKERTYPE);
			actualMarker->Draw();

		}

		/* draw the normal to the straight line */
		mN = -(1 / COORDINATESTRAIGHTLINEM);

		if ((mN > 1) || (mN < -1)) {

			nStartX = -1.2 * COORDINATESTRAIGHTLINEC / mN;
			nStartY = -1.2 * COORDINATESTRAIGHTLINEC;
			nStopX  = +1.2 * COORDINATESTRAIGHTLINEC / mN;
			nStopY  = +1.2 * COORDINATESTRAIGHTLINEC;

		}
		else {

			nStartX = -1.2 * COORDINATESTRAIGHTLINEC;
			nStartY = -1.2 * COORDINATESTRAIGHTLINEC * mN;
			nStopX  = +1.2 * COORDINATESTRAIGHTLINEC;
			nStopY  = +1.2 * COORDINATESTRAIGHTLINEC * mN;

		}

		actualArrow = addArrow(nStartX, nStartY, nStopX, nStopY,"|>");
		actualArrow->Draw();

		/* draw the normal description */
		actualText = addText(nStopX + RELATIVEDISTANCE * COORDINATESTRAIGHTLINERANGEX / sqrt((double)2), nStopY - RELATIVEDISTANCE * COORDINATESTRAIGHTLINERANGEY / sqrt((double)2), TEXTSIZE, "#vec{n} = #left(#splitline{n_{x}}{n_{y}}#right)");
		actualText->Draw();

		/* draw the length of the normal */
		nStartX = - COORDINATELENGTH;
		nStartY = - COORDINATELENGTH * COORDINATESTRAIGHTLINEM;
		nStopX  = ((COORDINATESTRAIGHTLINEC) / (mN - COORDINATESTRAIGHTLINEM)) - COORDINATELENGTH;
		nStopY  = ((COORDINATESTRAIGHTLINEC * COORDINATESTRAIGHTLINEM) / (mN - COORDINATESTRAIGHTLINEM)) + COORDINATESTRAIGHTLINEC - COORDINATELENGTH * COORDINATESTRAIGHTLINEM;

		actualArrow = addArrow(nStartX, nStartY, nStopX, nStopY,"<|>");
		actualArrow->Draw();

		/* draw the length description */
		actualText = addText(nStartX + (nStopX - nStartX) / 2 - RELATIVEDISTANCE * COORDINATESTRAIGHTLINERANGEX / sqrt((double)2) - TEXTSIZE, nStartY + (nStopY - nStartY) / 2 + RELATIVEDISTANCE * COORDINATESTRAIGHTLINERANGEY / sqrt((double)2), TEXTSIZE, "p");
		actualText->Draw();

		/* draw the angle of the normal */
		normAngleMax  = atan(mN) * ((double)180 / PI);
		actualEllipse = addEllipse(0, 0, COORDINATENORMANGLE, 0, 0, normAngleMax, 0);
		actualEllipse->Draw();

		/* draw the inverse with inverse angle of the normal */
		actualArrow = addArrow(-COORDINATESTRAIGHTLINEC, - COORDINATESTRAIGHTLINEM * COORDINATESTRAIGHTLINEC, +COORDINATESTRAIGHTLINEC, COORDINATESTRAIGHTLINEM * COORDINATESTRAIGHTLINEC,"");
		actualArrow->Draw();
		inverseAngleMax = 90 - normAngleMax;
		actualEllipse = addEllipse(0, 0, COORDINATENORMANGLE, 0, 0, inverseAngleMax, 360 - inverseAngleMax);
		actualEllipse->Draw();

		/* draw the function description */
		actualText = addText(FUNCTIONTEXTXPOSITION * COORDINATESTRAIGHTLINERANGEX, FUNCTIONTEXTYPOSITION * COORDINATESTRAIGHTLINERANGEY, FUNCTIONTEXTSIZE, "0 = #left(#splitline{n_{x}}{n_{y}}#right)#upoint#left(#left(#splitline{x}{y}#right)-p#upoint#left(#splitline{n_{x}}{n_{y}}#right)#right)");
		actualText->Draw();
		actualText = addText(FUNCTIONTEXTXPOSITION * COORDINATESTRAIGHTLINERANGEX, FUNCTIONTEXTYPOSITION * COORDINATESTRAIGHTLINERANGEY - 2 * 2.1 * FUNCTIONTEXTSIZE * COORDINATESTRAIGHTLINERANGEY, FUNCTIONTEXTSIZE, "p = x#upointcos#left(#phi#right)+y#upointsin#left(#phi#right)");
		actualText->Draw();
		actualText = addText(FUNCTIONTEXTXPOSITION * COORDINATESTRAIGHTLINERANGEX, FUNCTIONTEXTYPOSITION * COORDINATESTRAIGHTLINERANGEY - 2 * 3.2 * FUNCTIONTEXTSIZE * COORDINATESTRAIGHTLINERANGEY, FUNCTIONTEXTSIZE, "p = y#upointcos#left(#theta#right)-x#upointsin#left(#theta#right)");
		actualText->Draw();

		/* draw the angles description */
		actualText = addText(0.6 * COORDINATENORMANGLE, 0.4 * COORDINATENORMANGLE * sin(normAngleMax) + TEXTSIZE * COORDINATESTRAIGHTLINERANGEY / 2, TEXTSIZE, "#phi");
		actualText->Draw();
		actualText = addText(0.6 * COORDINATENORMANGLE, 0.4 * COORDINATENORMANGLE * sin(inverseAngleMax), TEXTSIZE, "#theta");
		actualText->Draw();

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

/****************************************************************
 * This method creates the Hesse line in the Hough space in		*
 * respect to phi.												*
 ****************************************************************/

void houghPictures::createHoughPhiHesseLine() {

	double*     arrayX;
	double*     arrayY;
	double*     arrayPhi;
	double*     arrayP;
	double      houghRangePhi;
	double      houghRangeP;
	char        buffer[intConversionDigits + 1];
	std::string displayName;
	TMarker*    actualMarker;
	TLatex*     actualText;

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayY = new double[QUANTIZATION];
	if (arrayY == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayPhi = new double[QUANTIZATION];
	if (arrayPhi == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayP = new double[QUANTIZATION];
	if (arrayP == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	if (HOUGHHESSERANGEPHI < 0)
		houghRangePhi = - HOUGHHESSERANGEPHI;
	else
		houghRangePhi = HOUGHHESSERANGEPHI;

	if (HOUGHHESSERANGEPPHI < 0)
		houghRangeP = - HOUGHHESSERANGEPPHI;
	else
		houghRangeP = HOUGHHESSERANGEPPHI;

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayX[i] = (double)i * ((double)(2 * COORDINATESTRAIGHTLINERANGEX) / (double)(QUANTIZATION-1)) - (double)COORDINATESTRAIGHTLINERANGEX;
		arrayY[i] = (double)(COORDINATESTRAIGHTLINEM * arrayX[i] + COORDINATESTRAIGHTLINEC);

	}

	if (houghPhiHesseLineWindow == NULL) {

		houghPhiHesseLineWindow = addWindow("HoughPhiHesseLine", "Hough Space for the Hesse Line", -houghRangePhi, houghRangePhi, -houghRangeP, houghRangeP, WINDOWSIZE, WINDOWSIZE);

		houghPhiHesseLineDisplay = (TGraph**)calloc(NUMBEROFCOORDINATEPOINTS, sizeof(TGraph*));

		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			for (unsigned int k = 0; k < QUANTIZATION; k++) {

				arrayPhi[k] = (double)k * ((double)(2 * houghRangePhi) / (double)(QUANTIZATION-1)) - (double)houghRangePhi;
				arrayP[k]   = (double)(arrayX[COORDINATEPOINTS[j]] * cos(arrayPhi[k]) + arrayY[COORDINATEPOINTS[j]] * sin(arrayPhi[k]));

			}

			houghPhiHesseLineDisplay[j]   = addDisplay(QUANTIZATION, arrayPhi, arrayP, -houghRangePhi, houghRangePhi, -houghRangeP, houghRangeP, 1, "#phi", "p");

			displayName                   = "HoughPhiHesseLine_TGraph_";
			displayName                  += uitos(j, buffer, 10, intConversionDigits);
			houghPhiHesseLineDisplay[j]->SetName(displayName.c_str());
			houghPhiHesseLineDisplay[j]->SetTitle("Hough Space for the Hesse Line");
			houghPhiHesseLineDisplay[j]->GetXaxis()->SetTitle("phi");
			houghPhiHesseLineDisplay[j]->GetYaxis()->SetTitle("p");

			houghPhiHesseLineDisplay[j]->Draw("SAME C");

		}

		/* draw the point of the found straight line */
		actualMarker = addMarker((PI / 2) + atan(COORDINATESTRAIGHTLINEM), cos(-atan(COORDINATESTRAIGHTLINEM)) * COORDINATESTRAIGHTLINEC, MARKERTYPE);
		actualMarker->Draw();

		/* draw the function description */
		actualText = addText(FUNCTIONTEXTXPOSITION * houghRangePhi, FUNCTIONTEXTYPOSITION * houghRangeP, FUNCTIONTEXTSIZE, "p = x#upointcos#left(#phi#right)+y#upointsin#left(#phi#right)");
		actualText->Draw();

	}

	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}
	if (arrayY != NULL) {
		delete arrayY;
		arrayY = NULL;
	}
	if (arrayPhi != NULL) {
		delete arrayPhi;
		arrayPhi = NULL;
	}
	if (arrayP != NULL) {
		delete arrayP;
		arrayP = NULL;
	}

}

/****************************************************************
 * This method creates the Hesse line in the Hough space in		*
 * respect to theta.											*
 ****************************************************************/

void houghPictures::createHoughThetaHesseLine() {

	double*     arrayX;
	double*     arrayY;
	double*     arrayTheta;
	double*     arrayP;
	double      houghRangeTheta;
	double      houghRangeP;
	char        buffer[intConversionDigits + 1];
	std::string displayName;
	TMarker*    actualMarker;
	TLatex*     actualText;

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayY = new double[QUANTIZATION];
	if (arrayY == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayTheta = new double[QUANTIZATION];
	if (arrayTheta == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayP = new double[QUANTIZATION];
	if (arrayP == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	if (HOUGHHESSERANGETHETA < 0)
		houghRangeTheta = - HOUGHHESSERANGETHETA;
	else
		houghRangeTheta = HOUGHHESSERANGETHETA;

	if (HOUGHHESSERANGEPTHETA < 0)
		houghRangeP = - HOUGHHESSERANGEPTHETA;
	else
		houghRangeP = HOUGHHESSERANGEPTHETA;

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayX[i] = (double)i * ((double)(2 * COORDINATESTRAIGHTLINERANGEX) / (double)(QUANTIZATION-1)) - (double)COORDINATESTRAIGHTLINERANGEX;
		arrayY[i] = (double)(COORDINATESTRAIGHTLINEM * arrayX[i] + COORDINATESTRAIGHTLINEC);

	}

	if (houghThetaHesseLineWindow == NULL) {

		houghThetaHesseLineWindow = addWindow("HoughThetaHesseLine", "Hough Space for the Hesse Line", -houghRangeTheta, houghRangeTheta, -houghRangeP, houghRangeP, WINDOWSIZE, WINDOWSIZE);

		houghThetaHesseLineDisplay = (TGraph**)calloc(NUMBEROFCOORDINATEPOINTS, sizeof(TGraph*));

		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			for (unsigned int k = 0; k < QUANTIZATION; k++) {

				arrayTheta[k] = (double)k * ((double)(2 * houghRangeTheta) / (double)(QUANTIZATION-1)) - (double)houghRangeTheta;
				arrayP[k]   = (double)(arrayY[COORDINATEPOINTS[j]] * cos(arrayTheta[k]) - arrayX[COORDINATEPOINTS[j]] * sin(arrayTheta[k]));

			}

			houghThetaHesseLineDisplay[j]   = addDisplay(QUANTIZATION, arrayTheta, arrayP, -houghRangeTheta, houghRangeTheta, -houghRangeP, houghRangeP, 1, "#theta", "p");

			displayName                   = "HoughThetaHesseLine_TGraph_";
			displayName                  += uitos(j, buffer, 10, intConversionDigits);
			houghThetaHesseLineDisplay[j]->SetName(displayName.c_str());
			houghThetaHesseLineDisplay[j]->SetTitle("Hough Space for the Hesse Line");
			houghThetaHesseLineDisplay[j]->GetXaxis()->SetTitle("theta");
			houghThetaHesseLineDisplay[j]->GetYaxis()->SetTitle("p");

			houghThetaHesseLineDisplay[j]->Draw("SAME C");

		}

		/* draw the point of the found straight line */
		actualMarker = addMarker(atan(COORDINATESTRAIGHTLINEM), cos(-atan(COORDINATESTRAIGHTLINEM)) * COORDINATESTRAIGHTLINEC, MARKERTYPE);
		actualMarker->Draw();

		/* draw the function description */
		actualText = addText(FUNCTIONTEXTXPOSITION * houghRangeTheta, FUNCTIONTEXTYPOSITION * houghRangeP, FUNCTIONTEXTSIZE, "p = y#upointcos#left(#theta#right)-x#upointsin#left(#theta#right)");
		actualText->Draw();

	}

	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}
	if (arrayY != NULL) {
		delete arrayY;
		arrayY = NULL;
	}
	if (arrayTheta != NULL) {
		delete arrayTheta;
		arrayTheta = NULL;
	}
	if (arrayP != NULL) {
		delete arrayP;
		arrayP = NULL;
	}

}

/****************************************************************
 * This method creates the analyticFormula object in the		*
 * coordinate space with just one coordinate.					*
 ****************************************************************/

void houghPictures::createOneCoordinateHitAnalyticFormula() {

	double*         arrayZ;
	double*         arrayX;
	analyticFormula formula;
	TMarker*        actualMarker;
	TArrow*         actualArrow;

	arrayZ = new double[QUANTIZATION];
	if (arrayZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayZ[i] = (double)i * ((double)(COORDINATEANALYTICFORMULARANGEZ) / (double)(QUANTIZATION-1));
		arrayX[i] = (double)formula.evaluateFormulaLut(COORDINATEANALYTICFORMULAQ, COORDINATEANALYTICFORMULAPX, COORDINATEANALYTICFORMULAPZ, arrayZ[i], COORDINATEANALYTICFORMULAB);

	}

	if (oneCoordinateHitAnalyticFormulaWindow == NULL) {

		oneCoordinateHitAnalyticFormulaWindow    = addWindow("OneCoordinateHitAnalyticFormula", "One Hit in the Coordinate Space for the analyticFormula", -( 2 * RELATIVEDISTANCE * COORDINATEANALYTICFORMULARANGEZ), COORDINATEANALYTICFORMULARANGEZ, -COORDINATEANALYTICFORMULARANGEX, COORDINATEANALYTICFORMULARANGEX, WINDOWSIZE, WINDOWSIZE);

		oneCoordinateHitAnalyticFormulaDisplay   = addDisplay(QUANTIZATION, arrayZ, arrayX, -(2 * RELATIVEDISTANCE * COORDINATEANALYTICFORMULARANGEZ), COORDINATEANALYTICFORMULARANGEZ, -COORDINATEANALYTICFORMULARANGEX, COORDINATEANALYTICFORMULARANGEX, 1, "z", "x");

		/* draw the transforming points */
		if (NUMBEROFCOORDINATEPOINTS > 0) {

			actualMarker = addMarker(arrayZ[COORDINATEPOINTS[NUMBEROFCOORDINATEPOINTS/2]], arrayX[COORDINATEPOINTS[NUMBEROFCOORDINATEPOINTS/2]], MARKERTYPE);
			actualMarker->Draw();

		}

		/* draw the detector planes */
		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			actualArrow = addArrow(arrayZ[COORDINATEPOINTS[j]], -arrayZ[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]],"");
			actualArrow->Draw();

		}

	}

	if (arrayZ != NULL) {
		delete arrayZ;
		arrayZ = NULL;
	}
	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}

}

/****************************************************************
 * This method creates the analyticFormula object in the		*
 * Hough space with just one curve.								*
 ****************************************************************/

void houghPictures::createOneHoughCurveAnalyticFormula() {

	double*         arrayZ;
	double*         arrayX;
	double*         arrayTheta;
	double*         arrayQPxz;
	analyticFormula formula;
	double          houghRangeTheta;
	double          houghRangeQPxz;

	arrayZ = new double[QUANTIZATION];
	if (arrayZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayTheta = new double[QUANTIZATION];
	if (arrayTheta == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayQPxz = new double[QUANTIZATION];
	if (arrayQPxz == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	if (HOUGHANALYTICFORMULARANGETHETA < 0)
		houghRangeTheta = - HOUGHANALYTICFORMULARANGETHETA;
	else
		houghRangeTheta = HOUGHANALYTICFORMULARANGETHETA;

	if (HOUGHANALYTICFORMULARANGEQPXZ < 0)
		houghRangeQPxz = - HOUGHANALYTICFORMULARANGEQPXZ;
	else
		houghRangeQPxz = HOUGHANALYTICFORMULARANGEQPXZ;

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayZ[i] = (double)i * ((double)(COORDINATEANALYTICFORMULARANGEZ) / (double)(QUANTIZATION-1));
		arrayX[i] = (double)formula.evaluateFormulaLut(COORDINATEANALYTICFORMULAQ, COORDINATEANALYTICFORMULAPX, COORDINATEANALYTICFORMULAPZ, arrayZ[i], HOUGHANALYTICFORMULAB);

	}

	for (unsigned int j = 0; j < QUANTIZATION; j++) {

		arrayTheta[j] = (double)j * ((double)(2 * houghRangeTheta) / (double)(QUANTIZATION-1)) - (double)houghRangeTheta;
		arrayQPxz[j]  = (double)formula.evaluateLut(arrayX[COORDINATEPOINTS[NUMBEROFCOORDINATEPOINTS/2]], arrayZ[COORDINATEPOINTS[NUMBEROFCOORDINATEPOINTS/2]], arrayTheta[j], HOUGHANALYTICFORMULAB);

	}

	if (oneHoughCurveAnalyticFormulaWindow == NULL) {

		oneHoughCurveAnalyticFormulaWindow = addWindow("OneHoughCurveAnalyticFormula", "One Curve in the Hough Space for the analyticFormula", -houghRangeTheta, houghRangeTheta, -houghRangeQPxz, houghRangeQPxz, WINDOWSIZE, WINDOWSIZE);

		oneHoughCurveAnalyticFormulaDisplay  = addDisplay(QUANTIZATION, arrayTheta, arrayQPxz, -houghRangeTheta, houghRangeTheta, -houghRangeQPxz, houghRangeQPxz, 2, "#theta", "- #frac{q}{p_{xz}}");

		oneHoughCurveAnalyticFormulaDisplay->SetName("OneHoughCurveAnalyticFormula");
		oneHoughCurveAnalyticFormulaDisplay->SetTitle("One Curve in the Hough Space for the analyticFormula");
		oneHoughCurveAnalyticFormulaDisplay->GetXaxis()->SetTitle("theta");
		oneHoughCurveAnalyticFormulaDisplay->GetYaxis()->SetTitle("-q/p_xz");

		oneHoughCurveAnalyticFormulaDisplay->Draw("C");

	}

	if (arrayZ != NULL) {
		delete arrayZ;
		arrayZ = NULL;
	}
	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}
	if (arrayTheta != NULL) {
		delete arrayTheta;
		arrayTheta = NULL;
	}
	if (arrayQPxz != NULL) {
		delete arrayQPxz;
		arrayQPxz = NULL;
	}

}

/****************************************************************
 * This method creates the analyticFormula object in the		*
 * coordinate space with a whole track.							*
 ****************************************************************/

void houghPictures::createOneCoordinateTrackAnalyticFormula() {

	double*         arrayZ;
	double*         arrayX;
	analyticFormula formula;
	TArrow*         actualArrow;

	arrayZ = new double[QUANTIZATION];
	if (arrayZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayZ[i] = (double)i * ((double)(COORDINATEANALYTICFORMULARANGEZ) / (double)(QUANTIZATION-1));
		arrayX[i] = (double)formula.evaluateFormulaLut(COORDINATEANALYTICFORMULAQ, COORDINATEANALYTICFORMULAPX, COORDINATEANALYTICFORMULAPZ, arrayZ[i], COORDINATEANALYTICFORMULAB);

	}

	if (oneCoordinateTrackAnalyticFormulaWindow == NULL) {

		oneCoordinateTrackAnalyticFormulaWindow    = addWindow("OneCoordinateTrackAnalyticFormula", "One Track in the Coordinate Space for the analyticFormula", -( 2 * RELATIVEDISTANCE * COORDINATEANALYTICFORMULARANGEZ), COORDINATEANALYTICFORMULARANGEZ, -COORDINATEANALYTICFORMULARANGEX, COORDINATEANALYTICFORMULARANGEX, WINDOWSIZE, WINDOWSIZE);

		oneCoordinateTrackAnalyticFormulaDisplay   = addDisplay(QUANTIZATION, arrayZ, arrayX, -(2 * RELATIVEDISTANCE * COORDINATEANALYTICFORMULARANGEZ), COORDINATEANALYTICFORMULARANGEZ, -COORDINATEANALYTICFORMULARANGEX, COORDINATEANALYTICFORMULARANGEX, 1, "z", "x");

		oneCoordinateTrackAnalyticFormulaDisplay->SetName("OneCoordinateTrackAnalyticFormula");
		oneCoordinateTrackAnalyticFormulaDisplay->SetTitle("One Track in the Coordinate Space for the analyticFormula");
		oneCoordinateTrackAnalyticFormulaDisplay->GetXaxis()->SetTitle("z");
		oneCoordinateTrackAnalyticFormulaDisplay->GetYaxis()->SetTitle("x");

		oneCoordinateTrackAnalyticFormulaDisplay->Draw("C");

		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			/* draw the detector planes */
			actualArrow = addArrow(arrayZ[COORDINATEPOINTS[j]], -arrayZ[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]],"");
			actualArrow->Draw();

		}

	}

	if (arrayZ != NULL) {
		delete arrayZ;
		arrayZ = NULL;
	}
	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}

}

/****************************************************************
 * This method creates the analyticFormula object in the		*
 * Hough space with just the intersection point.				*
 ****************************************************************/

void houghPictures::createOneHoughPointAnalyticFormula() {

	double*         arrayZ;
	double*         arrayX;
	double*         arrayTheta;
	double*         arrayQPxz;
	analyticFormula formula;
	double          houghRangeTheta;
	double          houghRangeQPxz;
	TMarker*        actualMarker;

	arrayZ = new double[QUANTIZATION];
	if (arrayZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayTheta = new double[QUANTIZATION];
	if (arrayTheta == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayQPxz = new double[QUANTIZATION];
	if (arrayQPxz == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	if (HOUGHANALYTICFORMULARANGETHETA < 0)
		houghRangeTheta = - HOUGHANALYTICFORMULARANGETHETA;
	else
		houghRangeTheta = HOUGHANALYTICFORMULARANGETHETA;

	if (HOUGHANALYTICFORMULARANGEQPXZ < 0)
		houghRangeQPxz = - HOUGHANALYTICFORMULARANGEQPXZ;
	else
		houghRangeQPxz = HOUGHANALYTICFORMULARANGEQPXZ;

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayZ[i] = (double)i * ((double)(COORDINATEANALYTICFORMULARANGEZ) / (double)(QUANTIZATION-1));
		arrayX[i] = (double)formula.evaluateFormulaLut(COORDINATEANALYTICFORMULAQ, COORDINATEANALYTICFORMULAPX, COORDINATEANALYTICFORMULAPZ, arrayZ[i], HOUGHANALYTICFORMULAB);

	}

	if (NUMBEROFCOORDINATEPOINTS > 0) {

		for (unsigned int j = 0; j < QUANTIZATION; j++) {

			arrayTheta[j] = (double)j * ((double)(2 * houghRangeTheta) / (double)(QUANTIZATION-1)) - (double)houghRangeTheta;
			arrayQPxz[j]  = (double)formula.evaluateLut(arrayX[COORDINATEPOINTS[NUMBEROFCOORDINATEPOINTS/2]], arrayZ[COORDINATEPOINTS[NUMBEROFCOORDINATEPOINTS/2]], arrayTheta[j], HOUGHANALYTICFORMULAB);

		}

	}

	if (oneHoughPointAnalyticFormulaWindow == NULL) {

		oneHoughPointAnalyticFormulaWindow    = addWindow("OneHoughPointAnalyticFormula", "One Point in the Hough Space for the analyticFormula", -houghRangeTheta, houghRangeTheta, -houghRangeQPxz, houghRangeQPxz, WINDOWSIZE, WINDOWSIZE);

		oneHoughPointAnalyticFormulaDisplay   = addDisplay(QUANTIZATION, arrayTheta, arrayQPxz, -houghRangeTheta, houghRangeTheta, -houghRangeQPxz, houghRangeQPxz, 2, "#theta", "- #frac{q}{p_{xz}}");

		/* draw the point of the found curve */
		actualMarker = addMarker(atan(COORDINATEANALYTICFORMULAPX / COORDINATEANALYTICFORMULAPZ), -COORDINATEANALYTICFORMULAQ / sqrt(sqr(COORDINATEANALYTICFORMULAPX) + sqr(COORDINATEANALYTICFORMULAPZ)), MARKERTYPE);
		actualMarker->Draw();

	}

	if (arrayZ != NULL) {
		delete arrayZ;
		arrayZ = NULL;
	}
	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}
	if (arrayTheta != NULL) {
		delete arrayTheta;
		arrayTheta = NULL;
	}
	if (arrayQPxz != NULL) {
		delete arrayQPxz;
		arrayQPxz = NULL;
	}

}

/****************************************************************
 * This method creates the analyticFormula object in the		*
 * coordinate space.											*
 ****************************************************************/

void houghPictures::createCoordinateAnalyticFormulaPrelut() {

	double*         arrayZ;
	double*         arrayY;
	analyticFormula formula;
	TMarker*        actualMarker;
	TArrow*         actualArrow;
	TLatex*         actualText;

	arrayZ = new double[QUANTIZATION];
	if (arrayZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayY = new double[QUANTIZATION];
	if (arrayY == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayZ[i] = (double)i * ((double)(COORDINATEANALYTICFORMULARANGEZ) / (double)(QUANTIZATION-1));
		arrayY[i] = (double)formula.evaluateFormulaPrelut(COORDINATEANALYTICFORMULAM, COORDINATEANALYTICFORMULAPY, COORDINATEANALYTICFORMULAPZ, arrayZ[i]);

	}

	if (coordinateAnalyticFormulaPrelutWindow == NULL) {

		coordinateAnalyticFormulaPrelutWindow    = addWindow("CoordinatePrelutAnalyticFormula", "Coordinate Space for the prelut of the analyticFormula", -( 2 * RELATIVEDISTANCE * COORDINATEANALYTICFORMULARANGEZ), COORDINATEANALYTICFORMULARANGEZ, -COORDINATEANALYTICFORMULARANGEY, COORDINATEANALYTICFORMULARANGEY, WINDOWSIZE, WINDOWSIZE);

		coordinateAnalyticFormulaPrelutDisplay   = addDisplay(QUANTIZATION, arrayZ, arrayY, -(2 * RELATIVEDISTANCE * COORDINATEANALYTICFORMULARANGEZ), COORDINATEANALYTICFORMULARANGEZ, -COORDINATEANALYTICFORMULARANGEY, COORDINATEANALYTICFORMULARANGEY, 1, "z", "y");

		coordinateAnalyticFormulaPrelutDisplay->SetName("CoordinatePrelutAnalyticFormula");
		coordinateAnalyticFormulaPrelutDisplay->SetTitle("Coordinate Space for the prelut of the analyticFormula");
		coordinateAnalyticFormulaPrelutDisplay->GetXaxis()->SetTitle("z");
		coordinateAnalyticFormulaPrelutDisplay->GetYaxis()->SetTitle("y");

		coordinateAnalyticFormulaPrelutDisplay->Draw("C");

		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			/* draw the transforming points */
			actualMarker = addMarker(arrayZ[COORDINATEPOINTS[j]], arrayY[COORDINATEPOINTS[j]], MARKERTYPE);
			actualMarker->Draw();

			/* draw the detector planes */
			actualArrow = addArrow(arrayZ[COORDINATEPOINTS[j]], -arrayZ[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]],"");
			actualArrow->Draw();

		}

		/* draw the function description */
		actualText = addText(0.5 * FUNCTIONTEXTXPOSITION * COORDINATEANALYTICFORMULARANGEZ, FUNCTIONTEXTYPOSITION * COORDINATEANALYTICFORMULARANGEY, TEXTSIZE, "-m #upoint #frac{1}{100#upoint2} = #frac{z#upointsin#left(#gamma#right)-y#upointcos#left(#gamma#right)}{#left(y#upointsin#left(#gamma#right)+z#upointcos#left(#gamma#right)#right)^{2}}");
		actualText->Draw();
		actualText = addText(0.5 * FUNCTIONTEXTXPOSITION * COORDINATEANALYTICFORMULARANGEZ, FUNCTIONTEXTYPOSITION * COORDINATEANALYTICFORMULARANGEY - 2 * 4.1 * TEXTSIZE * COORDINATEANALYTICFORMULARANGEY, TEXTSIZE, "y_{1/2} = - #frac{p}{2}  #splitline{+}{-}  #sqrt{#left(#frac{p}{2}#right)^{2} - q}");
		actualText->Draw();
		actualText = addText(0.5 * FUNCTIONTEXTXPOSITION * COORDINATEANALYTICFORMULARANGEZ, -FUNCTIONTEXTYPOSITION * COORDINATEANALYTICFORMULARANGEY + 2 * 4.1 * TEXTSIZE * COORDINATEANALYTICFORMULARANGEY, TEXTSIZE, "#frac{p}{2} = #frac{cos#left(#gamma#right)}{2#upointsin#left(#gamma#right)} #upoint #left(z + #frac{100#upoint2}{-m#upointsin#left(#gamma#right)}#right)");
		actualText->Draw();
		actualText = addText(0.5 * FUNCTIONTEXTXPOSITION * COORDINATEANALYTICFORMULARANGEZ, -FUNCTIONTEXTYPOSITION * COORDINATEANALYTICFORMULARANGEY, TEXTSIZE, "q = #frac{z}{sin#left(#gamma#right)} #upoint #left(#frac{z#upointcos^{2}#left(#gamma#right)}{sin#left(#gamma#right)} - #frac{100#upoint2}{-m}#right)");
		actualText->Draw();

	}

	if (arrayZ != NULL) {
		delete arrayZ;
		arrayZ = NULL;
	}
	if (arrayY != NULL) {
		delete arrayY;
		arrayY = NULL;
	}

}
void houghPictures::createCoordinateAnalyticFormulaLut() {

	double*         arrayZ;
	double*         arrayX;
	analyticFormula formula;
	TMarker*        actualMarker;
	TArrow*         actualArrow;
	TLatex*         actualText;

	arrayZ = new double[QUANTIZATION];
	if (arrayZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayZ[i] = (double)i * ((double)(COORDINATEANALYTICFORMULARANGEZ) / (double)(QUANTIZATION-1));
		arrayX[i] = (double)formula.evaluateFormulaLut(COORDINATEANALYTICFORMULAQ, COORDINATEANALYTICFORMULAPX, COORDINATEANALYTICFORMULAPZ, arrayZ[i], COORDINATEANALYTICFORMULAB);

	}

	if (coordinateAnalyticFormulaLutWindow == NULL) {

		coordinateAnalyticFormulaLutWindow    = addWindow("CoordinateLutAnalyticFormula", "Coordinate Space for the lut of the analyticFormula", -( 2 * RELATIVEDISTANCE * COORDINATEANALYTICFORMULARANGEZ), COORDINATEANALYTICFORMULARANGEZ, -COORDINATEANALYTICFORMULARANGEX, COORDINATEANALYTICFORMULARANGEX, WINDOWSIZE, WINDOWSIZE);

		coordinateAnalyticFormulaLutDisplay   = addDisplay(QUANTIZATION, arrayZ, arrayX, -(2 * RELATIVEDISTANCE * COORDINATEANALYTICFORMULARANGEZ), COORDINATEANALYTICFORMULARANGEZ, -COORDINATEANALYTICFORMULARANGEX, COORDINATEANALYTICFORMULARANGEX, 1, "z", "x");

		coordinateAnalyticFormulaLutDisplay->SetName("CoordinateLutAnalyticFormula");
		coordinateAnalyticFormulaLutDisplay->SetTitle("Coordinate Space for the lut of the analyticFormula");
		coordinateAnalyticFormulaLutDisplay->GetXaxis()->SetTitle("z");
		coordinateAnalyticFormulaLutDisplay->GetYaxis()->SetTitle("x");

		coordinateAnalyticFormulaLutDisplay->Draw("C");

		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			/* draw the transforming points */
			actualMarker = addMarker(arrayZ[COORDINATEPOINTS[j]], arrayX[COORDINATEPOINTS[j]], MARKERTYPE);
			actualMarker->Draw();

			/* draw the detector planes */
			actualArrow = addArrow(arrayZ[COORDINATEPOINTS[j]], -arrayZ[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]],"");
			actualArrow->Draw();

		}

		/* draw the function description */
		actualText = addText(0.5 * FUNCTIONTEXTXPOSITION * COORDINATEANALYTICFORMULARANGEZ, FUNCTIONTEXTYPOSITION * COORDINATEANALYTICFORMULARANGEX, TEXTSIZE, "- #frac{q}{p_{xz}} #upoint #frac{0.3}{1000} #upoint #frac{B}{2} #upoint #left(x#upointsin#left(#theta#right)+z#upointcos#left(#theta#right)#right)^{2}-z#upointsin#left(#theta#right)+x#upointcos#left(#theta#right)=0");
		actualText->Draw();
		actualText = addText(0.5 * FUNCTIONTEXTXPOSITION * COORDINATEANALYTICFORMULARANGEZ, FUNCTIONTEXTYPOSITION * COORDINATEANALYTICFORMULARANGEX - 2 * 4.1 * TEXTSIZE * COORDINATEANALYTICFORMULARANGEX, TEXTSIZE, "x_{1/2} = - #frac{p}{2}  #splitline{+}{-}  #sqrt{#left(#frac{p}{2}#right)^{2} - q}");
		actualText->Draw();
		actualText = addText(0.5 * FUNCTIONTEXTXPOSITION * COORDINATEANALYTICFORMULARANGEZ, -FUNCTIONTEXTYPOSITION * COORDINATEANALYTICFORMULARANGEX + 2 * 4.1 * TEXTSIZE * COORDINATEANALYTICFORMULARANGEX, TEXTSIZE, "#frac{p}{2} = #frac{cos#left(#theta#right)}{2#upointsin#left(#theta#right)} #upoint #left(z - #frac{1000#upoint2}{0.3#upointB#upoint#frac{q}{p_{xz}}#upointsin#left(#theta#right)}#right)");
		actualText->Draw();
		actualText = addText(0.5 * FUNCTIONTEXTXPOSITION * COORDINATEANALYTICFORMULARANGEZ, -FUNCTIONTEXTYPOSITION * COORDINATEANALYTICFORMULARANGEX, TEXTSIZE, "q = #frac{z}{sin#left(#theta#right)} #upoint #left(#frac{z#upointcos^{2}#left(#theta#right)}{sin#left(#theta#right)} + #frac{1000#upoint2}{0.3#upointB#upoint#frac{q}{p_{xz}}}#right)");
		actualText->Draw();

	}

	if (arrayZ != NULL) {
		delete arrayZ;
		arrayZ = NULL;
	}
	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}

}

/****************************************************************
 * This method creates the analyticFormula object in the Hough	*
 * space.														*
 ****************************************************************/

void houghPictures::createHoughAnalyticFormulaPrelut() {

	double*         arrayZ;
	double*         arrayY;
	double*         arrayGamma;
	double*         arrayM;
	analyticFormula formula;
	double          houghRangeGamma;
	double          houghRangeM;
	char            buffer[intConversionDigits + 1];
	std::string     displayName;
	TMarker*        actualMarker;
	TLatex*         actualText;

	arrayZ = new double[QUANTIZATION];
	if (arrayZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayY = new double[QUANTIZATION];
	if (arrayY == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayGamma = new double[QUANTIZATION];
	if (arrayGamma == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayM = new double[QUANTIZATION];
	if (arrayM == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	if (HOUGHANALYTICFORMULARANGEGAMMA < 0)
		houghRangeGamma = - HOUGHANALYTICFORMULARANGEGAMMA;
	else
		houghRangeGamma = HOUGHANALYTICFORMULARANGEGAMMA;

	if (HOUGHANALYTICFORMULARANGEM < 0)
		houghRangeM = - HOUGHANALYTICFORMULARANGEM;
	else
		houghRangeM = HOUGHANALYTICFORMULARANGEM;

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayZ[i] = (double)i * ((double)(COORDINATEANALYTICFORMULARANGEZ) / (double)(QUANTIZATION-1));
		arrayY[i] = (double)formula.evaluateFormulaPrelut(COORDINATEANALYTICFORMULAM, COORDINATEANALYTICFORMULAPY, COORDINATEANALYTICFORMULAPZ, arrayZ[i]);

	}

	if (houghAnalyticFormulaPrelutWindow == NULL) {

		houghAnalyticFormulaPrelutWindow = addWindow("HoughPrelutAnalyticFormula", "Hough Space for the prelut of the analyticFormula", -houghRangeGamma, houghRangeGamma, -houghRangeM, houghRangeM, WINDOWSIZE, WINDOWSIZE);

		houghAnalyticFormulaPrelutDisplay = (TGraph**)calloc(NUMBEROFCOORDINATEPOINTS, sizeof(TGraph*));

		for (unsigned int j = STARTHOUGHCOORDINATEPOINT; j < STOPHOUGHCOORDINATEPOINT; j++) {

			for (unsigned int k = 0; k < QUANTIZATION; k++) {

				arrayGamma[k] = (double)k * ((double)(2 * houghRangeGamma) / (double)(QUANTIZATION-1)) - (double)houghRangeGamma;
				arrayM[k]     = (double)formula.evaluatePrelut(arrayY[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]], arrayGamma[k]);

			}

			houghAnalyticFormulaPrelutDisplay[j]  = addDisplay(QUANTIZATION, arrayGamma, arrayM, -houghRangeGamma, houghRangeGamma, -houghRangeM, houghRangeM, 1.5, "#gamma", "-m");

			displayName                           = "HoughPrelutAnalyticFormula_TGraph_";
			displayName                          += uitos(j, buffer, 10, intConversionDigits);
			houghAnalyticFormulaPrelutDisplay[j]->SetName(displayName.c_str());
			houghAnalyticFormulaPrelutDisplay[j]->SetTitle("Hough Space for the prelut of the analyticFormula");
			houghAnalyticFormulaPrelutDisplay[j]->GetXaxis()->SetTitle("gamma");
			houghAnalyticFormulaPrelutDisplay[j]->GetYaxis()->SetTitle("-m");

			houghAnalyticFormulaPrelutDisplay[j]->Draw("SAME C *");

		}

		/* draw the point of the found curve */
		actualMarker = addMarker(atan(COORDINATEANALYTICFORMULAPY / COORDINATEANALYTICFORMULAPZ), COORDINATEANALYTICFORMULAM, MARKERTYPE);
		actualMarker->Draw();

		/* draw the function description */
		actualText = addText(-HOUGHANALYTICFORMULARANGEGAMMA + 0.5 * FUNCTIONTEXTXPOSITION * HOUGHANALYTICFORMULARANGEGAMMA, FUNCTIONTEXTYPOSITION * HOUGHANALYTICFORMULARANGEM - 2 * 2.1 * TEXTSIZE * HOUGHANALYTICFORMULARANGEM, TEXTSIZE, "-m = 100#upoint2 #upoint #frac{z#upointsin#left(#theta#right)-y#upointcos#left(#theta#right)}{#left(y#upointsin#left(#theta#right)+z#upointcos#left(#theta#right)#right)^{2}}");
		actualText->Draw();

	}

	if (arrayZ != NULL) {
		delete arrayZ;
		arrayZ = NULL;
	}
	if (arrayY != NULL) {
		delete arrayY;
		arrayY = NULL;
	}
	if (arrayGamma != NULL) {
		delete arrayGamma;
		arrayGamma = NULL;
	}
	if (arrayM != NULL) {
		delete arrayM;
		arrayM = NULL;
	}

}
void houghPictures::createHoughAnalyticFormulaLut() {

	double*         arrayZ;
	double*         arrayX;
	double*         arrayTheta;
	double*         arrayQPxz;
	analyticFormula formula;
	double          houghRangeTheta;
	double          houghRangeQPxz;
	char            buffer[intConversionDigits + 1];
	std::string     displayName;
	TMarker*        actualMarker;
	TLatex*         actualText;

	arrayZ = new double[QUANTIZATION];
	if (arrayZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayX = new double[QUANTIZATION];
	if (arrayX == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayTheta = new double[QUANTIZATION];
	if (arrayTheta == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	arrayQPxz = new double[QUANTIZATION];
	if (arrayQPxz == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	if (HOUGHANALYTICFORMULARANGETHETA < 0)
		houghRangeTheta = - HOUGHANALYTICFORMULARANGETHETA;
	else
		houghRangeTheta = HOUGHANALYTICFORMULARANGETHETA;

	if (HOUGHANALYTICFORMULARANGEQPXZ < 0)
		houghRangeQPxz = - HOUGHANALYTICFORMULARANGEQPXZ;
	else
		houghRangeQPxz = HOUGHANALYTICFORMULARANGEQPXZ;

	for (unsigned int i = 0; i < QUANTIZATION; i++) {

		arrayZ[i] = (double)i * ((double)(COORDINATEANALYTICFORMULARANGEZ) / (double)(QUANTIZATION-1));
		arrayX[i] = (double)formula.evaluateFormulaLut(COORDINATEANALYTICFORMULAQ, COORDINATEANALYTICFORMULAPX, COORDINATEANALYTICFORMULAPZ, arrayZ[i], HOUGHANALYTICFORMULAB);

	}

	if (houghAnalyticFormulaLutWindow == NULL) {

		houghAnalyticFormulaLutWindow = addWindow("HoughLutAnalyticFormula", "Hough Space for the lut of the analyticFormula", -houghRangeTheta, houghRangeTheta, -houghRangeQPxz, houghRangeQPxz, WINDOWSIZE, WINDOWSIZE);

		houghAnalyticFormulaLutDisplay = (TGraph**)calloc(NUMBEROFCOORDINATEPOINTS, sizeof(TGraph*));

		for (unsigned int j = 0; j < NUMBEROFCOORDINATEPOINTS; j++) {

			for (unsigned int k = 0; k < QUANTIZATION; k++) {

				arrayTheta[k] = (double)k * ((double)(2 * houghRangeTheta) / (double)(QUANTIZATION-1)) - (double)houghRangeTheta;
				arrayQPxz[k]  = (double)formula.evaluateLut(arrayX[COORDINATEPOINTS[j]], arrayZ[COORDINATEPOINTS[j]], arrayTheta[k], HOUGHANALYTICFORMULAB);

			}

			houghAnalyticFormulaLutDisplay[j]  = addDisplay(QUANTIZATION, arrayTheta, arrayQPxz, -houghRangeTheta, houghRangeTheta, -houghRangeQPxz, houghRangeQPxz, 2, "#theta", "- #frac{q}{p_{xz}}");

			displayName                     = "HoughLutAnalyticFormula_TGraph_";
			displayName                    += uitos(j, buffer, 10, intConversionDigits);
			houghAnalyticFormulaLutDisplay[j]->SetName(displayName.c_str());
			houghAnalyticFormulaLutDisplay[j]->SetTitle("Hough Space for the lut of the analyticFormula");
			houghAnalyticFormulaLutDisplay[j]->GetXaxis()->SetTitle("theta");
			houghAnalyticFormulaLutDisplay[j]->GetYaxis()->SetTitle("-q/p_xz");

			houghAnalyticFormulaLutDisplay[j]->Draw("SAME C");

		}

		/* draw the point of the found curve */
		actualMarker = addMarker(atan(COORDINATEANALYTICFORMULAPX / COORDINATEANALYTICFORMULAPZ), -COORDINATEANALYTICFORMULAQ / sqrt(sqr(COORDINATEANALYTICFORMULAPX) + sqr(COORDINATEANALYTICFORMULAPZ)), MARKERTYPE);
		actualMarker->Draw();

		/* draw the function description */
		actualText = addText(-HOUGHANALYTICFORMULARANGETHETA + 0.5 * FUNCTIONTEXTXPOSITION * HOUGHANALYTICFORMULARANGETHETA, FUNCTIONTEXTYPOSITION * HOUGHANALYTICFORMULARANGEQPXZ - 2 * 2.1 * TEXTSIZE * HOUGHANALYTICFORMULARANGEQPXZ, TEXTSIZE, "- #frac{q}{p_{xz}} = #frac{1000}{0.3} #frac{2#left(z#upointsin#left(#theta#right)#right)-x#upointcos#left(#theta#right)}{B#upoint#left(x#upointsin#left(#theta#right)+z#upointcos#left(#theta#right)#right)^{2}}");
		actualText->Draw();

	}

	if (arrayZ != NULL) {
		delete arrayZ;
		arrayZ = NULL;
	}
	if (arrayX != NULL) {
		delete arrayX;
		arrayX = NULL;
	}
	if (arrayTheta != NULL) {
		delete arrayTheta;
		arrayTheta = NULL;
	}
	if (arrayQPxz != NULL) {
		delete arrayQPxz;
		arrayQPxz = NULL;
	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghPictures::houghPictures() 
 : window(NULL),
   numberOfWindows(0),
   display(NULL),
   numberOfDisplays(0),
   marker(NULL),
   numberOfMarkers(0),
   arrow(NULL),
   numberOfArrows(0),
   ellipse(NULL),
   numberOfEllipses(0),
   text(NULL),
   numberOfTexts(0),
   coordinateStraightLineWindow(NULL),
   coordinateStraightLineDisplay(NULL),
   houghStraightLineWindow(NULL),
   houghStraightLineDisplay(NULL),
   coordinateHesseLineWindow(NULL),
   coordinateHesseLineDisplay(NULL),
   houghPhiHesseLineWindow(NULL),
   houghPhiHesseLineDisplay(NULL),
   houghThetaHesseLineWindow(NULL),
   houghThetaHesseLineDisplay(NULL),
   oneCoordinateHitAnalyticFormulaWindow(NULL),
   oneCoordinateHitAnalyticFormulaDisplay(NULL),
   oneHoughCurveAnalyticFormulaWindow(NULL),
   oneHoughCurveAnalyticFormulaDisplay(NULL),
   oneCoordinateTrackAnalyticFormulaWindow(NULL),
   oneCoordinateTrackAnalyticFormulaDisplay(NULL),
   oneHoughPointAnalyticFormulaWindow(NULL),
   oneHoughPointAnalyticFormulaDisplay(NULL),
   coordinateAnalyticFormulaPrelutWindow(NULL),
   coordinateAnalyticFormulaPrelutDisplay(NULL),
   coordinateAnalyticFormulaLutWindow(NULL),
   coordinateAnalyticFormulaLutDisplay(NULL),
   houghAnalyticFormulaPrelutWindow(NULL),
   houghAnalyticFormulaPrelutDisplay(NULL),
   houghAnalyticFormulaLutWindow(NULL),
   houghAnalyticFormulaLutDisplay(NULL)
{
  /*
	window                                   = NULL;
	numberOfWindows                          = 0;
	display                                  = NULL;
	numberOfDisplays                         = 0;
	marker                                   = NULL;
	numberOfMarkers                          = 0;
	arrow                                    = NULL;
	numberOfArrows                           = 0;
	ellipse                                  = NULL;
	numberOfEllipses                         = 0;
	text                                     = NULL;
	numberOfTexts                            = 0;
	coordinateStraightLineWindow             = NULL;
	coordinateStraightLineDisplay            = NULL;
	houghStraightLineWindow                  = NULL;
	houghStraightLineDisplay                 = NULL;
	coordinateHesseLineWindow                = NULL;
	coordinateHesseLineDisplay               = NULL;
	houghPhiHesseLineWindow                  = NULL;
	houghPhiHesseLineDisplay                 = NULL;
	houghThetaHesseLineWindow                = NULL;
	houghThetaHesseLineDisplay               = NULL;
	oneCoordinateHitAnalyticFormulaWindow    = NULL;
	oneCoordinateHitAnalyticFormulaDisplay   = NULL;
	oneHoughCurveAnalyticFormulaWindow       = NULL;
	oneHoughCurveAnalyticFormulaDisplay      = NULL;
	oneCoordinateTrackAnalyticFormulaWindow  = NULL;
	oneCoordinateTrackAnalyticFormulaDisplay = NULL;
	oneHoughPointAnalyticFormulaWindow       = NULL;
	oneHoughPointAnalyticFormulaDisplay      = NULL;
	coordinateAnalyticFormulaPrelutWindow    = NULL;
	coordinateAnalyticFormulaPrelutDisplay   = NULL;
	coordinateAnalyticFormulaLutWindow       = NULL;
	coordinateAnalyticFormulaLutDisplay      = NULL;
	houghAnalyticFormulaPrelutWindow         = NULL;
	houghAnalyticFormulaPrelutDisplay        = NULL;
	houghAnalyticFormulaLutWindow            = NULL;
	houghAnalyticFormulaLutDisplay           = NULL;
  */
}

/* **************************************************************
 * Destructor													*
 ****************************************************************/

houghPictures::~houghPictures() {

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

	if (marker != NULL) {

		for (unsigned short i = 0; i < numberOfMarkers; i++) {

			if (marker[i] != NULL) {

				delete marker[i];
				marker[i] = NULL;

			}

		}

		delete marker;
		marker = NULL;

	}

	if (arrow != NULL) {

		for (unsigned short i = 0; i < numberOfArrows; i++) {

			if (arrow[i] != NULL) {

				delete arrow[i];
				arrow[i] = NULL;

			}

		}

		delete arrow;
		arrow = NULL;

	}

	if (ellipse != NULL) {

		for (unsigned short i = 0; i < numberOfEllipses; i++) {

			if (ellipse[i] != NULL) {

				delete ellipse[i];
				ellipse[i] = NULL;

			}

		}

		delete ellipse;
		ellipse = NULL;

	}

	if (text != NULL) {

		for (unsigned short i = 0; i < numberOfTexts; i++) {

			if (text[i] != NULL) {

				delete text[i];
				text[i] = NULL;

			}

		}

		delete text;
		text = NULL;

	}

	if (houghStraightLineDisplay != NULL) {
	
		free(houghStraightLineDisplay);
		houghStraightLineDisplay = NULL;
	
	}

	if (houghPhiHesseLineDisplay != NULL) {
	
		free(houghPhiHesseLineDisplay);
		houghPhiHesseLineDisplay = NULL;
	
	}

	if (houghThetaHesseLineDisplay != NULL) {
	
		free(houghThetaHesseLineDisplay);
		houghThetaHesseLineDisplay = NULL;
	
	}

	if (houghAnalyticFormulaPrelutDisplay != NULL) {
	
		free(houghAnalyticFormulaPrelutDisplay);
		houghAnalyticFormulaPrelutDisplay = NULL;
	
	}

	if (houghAnalyticFormulaLutDisplay != NULL) {
	
		free(houghAnalyticFormulaLutDisplay);
		houghAnalyticFormulaLutDisplay = NULL;
	
	}

}

/* **************************************************************
 * This method resets the whole object.							*
 ****************************************************************/

void houghPictures::draw() {

	/* Draw the coordinate and the Hough pictures for the straight line */
	createCoordinateStraightLine();
	createHoughStraightLine();
	createCoordinateHesseLine();
	createHoughPhiHesseLine();
	createHoughThetaHesseLine();

	/* Draw the coordinate and the Hough pictures for the analyticFormula object */
	createOneCoordinateHitAnalyticFormula();
	createOneHoughCurveAnalyticFormula();
	createOneCoordinateTrackAnalyticFormula();
	createOneHoughPointAnalyticFormula();
	createCoordinateAnalyticFormulaPrelut();
	createCoordinateAnalyticFormulaLut();
	createHoughAnalyticFormulaPrelut();
	createHoughAnalyticFormulaLut();

}
