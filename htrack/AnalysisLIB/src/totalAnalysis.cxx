/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl�
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
//     - implements methods to show a total analysis summary
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-06 14:18:42 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../include/analysisWarningMsg.h"
#include "../include/totalAnalysis.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

totalAnalysis::totalAnalysis() 
  : numberOfCorrections(0),			
    numberOfCoordCorrections(0),			
    numberOfTracksWhichCannotBeFound(0),		
    numberOfTracksWithGoodPrelutSignature(0),	
    numberOfTracksWithGoodLutSignature(0),
    prelutSignatures(),		
    lutSignatures()				
{

	reset();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

totalAnalysis::~totalAnalysis() {

	reset();

}

/****************************************************************
 * This method resets the whole object.							*
 ****************************************************************/

void totalAnalysis::reset() {

	resetCorrectionCounter();
	resetNumberOfTracksWhichCannotBeFound();
	resetNumberOfTracksWithGoodPrelutSignature();
	resetNumberOfTracksWithGoodLutSignature();
	resetGoodPrelutSignatures();
	resetGoodLutSignatures();

}

/****************************************************************
 * This method returns the number of corrections.				*
 ****************************************************************/

unsigned long totalAnalysis::getNumberOfCorrections() {

	return numberOfCorrections;

}

/****************************************************************
 * This method returns the number of coord corrections.			*
 ****************************************************************/

unsigned long totalAnalysis::getNumberOfCoordCorrections() {

	return numberOfCoordCorrections;

}

/****************************************************************
 * This method returns the number of tracks which cannot be		*
 * found.														*
 ****************************************************************/

unsigned long totalAnalysis::getNumberOfTracksWhichCannotBeFound() {

	return numberOfTracksWhichCannotBeFound;

}

/****************************************************************
 * This method returns the number of tracks which have a		*
 * good prelut signature.										*
 ****************************************************************/

unsigned long totalAnalysis::getNumberOfTracksWithGoodPrelutSignature() {

	return numberOfTracksWithGoodPrelutSignature;

}

/****************************************************************
 * This method returns the number of tracks which have a		*
 * good lut signature.											*
 ****************************************************************/

unsigned long totalAnalysis::getNumberOfTracksWithGoodLutSignature() {

	return numberOfTracksWithGoodLutSignature;

}

/****************************************************************
 * This method returns the number of							*
 * good prelut signatures.										*
 ****************************************************************/

unsigned long totalAnalysis::getNumberOfGoodPrelutSignatures() {

	return prelutSignatures.getNumberOfEntries();

}

/****************************************************************
 * This method returns the table containing the					*
 * good prelut signatures.										*
 ****************************************************************/

table& totalAnalysis::getGoodPrelutSignatureTable() {

	return prelutSignatures;

}

/****************************************************************
 * This method returns the number of							*
 * good lut signatures.											*
 ****************************************************************/

unsigned long totalAnalysis::getNumberOfGoodLutSignatures() {

	return lutSignatures.getNumberOfEntries();

}

/****************************************************************
 * This method returns the table containing the					*
 * good lut signatures.											*
 ****************************************************************/

table& totalAnalysis::getGoodLutSignatureTable() {

	return lutSignatures;

}

/****************************************************************
 * This method sets the number of corrections.					*
 ****************************************************************/

void totalAnalysis::setNumberOfCorrections(unsigned long value) {

	numberOfCorrections = value;

}

/****************************************************************
 * This method sets the number of coord corrections.			*
 ****************************************************************/

void totalAnalysis::setNumberOfCoordCorrections(unsigned long value) {

	numberOfCoordCorrections = value;

}

/****************************************************************
 * This method sets the number of tracks which cannot be found.	*
 ****************************************************************/

void totalAnalysis::setNumberOfTracksWhichCannotBeFound(unsigned long value) {

	numberOfTracksWhichCannotBeFound = value;

}

/****************************************************************
 * This method sets the number of tracks which have a			*
 * good prelut signature.										*
 ****************************************************************/

void totalAnalysis::setNumberOfTracksWithGoodPrelutSignature(unsigned long value) {

	numberOfTracksWithGoodPrelutSignature = value;

}

/****************************************************************
 * This method sets the number of tracks which have a			*
 * good lut signature.											*
 ****************************************************************/

void totalAnalysis::setNumberOfTracksWithGoodLutSignature(unsigned long value) {

	numberOfTracksWithGoodLutSignature = value;

}

/****************************************************************
 * This method adds the number of corrections.					*
 ****************************************************************/

void totalAnalysis::addNumberOfCorrections(unsigned long value) {

	numberOfCorrections += value;

}

/****************************************************************
 * This method adds the number of coord corrections.			*
 ****************************************************************/

void totalAnalysis::addNumberOfCoordCorrections(unsigned long value) {

	numberOfCoordCorrections += value;

}

/****************************************************************
 * This method adds the number of tracks which cannot be found.	*
 ****************************************************************/

void totalAnalysis::addNumberOfTracksWhichCannotBeFound(unsigned long value) {

	numberOfTracksWhichCannotBeFound += value;

}

/****************************************************************
 * This method adds the number of tracks which have a			*
 * good prelut signature.										*
 ****************************************************************/

void totalAnalysis::addNumberOfTracksWithGoodPrelutSignature(unsigned long value) {

	numberOfTracksWithGoodPrelutSignature += value;

}

/****************************************************************
 * This method adds the number of tracks which have a			*
 * good lut signature.											*
 ****************************************************************/

void totalAnalysis::addNumberOfTracksWithGoodLutSignature(unsigned long value) {

	numberOfTracksWithGoodLutSignature += value;

}

/****************************************************************
 * This method adds a good prelut signature.					*
 ****************************************************************/

void totalAnalysis::addGoodPrelutSignature(bitArray signature, bitArray classification) {

	prelutSignatures.sumEntry(signature, classification);

}

/****************************************************************
 * This method adds a good lut signature.						*
 ****************************************************************/

void totalAnalysis::addGoodLutSignature(bitArray signature, bitArray classification) {

	lutSignatures.sumEntry(signature, classification);

}

/****************************************************************
 * This method resets both correction counters.					*
 ****************************************************************/

void totalAnalysis::resetCorrectionCounter() {

	numberOfCorrections      = 0;
	numberOfCoordCorrections = 0;

}

/****************************************************************
 * This method resets the number of tracks which cannot be found*
 ****************************************************************/

void totalAnalysis::resetNumberOfTracksWhichCannotBeFound() {

	numberOfTracksWhichCannotBeFound = 0;

}

/****************************************************************
 * This method resets the number of tracks which have a			*
 * good prelut signature.										*
 ****************************************************************/

void totalAnalysis::resetNumberOfTracksWithGoodPrelutSignature() {

	numberOfTracksWithGoodPrelutSignature = 0;

}

/****************************************************************
 * This method resets the number of tracks which have a			*
 * good lut signature.											*
 ****************************************************************/

void totalAnalysis::resetNumberOfTracksWithGoodLutSignature() {

	numberOfTracksWithGoodLutSignature = 0;

}

/****************************************************************
 * This method resets the good prelut signatures.				*
 ****************************************************************/

void totalAnalysis::resetGoodPrelutSignatures() {

	prelutSignatures.clear();

}

/****************************************************************
 * This method resets the good lut signatures.					*
 ****************************************************************/

void totalAnalysis::resetGoodLutSignatures() {

	lutSignatures.clear();

}

/****************************************************************
 * This method prints both correction counters.					*
 ****************************************************************/

void totalAnalysis::printCorrectionCounter() {

	houghLutCorrectionWarningMsg* houghLutCorrection = new houghLutCorrectionWarningMsg("ANALYSISLIB", numberOfCorrections, numberOfCoordCorrections);
	houghLutCorrection->warningMsg();
	if(houghLutCorrection != NULL) {
		delete houghLutCorrection;
		houghLutCorrection = NULL;
	}

}

/****************************************************************
 * This method prints the number of tracks which cannot be found*
 ****************************************************************/

void totalAnalysis::printNumberOfTracksWhichCannotBeFound() {

	numberOfTracksWhichCannotBeFoundWarningMsg* _numberOfTracksWhichCannotBeFound = new numberOfTracksWhichCannotBeFoundWarningMsg(this->numberOfTracksWhichCannotBeFound);
	_numberOfTracksWhichCannotBeFound->warningMsg();
	if(_numberOfTracksWhichCannotBeFound != NULL) {
		delete _numberOfTracksWhichCannotBeFound;
		_numberOfTracksWhichCannotBeFound = NULL;
	}

}

/****************************************************************
 * This method prints the number of tracks which have a			*
 * good prelut signature.										*
 ****************************************************************/

void totalAnalysis::printNumberOfTracksWithGoodPrelutSignature() {

	numberOfTracksWithGoodPrelutSignatureWarningMsg* _numberOfTracksWithGoodPrelutSignature = new numberOfTracksWithGoodPrelutSignatureWarningMsg(this->numberOfTracksWithGoodPrelutSignature);
	_numberOfTracksWithGoodPrelutSignature->warningMsg();
	if(_numberOfTracksWithGoodPrelutSignature != NULL) {
		delete _numberOfTracksWithGoodPrelutSignature;
		_numberOfTracksWithGoodPrelutSignature = NULL;
	}

}

/****************************************************************
 * This method prints the number of tracks which have a			*
 * good lut signature.											*
 ****************************************************************/

void totalAnalysis::printNumberOfTracksWithGoodLutSignature() {

	numberOfTracksWithGoodLutSignatureWarningMsg* _numberOfTracksWithGoodLutSignature = new numberOfTracksWithGoodLutSignatureWarningMsg(this->numberOfTracksWithGoodLutSignature);
	_numberOfTracksWithGoodLutSignature->warningMsg();
	if(_numberOfTracksWithGoodLutSignature != NULL) {
		delete _numberOfTracksWithGoodLutSignature;
		_numberOfTracksWithGoodLutSignature = NULL;
	}

}

/****************************************************************
 * This method prints the number of good prelut signatures.		*
 ****************************************************************/

void totalAnalysis::printNumberOfGoodPrelutSignatures() {

	numberOfGoodPrelutSignatureWarningMsg* numberOfGoodPrelutSignature = new numberOfGoodPrelutSignatureWarningMsg(prelutSignatures.getNumberOfEntries());
	numberOfGoodPrelutSignature->warningMsg();
	if(numberOfGoodPrelutSignature != NULL) {
		delete numberOfGoodPrelutSignature;
		numberOfGoodPrelutSignature = NULL;
	}

}

/****************************************************************
 * This method prints the number of good lut signatures.		*
 ****************************************************************/

void totalAnalysis::printNumberOfGoodLutSignatures() {

	numberOfGoodLutSignatureWarningMsg* numberOfGoodLutSignature = new numberOfGoodLutSignatureWarningMsg(lutSignatures.getNumberOfEntries());
	numberOfGoodLutSignature->warningMsg();
	if(numberOfGoodLutSignature != NULL) {
		delete numberOfGoodLutSignature;
		numberOfGoodLutSignature = NULL;
	}

}

/****************************************************************
 * This method prints the good prelut signatures.				*
 ****************************************************************/

void totalAnalysis::printGoodPrelutSignatures() {

	goodPrelutSignatureWarningMsg* goodPrelutSignature = new goodPrelutSignatureWarningMsg(prelutSignatures.toString());
	goodPrelutSignature->warningMsg();
	if(goodPrelutSignature != NULL) {
		delete goodPrelutSignature;
		goodPrelutSignature = NULL;
	}

}

/****************************************************************
 * This method prints the good lut signatures.					*
 ****************************************************************/

void totalAnalysis::printGoodLutSignatures() {

	goodLutSignatureWarningMsg* goodLutSignature = new goodLutSignatureWarningMsg(lutSignatures.toString());
	goodLutSignature->warningMsg();
	if(goodLutSignature != NULL) {
		delete goodLutSignature;
		goodLutSignature = NULL;
	}

}
