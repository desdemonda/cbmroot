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
// $Date: 2008-08-14 12:29:24 $
// $Revision: 1.4 $
//
// *******************************************************************/


#ifndef _HOUGHPICTURES_H
#define _HOUGHPICTURES_H


#include "TCanvas.h"
#include "TGraph.h"
#include "TMarker.h"
#include "TArrow.h"
#include "TEllipse.h"
#include "TLatex.h"


/* **************************************************************
 * CLASS houghPictures					 						*
 * **************************************************************/

class houghPictures {

 private:
  houghPictures(const houghPictures&);
  houghPictures& operator=(const houghPictures&);

protected:

	TCanvas**      window;
	unsigned short numberOfWindows;
	TGraph**       display;
	unsigned short numberOfDisplays;
	TMarker**      marker;
	unsigned short numberOfMarkers;
	TArrow**       arrow;
	unsigned short numberOfArrows;
	TEllipse**     ellipse;
	unsigned short numberOfEllipses;
	TLatex**       text;
	unsigned short numberOfTexts;
	TCanvas*       coordinateStraightLineWindow;
	TGraph*        coordinateStraightLineDisplay;
	TCanvas*       houghStraightLineWindow;
	TGraph**       houghStraightLineDisplay;
	TCanvas*       coordinateHesseLineWindow;
	TGraph*        coordinateHesseLineDisplay;
	TCanvas*       houghPhiHesseLineWindow;
	TGraph**       houghPhiHesseLineDisplay;
	TCanvas*       houghThetaHesseLineWindow;
	TGraph**       houghThetaHesseLineDisplay;
	TCanvas*       oneCoordinateHitAnalyticFormulaWindow;
	TGraph*        oneCoordinateHitAnalyticFormulaDisplay;
	TCanvas*       oneHoughCurveAnalyticFormulaWindow;
	TGraph*        oneHoughCurveAnalyticFormulaDisplay;
	TCanvas*       oneCoordinateTrackAnalyticFormulaWindow;
	TGraph*        oneCoordinateTrackAnalyticFormulaDisplay;
	TCanvas*       oneHoughPointAnalyticFormulaWindow;
	TGraph*        oneHoughPointAnalyticFormulaDisplay;
	TCanvas*       coordinateAnalyticFormulaPrelutWindow;
	TGraph*        coordinateAnalyticFormulaPrelutDisplay;
	TCanvas*       coordinateAnalyticFormulaLutWindow;
	TGraph*        coordinateAnalyticFormulaLutDisplay;
	TCanvas*       houghAnalyticFormulaPrelutWindow;
	TGraph**       houghAnalyticFormulaPrelutDisplay;
	TCanvas*       houghAnalyticFormulaLutWindow;
	TGraph**       houghAnalyticFormulaLutDisplay;

/**
 * This method initializes the global style for each display.
 */

	void initWindowStyle();

/**
 * This method initializes the window with a name and a title.
 */

	TCanvas* addWindow(std::string name, std::string title, double xRangeStart, double xRangeStop, double yRangeStart, double yRangeStop, int xSize, int ySize);

/**
 * This method initializes the display.
 */

	TGraph* addDisplay(int numberOfEntries, double* arrayX, double* arrayY, double xRangeStart, double xRangeStop, double yRangeStart, double yRangeStop, double rangeFactor, std::string xAxiTitle, std::string yAxiTitle);

/**
 * This method initializes a marker.
 */

	TMarker* addMarker(double xPos, double yPos, int type);

/**
 * This method initializes an arrow.
 */

	TArrow* addArrow(double xPosStart, double yPosStart, double xPosStop, double yPosStop, std::string option);

/**
 * This method initializes an ellipse.
 */

	TEllipse* addEllipse(double xCenter, double yCenter, double xRadius, double yRadius, double angleMin, double angleMax, double rotationAngle);

/*
 * This method initializes a TLatex.
 */

	TLatex* addText(double xPos, double yPos, double textSize, std::string value);

/**
 * This method creates the straight line in the coordinate space.
 */

	void createCoordinateStraightLine();

/**
 * This method creates the straight line in the Hough space.
 */

	void createHoughStraightLine();

/**
 * This method creates the Hesse line in the coordinate space.
 */

	void createCoordinateHesseLine();

/**
 * This method creates the Hesse line in the Hough space in respect to phi.
 */

	void createHoughPhiHesseLine();

/**
 * This method creates the Hesse line in the Hough space in respect to theta.
 */

	void createHoughThetaHesseLine();

/**
 * This method creates the analyticFormula object in the
 * coordinate space with just one coordinate.
 */

	void createOneCoordinateHitAnalyticFormula();

/**
 * This method creates the analyticFormula object in the
 * Hough space with just one curve.
 */

	void createOneHoughCurveAnalyticFormula();

/**
 * This method creates the analyticFormula object in the
 * coordinate space with a whole track.
 */

	void createOneCoordinateTrackAnalyticFormula();

/**
 * This method creates the analyticFormula object in the
 * Hough space with just the intersection point.
 */

	void createOneHoughPointAnalyticFormula();

/**
 * This method creates the analyticFormula object in the
 * coordinate space.
 */

	void createCoordinateAnalyticFormulaPrelut();
	void createCoordinateAnalyticFormulaLut();

/**
 * This method creates the analyticFormula object in the Hough space.
 */

	void createHoughAnalyticFormulaPrelut();
	void createHoughAnalyticFormulaLut();

public:

/**
 * Default constructor
 */

	houghPictures();

/**
 * Destructor
 */

	virtual ~houghPictures();

/**
 * This method resets the whole object.
 */

	void draw();

};

#endif
