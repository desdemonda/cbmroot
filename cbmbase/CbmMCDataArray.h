/** @file  CbmMCDataArray.h
 ** @author //Dr.Sys <mikhail.prokudin@cern.ch>
 **/

#ifndef CBMMCDATAARRAY_H
#define CBMMCDATAARRAY_H

#include "CbmLink.h"
#include "TString.h"

#include <vector>
#include <map>
#include <list>

class TChain;
class TClonesArray;


using std::list;
using std::map;
using std::vector;


/** @class CbmMCDataArray
 ** @brief Access to a MC data branch for time-based analysis
 ** @author //Dr.Sys <mikhail.prokudin@cern.ch>
 ** @date 31 March 2015
 **
 ** This class provides cached access to data for an event-by-event MC data branch,
 ** which is used when matching time-based reconstruction data to the input MC data.
 ** Data arrays from already used events are cached, which allows switching between
 ** events without the performance deterioration induced by frequent calls to
 ** TTree::GetEntry().
 ** In legacy mode, the data from the current event will be retrieved from
 ** FairRootManager.
 **/
class CbmMCDataArray
{
		friend class CbmMCDataManager;

	public:

		/** Destructor **/
		virtual ~CbmMCDataArray() { }


		/** Get an object by CbmLink pointer
		 ** @param link Pointer to CbmLink object
		 ** @value      Pointer to linked object
		 **/
		TObject* Get(const CbmLink* lnk) {
			return Get(lnk->GetFile(), lnk->GetEntry(), lnk->GetIndex());
		}


		/** Get an object by CbmLink reference
		 ** @param link Reference to CbmLink object
		 ** @value      Pointer to linked object
		 **/
		TObject* Get(const CbmLink& lnk) {
			return Get(lnk.GetFile(), lnk.GetEntry(), lnk.GetIndex());
		}


		/** Get an object by file number, event number and index
		 ** @param fileNumber	  Input file number
		 ** @param eventNumber  Event number
		 ** @param index        Index of desired object in its array
		 ** @value              Pointer to referenced object
		 **/
		TObject* Get(Int_t fileNumber, Int_t eventNumber, Int_t index);


	private:

		/** Default constructor. Should be called from CbmMCDataManager. **/
		CbmMCDataArray() : fLegacy(0),
			                 fLegacyArray(NULL),
			                 fBranchName(),
			                 fSize(0),
			                 fChains(),
			                 fTArr(),
			                 fN(0),
			                 fArrays() { }
  
		/** Standard constructor. Should be called from CbmMCDataManager.
		 ** @param branchName   Name of data branch
		 ** @param fileList     Vector of file list (one for each input source)
		 **/
		CbmMCDataArray(const char* branchName,
						       const vector<list<TString> >& fileList);

	  /** Constructor in legacy mode. Gets the branch from FairRootManager.
	   ** @param branchName  Name of data branch
	   **/
	  CbmMCDataArray(const char* branchName);


		/** Copy constructor. Should not be called. **/
		CbmMCDataArray(const CbmMCDataArray&);


		/** Done. Clear all arrays and delete file chain. **/
		void Done();


		/** Finish event. Delete all arrays to free memory. **/
		void FinishEvent();


		/** Get an object in legacy mode
		 ** @param fileNumber   Input number
		 ** @param eventNumber  Event number
		 ** @param index        Index of desired object in its array
		 ** @value              Pointer to referenced object
		 **/
		TObject* LegacyGet(Int_t fileNumber, Int_t eventNumber, Int_t index);


		//----   Private data members -------
		Int_t fLegacy;					     //! If true, run in legacy mode
		TClonesArray* fLegacyArray;	 //! Pointer to TClonesArray for legacy mode
		TString fBranchName;				 //! Name of the data branch
		Int_t fSize;						     //! Number of input file lists (one per source)
		vector<TChain*> fChains;		 //! Arrays of chains (one per input source)
		vector<TClonesArray*> fTArr; //! Data arrays from chains (one per input source)
		vector<Long64_t> fN;				 //! Number of entries in chains

		/** Cached data arrays. The vector index is the input source number, the map
		 ** index is the event number.
		 **/
		vector<map<Int_t, TClonesArray*> > fArrays;	//!

                CbmMCDataArray& operator=(const CbmMCDataArray&);

		ClassDef(CbmMCDataArray, 1);
};

#endif
