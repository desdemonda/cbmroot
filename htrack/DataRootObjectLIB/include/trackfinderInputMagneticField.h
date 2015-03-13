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
/// $Date: 2008-10-07 10:36:50 $
/// $Revision: 1.9 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTMAGNETICFIELD_H
#define _TRACKFINDERINPUTMAGNETICFIELD_H


#include "../../MiscLIB/include/defs.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/FairField.h"
#elif (ARCHITECTURE == CBMROOT)
	#include "FairField.h"
#endif
#include "../../DataObjectLIB/include/magneticFieldValue.h"
#include "trackfinderInputHit.h"
#include "TObject.h"


typedef struct {

	double         value;
	unsigned short weigth;

} magnetfieldFactor;

/* **************************************************************
 * CLASS trackfinderInputMagneticField							*
 * **************************************************************/

class trackfinderInputMagneticField : public TObject {

protected:

	FairField*          magneticField;
	bool               isLocalField;
	unsigned short     magneticFieldIntegrationStepwidthPerStation;
	double             magneticFieldIntegrationFactor;
	std::string        fileName;
	std::string        mapName;
	bool               isRootFile;
	unsigned short     numberOfMagnetfieldFactors;
	magnetfieldFactor* magnetfieldFactors;

/**
 * method tries to read the field.
 */

	void readField();

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputMagneticField, 1);

/**
 * Default constructor
 */

	trackfinderInputMagneticField();

/**
 * Constructor
 */

	trackfinderInputMagneticField(const trackfinderInputMagneticField& value);
	trackfinderInputMagneticField(const char* fileName, bool isRootFile = true, const char* mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0);
	trackfinderInputMagneticField(std::string fileName, bool isRootFile = true, std::string mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0);
	trackfinderInputMagneticField(unsigned short numberOfMagnetfieldFactors);
	trackfinderInputMagneticField(FairField* field, unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0);

/**
 * Destructor
 */

	virtual ~trackfinderInputMagneticField();

/**
 * operator = ()
 */

	const trackfinderInputMagneticField& operator = (const trackfinderInputMagneticField& value);

/**
 * method inits the fieldmap.
 */

	virtual void initialize() = 0;
	void init(const char* fileName, bool isRootFile = true, const char* mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0);
	void init(std::string fileName, bool isRootFile = true, std::string mapName = "NewMap", unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0);
	void init(unsigned short numberOfMagnetfieldFactors);
	void init(FairField* field, unsigned short magneticFieldIntegrationStepwidthPerStation = 10, double magneticFieldIntegrationFactor = 1.0, unsigned short numberOfMagnetfieldFactors = 0);

/**
 * Returns the field values at a given point. This is defined at the
 * value as the nearest grid point. The dimension of the values read from
 * the file is Tesla.
 */

	void getFieldValues(trackfinderInputHit* hit, magneticFieldValue* fieldValue);
	void getFieldValues(double hitPosX, double hitPosY, double hitPosZ, magneticFieldValue* fieldValue);

/**
 * Returns the integrated field values at a given point. This is
 * defined as the integrated value at the nearest grid point.
 */

	void getIntegratedFieldValues(trackfinderInputHit* hit, magneticFieldValue* fieldValue, bool negativFieldOrientation = true, bool median = true);
	void getIntegratedFieldValues(double hitPosX, double hitPosY, double hitPosZ, unsigned short stationIndex, magneticFieldValue* fieldValue, bool negativFieldOrientation = true, bool median = true);

/**
 * Returns the field factor at a given point
 */

	double getFieldFactor(trackfinderInputHit* hit, bool negativFieldOrientation = true);
	double getFieldFactor(double hitPosX, double hitPosY, double hitPosZ, unsigned short stationIndex, bool negativFieldOrientation = true);

/*
 * method sets the number of magnetfield factors (stations)
 */

	void setNumberOfMagnetfieldFactors(unsigned short numberOfMagnetfieldFactors);

/*
 * method gets the magnetfield factor for station stationIndex
 */

	double getMagnetfieldFactor(unsigned short stationIndex);

/*
 * method sets the magnetfield factor for station stationIndex
 */

	void setMagnetfieldFactor(unsigned short stationIndex, double value);

/*
 * method updates the magnetfield factor for station stationIndex
 */

	void updateMagnetfieldFactor(unsigned short stationIndex, double value);

/**
 * method returns the magnetfield factor for the station
 */

	double evaluateMagnetfieldFactor(trackfinderInputHit* hit);
	double evaluateMagnetfieldFactor(unsigned short stationIndex);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfData(unsigned short dimension = 0);

};

#endif
