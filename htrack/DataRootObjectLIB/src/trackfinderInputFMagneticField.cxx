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
///     - consists of the information for the fieldmap
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-07 10:36:51 $
/// $Revision: 1.10 $
///
//////////////////////////////////////////////////////////////////////

#include "../../MiscLIB/include/defs.h"
#if (ARCHITECTURE == CBMROOT)
	#include "FairRunAna.h"
	#include "FairRuntimeDb.h"
	#include "CbmFieldPar.h"
	#include "CbmFieldConst.h"
	#include "CbmFieldMap.h"
	#include "CbmFieldMapSym2.h"
	#include "CbmFieldMapSym3.h"
#endif
#include "../include/dataRootObjectWarningMsg.h"
#include "../include/trackfinderInputFMagneticField.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderInputFMagneticField::trackfinderInputFMagneticField() 
  : trackfinderInputMagneticField(),
    disableAutomaticMagneticField(false)
{

  //	disableAutomaticMagneticField = false;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinderInputFMagneticField::trackfinderInputFMagneticField(const trackfinderInputFMagneticField& value) 
  : trackfinderInputMagneticField(value), 
    disableAutomaticMagneticField(value.disableAutomaticMagneticField)
{
	
  //	this->disableAutomaticMagneticField = value.disableAutomaticMagneticField;

}
trackfinderInputFMagneticField::trackfinderInputFMagneticField(const char* _fileName, bool _isRootFile, const char* _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, bool _disableAutomaticMagneticField) 
  : trackfinderInputMagneticField(_fileName, _isRootFile, _mapName, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor),
    disableAutomaticMagneticField(_disableAutomaticMagneticField)
{

  //	this->disableAutomaticMagneticField = _disableAutomaticMagneticField;

}
trackfinderInputFMagneticField::trackfinderInputFMagneticField(std::string _fileName, bool _isRootFile, std::string _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, bool _disableAutomaticMagneticField) 
  : trackfinderInputMagneticField(fileName, isRootFile, mapName, magneticFieldIntegrationStepwidthPerStation, magneticFieldIntegrationFactor),
    disableAutomaticMagneticField(_disableAutomaticMagneticField)
{

  //	this->disableAutomaticMagneticField = _disableAutomaticMagneticField;

}
trackfinderInputFMagneticField::trackfinderInputFMagneticField(unsigned short _numberOfMagnetfieldFactors) 
  : trackfinderInputMagneticField(_numberOfMagnetfieldFactors),
    disableAutomaticMagneticField(false)
{

  //	this->disableAutomaticMagneticField = false;

}
trackfinderInputFMagneticField::trackfinderInputFMagneticField(FairField* field, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, bool _disableAutomaticMagneticField) 
  : trackfinderInputMagneticField(field, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor),
    disableAutomaticMagneticField(_disableAutomaticMagneticField)
{

  //	this->disableAutomaticMagneticField = _disableAutomaticMagneticField;

}
trackfinderInputFMagneticField::trackfinderInputFMagneticField(FairField* field, const char* _fileName, bool _isRootFile, const char* _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors, bool _disableAutomaticMagneticField) 
  : trackfinderInputMagneticField(),
    disableAutomaticMagneticField(_disableAutomaticMagneticField)
 {
   
   init(field, _fileName, _isRootFile, _mapName, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors, _disableAutomaticMagneticField);

}
trackfinderInputFMagneticField::trackfinderInputFMagneticField(FairField* field, std::string _fileName, bool _isRootFile, std::string _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors, bool _disableAutomaticMagneticField) 
  : trackfinderInputMagneticField(),
    disableAutomaticMagneticField(_disableAutomaticMagneticField)
{

  init(field, _fileName, _isRootFile, _mapName, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors, _disableAutomaticMagneticField);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderInputFMagneticField::~trackfinderInputFMagneticField() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackfinderInputFMagneticField& trackfinderInputFMagneticField::operator = (const trackfinderInputFMagneticField& value) {

	trackfinderInputMagneticField::operator = (value);

	this->disableAutomaticMagneticField = value.disableAutomaticMagneticField;

	return *this;

}

/****************************************************************
 * method inits the fieldmap.									*
 ****************************************************************/

void trackfinderInputFMagneticField::initialize() {

#if (ARCHITECTURE == CBMROOT)

	bool          cannotAccessFrameworkField;
	FairRunAna*    runAna;
	FairRuntimeDb* runtimeDb;
	CbmFieldPar*  fieldPar;
	int           fieldType;

#endif

	if (disableAutomaticMagneticField)
		readField();
	else {

		if (magneticField == NULL) {

#if (ARCHITECTURE == CBMROOT)

			cannotAccessFrameworkField = false;
	
			runAna = FairRunAna::Instance();

			if (!cannotAccessFrameworkField) {
	
				if (runAna == NULL) {

					cannotAccessRunAnaWarningMsg* cannotAccessRunAna = new cannotAccessRunAnaWarningMsg();
					cannotAccessRunAna->warningMsg();
					if(cannotAccessRunAna != NULL) {
						delete cannotAccessRunAna;
						cannotAccessRunAna = NULL;
					}

					cannotAccessFrameworkField = true;

				}

			}

			if (!cannotAccessFrameworkField) {

				runtimeDb = runAna->GetRuntimeDb();
			
				if (runtimeDb == NULL) {

					cannotAccessRuntimeDbWarningMsg* cannotAccessRuntimeDb = new cannotAccessRuntimeDbWarningMsg();
					cannotAccessRuntimeDb->warningMsg();
					if(cannotAccessRuntimeDb != NULL) {
						delete cannotAccessRuntimeDb;
						cannotAccessRuntimeDb = NULL;
					}

					cannotAccessFrameworkField = true;

				}

			}

			if (!cannotAccessFrameworkField) {

				fieldPar = (CbmFieldPar*)runtimeDb->findContainer("CbmFieldPar");
			
				if (fieldPar == NULL) {

					cannotAccessFieldParWarningMsg* cannotAccessFieldPar = new cannotAccessFieldParWarningMsg();
					cannotAccessFieldPar->warningMsg();
					if(cannotAccessFieldPar != NULL) {
						delete cannotAccessFieldPar;
						cannotAccessFieldPar = NULL;
					}

					cannotAccessFrameworkField = true;

				}

			}

			if (!cannotAccessFrameworkField) {

				fieldType = fieldPar->GetType();

				switch(fieldType) {

					case 0:
						magneticField = new CbmFieldConst(fieldPar);
						break;
					case 1:
						magneticField = new CbmFieldMap(fieldPar);
						break;
					case 2:
						magneticField = new CbmFieldMapSym2(fieldPar);
						break;
					case 3:
						magneticField = new CbmFieldMapSym3(fieldPar);
						break;
					default:
						magneticField = NULL;
						unknownFieldTypeWarningMsg* unknownFieldType = new unknownFieldTypeWarningMsg();
						unknownFieldType->warningMsg();
						if(unknownFieldType != NULL) {
							delete unknownFieldType;
							unknownFieldType = NULL;
						}
						break;

				}

				if (magneticField == NULL) {

					cannotGetMagneticFieldWarningMsg* cannotGetMagneticField = new cannotGetMagneticFieldWarningMsg();
					cannotGetMagneticField->warningMsg();
					if(cannotGetMagneticField != NULL) {
						delete cannotGetMagneticField;
						cannotGetMagneticField = NULL;
					}

					cannotAccessFrameworkField = true;

				}
				else
					magneticField->Init();

			}

			if (cannotAccessFrameworkField)
				readField();

#endif

		}

	}

}
void trackfinderInputFMagneticField::init(const char* _fileName, bool _isRootFile, const char* _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors, bool _disableAutomaticMagneticField) {

	this->disableAutomaticMagneticField = _disableAutomaticMagneticField;
	trackfinderInputMagneticField::init(_fileName, _isRootFile, _mapName, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors);

}
void trackfinderInputFMagneticField::init(std::string _fileName, bool _isRootFile, std::string _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors, bool _disableAutomaticMagneticField) {

	this->disableAutomaticMagneticField = _disableAutomaticMagneticField;
	trackfinderInputMagneticField::init(_fileName, _isRootFile, _mapName, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors);

}
void trackfinderInputFMagneticField::init(FairField* field, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors, bool _disableAutomaticMagneticField) {

	this->disableAutomaticMagneticField = _disableAutomaticMagneticField;
	trackfinderInputMagneticField::init(field, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors);

}
void trackfinderInputFMagneticField::init(FairField* field, const char* _fileName, bool _isRootFile, const char* _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors, bool _disableAutomaticMagneticField) {

	this->disableAutomaticMagneticField = _disableAutomaticMagneticField;
	if ((field == NULL) || (disableAutomaticMagneticField)) {

		if (field == NULL) {

			reReadMagneticFieldWarningMsg* reReadMagneticField = new reReadMagneticFieldWarningMsg();
			reReadMagneticField->warningMsg();
			if(reReadMagneticField != NULL) {
				delete reReadMagneticField;
				reReadMagneticField = NULL;
			}

		}

		trackfinderInputMagneticField::init(_fileName, _isRootFile, _mapName, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors);

	}
	else
		trackfinderInputMagneticField::init(field, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors);

}
void trackfinderInputFMagneticField::init(FairField* field, std::string _fileName, bool _isRootFile, std::string _mapName, unsigned short _magneticFieldIntegrationStepwidthPerStation, double _magneticFieldIntegrationFactor, unsigned short _numberOfMagnetfieldFactors, bool _disableAutomaticMagneticField) {

	this->disableAutomaticMagneticField = _disableAutomaticMagneticField;
	if ((field == NULL) || (disableAutomaticMagneticField)) {

		if (field == NULL) {

			reReadMagneticFieldWarningMsg* reReadMagneticField = new reReadMagneticFieldWarningMsg();
			reReadMagneticField->warningMsg();
			if(reReadMagneticField != NULL) {
				delete reReadMagneticField;
				reReadMagneticField = NULL;
			}

		}

		trackfinderInputMagneticField::init(_fileName, _isRootFile, _mapName, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors);

	}
	else
		trackfinderInputMagneticField::init(field, _magneticFieldIntegrationStepwidthPerStation, _magneticFieldIntegrationFactor, _numberOfMagnetfieldFactors);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputFMagneticField::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = trackfinderInputMagneticField::getReservedSizeOfData(0);
	returnValue += sizeof(disableAutomaticMagneticField);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputFMagneticField::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = trackfinderInputMagneticField::getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputFMagneticField::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = trackfinderInputMagneticField::getUsedSizeOfData(0);
	returnValue += sizeof(disableAutomaticMagneticField);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(trackfinderInputFMagneticField)
