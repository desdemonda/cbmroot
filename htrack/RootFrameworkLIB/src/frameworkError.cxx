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
//     - derived from errorHandling
//     - class for errors occuring while doing some framework operations
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:38:41 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/frameworkError.h"


/****************************************************************
 * CLASS frameworkError							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

frameworkError::frameworkError() 
  : errorHandling(ROOTFRAMEWORKLIB)
{

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

frameworkError::~frameworkError() {

}


/****************************************************************
 * CLASS folderNameNotFoundError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

folderNameNotFoundError::folderNameNotFoundError() 
  : frameworkError(), folderName()
{

  //	folderName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

folderNameNotFoundError::folderNameNotFoundError(std::string actualFolderName) 
  : frameworkError(), folderName(actualFolderName) 
{

  //	folderName = actualFolderName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

folderNameNotFoundError::~folderNameNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void folderNameNotFoundError::errorMsg() {

	std::string temp;

	temp = "The folder with the name '";
	temp += folderName;
	temp += "', is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS listOfBranchesNotFoundError					 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

listOfBranchesNotFoundError::listOfBranchesNotFoundError() 
  : frameworkError(),
    branchName()
{

  //	branchName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

listOfBranchesNotFoundError::listOfBranchesNotFoundError(std::string actualBranchName) 
  : frameworkError(),
    branchName(actualBranchName)
{

	branchName = actualBranchName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

listOfBranchesNotFoundError::~listOfBranchesNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void listOfBranchesNotFoundError::errorMsg() {

	std::string temp;

	temp = "The list of the branch with the name '";
	temp += branchName;
	temp += "', is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS listOfFoldersNotFoundError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

listOfFoldersNotFoundError::listOfFoldersNotFoundError() 
  : frameworkError(),
    folderName()
{

  //	folderName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

listOfFoldersNotFoundError::listOfFoldersNotFoundError(std::string actualFolderName) 
  : frameworkError(),
    folderName(actualFolderName)
{

  //	folderName = actualFolderName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

listOfFoldersNotFoundError::~listOfFoldersNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void listOfFoldersNotFoundError::errorMsg() {

	std::string temp;

	temp = "The list of the folder with the name '";
	temp += folderName;
	temp += "', is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotMakeListIteratorError					 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotMakeListIteratorError::cannotMakeListIteratorError() 
: frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotMakeListIteratorError::~cannotMakeListIteratorError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotMakeListIteratorError::errorMsg() {

	printMsg("Cannot make the iterator for the list!!!");

}


/****************************************************************
 * CLASS cannotAddFolderError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAddFolderError::cannotAddFolderError() 
  : frameworkError(),
    folderName()
{

  //	folderName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAddFolderError::cannotAddFolderError(std::string actualFolderName) 
  : frameworkError(),
    folderName(actualFolderName)
{

  //	folderName = actualFolderName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAddFolderError::~cannotAddFolderError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAddFolderError::errorMsg() {

	std::string temp;

	temp = "The folder with the name '";
	temp += folderName;
	temp += "', cannot be added!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS treeNameNotFoundError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

treeNameNotFoundError::treeNameNotFoundError() 
  : frameworkError(),
    treeName()
 {

   //	treeName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

treeNameNotFoundError::treeNameNotFoundError(std::string actualTreeName) 
  : frameworkError(),
    treeName(actualTreeName)
{

  //	treeName = actualTreeName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

treeNameNotFoundError::~treeNameNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void treeNameNotFoundError::errorMsg() {

	std::string temp;

	temp = "The tree with the name '";
	temp += treeName;
	temp += "', is not found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotAccessBranchesError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessBranchesError::cannotAccessBranchesError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessBranchesError::~cannotAccessBranchesError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessBranchesError::errorMsg() {

	printMsg("Some branches in the tree are not accessible!!!");

}


/****************************************************************
 * CLASS noOutputTreeError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noOutputTreeError::noOutputTreeError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noOutputTreeError::~noOutputTreeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noOutputTreeError::errorMsg() {

	printMsg("There is no tree for the output!!!");

}


/****************************************************************
 * CLASS noOutputFileError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noOutputFileError::noOutputFileError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noOutputFileError::~noOutputFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noOutputFileError::errorMsg() {

	printMsg("There is no file for the output!!!");

}


/****************************************************************
 * CLASS cannotAccessHitsError									*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHitsError::cannotAccessHitsError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHitsError::~cannotAccessHitsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHitsError::errorMsg() {

	printMsg("The hits are not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessPointError									*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessPointError::cannotAccessPointError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessPointError::~cannotAccessPointError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessPointError::errorMsg() {

	printMsg("Some points are not accessible!!!");

}


/****************************************************************
 * CLASS cannotSmearError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotSmearError::cannotSmearError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotSmearError::~cannotSmearError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotSmearError::errorMsg() {

	printMsg("The smearing cannot be done!!!");

}


/****************************************************************
 * CLASS noInputFileError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noInputFileError::noInputFileError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noInputFileError::~noInputFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noInputFileError::errorMsg() {

	printMsg("There is no file for the input!!!");

}


/****************************************************************
 * CLASS noOutputFolderError									*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noOutputFolderError::noOutputFolderError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noOutputFolderError::~noOutputFolderError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noOutputFolderError::errorMsg() {

	printMsg("There is no folder for the output!!!");

}


/****************************************************************
 * CLASS noTreeError											*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noTreeError::noTreeError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noTreeError::~noTreeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noTreeError::errorMsg() {

	printMsg("There is no tree!!!");

}


/****************************************************************
 * CLASS noDetectorTypeError									*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noDetectorTypeError::noDetectorTypeError() 
  : frameworkError() 
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noDetectorTypeError::~noDetectorTypeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noDetectorTypeError::errorMsg() {

	printMsg("The pointer to a detector type is NULL!!!");

}
