//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl�
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
///     - The class handles the usage of the three neccessary tables
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:39:20 $
/// $Revision: 1.11 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/dataRootObjectWarningMsg.h"
#include "../include/tables.h"


#define CONSECUTIVEONLINELAYERS		/**< This definition enables that just layers with consecutive hits are accepted in the online generation of the table. */


/* **************************************************************
 * CLASS tables									 				*
 * **************************************************************/

/* **************************************************************
 * method generates a table which has the same classes than		*
 * signatures													*
 * **************************************************************/

void tables::generateNoTable(table& actualTable) {

	unsigned int numberOfTableBits;
	bitArray     actualEntry;

	numberOfTableBits = (unsigned int)actualEntry.length();

	if (analyser != NULL) {

		if (analyser->getNumberOfActiveStations() > 0) {

			numberOfTableBits = analyser->getNumberOfActiveStations();

		}

	}

	for (unsigned long i = 0; i < (unsigned long)(1 << numberOfTableBits); i++) {

		actualEntry = bitArray(i);
		actualTable.addEntry(actualEntry, actualEntry);

	}

}

/* **************************************************************
 * method generates a table which has the as classes the		*
 * crossfoot of the signatures									*
 * **************************************************************/

void tables::generateCrossfootTable(table& actualTable) {

	unsigned int numberOfTableBits;
	bitArray     actualSignatureEntry;
	bitArray     actualClassificationEntry;

	numberOfTableBits = (unsigned int)actualSignatureEntry.length();

	if (analyser != NULL) {

		if (analyser->getNumberOfActiveStations() > 0) {

			numberOfTableBits = analyser->getNumberOfActiveStations();

		}

	}

	for (unsigned long i = 0; i < (unsigned long)(1 << numberOfTableBits); i++) {

		actualSignatureEntry      = bitArray(i);
		actualClassificationEntry = bitArray((unsigned long)actualSignatureEntry.count());
		actualTable.addEntry(actualSignatureEntry, actualClassificationEntry);

	}

}

/* **************************************************************
 * method generates a table based on the						*
 * analysis::evaluateLutGoodness() function						*
 * **************************************************************/

void tables::generateLutGoodnessTable(table& actualTable) {

	unsigned int   numberOfTableBits;
	unsigned int   i;
	bitArray       classification;
	unsigned int*  distribution;
	unsigned short actualNumberOfTracksForSignatureAcceptance;
	unsigned short minimalNumberOfTracksForSignatureAcceptance;
	bitArray       actualSignatureForSignatureAcceptance;
	bitArray       searchSignatureForSignatureAcceptance;

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	unsigned short actualClassForSignatureAcceptance;
	bool           searchFoundForSignatureAcceptance;

#endif

	if (analyser != NULL) {

		numberOfTableBits = (unsigned int)classification.length();

		if (analyser->getNumberOfActiveStations() > 0) {

			numberOfTableBits = analyser->getNumberOfActiveStations();

		}

		classification = bitArray(1);

		distribution  = new unsigned int[1 << numberOfTableBits];
		if (distribution == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		for (i = 0; i < (unsigned int)(1 << numberOfTableBits); i++)
			distribution[i] = 0;

		analyser->evaluateBothLutsDistribution(distribution);

		actualNumberOfTracksForSignatureAcceptance  = 0;
		minimalNumberOfTracksForSignatureAcceptance = 0;
		for (i = 0; i < (unsigned int)(1 << numberOfTableBits); i++)
			minimalNumberOfTracksForSignatureAcceptance += distribution[i];
		minimalNumberOfTracksForSignatureAcceptance = (unsigned short)(((double)analyser->getPercentageOfTracksForSignature() / (double)100) * (double)minimalNumberOfTracksForSignatureAcceptance);

#ifndef HIGHOCCURENCEISGOODSIGNATURE

		actualClassForSignatureAcceptance = (unsigned short)numberOfTableBits;

#endif

		while (minimalNumberOfTracksForSignatureAcceptance > actualNumberOfTracksForSignatureAcceptance) {

			searchSignatureForSignatureAcceptance = bitArray(0);

#ifdef HIGHOCCURENCEISGOODSIGNATURE

			for (i = 0; i < (unsigned int)(1 << numberOfTableBits); i++) {

				actualSignatureForSignatureAcceptance = bitArray(i);

				if (distribution[i] > distribution[searchSignatureForSignatureAcceptance.toULong()])
					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
				else if (distribution[i] == distribution[searchSignatureForSignatureAcceptance.toULong()])
					if (actualSignatureForSignatureAcceptance.count() > searchSignatureForSignatureAcceptance.count())
						searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;

			}

			actualTable.sumEntry(searchSignatureForSignatureAcceptance, classification);
			actualNumberOfTracksForSignatureAcceptance                    += distribution[searchSignatureForSignatureAcceptance.toULong()];
			distribution[searchSignatureForSignatureAcceptance.toULong()]  = 0;

#else

			searchFoundForSignatureAcceptance = false;

			for (i = 0; i < (unsigned int)(1 << numberOfTableBits); i++) {

				actualSignatureForSignatureAcceptance = bitArray(i);
		
				if (actualSignatureForSignatureAcceptance.count() == actualClassForSignatureAcceptance) {

					if (distribution[i] > distribution[searchSignatureForSignatureAcceptance.toULong()]) {

						searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
						searchFoundForSignatureAcceptance     = true;

					}

				}

			}

			if (searchFoundForSignatureAcceptance) {

				actualTable.sumEntry(searchSignatureForSignatureAcceptance, classification);
				actualNumberOfTracksForSignatureAcceptance                    += distribution[searchSignatureForSignatureAcceptance.toULong()];
				distribution[searchSignatureForSignatureAcceptance.toULong()]  = 0;

			}
			else
				actualClassForSignatureAcceptance--;

#endif

		}

		if (distribution != NULL) {
			delete[] distribution;
			distribution = NULL;
		}

	}
	else {

		analyserIsNotAccessibleWarningMsg* analyserIsNotAccessible = new analyserIsNotAccessibleWarningMsg();
		analyserIsNotAccessible->warningMsg();
		if(analyserIsNotAccessible != NULL) {
			delete analyserIsNotAccessible;
			analyserIsNotAccessible = NULL;
		}

	}

}

/* **************************************************************
 * method generates a table based on the detector layout		*
 * **************************************************************/

void tables::generateOnlineTable(table& actualTable) {

	unsigned int   numberOfTableBits;
	unsigned short minHits;
	bitArray       stopSignature;
	bitArray       actualSignatureEntry;
	bitArray       actualClassificationEntry;

	if (analyser != NULL) {

		numberOfTableBits = (unsigned int)stopSignature.length();
		stopSignature     = bitArray(0);

		if ((analyser->getNumberOfActiveStations() > 0) && (analyser->getNumberOfActiveStations() < (unsigned int)stopSignature.length())) {

			numberOfTableBits = analyser->getNumberOfActiveStations();
			stopSignature     = bitArray(1 << numberOfTableBits);

		}

		minHits = (unsigned short)ceil(((double)analyser->getPercentageOfHitsInSignature() / (double)100) * (double)numberOfTableBits);

		/* initialize first signature */
		actualSignatureEntry = bitArray(0);

		/* check if signature is acceptable */

#ifdef CONSECUTIVEONLINELAYERS

		if ((unsigned short)actualSignatureEntry.count_consecutive() >= minHits) {

#else

		if ((unsigned short)actualSignatureEntry.count() >= minHits) {

#endif
			actualClassificationEntry = bitArray((unsigned long)actualSignatureEntry.count() - minHits + 1);
			actualTable.addEntry(actualSignatureEntry, actualClassificationEntry);

		}

		/* go for next signature */
		actualSignatureEntry++;

		/* repeat until first signature is reached again */
		while (actualSignatureEntry != stopSignature) {

			/* check if signature is acceptable */

#ifdef CONSECUTIVEONLINELAYERS

			if ((unsigned short)actualSignatureEntry.count_consecutive() >= minHits) {

#else

			if ((unsigned short)actualSignatureEntry.count() >= minHits) {

#endif
				actualClassificationEntry = bitArray((unsigned long)actualSignatureEntry.count() - minHits + 1);
				actualTable.addEntry(actualSignatureEntry, actualClassificationEntry);

			}

			/* go for next signature */
			actualSignatureEntry++;

		}

	}
	else {

		analyserIsNotAccessibleWarningMsg* analyserIsNotAccessible = new analyserIsNotAccessibleWarningMsg();
		analyserIsNotAccessible->warningMsg();
		if(analyserIsNotAccessible != NULL) {
			delete analyserIsNotAccessible;
			analyserIsNotAccessible = NULL;
		}

	}

}

/* **************************************************************
 * Default constructor											*
 * **************************************************************/

tables::tables() 
  : codingTableMode(0),
  codingTable(),
  codingTableName(),
  gradingPTableMode(0),
  gradingPTable(),
  gradingPTableName(),
  gradingRTableMode(0),
  gradingRTable(),
  gradingRTableName(),
  analyser(NULL)
{

  //	analyser = NULL;

	init(NOTABLE, NOTABLE, NOTABLE, "", "", "");

}

/* **************************************************************
 * Constructor													*
 * **************************************************************/

tables::tables(int _codingTableMode, int _gradingPTableMode, int _gradingRTableMode, std::string _codingTableName, std::string _gradingPTableName, std::string _gradingRTableName) 
  : codingTableMode(_codingTableMode),
  codingTable(),
  codingTableName(_codingTableName),
  gradingPTableMode(_gradingPTableMode),
  gradingPTable(),
  gradingPTableName(_gradingPTableName),
  gradingRTableMode(_gradingRTableMode),
  gradingRTable(),
  gradingRTableName(_gradingRTableName),
  analyser(NULL)
{

  //	analyser = NULL;

	init(_codingTableMode, _gradingPTableMode, _gradingRTableMode, _codingTableName, _gradingPTableName, _gradingRTableName);

}

/* **************************************************************
 * Destructor													*
 * **************************************************************/

tables::~tables() {

	codingTable.clear();
	codingTableName.clear();
	gradingPTable.clear();
	gradingPTableName.clear();
	gradingRTable.clear();
	gradingRTableName.clear();

}

/* **************************************************************
 * method initializes the tables								*
 * **************************************************************/

void tables::initCoding(int _codingTableMode, std::string _codingTableName) {

	this->codingTableMode   = _codingTableMode;
	codingTable.clear();
	this->codingTableName   = _codingTableName;

	switch (this->codingTableMode) {

		case NOTABLE:
			generateNoTable(codingTable);
			break;
		case CROSSFOOTTABLE:
			generateCrossfootTable(codingTable);
			break;
		case FILETABLE:
			readCodingTable();
			break;
		case ONLINETABLE:
			generateOnlineTable(codingTable);
			break;
		default:				// LUTGOODNESSTABLE:
			generateLutGoodnessTable(codingTable);
			break;

	}

}
void tables::initGradingP(int _gradingPTableMode, std::string _gradingPTableName) {

	this->gradingPTableMode = _gradingPTableMode;
	gradingPTable.clear();
	this->gradingPTableName = _gradingPTableName;

	switch (this->gradingPTableMode) {

		case NOTABLE:
			generateNoTable(gradingPTable);
			break;
		case CROSSFOOTTABLE:
			generateCrossfootTable(gradingPTable);
			break;
		case LUTGOODNESSTABLE: /* exception because it is prerequisite for the algorithm lutGoodness */
			generateOnlineTable(gradingPTable);
			break;
		case FILETABLE:
			readGradingPTable(this->gradingPTableName);
			break;
		default:				// ONLINETABLE:
			generateOnlineTable(gradingPTable);
			break;

	}

	if (this->gradingPTableMode == LUTGOODNESSTABLE) {

		lutGoodnessNotPossibleForGradingPWarningMsg* lutGoodnessNotPossibleForGradingP = new lutGoodnessNotPossibleForGradingPWarningMsg();	
		lutGoodnessNotPossibleForGradingP->warningMsg();
		if(lutGoodnessNotPossibleForGradingP != NULL) {
			delete lutGoodnessNotPossibleForGradingP;
			lutGoodnessNotPossibleForGradingP = NULL;
		}

	}

}
void tables::initGradingR(int _gradingRTableMode, std::string _gradingRTableName) {

	this->gradingRTableMode = _gradingRTableMode;
	gradingRTable.clear();
	this->gradingRTableName = _gradingRTableName;

	switch (this->gradingRTableMode) {

		case NOTABLE:
			generateNoTable(gradingRTable);
			break;
		case LUTGOODNESSTABLE:
			generateLutGoodnessTable(gradingRTable);
			break;
		case FILETABLE:
			readGradingRTable();
			break;
		case ONLINETABLE:
			generateOnlineTable(gradingRTable);
			break;
		default:				// CROSSFOOTTABLE:
			generateCrossfootTable(gradingRTable);
			break;

	}

}
void tables::init(int _codingTableMode, int _gradingPTableMode, int _gradingRTableMode, std::string _codingTableName, std::string _gradingPTableName, std::string _gradingRTableName) {

	initGradingP(_gradingPTableMode, _gradingPTableName);
	initCoding(_codingTableMode, _codingTableName);
	initGradingR(_gradingRTableMode, _gradingRTableName);

}

/* **************************************************************
 * method updates the tables belonging to LUTGOODNESSTABLE		*
 * **************************************************************/

void tables::update() {

	if (codingTableMode == LUTGOODNESSTABLE)
		generateLutGoodnessTable(codingTable);

	if (gradingRTableMode == LUTGOODNESSTABLE)
		generateLutGoodnessTable(gradingRTable);

}

/* **************************************************************
 * method sets the analysis object which is used for			*
 * LUTGOODNESSTABLE and ONLINETABLE								*
 * **************************************************************/

void tables::setAnalyser(analysis* _analyser) {

	this->analyser = _analyser;

}

/* **************************************************************
 * method returns the classification for the given signature	*
 * **************************************************************/

bitArray tables::getCodingTableClassification(bitArray& signature) {

	return codingTable.getClassification(signature);

}

/* **************************************************************
 * method returns the maximal classification of this table		*
 * **************************************************************/

bitArray tables::getCodingTableMaximumClassification() {

	return codingTable.getMaximumClassification();

}

/* **************************************************************
 * method returns the minimal classification of this table		*
 * **************************************************************/

bitArray tables::getCodingTableMinimumClassification() {

	return codingTable.getMinimumClassification();

}

/* **************************************************************
 * method sets the minimal classification of this table			*
 * **************************************************************/

void tables::setCodingTableMinimumClassification(bitArray value) {

	if (codingTable.getMinimumClassification() > value) {
	
		codingTable.setMinimumClassification(value);
		initCoding(codingTableMode, codingTableName);

	}
	else if (codingTable.getMinimumClassification() < value) {

		codingTable.setMinimumClassification(value);
		codingTable.update();

	}

}

/* **************************************************************
 * method reads a file to get the table							*
 * **************************************************************/

void tables::readCodingTable(std::string fileName, std::streambuf* terminal) {

	if (fileName.empty())
		codingTable.read(codingTableName, terminal);
	else
		codingTable.read(fileName, terminal);

}

/* **************************************************************
 * method writes a file representing the table					*
 * **************************************************************/

void tables::writeCodingTable(std::string fileName, std::string name, std::streambuf* terminal) {

	if (fileName.empty())
		if (name.empty())
			codingTable.write(codingTableName, codingTableName, "codingTable", true, terminal);
		else
			codingTable.write(codingTableName, name, "codingTable", true, terminal);
	else
		codingTable.write(fileName, name, "codingTable", true, terminal);

}

/* **************************************************************
 * method returns the classification for the given signature	*
 * **************************************************************/

bitArray tables::getGradingPTableClassification(bitArray& signature) {

	return gradingPTable.getClassification(signature);

}

/* **************************************************************
 * method returns the maximal classification of this table		*
 * **************************************************************/

bitArray tables::getGradingPTableMaximumClassification() {

	return gradingPTable.getMaximumClassification();

}

/* **************************************************************
 * method returns the minimal classification of this table		*
 * **************************************************************/

bitArray tables::getGradingPTableMinimumClassification() {

	return gradingPTable.getMinimumClassification();

}

/* **************************************************************
 * method sets the minimal classification of this table			*
 * **************************************************************/

void tables::setGradingPTableMinimumClassification(bitArray value) {

	if (gradingPTable.getMinimumClassification() > value) {
	
		gradingPTable.setMinimumClassification(value);
		initGradingP(gradingPTableMode, gradingPTableName);

	}
	else if (gradingPTable.getMinimumClassification() < value) {

		gradingPTable.setMinimumClassification(value);
		gradingPTable.update();

	}

}

/* **************************************************************
 * method reads a file to get the table							*
 * **************************************************************/

void tables::readGradingPTable(std::string fileName, std::streambuf* terminal) {

	if (fileName.empty())
		gradingPTable.read(gradingPTableName, terminal);
	else
		gradingPTable.read(fileName, terminal);

}

/* **************************************************************
 * method writes a file representing the table					*
 * **************************************************************/

void tables::writeGradingPTable(std::string fileName, std::string name, std::streambuf* terminal) {

	if (fileName.empty())
		if (name.empty())
			gradingPTable.write(gradingPTableName, gradingPTableName, "gradingPTable", true, terminal);
		else
			gradingPTable.write(gradingPTableName, name, "gradingPTable", true, terminal);
	else
		gradingPTable.write(fileName, name, "gradingPTable", true, terminal);

}

/* **************************************************************
 * method returns the classification for the given signature	*
 * **************************************************************/

bitArray tables::getGradingRTableClassification(bitArray& signature) {

	return gradingRTable.getClassification(signature);

}

/* **************************************************************
 * method returns the maximal classification of this table		*
 * **************************************************************/

bitArray tables::getGradingRTableMaximumClassification() {

	return gradingRTable.getMaximumClassification();

}

/* **************************************************************
 * method returns the minimal classification of this table		*
 * **************************************************************/

bitArray tables::getGradingRTableMinimumClassification() {

	return gradingRTable.getMinimumClassification();

}

/* **************************************************************
 * method sets the minimal classification of this table			*
 * **************************************************************/

void tables::setGradingRTableMinimumClassification(bitArray value) {

	if (gradingRTable.getMinimumClassification() > value) {
	
		gradingRTable.setMinimumClassification(value);
		initGradingR(gradingRTableMode, gradingRTableName);

	}
	else if (gradingRTable.getMinimumClassification() < value) {

		gradingRTable.setMinimumClassification(value);
		gradingRTable.update();

	}

}

/* **************************************************************
 * method reads a file to get the table							*
 * **************************************************************/

void tables::readGradingRTable(std::string fileName, std::streambuf* terminal) {

	if (fileName.empty())
		gradingRTable.read(gradingRTableName, terminal);
	else
		gradingRTable.read(fileName, terminal);

}

/* **************************************************************
 * method writes a file representing the table					*
 * **************************************************************/

void tables::writeGradingRTable(std::string fileName, std::string name, std::streambuf* terminal) {

	if (fileName.empty())
		if (name.empty())
			gradingRTable.write(gradingRTableName, gradingRTableName, "gradingRTable", true, terminal);
		else
			gradingRTable.write(gradingRTableName, name, "gradingRTable", true, terminal);
	else
		gradingRTable.write(fileName, name, "gradingRTable", true, terminal);

}

/* **************************************************************
 * method returns the coding table								*
 * **************************************************************/

table& tables::getCodingTable() {

	return codingTable;

}

/* **************************************************************
 * method returns the gradingP table							*
 * **************************************************************/

table& tables::getGradingPTable() {

	return gradingPTable;

}

/* **************************************************************
 * method returns the gradingR table							*
 * **************************************************************/

table& tables::getGradingRTable() {

	return gradingRTable;

}

/* **************************************************************
 * method returns a std::string representing some information	*
 * **************************************************************/

std::string tables::getInfo() {

	std::string returnValue;

	returnValue  = "CodingTable:\n";
	returnValue += codingTable.toString();
	returnValue += "GradingPTable:\n";
	returnValue += gradingPTable.toString();
	returnValue += "GradingRTable:\n";
	returnValue += gradingRTable.toString();

	return returnValue;

}
