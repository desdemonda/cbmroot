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
//     - base class for warnings occuring during input
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:17 $
// $Revision: 1.7 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/inputWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS inputWarningMsg						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputWarningMsg::inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputWarningMsg::~inputWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void inputWarningMsg::printMsg(std::string message) {

#ifndef NOINPUTWARNINGMESSAGE

	std::cerr << std::endl;
	
	std::cerr << INPUTLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}

/****************************************************************
 * CLASS noHitWarningMsg						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noHitWarningMsg::noHitWarningMsg() 
  : inputWarningMsg(),
    branchNameHit(),
    branchNamePoint()
{
  /*
	branchNameHit.clear();
	branchNamePoint.clear();
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noHitWarningMsg::noHitWarningMsg(std::string actualBranchNameHit, std::string actualBranchNamePoint) 
  : inputWarningMsg(),
    branchNameHit(actualBranchNameHit),
    branchNamePoint(actualBranchNamePoint)
{

  //	branchNameHit   = actualBranchNameHit;
  //	branchNamePoint = actualBranchNamePoint;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noHitWarningMsg::~noHitWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void noHitWarningMsg::warningMsg() {

	std::string temp;

	temp = "The object(s) '";
	temp += branchNameHit;
	temp += "', is(are) not found!!!\n";
	temp += "A standard hitProducer will be applied to the object '";
	temp += branchNamePoint;
	temp += "'!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS stationShapeNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

stationShapeNotFoundWarningMsg::stationShapeNotFoundWarningMsg() 
  : inputWarningMsg(),
    stationName()
{

  //	stationName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

stationShapeNotFoundWarningMsg::stationShapeNotFoundWarningMsg(std::string actualStationName) 
  : inputWarningMsg(),
    stationName(actualStationName)
{

  //	stationName = actualStationName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

stationShapeNotFoundWarningMsg::~stationShapeNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void stationShapeNotFoundWarningMsg::warningMsg() {

	std::string temp;

	temp = "The shape from the station '";
	temp += stationName;
	temp += "', is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS stationMaterialNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

stationMaterialNotFoundWarningMsg::stationMaterialNotFoundWarningMsg() 
  : inputWarningMsg(),
    stationName()
{

  //	stationName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

stationMaterialNotFoundWarningMsg::stationMaterialNotFoundWarningMsg(std::string actualStationName) 
  : inputWarningMsg(), 
    stationName(actualStationName)
{

  //	stationName = actualStationName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

stationMaterialNotFoundWarningMsg::~stationMaterialNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void stationMaterialNotFoundWarningMsg::warningMsg() {

	std::string temp;

	temp = "The material from the station '";
	temp += stationName;
	temp += "', is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS asciiFilesNotFoundWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

asciiFilesNotFoundWarningMsg::asciiFilesNotFoundWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

asciiFilesNotFoundWarningMsg::~asciiFilesNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void asciiFilesNotFoundWarningMsg::warningMsg() {

	printMsg("The standard ascii files to read the station geometry are not found!!!");

}


/****************************************************************
 * CLASS asciiFileNotFoundWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

asciiFileNotFoundWarningMsg::asciiFileNotFoundWarningMsg() 
  : inputWarningMsg(), 
    fileName()
{

  //	fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

asciiFileNotFoundWarningMsg::asciiFileNotFoundWarningMsg(std::string _fileName) 
  : inputWarningMsg(), 
    fileName(_fileName)
{

  //	this->fileName = _fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

asciiFileNotFoundWarningMsg::~asciiFileNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void asciiFileNotFoundWarningMsg::warningMsg() {

	std::string temp;

	temp  = "The ascii file '";
	temp += fileName;
	temp += "' to read the station geometry is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cbmrootEnvironmentVariableNotFoundWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cbmrootEnvironmentVariableNotFoundWarningMsg::cbmrootEnvironmentVariableNotFoundWarningMsg() 
  : inputWarningMsg(), 
    defaultValue()
{

  //	defaultValue.clear();

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cbmrootEnvironmentVariableNotFoundWarningMsg::cbmrootEnvironmentVariableNotFoundWarningMsg(std::string actualDefaultValue) 
  : inputWarningMsg(), 
    defaultValue(actualDefaultValue)
{

  //	defaultValue = actualDefaultValue;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cbmrootEnvironmentVariableNotFoundWarningMsg::~cbmrootEnvironmentVariableNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cbmrootEnvironmentVariableNotFoundWarningMsg::warningMsg() {


	std::string temp;

	temp  = "The CBMROOT environment variable is not found.\n";
	temp += "It is a path which is used to find the station's geometry definition.\n";
	temp += "Instead the default path '";
	temp += defaultValue;
	temp += "' is used!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS hitsFromFileReadWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitsFromFileReadWarningMsg::hitsFromFileReadWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitsFromFileReadWarningMsg::~hitsFromFileReadWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitsFromFileReadWarningMsg::warningMsg() {

	printMsg("The hits are read out of the file!!!");

}


/****************************************************************
 * CLASS hitsFromMemoryReadWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitsFromMemoryReadWarningMsg::hitsFromMemoryReadWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitsFromMemoryReadWarningMsg::~hitsFromMemoryReadWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitsFromMemoryReadWarningMsg::warningMsg() {

	printMsg("The hits are read out of the memory!!!");

}


/****************************************************************
 * CLASS hitWithWrongStationFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitWithWrongStationFoundWarningMsg::hitWithWrongStationFoundWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitWithWrongStationFoundWarningMsg::~hitWithWrongStationFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitWithWrongStationFoundWarningMsg::warningMsg() {

	printMsg("There is a hit with difference in type and station type!!!");

}


/****************************************************************
 * CLASS cannotGetRunAnaInstanceWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetRunAnaInstanceWarningMsg::cannotGetRunAnaInstanceWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetRunAnaInstanceWarningMsg::~cannotGetRunAnaInstanceWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetRunAnaInstanceWarningMsg::warningMsg() {

	printMsg("The instance of the FairRunAna object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotGetRuntimeDbWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetRuntimeDbWarningMsg::cannotGetRuntimeDbWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetRuntimeDbWarningMsg::~cannotGetRuntimeDbWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetRuntimeDbWarningMsg::warningMsg() {

	printMsg("The RuntimeDb object is not accessible via the FairRunAna object!!!");

}


/****************************************************************
 * CLASS cannotGetStsParameterContainerWarningMsg 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetStsParameterContainerWarningMsg::cannotGetStsParameterContainerWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetStsParameterContainerWarningMsg::~cannotGetStsParameterContainerWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetStsParameterContainerWarningMsg::warningMsg() {

	printMsg("The object for the parameter container of the STS detector is not accessible!!!");

}


/****************************************************************
 * CLASS cannotGetMvdParameterContainerWarningMsg 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetMvdParameterContainerWarningMsg::cannotGetMvdParameterContainerWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetMvdParameterContainerWarningMsg::~cannotGetMvdParameterContainerWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetMvdParameterContainerWarningMsg::warningMsg() {

	printMsg("The object for the parameter container of the MVD detector is not accessible!!!");

}


/****************************************************************
 * CLASS cannotGetStsGeoSensitiveNodesWarningMsg 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetStsGeoSensitiveNodesWarningMsg::cannotGetStsGeoSensitiveNodesWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetStsGeoSensitiveNodesWarningMsg::~cannotGetStsGeoSensitiveNodesWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetStsGeoSensitiveNodesWarningMsg::warningMsg() {

	printMsg("The STS geo sensitive nodes are not accessible via the parameter container!!!");

}


/****************************************************************
 * CLASS cannotGetMvdGeoSensitiveNodesWarningMsg 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetMvdGeoSensitiveNodesWarningMsg::cannotGetMvdGeoSensitiveNodesWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetMvdGeoSensitiveNodesWarningMsg::~cannotGetMvdGeoSensitiveNodesWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetMvdGeoSensitiveNodesWarningMsg::warningMsg() {

	printMsg("The MVD geo sensitive nodes are not accessible via the parameter container!!!");

}


/****************************************************************
 * CLASS cannotGetLabTransformWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetLabTransformWarningMsg::cannotGetLabTransformWarningMsg() 
  : inputWarningMsg(), 
    stationId(0)
{

  //	stationId = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotGetLabTransformWarningMsg::cannotGetLabTransformWarningMsg(int _stationId) 
  : inputWarningMsg(), 
    stationId(_stationId)
{

  //	this->stationId = _stationId;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetLabTransformWarningMsg::~cannotGetLabTransformWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetLabTransformWarningMsg::warningMsg() {

	std::string temp;
	char        intBuffer[intConversionDigits+1];

	temp  = "The lab transformation of the STS geo sensitive node with the MCid '";
	itos(stationId, intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += "' is not accessible!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotAutomaticReadDetectorWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAutomaticReadDetectorWarningMsg::cannotAutomaticReadDetectorWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAutomaticReadDetectorWarningMsg::~cannotAutomaticReadDetectorWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotAutomaticReadDetectorWarningMsg::warningMsg() {

	printMsg("The STS detector is not accessible. So the program switches to an older way for reading the information!!!");

}


/****************************************************************
 * CLASS disableAutomaticReadHitsWarningMsg						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

disableAutomaticReadHitsWarningMsg::disableAutomaticReadHitsWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

disableAutomaticReadHitsWarningMsg::~disableAutomaticReadHitsWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void disableAutomaticReadHitsWarningMsg::warningMsg() {

	printMsg("Getting the hits directly from the framework is disabled!!!");

}


/****************************************************************
 * CLASS takePointsAsHitsWarningMsg								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

takePointsAsHitsWarningMsg::takePointsAsHitsWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

takePointsAsHitsWarningMsg::~takePointsAsHitsWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void takePointsAsHitsWarningMsg::warningMsg() {

	printMsg("The points are taken as hits without any hitproducer!!!");

}


/****************************************************************
 * CLASS takeMyHitProducerWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

takeMyHitProducerWarningMsg::takeMyHitProducerWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

takeMyHitProducerWarningMsg::~takeMyHitProducerWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void takeMyHitProducerWarningMsg::warningMsg() {

	printMsg("The points are taken as hits with my special hitproducer (binning or smearing)!!!");

}


/****************************************************************
 * CLASS hitArrayPointerIsNotSupportedInAsciiWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitArrayPointerIsNotSupportedInAsciiWarningMsg::hitArrayPointerIsNotSupportedInAsciiWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitArrayPointerIsNotSupportedInAsciiWarningMsg::~hitArrayPointerIsNotSupportedInAsciiWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitArrayPointerIsNotSupportedInAsciiWarningMsg::warningMsg() {

	printMsg("In the used ascii object the data can just be read out of the file. So using the hit pointers is not supported!!!");

}


/****************************************************************
 * CLASS noHitToPointWarningMsg									*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noHitToPointWarningMsg::noHitToPointWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noHitToPointWarningMsg::~noHitToPointWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void noHitToPointWarningMsg::warningMsg() {

#ifndef NOANALYSIS

	printMsg("There are no MCPoints read. So there is no MCPoint object for all hits!!!");

#endif

}


/****************************************************************
 * CLASS digiSchemeIsNotSupportedInAsciiWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

digiSchemeIsNotSupportedInAsciiWarningMsg::digiSchemeIsNotSupportedInAsciiWarningMsg() : inputWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

digiSchemeIsNotSupportedInAsciiWarningMsg::~digiSchemeIsNotSupportedInAsciiWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void digiSchemeIsNotSupportedInAsciiWarningMsg::warningMsg() {

	printMsg("In the used ascii object the data can just be read out of the file. So using the digi scheme is not supported!!!");

}


/****************************************************************
 * CLASS cannotGetStsStationWithIndexWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetStsStationWithIndexWarningMsg::cannotGetStsStationWithIndexWarningMsg() 
  : inputWarningMsg(), 
    stationId(0)
{

  //	stationId = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotGetStsStationWithIndexWarningMsg::cannotGetStsStationWithIndexWarningMsg(int _stationId) 
  : inputWarningMsg(),
    stationId(_stationId)
{

  //	this->stationId = _stationId;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetStsStationWithIndexWarningMsg::~cannotGetStsStationWithIndexWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetStsStationWithIndexWarningMsg::warningMsg() {

	std::string temp;
	char        intBuffer[intConversionDigits+1];

	temp  = "The STS station with the index ";
	itos(stationId, intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += " is not accessible!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotGetMvdStationWithIndexWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetMvdStationWithIndexWarningMsg::cannotGetMvdStationWithIndexWarningMsg() 
  : inputWarningMsg(), 
    stationId(0)
{

  //	stationId = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotGetMvdStationWithIndexWarningMsg::cannotGetMvdStationWithIndexWarningMsg(int _stationId) 
  : inputWarningMsg(), 
    stationId(_stationId)
{

  //	this->stationId = _stationId;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetMvdStationWithIndexWarningMsg::~cannotGetMvdStationWithIndexWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetMvdStationWithIndexWarningMsg::warningMsg() {

	std::string temp;
	char        intBuffer[intConversionDigits+1];

	temp  = "The MVD station with the index ";
	itos(stationId, intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += " is not accessible!!!";
	printMsg(temp);

}
