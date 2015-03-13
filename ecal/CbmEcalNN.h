// A fast version of multilayer perceptron.
// Can be even faster using SIMD.
// An activation function (Gauss) is hardcoded.
#ifndef CBMECALNN_H
#define CBMECALNN_H

#include "TObject.h"
#include "TMatrixD.h"

class CbmEcalNN : public TObject
{
public:
  CbmEcalNN(const char* str);
  //LoadWeights and DumpWeights are compatible with ROOT format
  void LoadWeights(const char* filename);
  void DumpWeights(const char* filename);
  void Randomize();	//Randomize weights. Uses gRandom
  Double_t GetError(Int_t n, Double_t* dta); //Get error for given dataset
  Double_t Eval(Int_t idx, Double_t* input);
  Double_t EvalD(Int_t idx, Double_t* input); //Compute output and derivaties of neural network
  void Train(Int_t epochs, Int_t n, Double_t* dta);	//Train a neural netrork. Use BFGS algo
  void TrainStochastic(Int_t epochs, Int_t n, Double_t* dta);	//Train a neural netrork. Use stochastic algo

  void SetReset(Int_t reset=50) {fReset=reset;}
  Int_t GetReset() const {return fReset;}

  void SetEta(Double_t eta=0.1) {fEta=eta;}
  Double_t GetEta() const {return fEta;}

  void SetEpsilon(Double_t epsilon=0.0) {fEpsilon=epsilon;}
  Double_t GetEpsilon() const {return fEpsilon;} 

  void SetDelta(Double_t delta=0.0) {fDelta=delta;}
  Double_t GetDelta() const {return fDelta;} 

  void SetEtaDecay(Double_t etadecay=1.0) {fEtaDecay=etadecay;}
  Double_t GetEtaDecay() const {return fEtaDecay;} 

  void SetTau(Double_t tau=3.0) {fTau=tau;}
  Double_t GetTau() const {return fTau;}
  ~CbmEcalNN();
private:
  CbmEcalNN()
    : fN(-1111),
      fStr(NULL), fNeurons(NULL), fWNeurons(NULL), fWSynapses(NULL),
      fNNeurons(-1111), fNSynapses(-1111)
  {};

  Int_t fN;			//Number of layers
  Int_t* fStr;			//!Structure of neural network
  Double_t* fNeurons; 		//!Values of neurons
  Double_t* fWNeurons; 		//!Weights of neurons
  Double_t* fWSynapses;		//!Weights of synapses
  Int_t fNNeurons;		//Number of neurons
  Int_t fNSynapses;		//Number of synapses
// Other used for training
  void ComputeDeDs();		//Compute derivative of the error for current data
  void ComputeDEDs(Int_t n, Double_t* dta);	//Compute summ of derivatives of the error for whole data set
  void Shuffle(Int_t n, Int_t* idx);		//Shuffle indexes
  void SteepestDir(Double_t* dir);		//Steepest descent search direction
  Bool_t GetBFGSH(TMatrixD& bfgsh, TMatrixD& gamma, TMatrixD& delta);
  Double_t DerivDir(Double_t* dir);		//Scalar product between gradient and direction
  void MLP_Line(Double_t* old, Double_t* dir, Double_t dst);
  void BFGSDir(TMatrixD& bfgsh, Double_t* dir);	//direction=Hessian estimate x dir

  //Search along the line defined by direction
  Bool_t LineSearch(Double_t* dir, Double_t* buf, Int_t n, Double_t* dta);

  void SetGammaDelta(TMatrixD& gamma, TMatrixD& delta, Double_t* buf, Int_t n, Double_t* dta);


  Int_t fMS;			//Size of matrixes: fNNeurons+fNSynapses
  Double_t* fCurrent;		//!Current training set
  Double_t* fDNeurons; 		//!Derivatives of neurons
  Double_t* fDeDNeurons;	//!Current DeDs of neurons
  Double_t* fDeDSynapses;	//!Current DeDs of synapses
  Double_t* fDEDNeurons;	//!Sum of DeDs over all events for neurons
  Double_t* fDEDSynapses;	//!Sum of DeDs over all events for synapses
  Double_t* fOld;		//!Old weights

  Int_t fReset;			//Number of epochs between of the search of new steepest descent
  Double_t fEta;		//Used in stochastic training (0.1)
  Double_t fEpsilon;		//Used in stochastic training (0.0)
  Double_t fDelta;		//Used in stochastic training (0.0)
  Double_t fEtaDecay;		//Used in stochastic training (1.0)
  Double_t fLastAlpha;		//Used in line search
  Double_t fTau;		//Used in line search
  ClassDef(CbmEcalNN, 1)
};


#endif
