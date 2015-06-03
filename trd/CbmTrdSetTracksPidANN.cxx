// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidANN source file          -----
// -----                  Created 06/03/07 by Simeon Lebedev           -----
// -------------------------------------------------------------------------
#include "CbmTrdSetTracksPidANN.h"

#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMultiLayerPerceptron.h"
#include "TTree.h"
#include "TMath.h"
#include "TSystem.h"
#include "TString.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

CbmTrdSetTracksPidANN::CbmTrdSetTracksPidANN() 
  : FairTask(),
    fTrackArray(NULL),
    fTrdHitArray(NULL),
    fNofTracks(0),
    fANNPar1(-1.),
    fANNPar2(-1.), 
    fNN(),
    fTRDGeometryType("h++")
{
}

CbmTrdSetTracksPidANN::CbmTrdSetTracksPidANN(
      const char* name,
      const char* title)
  : FairTask(name),
    fTrackArray(NULL),
    fTrdHitArray(NULL),
    fNofTracks(0),
    fANNPar1(-1.),
    fANNPar2(-1.), 
    fNN(),
    fTRDGeometryType("h++")
{
}


CbmTrdSetTracksPidANN::~CbmTrdSetTracksPidANN()
{
}

void CbmTrdSetTracksPidANN::SetParContainers()
{

}

Bool_t CbmTrdSetTracksPidANN::ReadData()
{
	string fileName = string(gSystem->Getenv("VMCWORKDIR"));
	vector<string> weightsFilesANN;

	if (fTRDGeometryType != "h++" ){
		cout << "-E- CbmTrdSetTracksPidANN::Init: " << fTRDGeometryType << " is wrong geometry type." << endl;
		return kFALSE;
	}
	fileName += "/parameters/trd/elid/ann/" + string(fTRDGeometryType)+"/";

	for (int i = 0; i < 10; i++){
	   stringstream ss;
	   ss << fileName << "ann_weights_" << (i+1) << ".txt";
	   weightsFilesANN.push_back(ss.str());
	}

	if (fTRDGeometryType == "h++"){
	   fANNPar1 = 1.06;
	   fANNPar2 = 0.57;
	}

	for (Int_t i = 0; i < weightsFilesANN.size(); i++) {
		ifstream myfile(weightsFilesANN[i].c_str());
		if (!myfile.is_open()) {
			cout << "-E- CbmTrdSetTracksPidANN::Init: "<< "Could not open input file:" << weightsFilesANN[i] <<  endl;
			weightsFilesANN[i] = "";
			//return kFALSE;
		}
		myfile.close();
	}

	Float_t inVector[10];
	Int_t xOut;//output value

	//init TTree as input data to neural net
	TTree *simu = new TTree ("MonteCarlo","MontecarloData");
	simu->Branch("x1", &inVector[0], "x1/F");
	simu->Branch("x2", &inVector[1], "x2/F");
	simu->Branch("x3", &inVector[2], "x3/F");
	simu->Branch("x4", &inVector[3], "x4/F");
	simu->Branch("x5", &inVector[4], "x5/F");
	simu->Branch("x6", &inVector[5], "x6/F");
	simu->Branch("x7", &inVector[6], "x7/F");
	simu->Branch("x8", &inVector[7], "x8/F");
	simu->Branch("x9", &inVector[8], "x9/F");
	simu->Branch("x10", &inVector[9], "x10/F");
	simu->Branch("xOut", &xOut, "xOut/I");

	fNN.clear();

	for (int iH = 0; iH < 10; iH++){
	   if (weightsFilesANN[iH] == ""){
	      fNN.push_back(NULL);
	      continue;
	   }
	   stringstream ss;
	   for (int iL = 0; iL <= iH; iL++){
	      if (iL != iH) ss << "x" << (iL+1) << ",";
	      int nofHidden = 2 * (iH+1);
	      if (iL == iH) ss << "x" << (iL+1) << ":" << nofHidden << ":xOut";
	   }
	   TMultiLayerPerceptron* ann = new TMultiLayerPerceptron(ss.str().c_str(), simu);
	   ann->LoadWeights(weightsFilesANN[iH].c_str());
	   fNN.push_back(ann);
	}

	return kTRUE;
}

InitStatus CbmTrdSetTracksPidANN::Init()
{
	if (!ReadData()) { Fatal("CbmTrdSetTracksPidANN::Init", "ReadData"); }

	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmTrdSetTracksPidANN::Init", "RootManager not instantised"); }

	fTrackArray = (TClonesArray*) ioman->GetObject("TrdTrack");
	if ( NULL == fTrackArray) { Fatal("CbmTrdSetTracksPidANN::Init", "No TrdTrack array");}

	fTrdHitArray = (TClonesArray*) ioman->GetObject("TrdHit");
	if ( NULL == fTrdHitArray) { Fatal("CbmTrdSetTracksPidANN::Init", "No TrdHit array");}

	return kSUCCESS;
}

void CbmTrdSetTracksPidANN::Exec(
      Option_t* opt)
{
	Int_t nTracks = fTrackArray->GetEntriesFast();
	std::vector<Double_t> eLossVector;

	for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {
		eLossVector.clear();
		CbmTrdTrack* pTrack = (CbmTrdTrack*)fTrackArray->At(iTrack);
		Int_t nofHits = pTrack->GetNofHits();
		for (Int_t iTRD=0; iTRD < nofHits; iTRD++) {
			Int_t index = pTrack->GetHitIndex(iTRD);
			CbmTrdHit* trdHit = (CbmTrdHit*) fTrdHitArray->At(index);
			eLossVector.push_back(trdHit->GetELoss());
		}

		//------------------transform Data BEGIN--------------------------
		for (UInt_t j = 0; j< eLossVector.size(); j++) {
			eLossVector[j] = eLossVector[j] * 1.e6;
			eLossVector[j] = (eLossVector[j] - fANNPar1)/fANNPar2-0.225;
		}
		sort(eLossVector.begin(), eLossVector.end());
		for (UInt_t j = 0; j<eLossVector.size(); j++) {
		  eLossVector[j] = TMath::LandauI(eLossVector[j]);
		}
		//------------------transform Data END----------------------------

		Int_t iANN = nofHits - 1;
		Double_t nnEval;
		if (iANN < 0 || iANN >= 12 || fNN[iANN] == NULL) {
		   nnEval = -2.;
		} else {
         nnEval = fNN[iANN]->Evaluate(0, &eLossVector[0]);
         if (TMath::IsNaN(nnEval) == 1) {
            cout << " -W- CbmTrdSetTracksPidANN: nnEval nan " << endl;
            nnEval = -2;
         }
		}
		pTrack->SetPidANN(nnEval);
	}
}

void CbmTrdSetTracksPidANN::Finish()
{
}

ClassImp(CbmTrdSetTracksPidANN)
