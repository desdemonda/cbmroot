
#include "CbmTrdElectronsTrainAnn.h"
#include "TMath.h"
#include "TFile.h"
#include "TCut.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TMVA/PDF.h"
#include "TMVA/Config.h"
#include "TLine.h"
#include "TClonesArray.h"
#include "TSystem.h"
#include "TGraph.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrackMatchNew.h"
#include "CbmTrdPoint.h"
#include "CbmMCTrack.h"

#include "CbmDrawHist.h"

#include <boost/assign/list_of.hpp>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::stringstream;
using std::string;
using boost::assign::list_of;

CbmTrdElectronsTrainAnn::CbmTrdElectronsTrainAnn(
      Int_t nofTrdLayers)
  : FairTask(),
    fMCTracks(NULL),	 
    fTrdPoints(NULL),
    fTrdTracks(NULL),
    fTrdTrackMatches(NULL),
    fTrdHits(NULL),
    fEloss(),
    fHists(),
    fhResults(),
    fhMeanEloss(),
    fhEloss(),
    fhElossSort(),
    fEventNum(0),
    fOutputDir("results/"),
    fSigmaError(0.0),
    fIsDoTrain(true),
    fTransformType(1),
    fBeamDataFile(""),
    fBeamDataPiHist(""),
    fBeamDataElHist(""),
    fAnnInput(),
    fXOut(-1.),
    fNofTrdLayers(nofTrdLayers),
    fMaxEval(1.3),
    fMinEval(-1.3),
    fNN(NULL),
    fReader(NULL),
    fIdMethod(kANN),
    fNofAnnEpochs(50),
    fNofTrainSamples(2500),
    fElIdEfficiency(0.9),
    fRandom(new TRandom(0)),
    fhOutput(),
    fhCumProbOutput(),
    fhInput()
{
   fEloss.resize(2);
   fhMeanEloss.resize(2);
   fhEloss.resize(2);
   string s;

   TH1::SetDefaultBufferSize(30000);

   fhResults = new TH1D("fhResults", "fhResults", 2, 0, 2);
   fHists.push_back(fhResults);

   for (int i = 0; i < 2; i++){
      if (i == 0) s = "El";
      if (i == 1) s = "Pi";
      fhMeanEloss[i] = new TH1D(("fhMeanEloss"+s).c_str(),"fhMeanEloss;Mean energy loss [a.u.];Yield",100, 0., 50e-6);
      fHists.push_back(fhMeanEloss[i]);
      fhEloss[i] = new TH1D(("fhEloss"+s).c_str(),"fhEloss;Energy loss [a.u.];Yield",100, 0., 50e-6);
      fHists.push_back(fhEloss[i]);
   }

   Int_t nofSortBins = 200;
   fhElossSort.resize(2);
   for (int j = 0; j < 2; j++){
      fhElossSort[j].resize(fNofTrdLayers);
      for (Int_t i = 0; i < fNofTrdLayers; i++){
         std::stringstream ss1;
         if (j == 0) ss1 << "fhElossSortEl" << i;
         if (j == 1) ss1 << "fhElossSortPi" << i;
         fhElossSort[j][i] = new TH1D(ss1.str().c_str(), (ss1.str()+";Energy loss [a.u.];Counters").c_str(), nofSortBins, 0., 0.);
      }
   }

   // Histograms for algorithm testing
   Int_t nofBins = 10000;
   fhOutput.resize(2);
   fhCumProbOutput.resize(2);
   fhInput.resize(2);
   for (Int_t i = 0; i < 2; i++){
      s = (i == 0)? "El": "Pi";
      fhOutput[i] = new TH1D(("fhOutput"+s).c_str(),"fhOutput;Output;Counter",nofBins, fMinEval, fMaxEval);
      fhCumProbOutput[i] = new TH1D(("fhCumProbOutput"+s).c_str(),"fhCumProbOutput;Output;Cumulative probability",nofBins, fMinEval, fMaxEval);

      fhInput[i].resize(fNofTrdLayers);
      for (Int_t j = 0; j < fNofTrdLayers; j++){
         stringstream ss;
         ss << s << j;
         fhInput[i][j] = new TH1D(("fhInput"+ss.str()).c_str(), "fhInput", 100, 0.0, 0.0);
      }
   }
}

CbmTrdElectronsTrainAnn::~CbmTrdElectronsTrainAnn()
{

}

InitStatus CbmTrdElectronsTrainAnn::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("-E- CbmTrdElectronsTrainAnn::Init","RootManager not instantised!"); }

   fMCTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMCTracks) {Fatal("-E- CbmTrdElectronsTrainAnn::Init","No MCTrack array!");}

   fTrdPoints  = (TClonesArray*) ioman->GetObject("TrdPoint");
   if (NULL == fTrdPoints) {Fatal("-E- CbmTrdElectronsTrainAnn::Init","No TRDPoint array!");}

   fTrdTracks  = (TClonesArray*) ioman->GetObject("TrdTrack");
   if (NULL == fTrdTracks) {Fatal("-E- CbmTrdElectronsTrainAnn::Init","No TrdTrack array!");}

   fTrdTrackMatches  = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
   if (NULL == fTrdTrackMatches) {Fatal("-E- CbmTrdElectronsTrainAnn::Init","No TrdTrackMatch array!");}

   fTrdHits  = (TClonesArray*) ioman->GetObject("TrdHit");
   if (NULL == fTrdHits) {Fatal("-E- CbmTrdElectronsTrainAnn::Init","No TRDHit array!");}

   return kSUCCESS;
}

void CbmTrdElectronsTrainAnn::Exec(
      Option_t* opt)
{
   fEventNum++;
   cout << "CbmTrdElectronsTrainAnn, event " << fEventNum << endl;

   FillElossVectorSim();
   FillElossHist();
   SortElossAndFillHist();

   cout << "Nof electrons = " << fEloss[0].size() << endl;
   cout << "Nof pions = " << fEloss[1].size() << endl;

  // Finish();
}

void CbmTrdElectronsTrainAnn::Finish()
{
   Run();
   Draw();

   if (fOutputDir != "") { gSystem->mkdir(fOutputDir.c_str(), true); }
   TFile* f = new TFile(string(fOutputDir+"/trd_elid_hist.root").c_str(), "RECREATE");
   for (int i = 0; i < fHists.size(); i++){
      fHists[i]->Write();
   }
   f->Close();
}

void CbmTrdElectronsTrainAnn::RunBeamData()
{
   FillElossVectorReal();
   FillElossHist();
   SortElossAndFillHist();

   cout << "Nof electrons = " << fEloss[0].size() << endl;
   cout << "Nof pions = " << fEloss[1].size() << endl;

   Finish();
}

void CbmTrdElectronsTrainAnn::FillElossVectorReal()
{
   if (fBeamDataFile == "" || fBeamDataPiHist == "" || fBeamDataElHist == "") {
      Fatal("-E- CbmTrdElectronsTrainAnn::FillElossVectorReal()", "Set input file for beam data and histogram names!");
   }

	TFile* file = new TFile(fBeamDataFile.c_str(), "READ");
	TH1F* hPion = (TH1F*)file->Get(fBeamDataPiHist.c_str())->Clone();
	TH1F* hElectron = (TH1F*)file->Get(fBeamDataElHist.c_str())->Clone();

   double scaleX = fhEloss[0]->GetXaxis()->GetBinUpEdge(fhEloss[0]->GetNbinsX()) / hPion->GetXaxis()->GetBinUpEdge(hPion->GetNbinsX());

	int nofSimulatedParticles = 1000000;
	fEloss[0].resize(nofSimulatedParticles);
	fEloss[1].resize(nofSimulatedParticles);

	for (int iP = 0; iP < 2; iP++){
		TH1F* hist = hElectron;
		if (iP == 1) hist = hPion;
		for (int iT = 0; iT < nofSimulatedParticles; iT++){
			fEloss[iP][iT].resize(fNofTrdLayers);
			for (int iH = 0; iH < fNofTrdLayers; iH++) {
				double eloss = scaleX * hist->GetRandom();
				TrdEloss e (eloss);
				fEloss[iP][iT][iH] = e;
			}
		}
	}
}

void CbmTrdElectronsTrainAnn::FillElossVectorSim()
{
   Int_t nofTrdTracks = fTrdTracks->GetEntries();
   for(Int_t iTrdTrack=0; iTrdTrack < nofTrdTracks; iTrdTrack++){
      CbmTrdTrack* trdtrack = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
      Int_t nHits = trdtrack->GetNofHits();


      CbmTrackMatchNew* trdmatch = (CbmTrackMatchNew*) fTrdTrackMatches->At(iTrdTrack);
      Int_t iMC = trdmatch->GetMatchedLink().GetIndex();
      if (iMC < 0 || iMC > fMCTracks->GetEntriesFast())  continue;

      CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(iMC);
      Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
      Double_t momMC = mctrack->GetP();
      Double_t motherId = mctrack->GetMotherId();

      ///only primary tracks generated by BoxGenerator
      if (motherId != -1)  continue;

      int iP = 0; //[0] = electron
      if (pdg == 211) iP = 1; //[1] = pion
      vector<TrdEloss> v;

      if (nHits < fNofTrdLayers) continue;

      for (Int_t iHit = 0; iHit < nHits; iHit++) {
         Int_t hitIndex = trdtrack->GetHitIndex(iHit);
         CbmTrdHit* trdhit = (CbmTrdHit*) fTrdHits->At(hitIndex);
         TrdEloss e (trdhit->GetELoss(), trdhit->GetELossdEdX(), trdhit->GetELossTR());
         if (fNofTrdLayers == v.size()) break;
         v.push_back(e);
      } //iHit
      fEloss[iP].push_back(v);
   }//iTrdTrack
}

void CbmTrdElectronsTrainAnn::FillElossHist()
{
   // [0]=electron, [1]=pion
   for (int i = 0; i < 2; i++){
      for(int iT = 0; iT < fEloss[i].size(); iT++){
         Int_t nHits = fEloss[i][iT].size();
         double sumEloss = 0.;
         for (int iH = 0; iH < nHits; iH++){
            sumEloss += fEloss[i][iT][iH].fEloss;
            fhEloss[i]->Fill(fEloss[i][iT][iH].fEloss);
         }// iH
         fhMeanEloss[i]->Fill(sumEloss / nHits);
      }//iT
   }
}

void CbmTrdElectronsTrainAnn::SortElossAndFillHist()
{
   vector<double> v;
   v.resize(fNofTrdLayers);
   for (int iP = 0; iP < 2; iP++){
      for (int iT = 0; iT < fEloss[iP].size(); iT++){
         for (int iH = 0; iH < fNofTrdLayers; iH++){
            v[iH] = fEloss[iP][iT][iH].fEloss;
         }
         std::sort(v.begin(), v.end());
         for (int iH = 0; iH < fNofTrdLayers; iH++){
            fhElossSort[iP][iH]->Fill(v[iH]);
         }
      }//iT

      for (int iH = 0; iH < fNofTrdLayers; iH++){
         fhElossSort[iP][iH]->Scale(1./fhElossSort[iP][iH]->Integral());
      }
   }
}

void CbmTrdElectronsTrainAnn::Run()
{
	if (fIdMethod == kBDT || fIdMethod == kANN){
		if (fIsDoTrain) DoTrain();
		DoTest();
	}else if (fIdMethod == kMEDIAN || fIdMethod == kLIKELIHOOD || fIdMethod == kMeanCut) {
	   DoTest();
	}
}

void CbmTrdElectronsTrainAnn::Transform()
{
	if (fIdMethod == kBDT || fIdMethod == kANN){
		if (fTransformType == 1){
			Transform1();
		} else if (fTransformType == 2){
			Transform2();
		}
	}
}


void CbmTrdElectronsTrainAnn::Transform1()
{
	Double_t ANNCoef1 = 1.06;
	Double_t ANNCoef2 = 0.57;
	//Double_t ANNCoef1[] = {1.04,1.105,1.154,1.277,1.333,1.394,1.47,1.50,1.54,1.58};
	//Double_t ANNCoef2[] = {0.548,0.567,0.585,0.63,0.645,0.664,0.69,0.705,0.716,0.723};
	for (Int_t i = 0; i < fAnnInput.size(); i++) {
	   fAnnInput[i] = fAnnInput[i]*1e6;
	   fAnnInput[i] = (fAnnInput[i] - ANNCoef1)/ANNCoef2 - 0.225;
	}
	sort(fAnnInput.begin(), fAnnInput.end());
	for (Int_t i = 0; i < fAnnInput.size(); i++){
	   fAnnInput[i] = TMath::LandauI(fAnnInput[i]);
	}
}

void CbmTrdElectronsTrainAnn::Transform2()
{
	sort(fAnnInput.begin(), fAnnInput.end());

	Int_t size = fAnnInput.size();
	for (Int_t j = 0; j < size; j++) {
		Int_t binNumEl = fhElossSort[0][j]->FindBin(fAnnInput[j]);
		if (binNumEl > fhElossSort[0][j]->GetNbinsX())
			binNumEl = fhElossSort[0][j]->GetNbinsX();
		Double_t probEl = fhElossSort[0][j]->GetBinContent(binNumEl);

		Int_t binNumPi = fhElossSort[1][j]->FindBin(fAnnInput[j]);
		if (binNumPi > fhElossSort[1][j]->GetNbinsX())
			binNumPi = fhElossSort[1][j]->GetNbinsX();
		Double_t probPi = fhElossSort[1][j]->GetBinContent(binNumPi);

		if (TMath::IsNaN(probPi/(probEl+probPi))){
		   fAnnInput[j] = 0.;
		}else{
		   fAnnInput[j] = probPi/(probEl+probPi);
		}
	}
}

Double_t CbmTrdElectronsTrainAnn::Likelihood()
{
	Double_t lPi = 1.;
	Double_t lEl = 1.;
  // sort(fAnnInput.begin(), fAnnInput.end());
   for (Int_t j = 0; j < fAnnInput.size(); j++) {
      Int_t binNumEl = fhEloss[0]->FindBin(fAnnInput[j]);
		if (binNumEl > fhEloss[0]->GetNbinsX())
			binNumEl = fhEloss[0]->GetNbinsX();
		Double_t probEl = fhEloss[0]->GetBinContent(binNumEl);
		lEl = lEl*probEl;

		Int_t binNumPi = fhEloss[1]->FindBin(fAnnInput[j]);
		if (binNumPi > fhEloss[1]->GetNbinsX())
			binNumPi = fhEloss[1]->GetNbinsX();
		Double_t probPi = fhEloss[1]->GetBinContent(binNumPi);
		lPi = lPi*probPi;
	}
	return lEl/(lEl + lPi);
}

Double_t CbmTrdElectronsTrainAnn::Median()
{
   sort(fAnnInput.begin(), fAnnInput.end());
   double eval = 0.;
   if (fNofTrdLayers%2 == 0) {
      eval = (fAnnInput[fNofTrdLayers/2] + fAnnInput[fNofTrdLayers/2 + 1])/2.;
   } else {
      eval = fAnnInput[fNofTrdLayers/2 + 1];
   }
   double scaleX = 1./fhEloss[0]->GetXaxis()->GetBinUpEdge(fhEloss[0]->GetNbinsX());
   return eval * scaleX;
}

Double_t CbmTrdElectronsTrainAnn::MeanCut()
{
   double eval = 0.;
   for (int i = 0; i < fNofTrdLayers; i++) {
      eval += fAnnInput[i];
   }
   eval = eval / fNofTrdLayers;
   double scaleX = 1./fhEloss[0]->GetXaxis()->GetBinUpEdge(fhEloss[0]->GetNbinsX());
   return eval * scaleX;
}

Double_t CbmTrdElectronsTrainAnn::Eval(
      Bool_t isEl)
{
	if (fIdMethod == kBDT){
		return fReader->EvaluateMVA("BDT");
	}else if (fIdMethod == kANN){
		Double_t par[fNofTrdLayers];
		for (UInt_t i = 0; i < fAnnInput.size(); i++) par[i] = fAnnInput[i];
		return fNN->Evaluate(0, par);
	}else if (fIdMethod == kMEDIAN){
	   return Median();
	}else if (fIdMethod == kLIKELIHOOD){
		return Likelihood();
	}else if (fIdMethod == kMeanCut){
	   return MeanCut();
	}
	return -1.;
}


void CbmTrdElectronsTrainAnn::DoTrain()
{
   fAnnInput.clear();
   fAnnInput.resize(fNofTrdLayers);

   TTree* simu = CreateTree();
   for (Int_t i = 0; i < 2; i++) {
      for (Int_t iT = 0; iT < fEloss[i].size() - 2; iT++) {
         for (int iH = 0; iH < fNofTrdLayers; iH++){
            fAnnInput[iH] = fEloss[i][iT][iH].fEloss;
         }
         fXOut = (i == 0)? 1.: -1.;
         Transform();
         simu->Fill();
         if (iT >= fNofTrainSamples) break;
      }//iT
   }//i

   if (fIdMethod == kANN){
      if (NULL != fNN) delete fNN;
      TString mlpString = CreateAnnString();
      cout << "-I- Create ANN: "<< mlpString << endl;
      cout << "-I- Number of training epochs = " << fNofAnnEpochs << endl;
      fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
      fNN->Train(fNofAnnEpochs, "+text,update=10");
      stringstream ss;
      ss<< fOutputDir+"/ann_weights_" <<fNofTrdLayers << ".txt";
      fNN->DumpWeights(ss.str().c_str());
   } else if (fIdMethod == kBDT) {
      TMVA::Factory* factory = CreateFactory(simu);
      (TMVA::gConfig().GetIONames()).fWeightFileDir = fOutputDir;
      TCut mycuts = "";
      TCut mycutb = "";
      factory->PrepareTrainingAndTestTree(mycuts, mycutb,"SplitMode=Random:NormMode=NumEvents:!V");
      //factory->BookMethod(TMVA::Types::kTMlpANN, "TMlpANN","!H:!V:NCycles=50:HiddenLayers=N+1");
      stringstream ss;
      ss << "nTrain_Signal=" << fNofTrainSamples - 500 <<":nTrain_Background=" << fNofTrainSamples - 500 <<":nTest_Signal=0:nTest_Background=0";
      factory->PrepareTrainingAndTestTree("", ss.str().c_str());
      factory->BookMethod(TMVA::Types::kBDT, "BDT", "!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5");
      factory->TrainAllMethods();
   }
}

void CbmTrdElectronsTrainAnn::DoPreTest()
{
	cout << "-I- Start pretesting " << endl;
	if (fIdMethod == kBDT){
		cout << "-I- IdMethod = kBDT " << endl;
	}else if (fIdMethod == kANN){
		cout << "-I- IdMethod = kANN " << endl;
	}else if (fIdMethod == kMEDIAN){
		cout << "-I- IdMethod = kMEDIANA " << endl;
	}else if (fIdMethod == kLIKELIHOOD){
		cout << "-I- IdMethod = kLIKELIHOOD " << endl;
	}

	fAnnInput.clear();
	fAnnInput.resize(fNofTrdLayers);

	if (fIdMethod == kBDT){
		fReader = CreateTmvaReader();
		fReader->BookMVA("BDT", fOutputDir+"/TMVAnalysis_BDT.weights.xml");
	} else if (fIdMethod == kANN){
		if (fNN != NULL) delete fNN;
		TTree* simu = CreateTree();
		TString mlpString = CreateAnnString();
		fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
      stringstream ss;
      ss<< fOutputDir+"/ann_weights_" <<fNofTrdLayers << ".txt";
      fNN->LoadWeights(ss.str().c_str());
	}

	for (Int_t i = 0; i < 2; i++){
      for (Int_t iT = 0; iT < fEloss[i].size(); iT++) {
         if (iT < fNofTrainSamples) continue;// exclude training samples
         if (iT%100000 == 0) cout << "-I- read number: "<< iT << endl;

         //change electron hit to pion hit and vice versa
         //int randPi = rand()%fElossPi.size();
         //for (int i1 = 0; i1 < 5; i1++){
         //     fElossEl[iE][i1] = fElossPi[randPi][i1];
         // }
         for (int iH = 0; iH < fNofTrdLayers; iH++){
            if (fSigmaError != 0.) fEloss[i][iT][iH].fEloss += fRandom->Gaus(0., fSigmaError);
            fAnnInput[iH] = fEloss[i][iT][iH].fEloss;
         }

         Transform();
         Bool_t isEl = (i == 0)?true:false;
         FillAnnInputHist(isEl);
         Double_t nnEval = Eval(isEl);
         if (nnEval > fMaxEval) nnEval = fMaxEval - 0.01;
         if (nnEval < fMinEval) nnEval = fMinEval + 0.01;
         fhOutput[i]->Fill(nnEval);
      }
	}
	CreateCumProbOutputHist();
}

void CbmTrdElectronsTrainAnn::DoTest()
{
   fAnnInput.clear();
   fAnnInput.resize(fNofTrdLayers);

	DoPreTest();
	double cut = FindOptimalCut();
	cout << "-I- Optimal cut = " << cut << " for "<< 100 * fElIdEfficiency<< "% electron efficiency" << endl;
	cout << "-I- Start testing" << endl;
	fAnnInput.clear();
	fAnnInput.resize(fNofTrdLayers);

	if (fIdMethod == kBDT){
		fReader = CreateTmvaReader();
		//reader->BookMVA("TMlpANN", "weights/TMVAnalysis_TMlpANN.weights.txt");
		fReader->BookMVA("BDT", fOutputDir+"/TMVAnalysis_BDT.weights.xml");
	} else if (fIdMethod == kANN){
		if (fNN != NULL) delete fNN;
		TTree* simu = CreateTree();
		TString mlpString = CreateAnnString();
		fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
		stringstream ss;
		ss<< fOutputDir+"/ann_weights_" <<fNofTrdLayers << ".txt";
		fNN->LoadWeights(ss.str().c_str());
	}

	int nofPiLikeEl = 0;
	int nofElLikePi = 0;
	int nofElTest = 0;
	int nofPiTest = 0;

	for (Int_t i = 0; i < 2; i++){
      for (Int_t iT = 0; iT < fEloss[i].size(); iT++) {
         if (iT < fNofTrainSamples) continue;//exclude training samples
         if (iT%100000 == 0) cout << "-I- Read number: "<< iT<< endl;

         for (Int_t iH = 0; iH < fNofTrdLayers; iH++){
            fAnnInput[iH] = fEloss[i][iT][iH].fEloss;
         }
         Transform();

         Bool_t isEl = (i == 0)?true:false;
         Double_t nnEval = Eval(isEl);
         if (nnEval > fMaxEval) nnEval = fMaxEval - 0.01;
         if (nnEval < fMinEval) nnEval = fMinEval + 0.01;
         if (i == 0){
            nofElTest++;
            if (nnEval < cut) nofElLikePi++;
         } else if (i == 1){
            nofPiTest++;
            if (nnEval > cut) nofPiLikeEl++;
         }
      }
	}
	double piSupp = -1;
	if (nofPiLikeEl != 0) piSupp = (double) nofPiTest / nofPiLikeEl;
	double elEff =  (double) nofElLikePi/nofElTest * 100.;

	cout << "Testing results:" << endl;
	cout << "nof TRD layers " << fNofTrdLayers << endl;
	cout << "cut = " << cut << endl;
	cout << "nof El = " << nofElTest << endl;
	cout << "nof Pi = " << nofPiTest << endl;
	cout << "nof Pi identified as El = " << nofPiLikeEl<<endl;
	cout << "nof El identified as Pi = " << nofElLikePi<<endl;
	cout << "pion suppression = "<< nofPiTest<< "/"<< nofPiLikeEl<< " = " << piSupp << endl;
	cout << "electron efficiency loss in % = " << nofElLikePi << "/" << nofElTest << " = " << elEff << endl;

	// write results to histogramm
	fhResults->SetBinContent(1, piSupp);
	fhResults->SetBinContent(2, elEff);
}

void CbmTrdElectronsTrainAnn::CreateCumProbOutputHist()
{
	for (Int_t i = 0; i < 2; i++){
	   Double_t cumProb = 0.;
	   Double_t nofTr = fhOutput[i]->GetEntries();
      for (Int_t iH = 1; iH <= fhOutput[i]->GetNbinsX(); iH++){
         cumProb += fhOutput[i]->GetBinContent(iH);
         Double_t pr = (i == 0)? 1. - cumProb / nofTr:cumProb/nofTr;
         fhCumProbOutput[i]->SetBinContent(iH, pr);
      }
	}
}

TGraph* CbmTrdElectronsTrainAnn::CreateRocDiagramm()
{
	Int_t nBins = fhCumProbOutput[0]->GetNbinsX();
	Double_t x[nBins], y[nBins];

	for (Int_t i = 1; i <= nBins; i++) {
		Double_t cpi = 100. * fhCumProbOutput[1]->GetBinContent(i);
		if (cpi == 100.) cpi = 100. - 0.0001;
		y[i - 1] = 100. / (100. - cpi);
		x[i - 1] = 100. * fhCumProbOutput[0]->GetBinContent(i);
	}
	TGraph* rocGraph = new TGraph(nBins, x, y);
	rocGraph->SetTitle("ROC diagram");
	rocGraph->GetXaxis()->SetTitle("Efficiency [%]");
	rocGraph->GetYaxis()->SetTitle("Pion suppression");
   rocGraph->SetLineWidth(3);
	return rocGraph;
}

Double_t CbmTrdElectronsTrainAnn::FindOptimalCut()
{
	Double_t optimalCut = -1;
	for (Int_t i = 1; i <= fhCumProbOutput[0]->GetNbinsX(); i++) {
		if (fhCumProbOutput[0]->GetBinContent(i) <= fElIdEfficiency) {
			optimalCut = fhCumProbOutput[0]->GetBinCenter(i);
			return optimalCut;
		}
	}
	return optimalCut;
}

TTree* CbmTrdElectronsTrainAnn::CreateTree()
{
   fAnnInput.clear();
   fAnnInput.resize(fNofTrdLayers);
	TTree *simu = new TTree("MonteCarlo", "MontecarloData");
	char txt1[100], txt2[100];
	for (Int_t i = 0; i < fNofTrdLayers; i++){
		sprintf(txt1, "x%d",i);
		sprintf(txt2, "x%d/F", i);
		simu->Branch(txt1, &fAnnInput[i], txt2);
	}
	simu->Branch("xOut", &fXOut, "xOut/F");

	return simu;
}

string CbmTrdElectronsTrainAnn::CreateAnnString()
{
	string st = "";
	char txt[50];
	for (Int_t i = 0; i < fNofTrdLayers - 1; i++){
		sprintf(txt, "x%d",i);
		st = st + txt + ",";

	}
	sprintf(txt, "x%d",fNofTrdLayers - 1);
	st = st + txt+ "";
	sprintf(txt, "%d", 2 * fNofTrdLayers);
	st = st+ ":"+txt+":xOut";
	return st;
}

TMVA::Factory* CbmTrdElectronsTrainAnn::CreateFactory(TTree* simu)
{
   if (fOutputDir != "") gSystem->mkdir(fOutputDir.c_str(), true);
	TFile* outputFile = TFile::Open(string(fOutputDir+"/tmva_output.root").c_str(), "RECREATE");

	TMVA::Factory *factory = new TMVA::Factory("TMVAnalysis", outputFile);

	TCut piCut = "xOut<0";
	TCut elCut = "xOut>0";

	factory->SetInputTrees(simu, elCut, piCut);
	char txt1[100];
	for (Int_t i = 0; i < fNofTrdLayers; i++){
		sprintf(txt1, "x%d",i);
		factory->AddVariable(txt1, 'F');
	}

	return factory;
}

TMVA::Reader* CbmTrdElectronsTrainAnn::CreateTmvaReader()
{
	if (!fReader) delete fReader;

	fAnnInput.clear();
	fAnnInput.resize(fNofTrdLayers);

	TMVA::Reader* reader = new TMVA::Reader();

	char txt1[100];
	for (Int_t i = 0; i < fNofTrdLayers; i++){
		sprintf(txt1, "x%d",i);
		reader->AddVariable(txt1, &fAnnInput[i]);
	}
	return reader;
}

void CbmTrdElectronsTrainAnn::FillAnnInputHist(
      Bool_t isEl)
{
	for (Int_t j = 0; j < fAnnInput.size(); j++) {
		if (isEl) {
			fhInput[0][j]->Fill(fAnnInput[j]);
		}else {
			fhInput[1][j]->Fill(fAnnInput[j]);
		}
	}
}

void CbmTrdElectronsTrainAnn::Draw()
{
   SetDefaultDrawStyle();
   TCanvas* cEloss = new TCanvas("trd_elid_eloss", "trd_elid_eloss", 1200, 600);
   cEloss->Divide(2, 1);
   cEloss->cd(1);
   DrawH1(fhMeanEloss, list_of("e^{#pm}")("#pi^{#pm}"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cEloss->cd(2);
   DrawH1(fhEloss, list_of("e^{#pm}")("#pi^{#pm}"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);


   TCanvas* cElossSort = new TCanvas("trd_elid_eloss_sort", "trd_elid_eloss_sort", 1200, 900);
   cElossSort->Divide(4, 3);
   for (int iL = 0; iL < fNofTrdLayers; iL++){
      cElossSort->cd(iL+1);
      DrawH1(list_of(fhElossSort[0][iL])(fhElossSort[1][iL]), list_of("e^{#pm}")("#pi^{#pm}"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   }

   TCanvas* cClassifierOutput = new TCanvas("trd_elid_classifier_output","trd_elid_classifier_output", 500, 500);
   TH1D* out0 = (TH1D*)fhOutput[0]->Clone();
   TH1D* out1 = (TH1D*)fhOutput[1]->Clone();
   out0->Rebin(50);
   out1->Rebin(50);
   DrawH1(list_of(out0)(out1), list_of("e^{#pm}")("#pi^{#pm}"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   out0->Scale(1./out0->Integral());
   out1->Scale(1./out1->Integral());

	TCanvas* cCumProbOutput = new TCanvas("trd_elid_cum_prob_output","trd_elid_cum_prob_output", 500,500);
   DrawH1(fhCumProbOutput, list_of("e^{#pm}")("#pi^{#pm}"), kLinear, kLinear, true, 0.8, 0.8, 0.99, 0.99);

	TCanvas* cRoc = new TCanvas("trd_elid_roc","trd_elid_roc", 500, 500);
   TGraph* rocGraph = CreateRocDiagramm();
	DrawGraph(rocGraph);
	gPad->SetLogy();

	TCanvas* cInput = new TCanvas("trd_elid_input", "trd_elid_ann_input", 10, 10, 800, 800);
	cInput->Divide(4, 3);
	for (int i = 0; i < fNofTrdLayers; i++) {
	   cInput->cd(i + 1);
      DrawH1(list_of(fhInput[0][i])(fhInput[1][i]), list_of("e^{#pm}")("#pi^{#pm}"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
	}
}

ClassImp(CbmTrdElectronsTrainAnn)
