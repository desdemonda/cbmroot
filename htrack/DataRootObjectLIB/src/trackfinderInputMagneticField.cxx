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
///     - consists of the information for the magnetic field
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-07 10:36:51 $
/// $Revision: 1.12 $
///
//////////////////////////////////////////////////////////////////////


#include "../../MiscLIB/include/errorHandling.h"
#include "../include/trackfinderInputMagneticField.h"
#include "../include/dataRootObjectError.h"
#include "../include/dataRootObjectWarningMsg.h"


#define INTEGRATEDMAGNETICFIELD
//#undef INTEGRATEDMAGNETICFIELD


#if (ARCHITECTURE == STANDALONE)

	#define numberOfDefaultMagnetfieldFactors 8
	const double defaultMagnetfieldFactors[numberOfDefaultMagnetfieldFactors] = {5.98, 6.66, 7.01, 7.70, 8.37, 8.89, 9.04, 10.0};							// dimension: [kG]

#elif (ARCHITECTURE == CBMROOT)

	#define numberOfDefaultMagnetfieldFactors 7
	const double defaultMagnetfieldFactors[numberOfDefaultMagnetfieldFactors] = {6.574331, 6.977340, 7.253845, 7.521950, 7.981060, 8.076062, 8.1};			// dimension: [kG]

#endif


#define sqr(a)  ((a) * (a)) 


/****************************************************************
 * method tries to read the fiel.								*
 ****************************************************************/

void trackfinderInputMagneticField::readField() {

	if (!fileName.empty()) {
	
#if (ARCHITECTURE == STANDALONE)

		if (isLocalField) {

			if (magneticField != NULL) {
				delete magneticField;
				magneticField = NULL;
			}
	
		}

		magneticField = new FairField("Magnetic Field");

		isLocalField  = true;

		if (isRootFile)
			magneticField->readRootfile(fileName.c_str(), mapName.c_str());
		else
			magneticField->readAsciifile(fileName.c_str());
	
#endif

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderInputMagneticField::trackfinderInputMagneticField() 
  : TObject(), 
    magneticField(NULL),
    isLocalField(false),
    fileName(),
    isRootFile(false),
    mapName(),
    magneticFieldIntegrationStepwidthPerStation(1),
    magneticFieldIntegrationFactor(1),
    numberOfMagnetfieldFactors(0),
    magnetfieldFactors(NULL)
{
  /*
	magneticField                               = NULL;
	isLocalField                                = false;
	fileName.clear();
	isRootFile                                  = false;
	mapName.clear();
	magneticFieldIntegrationStepwidthPerStation = 1;
	magneticFieldIntegrationFactor              = 1;
	numberOfMagnetfieldFactors                  = 0;
	magnetfieldFactors                          = NULL;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinderInputMagneticField::trackfinderInputMagneticField(const trackfinderInputMagneticField& value) 
  : TObject(value),
    magneticField(NULL),
    isLocalField(value.isLocalField),
    fileName(value.fileName),
    isRootFile(value.isRootFile),
    mapName(value.mapName),
    magneticFieldIntegrationStepwidthPerStation(value.magneticFieldIntegrationStepwidthPerStation),
    magneticFieldIntegrationFactor(value.magneticFieldIntegrationFactor),
    numberOfMagnetfieldFactors(value.numberOfMagnetfieldFactors),
    magnetfieldFactors(value.magnetfieldFactors)
{

	if (value.isLocalField)
		*(this->magneticField)                        = *(value.magneticField);
	else
		this->magneticField                           = value.magneticField;
	/*
	this->isLocalField                                = value.isLocalField;
	this->fileName                                    = value.fileName;
	this->isRootFile                                  = value.isRootFile;
	this->mapName                                     = value.mapName;
	this->magneticFieldIntegrationStepwidthPerStation = value.magneticFieldIntegrationStepwidthPerStation;
	this->magneticFieldIntegrationFactor              = value.magneticFieldIntegrationFactor;
	this->numberOfMagnetfieldFactors                  = value.numberOfMagnetfieldFactors;
	this->magnetfieldFactors                          = value.magnetfieldFactors;
	*/
}
trackfinderInputMagneticField::trackfinderInputMagneticField(const char* _fileName, bool _isRootFile, const char* _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor) 
  : TObject(), 
    magneticField(NULL),
    isLocalField(false),
    fileName(_fileName),
    isRootFile(_isRootFile),
    mapName(_mapName),
    magneticFieldIntegrationStepwidthPerStation(1),
    magneticFieldIntegrationFactor(_magneticFieldIntegrationFactor),
    numberOfMagnetfieldFactors(0),
    magnetfieldFactors(NULL)
{

  /*
	this->magneticField                                   = NULL;
	this->isLocalField                                    = false;
	this->fileName                                        = _fileName;
	this->isRootFile                                      = _isRootFile;
	this->mapName                                         = _mapName;
  */
	if (_magneticFieldIntegrationStepwidthPerStation > 0)
		this->magneticFieldIntegrationStepwidthPerStation = _magneticFieldIntegrationStepwidthPerStation;
	else
		this->magneticFieldIntegrationStepwidthPerStation = 1;
	/*
	this->magneticFieldIntegrationFactor                  = _magneticFieldIntegrationFactor;
	this->numberOfMagnetfieldFactors                      = 0;
	this->magnetfieldFactors                              = NULL;
	*/
}
trackfinderInputMagneticField::trackfinderInputMagneticField(std::string _fileName, bool _isRootFile, std::string _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor) 
  : TObject(),
    magneticField(NULL),
    isLocalField(false),
    fileName(_fileName),
    isRootFile(_isRootFile),
    mapName(_mapName.c_str()),
    magneticFieldIntegrationStepwidthPerStation(1),
    magneticFieldIntegrationFactor(_magneticFieldIntegrationFactor),
    numberOfMagnetfieldFactors(0),
    magnetfieldFactors(NULL)
{

   /*
	this->magneticField                                   = NULL;
	this->isLocalField                                    = false;
	this->fileName                                        = _fileName;
	this->isRootFile                                      = _isRootFile;
	this->mapName                                         = _mapName.c_str();
   */
	if (_magneticFieldIntegrationStepwidthPerStation > 0)
		this->magneticFieldIntegrationStepwidthPerStation = _magneticFieldIntegrationStepwidthPerStation;
	else
		this->magneticFieldIntegrationStepwidthPerStation = 1;
	/*
	this->magneticFieldIntegrationFactor                  = _magneticFieldIntegrationFactor;
	this->numberOfMagnetfieldFactors                      = 0;
	this->magnetfieldFactors                              = NULL;
	*/
}
trackfinderInputMagneticField::trackfinderInputMagneticField(unsigned short _numberOfMagnetfieldFactors) 
  : TObject(),
    magneticField(NULL),
    isLocalField(false),
    fileName(),
    isRootFile(false),
    mapName(),
    magneticFieldIntegrationStepwidthPerStation(1),
    magneticFieldIntegrationFactor(1),
    numberOfMagnetfieldFactors(0),
    magnetfieldFactors(NULL)
{
   /*
	magneticField                               = NULL;
	isLocalField                                = false;
	fileName.clear();
	isRootFile                                  = false;
	mapName.clear();
	magneticFieldIntegrationStepwidthPerStation = 1;
	magneticFieldIntegrationFactor              = 1;
	numberOfMagnetfieldFactors                  = 0;
	magnetfieldFactors                          = NULL;
   */
	setNumberOfMagnetfieldFactors(_numberOfMagnetfieldFactors);

}
trackfinderInputMagneticField::trackfinderInputMagneticField(FairField* field, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor) 
  : TObject(),
    magneticField(field),
    isLocalField(false),
    fileName(),
    isRootFile(false),
    mapName(),
    magneticFieldIntegrationStepwidthPerStation(1),
    magneticFieldIntegrationFactor(_magneticFieldIntegrationFactor),
    numberOfMagnetfieldFactors(0),
    magnetfieldFactors(NULL)
{
  /*
	this->magneticField                                   = field;
	this->isLocalField                                    = false;
	this->fileName.clear();
	this->isRootFile                                      = false;
	this->mapName.clear();
  */
	if (_magneticFieldIntegrationStepwidthPerStation > 0)
		this->magneticFieldIntegrationStepwidthPerStation = _magneticFieldIntegrationStepwidthPerStation;
	else
		this->magneticFieldIntegrationStepwidthPerStation = 1;
	/*
	this->magneticFieldIntegrationFactor                  = _magneticFieldIntegrationFactor;
	this->numberOfMagnetfieldFactors                      = 0;
	this->magnetfieldFactors                              = NULL;
	*/
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderInputMagneticField::~trackfinderInputMagneticField() {

	if (isLocalField) {

		if (magneticField != NULL) {
			delete magneticField;
			magneticField = NULL;
		}
	
	}

	if (magnetfieldFactors != NULL) {
		delete [] magnetfieldFactors;
		magnetfieldFactors = NULL;
	}

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackfinderInputMagneticField& trackfinderInputMagneticField::operator = (const trackfinderInputMagneticField& value) {

	if (isLocalField) {

		if (this->magneticField != NULL) {
			delete this->magneticField;
			this->magneticField = NULL;
		}
		isLocalField = false;

	}

	if (magnetfieldFactors != NULL) {
		delete [] magnetfieldFactors;
		magnetfieldFactors = NULL;
	}

	this->fileName                                    = value.fileName;
	this->isRootFile                                  = value.isRootFile;
	this->mapName                                     = value.mapName;
	this->magneticFieldIntegrationStepwidthPerStation = value.magneticFieldIntegrationStepwidthPerStation;
	this->magneticFieldIntegrationFactor              = value.magneticFieldIntegrationFactor;
	initialize();

	this->numberOfMagnetfieldFactors                  = value.numberOfMagnetfieldFactors;
	this->magnetfieldFactors                          = new magnetfieldFactor[this->numberOfMagnetfieldFactors];
	if (this->magnetfieldFactors == NULL)
		throw memoryAllocationError(DATAROOTOBJECTLIB);
	for (unsigned short i = 0; i < this->numberOfMagnetfieldFactors; i++)
		this->magnetfieldFactors[i]                   = value.magnetfieldFactors[i];

	return *this;

}

/****************************************************************
 * method inits the field.										*
 ****************************************************************/

void trackfinderInputMagneticField::init(const char* _fileName, bool _isRootFile, const char* _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors) {

	if (isLocalField) {

		if (magneticField != NULL) {
			delete magneticField;
			magneticField = NULL;
		}

	}

	if (magnetfieldFactors != NULL) {
		delete [] magnetfieldFactors;
		magnetfieldFactors = NULL;
	}

	this->fileName                                        = _fileName;
	this->isRootFile                                      = _isRootFile;
	this->mapName                                         = _mapName;
	if (_magneticFieldIntegrationStepwidthPerStation > 0)
		this->magneticFieldIntegrationStepwidthPerStation = _magneticFieldIntegrationStepwidthPerStation;
	else
		this->magneticFieldIntegrationStepwidthPerStation = 1;
	this->magneticFieldIntegrationFactor                  = _magneticFieldIntegrationFactor;
	initialize();

	this->numberOfMagnetfieldFactors                      = 0;
	this->magnetfieldFactors                              = NULL;

	setNumberOfMagnetfieldFactors(_numberOfMagnetfieldFactors);

}
void trackfinderInputMagneticField::init(std::string _fileName, bool _isRootFile, std::string _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors) {

	if (isLocalField) {

		if (magneticField != NULL) {
			delete magneticField;
			magneticField = NULL;
		}

	}

	if (magnetfieldFactors != NULL) {
		delete [] magnetfieldFactors;
		magnetfieldFactors = NULL;
	}

	this->fileName                                        = _fileName;
	this->isRootFile                                      = _isRootFile;
	this->mapName                                         = _mapName;
	if (_magneticFieldIntegrationStepwidthPerStation > 0)
		this->magneticFieldIntegrationStepwidthPerStation = _magneticFieldIntegrationStepwidthPerStation;
	else
		this->magneticFieldIntegrationStepwidthPerStation = 1;
	this->magneticFieldIntegrationFactor                  = _magneticFieldIntegrationFactor;
	initialize();

	this->numberOfMagnetfieldFactors                      = 0;
	this->magnetfieldFactors                              = NULL;

	setNumberOfMagnetfieldFactors(_numberOfMagnetfieldFactors);

}
void trackfinderInputMagneticField::init(unsigned short _numberOfMagnetfieldFactors) {

	if (isLocalField) {

		if (magneticField != NULL) {
			delete magneticField;
			magneticField = NULL;
		}

	}

	if (magnetfieldFactors != NULL) {
		delete [] magnetfieldFactors;
		magnetfieldFactors = NULL;
	}

	fileName.clear();
	isRootFile                                  = false;
	mapName.clear();
	magneticFieldIntegrationStepwidthPerStation = 1;
	magneticFieldIntegrationFactor              = 1;
	numberOfMagnetfieldFactors                  = 0;
	magnetfieldFactors                          = NULL;

	setNumberOfMagnetfieldFactors(_numberOfMagnetfieldFactors);

}
void trackfinderInputMagneticField::init(FairField* field, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors) {

	if (isLocalField) {

		if (magneticField != NULL) {
			delete magneticField;
			magneticField = NULL;
		}

	}

	if (magnetfieldFactors != NULL) {
		delete [] magnetfieldFactors;
		magnetfieldFactors = NULL;
	}

	this->magneticField                               = field;
	this->isLocalField                                = false;
	this->fileName.clear();
	this->isRootFile                                  = false;
	this->mapName.clear();
	if (_magneticFieldIntegrationStepwidthPerStation > 0)
		this->magneticFieldIntegrationStepwidthPerStation = _magneticFieldIntegrationStepwidthPerStation;
	else
		this->magneticFieldIntegrationStepwidthPerStation = 1;
	this->magneticFieldIntegrationFactor              = _magneticFieldIntegrationFactor;
	this->numberOfMagnetfieldFactors                  = 0;
	this->magnetfieldFactors                          = NULL;

	setNumberOfMagnetfieldFactors(_numberOfMagnetfieldFactors);

}

/****************************************************************
 * Returns the field values at a given point. This is defined	*
 * at the value as the nearest grid point. The dimension of the	*
 * values read from the file is Tesla.							*
 ****************************************************************/

void trackfinderInputMagneticField::getFieldValues(trackfinderInputHit* hit, magneticFieldValue* fieldValue) {

	double fieldValues;

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	if (fieldValue == NULL)
		throw cannotAccessMagneticFieldValueError();

	if (magneticField != NULL) {

		getFieldValues(hit->getPosX(), hit->getPosY(), hit->getPosZ(), fieldValue);

	}
	else {

		fieldValues = evaluateMagnetfieldFactor(hit);
		fieldValue->setFieldX(fieldValues);
		fieldValue->setFieldY(fieldValues);
		fieldValue->setFieldZ(fieldValues);

	}

}
void trackfinderInputMagneticField::getFieldValues(double hitPosX, double hitPosY, double hitPosZ, magneticFieldValue* fieldValue) {

	double point[3];
	double fieldValues[3];

	if (fieldValue == NULL)
		throw cannotAccessMagneticFieldValueError();

	if (magneticField != NULL) {
	
		point[0] = hitPosX;
		point[1] = hitPosY;
		point[2] = hitPosZ;
		magneticField->GetFieldValue(point, fieldValues);
		fieldValue->setFieldX(fieldValues[0]);
		fieldValue->setFieldY(fieldValues[1]);
		fieldValue->setFieldZ(fieldValues[2]);

	}
	else
		throw cannotAccessMagneticFieldError();

}

/****************************************************************
 * Returns the integrated field values at a given point. This is*
 * defined as the integrated value at the nearest grid point.	*
 ****************************************************************/

void trackfinderInputMagneticField::getIntegratedFieldValues(trackfinderInputHit* hit, magneticFieldValue* fieldValue, bool negativFieldOrientation, bool median) {

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	if (hit->getStation() == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	getIntegratedFieldValues(hit->getPosX(), hit->getPosY(), hit->getPosZ(), hit->getStation()->getIndex(), fieldValue, negativFieldOrientation, median);

}
void trackfinderInputMagneticField::getIntegratedFieldValues(double hitPosX, double hitPosY, double hitPosZ, unsigned short stationIndex, magneticFieldValue* fieldValue, bool negativFieldOrientation, bool median) {

	double             roadLength;
	unsigned short     numberOfIntegrateValues;
	double             xCoordSteps;
	double             yCoordSteps;
	double             zCoordSteps;
	double             xCoord;
	double             yCoord;
	double             zCoord;
	double             integrationDivider;
	double             integrationFactor;
	magneticFieldValue fieldIntegrate;
	double             fieldValues;

	if (fieldValue == NULL)
		throw cannotAccessMagneticFieldValueError();

	if (magneticField != NULL) {

		fieldValue->setFieldX(0);
		fieldValue->setFieldY(0);
		fieldValue->setFieldZ(0);

		if ((hitPosX != 0) || (hitPosY != 0) || (hitPosZ != 0)) {

			roadLength = sqrt(sqr(hitPosX) + sqr(hitPosY) + sqr(hitPosZ));

			/* Here the Simpson Rule is used for the integration. */

			if (magneticFieldIntegrationStepwidthPerStation & 1)
				numberOfIntegrateValues = (stationIndex + 1 ) * (magneticFieldIntegrationStepwidthPerStation + 1);
			else
				numberOfIntegrateValues = (stationIndex + 1 ) * magneticFieldIntegrationStepwidthPerStation;

			xCoordSteps = (hitPosX - 0) / numberOfIntegrateValues;	/* xCoordSteps = h = (b - a) / n, n gerade */
			yCoordSteps = (hitPosY - 0) / numberOfIntegrateValues;	/* yCoordSteps = h = (b - a) / n, n gerade */
			zCoordSteps = (hitPosZ - 0) / numberOfIntegrateValues;	/* zCoordSteps = h = (b - a) / n, n gerade */

			integrationDivider =  sqrt(sqr(xCoordSteps) + sqr(yCoordSteps) + sqr(zCoordSteps)) / 3;					/* integrationDivider = h / 3 */

			for (unsigned short i = 0; i <= numberOfIntegrateValues; i++) {
		
				xCoord = 0 + i * xCoordSteps;						/* xCoord = a + ih */
				yCoord = 0 + i * yCoordSteps;						/* yCoord = a + ih */
				zCoord = 0 + i * zCoordSteps;						/* zCoord = a + ih */

				getFieldValues(xCoord, yCoord, zCoord, &fieldIntegrate);	/* f(Coord) = f(a + ih) */

				if ((i == 0) || (i == numberOfIntegrateValues))
					integrationFactor = 1;							/* factor for f(a) and f(b) */
				else if (i & 1)
					integrationFactor = 4;							/* factor for f(a + ih) with i is odd */
				else
					integrationFactor = 2;							/* factor for f(a + ih) with i is even */

				fieldValue->setFieldX(fieldValue->getFieldX() + integrationFactor * fieldIntegrate.getFieldX() * integrationDivider);
				fieldValue->setFieldY(fieldValue->getFieldY() + integrationFactor * fieldIntegrate.getFieldY() * integrationDivider);
				fieldValue->setFieldZ(fieldValue->getFieldZ() + integrationFactor * fieldIntegrate.getFieldZ() * integrationDivider);

			}

			if (negativFieldOrientation) {

				fieldValue->setFieldX(- fieldValue->getFieldX());
				fieldValue->setFieldY(- fieldValue->getFieldY());
				fieldValue->setFieldZ(- fieldValue->getFieldZ());

			}

			if (median) {

				fieldValue->setFieldX(fieldValue->getFieldX() / roadLength);
				fieldValue->setFieldY(fieldValue->getFieldY() / roadLength);
				fieldValue->setFieldZ(fieldValue->getFieldZ() / roadLength);

			}

		}
		else {

			integrationRangeOfZeroWarningMsg* integrationRangeOfZero = new integrationRangeOfZeroWarningMsg();
			integrationRangeOfZero->warningMsg();
			if(integrationRangeOfZero != NULL) {
				delete integrationRangeOfZero;
				integrationRangeOfZero = NULL;
			}

		}

	}
	else {

		fieldValues = evaluateMagnetfieldFactor(stationIndex);
		fieldValue->setFieldX(fieldValues);
		fieldValue->setFieldY(fieldValues);
		fieldValue->setFieldZ(fieldValues);

	}

}

/****************************************************************
 * Returns the field factor at a given point					*
 ****************************************************************/

double trackfinderInputMagneticField::getFieldFactor(trackfinderInputHit* hit, bool negativFieldOrientation) {

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	if (hit->getStation() == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	return getFieldFactor(hit->getPosX(), hit->getPosY(), hit->getPosZ(), hit->getStation()->getIndex(), negativFieldOrientation);

}
double trackfinderInputMagneticField::getFieldFactor(double hitPosX, double hitPosY, double hitPosZ, unsigned short stationIndex, bool negativFieldOrientation) {

	magneticFieldValue fieldValue;
	double             returnValue;

#ifdef INTEGRATEDMAGNETICFIELD

	getIntegratedFieldValues(hitPosX, hitPosY, hitPosZ, stationIndex, &fieldValue, negativFieldOrientation);

#else

	getFieldValues(hitPosX, hitPosY, hitPosZ, &fieldValue);

#endif

	returnValue = magneticFieldIntegrationFactor * fieldValue.getFieldY();

	return returnValue;

}

/****************************************************************
 * method sets the number of magnetfield factors (stations)		*
 ****************************************************************/

void trackfinderInputMagneticField::setNumberOfMagnetfieldFactors(unsigned short _numberOfMagnetfieldFactors) {

	if (magnetfieldFactors != NULL) {

		delete [] magnetfieldFactors;
		magnetfieldFactors = NULL;

	}

	this->numberOfMagnetfieldFactors = _numberOfMagnetfieldFactors;

	if (this->numberOfMagnetfieldFactors > 0) {

		magnetfieldFactors = new magnetfieldFactor[this->numberOfMagnetfieldFactors];
		if (magnetfieldFactors == NULL)
			throw memoryAllocationError(DATAROOTOBJECTLIB);

		for (unsigned short i = 0; i < this->numberOfMagnetfieldFactors; i++) {

			if (i < numberOfDefaultMagnetfieldFactors)
				magnetfieldFactors[i].value = defaultMagnetfieldFactors[i];
			else
				magnetfieldFactors[i].value = defaultMagnetfieldFactors[numberOfDefaultMagnetfieldFactors - 1];

			magnetfieldFactors[i].weigth    = 1;

		}
	
	}

}

/****************************************************************
 * method gets the magnetfield factor for station stationIndex	*
 ****************************************************************/

double trackfinderInputMagneticField::getMagnetfieldFactor(unsigned short stationIndex) {

	double returnValue = 0;

	if (stationIndex >= numberOfMagnetfieldFactors) {

		stationIndexIsBiggerThanLastStationIndexWarningMsg* stationIndexIsBiggerThanNumberOfFactors = new stationIndexIsBiggerThanLastStationIndexWarningMsg(stationIndex, numberOfMagnetfieldFactors - 1);
		stationIndexIsBiggerThanNumberOfFactors->warningMsg();
		if(stationIndexIsBiggerThanNumberOfFactors != NULL) {
			delete stationIndexIsBiggerThanNumberOfFactors;
			stationIndexIsBiggerThanNumberOfFactors = NULL;
		}

	}
	else {

		returnValue = magnetfieldFactors[stationIndex].value;

	}

	return returnValue;

}

/****************************************************************
 * method sets the magnetfield factor for station stationIndex	*
 ****************************************************************/

void trackfinderInputMagneticField::setMagnetfieldFactor(unsigned short stationIndex, double value) {

	if (stationIndex >= numberOfMagnetfieldFactors) {

		stationIndexIsBiggerThanLastStationIndexWarningMsg* stationIndexIsBiggerThanNumberOfFactors = new stationIndexIsBiggerThanLastStationIndexWarningMsg(stationIndex, numberOfMagnetfieldFactors - 1);
		stationIndexIsBiggerThanNumberOfFactors->warningMsg();
		if(stationIndexIsBiggerThanNumberOfFactors != NULL) {
			delete stationIndexIsBiggerThanNumberOfFactors;
			stationIndexIsBiggerThanNumberOfFactors = NULL;
		}

	}
	else {

		magnetfieldFactors[stationIndex].value  = value;
		magnetfieldFactors[stationIndex].weigth = 1;

	}

}

/****************************************************************
 * method updates the magnetfield factor for station			*
 * stationIndex													*
 ****************************************************************/

void trackfinderInputMagneticField::updateMagnetfieldFactor(unsigned short stationIndex, double value) {

	if (stationIndex >= numberOfMagnetfieldFactors) {

		stationIndexIsBiggerThanLastStationIndexWarningMsg* stationIndexIsBiggerThanNumberOfFactors = new stationIndexIsBiggerThanLastStationIndexWarningMsg(stationIndex, numberOfMagnetfieldFactors - 1);
		stationIndexIsBiggerThanNumberOfFactors->warningMsg();
		if(stationIndexIsBiggerThanNumberOfFactors != NULL) {
			delete stationIndexIsBiggerThanNumberOfFactors;
			stationIndexIsBiggerThanNumberOfFactors = NULL;
		}

	}
	else {
		/* weighted middle value
		   basic formula: new = (weigth * old + value) / (weight + 1)
		                      = ((weigth * old + old - old + value) / (weight +1)
		                      = ((weight + 1) * old + value - old) / (weigth + 1)
							  = old + (value - old) / (weigth +1)
	    */
		magnetfieldFactors[stationIndex].weigth++;
		magnetfieldFactors[stationIndex].value  = magnetfieldFactors[stationIndex].value + ((value - magnetfieldFactors[stationIndex].value) / magnetfieldFactors[stationIndex].weigth);

	}

}

/****************************************************************
 * method returns the magnetfield factor for the station		*
 ****************************************************************/

double trackfinderInputMagneticField::evaluateMagnetfieldFactor(trackfinderInputHit* hit) {

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	if (hit->getStation() == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	return evaluateMagnetfieldFactor(hit->getStation()->getIndex());

}
double trackfinderInputMagneticField::evaluateMagnetfieldFactor(unsigned short stationIndex) {

	double returnValue = 0;

	if (stationIndex >= numberOfMagnetfieldFactors) {

		stationIndexIsBiggerThanLastStationIndexWarningMsg* stationIndexIsBiggerThanNumberOfFactors = new stationIndexIsBiggerThanLastStationIndexWarningMsg(stationIndex, numberOfMagnetfieldFactors - 1);
		stationIndexIsBiggerThanNumberOfFactors->warningMsg();
		if(stationIndexIsBiggerThanNumberOfFactors != NULL) {
			delete stationIndexIsBiggerThanNumberOfFactors;
			stationIndexIsBiggerThanNumberOfFactors = NULL;
		}

		if (numberOfMagnetfieldFactors > 0)
			returnValue = magnetfieldFactors[numberOfMagnetfieldFactors - 1].value;
		else
			returnValue = 0;

	}
	else
		returnValue = magnetfieldFactors[stationIndex].value;

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputMagneticField::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(magneticField);
	returnValue += sizeof(isLocalField);
	returnValue += sizeof(magneticFieldIntegrationStepwidthPerStation);
	returnValue += sizeof(magneticFieldIntegrationFactor);
	returnValue += sizeof(fileName);
	returnValue += sizeof(mapName);
	returnValue += sizeof(isRootFile);
	returnValue += sizeof(numberOfMagnetfieldFactors);
	returnValue += sizeof(magnetfieldFactors);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputMagneticField::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = fileName.capacity() * sizeof(char);
	returnValue += mapName.capacity()  * sizeof(char);
	if (numberOfMagnetfieldFactors > 0)
		returnValue += numberOfMagnetfieldFactors * sizeof(magnetfieldFactors[0]);;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputMagneticField::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(magneticField);
	returnValue += sizeof(isLocalField);
	returnValue += sizeof(magneticFieldIntegrationStepwidthPerStation);
	returnValue += sizeof(magneticFieldIntegrationFactor);
	returnValue += sizeof(fileName);
	returnValue += fileName.capacity() * sizeof(char);
	returnValue += sizeof(mapName);
	returnValue += mapName.capacity()  * sizeof(char);
	returnValue += sizeof(isRootFile);
	returnValue += sizeof(numberOfMagnetfieldFactors);
	returnValue += sizeof(magnetfieldFactors);
	if (numberOfMagnetfieldFactors > 0)
		returnValue += numberOfMagnetfieldFactors * sizeof(magnetfieldFactors[0]);;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(trackfinderInputMagneticField)
