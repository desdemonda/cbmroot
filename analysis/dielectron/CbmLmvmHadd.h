/**
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2015
 * @version 1.0
 **/


#ifndef CBM_LMVM_HADD_H
#define CBM_LMVM_HADD_H

#include "TObject.h"

using namespace std;

class CbmLmvmHadd: public TObject {

public:

	/*
	 * Add histograms from all files into one file.
	 */
	void AddFiles(
			const string& dir,
			const string& fileTamplate,
			const string& addString,
			Int_t nofFiles);



private:
	Bool_t CheckQaFile(
			TFile* fileQa);

	Bool_t CheckFile(
			TFile* fileAna,
			TFile* fileReco);

	void CloseFilesFromList(
			TList* fileList);

	void AddFilesForParticle(
			const string& particleDir,
			const string& dir,
			const string& fileTamplate,
			const string& addString,
			Int_t nofFiles);

	TFile* CreateAndMergeTempTargetFile(
			const string& dir,
			const string& addString,
			Int_t targetFileNum,
			TList* fileList);

	/* Merge root file into one.
	 * This function was taken from hadd.C macro.
	 */
	void MergeRootfile(
			TDirectory *target,
			TList *sourcelist );

	ClassDef(CbmLmvmHadd,1);
};


#endif
