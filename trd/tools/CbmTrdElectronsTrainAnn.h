
/** CbmTrdElectronsTrainAnn
 * Author: Semen Lebedev (S.Lebedev@gsi.de)
 **/


#ifndef CBM_TRD_ELECTRONS_TRAIN_ANN
#define CBM_TRD_ELECTRONS_TRAIN_ANN

#include "FairTask.h"

#include "TMultiLayerPerceptron.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

class TH1D;
class TTree;
class TGraph;
class TRandom;
//class TMultiLayerPerceptron;

enum IdMethod {kANN = 0, kBDT = 1, kMEDIAN = 2, kLIKELIHOOD = 3, kMeanCut = 4};

/**
 * \class TrdEloss
 *
 * \brief Represents information about energy losses in one layer.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/
class TrdEloss
{
public:
 TrdEloss() 
   : fEloss(0.), fdEdX(0.), fTR(0.) {}
  
 TrdEloss(double eloss, double dedx = 0., double tr = 0.) 
   : fEloss(eloss), fdEdX(dedx), fTR(tr) {}
  
  double fEloss;
  double fdEdX;
  double fTR;
};


class CbmTrdElectronsTrainAnn : public FairTask
{
public:

   /**
    * \brief Default constructor
    */
   CbmTrdElectronsTrainAnn(int nofTrdLayers);

   /**
    * \brief Destructor
    */
   virtual ~CbmTrdElectronsTrainAnn();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
         Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /**
    * \brief Draw results.
    */
	void Draw();

private:

   /**
    * \brief Fill vector with energy loss information for simulated data.
    */
   void FillElossVectorSim();

   /**
    * \brief Fill vector with energy loss information simulated from real data spectra.
    */
   void FillElossVectorReal();

   /**
    * \brief Fill histograms with energy loss information.
    */
   void FillElossHist();

   /**
    * \brief Sort energy losses and fill histograms.
    */
   void SortElossAndFillHist();

	void Run();

	void DoTrain();

	void DoTest();

	void DoPreTest();

	TTree* CreateTree();

	string CreateAnnString();

	TMVA::Factory* CreateFactory(TTree* simu);

	TMVA::Reader* CreateTmvaReader();

	void Transform();

	void Transform1();

	void Transform2();

	Double_t FindOptimalCut();

	void CreateCumProbOutputHist();

	TGraph* CreateRocDiagramm();

	Double_t Likelihood();

	Double_t Median();

	Double_t MeanCut();

	Double_t Eval(Bool_t isEl);

public:
	void SetIsDoTrain(Bool_t doTrain){fIsDoTrain = doTrain;}

	void SetTransformType(Int_t type){fTransformType = type;}

	void SetIdMethod(IdMethod idMethod){fIdMethod = idMethod;}

	void SetOutputDir(const string& outputDir){fOutputDir = outputDir;}

	void SetNofAnnEpochs(Int_t nofAnnEpochs){fNofAnnEpochs = nofAnnEpochs;}

	void SetNofTrainSamples(Int_t nofTr){fNofTrainSamples = nofTr;}

	void SetSigmaError(Double_t sigma){fSigmaError = sigma;}

   void SetBeamDataFile(string beamDataFile){fBeamDataFile = beamDataFile;}
   void SetBeamDataPiHist(string beamDataPiHist){fBeamDataPiHist = beamDataPiHist;}
   void SetBeamDataElHist(string beamDataElHist){fBeamDataElHist = beamDataElHist;}

	void RunBeamData();

private:
	void FillAnnInputHist(
	      Bool_t isEl);

	// Input arrays of simulated events
   TClonesArray* fMCTracks;
   TClonesArray* fTrdPoints;
   TClonesArray* fTrdTracks;
   TClonesArray* fTrdTrackMatches;
   TClonesArray* fTrdHits;

   // store energy loss information from TrdTrack
   // 1st index -> [0] = electrons, [1] = pions
   // 2nd index -> track index
   // 3rd index -> hit index in track
   vector< vector< vector<TrdEloss> > > fEloss;

   vector<TH1*> fHists; //store all pointers to histograms

   TH1* fhResults; // histograms for the results storing: pi suppression, el efficiency etc.

   // [0] = electrons, [1] = pions
   vector<TH1*> fhMeanEloss; // sum of energy losses in all layers divided by number of layers
   vector<TH1*> fhEloss; // energy losses in one layer

   // store sorted energy losses and cumulative probability for each TRD layer
   // 1st index -> [0] = electrons, [1] = pions
   // 2nd index -> layer number
   vector<vector<TH1*> > fhElossSort;

   Int_t fEventNum; // event number
	string fOutputDir; // output directory
	Double_t fSigmaError; // additional sigma error for energy loss measurements
	Bool_t fIsDoTrain; // do you want to run training procedure?
	Int_t fTransformType; // Energy loss transformation type

   string fBeamDataFile; // path to file with beamtime data for energy losses
   string fBeamDataPiHist; // histogram name with energy losses for pions
   string fBeamDataElHist; // histogram name with energy losses for electrons

	vector<Float_t> fAnnInput; // input vector for ANN
	Float_t fXOut; // output value from ANN

	Int_t fNofTrdLayers; // number of TRD layers

	Double_t fMaxEval;
	Double_t fMinEval;

	TMultiLayerPerceptron* fNN;
	TMVA::Reader* fReader;
	IdMethod fIdMethod;
	Int_t fNofAnnEpochs;
	Int_t fNofTrainSamples;
	TRandom* fRandom;
	double fElIdEfficiency;

   // Histograms for testing
	// [0] = electron, [1] = pion
	vector<TH1*> fhOutput; // algorithm output
	vector<TH1*> fhCumProbOutput; // Cumulative probabilities for algorithm's output
   vector< vector<TH1*> > fhInput; // Input data for algorithm for each input

    CbmTrdElectronsTrainAnn(const CbmTrdElectronsTrainAnn&);
    CbmTrdElectronsTrainAnn& operator=(const CbmTrdElectronsTrainAnn&);

    ClassDef(CbmTrdElectronsTrainAnn,1);
};

#endif
