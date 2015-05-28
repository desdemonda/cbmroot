/** @file  CbmMCDataManager
 ** @brief An entry point for time-based analysis
 ** @author //Dr.Sys <mikhail.prokudin@cern.ch>
**/

#ifndef CBMMCDATAMANAGER_H
#define CBMMCDATAMANAGER_H 1

#include "FairTask.h"

#include <map>
#include <list>
#include <vector>

class CbmMCDataArray;


using std::map;
using std::list;
using std::vector;


/** @class CbmMCDataManager
 ** @brief Task class creating and managing CbmMCDataArray objects
 ** @author //Dr.Sys <mikhail.prokudin@cern.ch>
 ** @date 31 March 2015
 **/
class CbmMCDataManager : public FairTask
{

	public:

		/** Default constructor **/
		CbmMCDataManager();


		/** Standard constructor
		 ** @param name    Task name
		 ** @param legacy  If true, run in legacy mode (using FairRootManager)
	   **/
		CbmMCDataManager(const char* name, Int_t legacy=0);


		/** Destructor **/
	  ~CbmMCDataManager();


		/** Add a MC input file
		 ** @param fileName  Input file name
		 ** @value           Running number of input (chain)
		 **
		 ** A new input chain will be started with this file.
		 **/
		Int_t AddFile(const char* fileName);

		/** Add a MC file to an existing input chain
		 ** @param fileName     Input file name
		 ** @param chainNumber  Number of input chain to which the file will be added
		 ** @value              Size of input chain (number of files in chain)
		 **/
		Int_t AddFileToChain(const char* fileName, Int_t chainNumber);


		/** Execution (empty; just to match FairTask structure) **/
		virtual void Exec(Option_t* option) { };


		/** End of event action **/
		virtual void FinishEvent();


		/** Initialisation **/
		virtual InitStatus Init();


		/** Create a CbmMCDataArray for a given branch name
		 ** @param branchName   Name of data branch
		 ** @value              Pointer to CbmMCDataArray object for this branch
		 **/
		CbmMCDataArray* InitBranch(const char* name);


	private:

		Int_t fLegacy; ///< If true, use FairRootManager in event-by-event mode

		/** Array of chains, each with a list of file names **/
    vector<list<TString> > fFileList;		//! Array of filename chains

    /** List of created CbmMCDataArrays **/
    map<TString, CbmMCDataArray*> fActive;			//!


    ClassDef(CbmMCDataManager, 1);
};

#endif

