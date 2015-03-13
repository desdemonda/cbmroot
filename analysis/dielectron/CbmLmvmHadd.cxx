

#include <sstream>
#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
#include "CbmLmvmHadd.h"



ClassImp(CbmLmvmHadd);

void CbmLmvmHadd::AddFiles(
		const string& dir,
		const string& fileTamplate,
		const string& addString,
		Int_t nofFiles)
{
    if (addString == "analysis") { // for analysisi add up all particles
		string particleDir[4] = {"omegaepem", "phi", "omegadalitz", "rho0"};
		for (int iF = 0; iF < 4; iF++){
			AddFilesForParticle(particleDir[iF], dir, fileTamplate, addString, nofFiles);
		}//iF
    }

    if (addString == "litqa") { // for litqa add up only rho0
		AddFilesForParticle("rho0", dir, fileTamplate, addString, nofFiles);
    }
}

void CbmLmvmHadd::AddFilesForParticle(
		const string& particleDir,
		const string& dir,
		const string& fileTamplate,
		const string& addString,
		Int_t nofFiles)
{
	Int_t maxNofFiles = 100;
	string fullDir = dir + particleDir + "/";
	string fileNameAna = fullDir + string("analysis") + fileTamplate;
	string fileNameReco = fullDir + string("reco") + fileTamplate;
	string fileNameQa = fullDir + string("litqa") + fileTamplate;

	cout << "-I- " << dir << endl;
	int count = 0;
	TList* fileList = new TList();
	TList* tempTargetFiles = new TList();
	Int_t targetFileNum = 0;
	for (int i = 1; i < nofFiles; i++){
		stringstream ss;
		ss.fill('0');
		ss.width(5);
		ss  << i << ".root";

		TFile* fileAna = TFile::Open( (fileNameAna + ss.str() ).c_str(), "READ");
		TFile* fileReco = TFile::Open( (fileNameReco + ss.str() ).c_str(), "READ");

		Bool_t isGoodFile = CheckFile(fileAna, fileReco);
		if (fileReco != NULL) fileReco->Close();
		if ( isGoodFile ){
			if (addString == "analysis"){
				fileList->Add( fileAna );
				count++;
			}
			if (addString == "litqa"){
				TFile* fileQa = TFile::Open( (fileNameQa + ss.str() ).c_str(), "READ");
				Bool_t isGoodQaFile = CheckQaFile(fileQa);
				if (isGoodQaFile){
					fileList->Add( fileQa );
					count++;
				} else {
					if (fileQa != NULL) fileQa->Close();
				}
				if (fileAna != NULL) fileAna->Close();
			}
		} else {
			if ( fileAna != NULL) fileAna->Close();
			if ( fileReco != NULL) fileReco->Close();
		}

		if (fileList->GetEntries() >= maxNofFiles || i == nofFiles -1) {
			TFile* tf = CreateAndMergeTempTargetFile(fullDir, addString, targetFileNum, fileList);
			tempTargetFiles->Add(tf);
			CloseFilesFromList(fileList);
			fileList->RemoveAll();
			targetFileNum++;
		}
	}
	cout << endl<< "-I- number of files to merge = " << count << endl << endl;

	string outputFile = fileNameAna + "all.root";
	if (addString == "litqa") outputFile = fileNameQa + "all.root";
	cout << "-I- OUTPUT: " << outputFile << endl;
	TFile* targetFile = TFile::Open( outputFile.c_str(), "RECREATE" );
	MergeRootfile( targetFile, tempTargetFiles );

	CloseFilesFromList(tempTargetFiles);
	targetFile->Close();

	if (targetFile != NULL) delete targetFile;
	if (tempTargetFiles != NULL) delete tempTargetFiles;
	if (fileList != NULL) delete fileList;
}

TFile* CbmLmvmHadd::CreateAndMergeTempTargetFile(
		const string& dir,
		const string& addString,
		Int_t targetFileNum,
		TList* fileList)
{
	cout << "-I- CreateAndMergeTempTargetFile no:" << targetFileNum << ", nofFiles:" << fileList->GetEntries() << endl;
	stringstream ss;
	ss  << targetFileNum << ".root";
	TFile* targetFile = TFile::Open( string(dir + addString + ".temp.taget.file." + ss.str()).c_str(), "RECREATE" );
	MergeRootfile( targetFile, fileList );
	return targetFile;
}

void CbmLmvmHadd::CloseFilesFromList(
	    TList* fileList)
{
	int nFL = fileList->GetEntries();
	for (int iFL = 0; iFL < nFL; iFL++){
		TFile* f = (TFile*)fileList->At(iFL);
		f->Close();
		delete f;
	}
}

Bool_t CbmLmvmHadd::CheckQaFile(
		TFile* fileQa)
{
    if (fileQa == NULL) return false;
    if (fileQa->GetEND() < 50000) return false;

    return true;
}

Bool_t CbmLmvmHadd::CheckFile(
		TFile* fileAna,
		TFile* fileReco) {

    if (fileAna == NULL || fileReco == NULL)  return false;

    if (fileAna->GetEND() < 50000 || fileReco->GetEND() < 50000) return false;

    TTree* treeAna = (TTree*)fileAna->Get("cbmsim");
    TTree* treeReco = (TTree*)fileReco->Get("cbmsim");

    if (treeAna == NULL || treeReco == NULL) return false;

    Long64_t treeSizeAna = treeAna->GetEntriesFast();
    Long64_t treeSizeReco = treeReco->GetEntriesFast();

    if (treeSizeAna == 1000 && treeSizeReco == 1000) return true;

    return false;
}

void CbmLmvmHadd::MergeRootfile(
		TDirectory *target,
		TList *sourcelist ) {

   //  cout << "Target path: " << target->GetPath() << endl;
   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove( 0, 2 );

   TFile *first_source = (TFile*)sourcelist->First();
   first_source->cd( path );
   TDirectory *current_sourcedir = gDirectory;
   //gain time, do not add the objects in the list in memory
   Bool_t status = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);

   // loop over all keys in this directory
   TChain *globChain = 0;
   TIter nextkey( current_sourcedir->GetListOfKeys() );
   TKey *key, *oldkey=0;
   while ( (key = (TKey*)nextkey())) {
      //keep only the highest cycle number for each key
      if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

      // read object from first source file
      first_source->cd( path );
      TObject *obj = key->ReadObj();

      if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) {
         // descendant of TH1 -> merge it

         //      cout << "Merging histogram " << obj->GetName() << endl;
         TH1 *h1 = (TH1*)obj;

         // loop over all source files and add the content of the
         // correspondant histogram to the one pointed to by "h1"
         TFile *nextsource = (TFile*)sourcelist->After( first_source );
         while ( nextsource ) {
            // make sure we are at the correct directory level by cd'ing to path
            nextsource->cd( path );
            TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(h1->GetName());
            if (key2) {
               TH1 *h2 = (TH1*)key2->ReadObj();
               h1->Add( h2 );
               delete h2;
            }
           // cout << nextsource->GetPath() << endl;
            nextsource = (TFile*)sourcelist->After( nextsource );
         }
      }
//      else if ( obj->IsA()->InheritsFrom( TTree::Class() ) ) {
//
//         // loop over all source files create a chain of Trees "globChain"
//         const char* obj_name= obj->GetName();
//
//         globChain = new TChain(obj_name);
//         globChain->Add(first_source->GetName());
//         TFile *nextsource = (TFile*)sourcelist->After( first_source );
//         //      const char* file_name = nextsource->GetName();
//         // cout << "file name  " << file_name << endl;
//         while ( nextsource ) {
//
//            globChain->Add(nextsource->GetName());
//            nextsource = (TFile*)sourcelist->After( nextsource );
//         }
//
//      } else if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
//         // it's a subdirectory
//
//         cout << "Found subdirectory " << obj->GetName() << endl;
//
//         // create a new subdir of same name and title in the target file
//         target->cd();
//         TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );
//
//         // newdir is now the starting point of another round of merging
//         // newdir still knows its depth within the target file via
//         // GetPath(), so we can still figure out where we are in the recursion
//         MergeRootfile( newdir, sourcelist );
//
//      } else {
//
//         // object is of no type that we know or can handle
//         cout << "Unknown object type, name: "
//         << obj->GetName() << " title: " << obj->GetTitle() << endl;
//      }

      // now write the merged histogram (which is "in" obj) to the target file
      // note that this will just store obj in the current directory level,
      // which is not persistent until the complete directory itself is stored
      // by "target->Write()" below
      if ( obj ) {
         target->cd();

         //!!if the object is a tree, it is stored in globChain...
//         if(obj->IsA()->InheritsFrom( TTree::Class() ))
//            globChain->Merge(target->GetFile(),0,"keep");
//         else
            obj->Write( key->GetName() );
      }

   } // while ( ( TKey *key = (TKey*)nextkey() ) )

   // save modifications to target file
   target->SaveSelf(kTRUE);
   TH1::AddDirectory(status);
}
