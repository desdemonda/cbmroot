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
///     - The class handles the usage of the three neccessary tables
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:39:20 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TABLES_H
#define _TABLES_H


#include "../../AnalysisLIB/include/analysis.h"
#include "../../DataObjectLIB/include/table.h"


enum {NOTABLE, CROSSFOOTTABLE, LUTGOODNESSTABLE, FILETABLE, ONLINETABLE};


class analysis;


/* **************************************************************
 * CLASS tables									 				*
 * **************************************************************/

class tables {

 private:
  tables(const tables&);
  tables& operator=(const tables&);

protected:

	int         codingTableMode;
	table       codingTable;
	std::string codingTableName;
	int         gradingPTableMode;
	table       gradingPTable;
	std::string gradingPTableName;
	int         gradingRTableMode;
	table       gradingRTable;
	std::string gradingRTableName;
	analysis*   analyser;

/**
 * method generates a table which has the same classes than signatures
 */

	void generateNoTable(table& actualTable);

/**
 * method generates a table which has the as classes the crossfoot of the signatures
 */

	void generateCrossfootTable(table& actualTable);

/**
 * method generates a table based on the analysis::evaluateLutGoodness() function
 */

	void generateLutGoodnessTable(table& actualTable);

/**
 * method generates a table based on the detector layout
 */

	void generateOnlineTable(table& actualTable);

public:

/**
 * Default constructor
 */

	tables();

/**
 * Constructor
 */

	tables(int codingTableMode, int gradingPTableMode, int gradingRTableMode, std::string codingTableName = "", std::string gradingPTableName = "", std::string gradingRTableName = "");

/**
 * Destructor
 */

	virtual ~tables();

/**
 * method initializes the tables
 */

	void initCoding(int codingTableMode, std::string codingTableName = "");
	void initGradingP(int gradingPTableMode, std::string gradingPTableName = "");
	void initGradingR(int gradingRTableMode, std::string gradingRTableName = "");
	void init(int codingTableMode, int gradingPTableMode, int gradingRTableMode, std::string codingTableName = "", std::string gradingPTableName = "", std::string gradingRTableName = "");

/**
 * method updates the tables belonging to LUTGOODNESSTABLE
 */

	void update();

/**
 * method sets the analysis object which is used for LUTGOODNESSTABLE and ONLINETABLE
 */

	void setAnalyser(analysis* analyser);

/**
 * method returns the classification for the given signature
 */

	bitArray getCodingTableClassification(bitArray& signature);

/**
 * method returns the maximal classification of this table
 */

	bitArray getCodingTableMaximumClassification();

/**
 * method returns the minimal classification of this table
 */

	bitArray getCodingTableMinimumClassification();

/**
 * method sets the minimal classification of this table
 */

	void setCodingTableMinimumClassification(bitArray value);

/**
 * method reads a file to get the table
 * @param fileName is the name of the file to read the data
 * @param terminal is a buffer to place the process information
 */

	void readCodingTable(std::string fileName = "", std::streambuf* terminal = NULL);

/**
 * method writes a file representing the table
 * @param fileName is the name of the file to write the data
 * @param name is the name of the table which should be written to file
 * @param terminal is a buffer to place the process information
 */

	void writeCodingTable(std::string fileName = "", std::string name = "", std::streambuf* terminal = NULL);

/**
 * method returns the classification for the given signature
 */

	bitArray getGradingPTableClassification(bitArray& signature);

/**
 * method returns the maximal classification of this table
 */

	bitArray getGradingPTableMaximumClassification();

/**
 * method returns the minimal classification of this table
 */

	bitArray getGradingPTableMinimumClassification();

/**
 * method sets the minimal classification of this table
 */

	void setGradingPTableMinimumClassification(bitArray value);

/**
 * method reads a file to get the table
 * @param fileName is the name of the file to read the data
 * @param terminal is a buffer to place the process information
 */

	void readGradingPTable(std::string fileName = "", std::streambuf* terminal = NULL);

/**
 * method writes a file representing the table
 * @param fileName is the name of the file to write the data
 * @param name is the name of the table which should be written to file
 * @param terminal is a buffer to place the process information
 */

	void writeGradingPTable(std::string fileName = "", std::string name = "", std::streambuf* terminal = NULL);

/**
 * method returns the classification for the given signature
 */

	bitArray getGradingRTableClassification(bitArray& signature);

/**
 * method returns the maximal classification of this table
 */

	bitArray getGradingRTableMaximumClassification();

/**
 * method returns the minimal classification of this table
 */

	bitArray getGradingRTableMinimumClassification();

/**
 * method sets the minimal classification of this table
 */

	void setGradingRTableMinimumClassification(bitArray value);

/**
 * method reads a file to get the table
 * @param fileName is the name of the file to read the data
 * @param terminal is a buffer to place the process information
 */

	void readGradingRTable(std::string fileName = "", std::streambuf* terminal = NULL);

/**
 * method writes a file representing the table
 * @param fileName is the name of the file to write the data
 * @param name is the name of the table which should be written to file
 * @param terminal is a buffer to place the process information
 */

	void writeGradingRTable(std::string fileName = "", std::string name = "", std::streambuf* terminal = NULL);

/**
 * method returns the coding table
 */

	table& getCodingTable();

/**
 * method returns the gradingP table
 */

	table& getGradingPTable();

/**
 * method returns the gradingR table
 */

	table& getGradingRTable();

/**
 * method returns a std::string representing some information
 */

	std::string getInfo();

};

#endif
