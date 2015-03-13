// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidANN header file                -----
// -----                  Created 06/03/2007  by Simeon Lebedev              -----
// -------------------------------------------------------------------------


/** CbmTrdSetTracksPidANN
 **
 *@author Simeon Lebedev <salebedev@jinr.ru>
 **
 ** Task class for PID calculation in the TRD using an
 ** artificial neural network (ANN).
 ** Input: TClonesArray of CbmTrdTrack
 ** Parameters: The parameters of this method are fixed by 
 ** Simeon Lebedev and stored in the parameters/trd directory.
 ** There should be no need to change this parameters at all.
 **
 **/


#ifndef CBM_TRD_SET_TRACKS_PID_ANN_H
#define CBM_TRD_SET_TRACKS_PID_ANN_H

#include "FairTask.h"

#include "TString.h"

#include <vector>
class TMultiLayerPerceptron;
class TClonesArray;

class CbmTrdSetTracksPidANN : public FairTask
{
	  
 public:

  /** Default constructor **/
  CbmTrdSetTracksPidANN();


  /** Standard constructor 
   **
   *@param name   Name of class
   *@param title  Task title
   **/
  CbmTrdSetTracksPidANN(const char* name, const char* title = "FairTask");


  /** Destructor **/
  virtual ~CbmTrdSetTracksPidANN();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** SetParContainers **/
  virtual void SetParContainers();


  /** Accessors **/
  Int_t GetNofTracks() const  { return fNofTracks; };
  Double_t GetANNPar1() const { return fANNPar1; };
  Double_t GetANNPar2() const { return fANNPar2; };

  /** Setters **/
  void SetANNPar1(Double_t param) { fANNPar1=param; };
  void SetANNPar2(Double_t param) { fANNPar2=param; };
  
  ///deprecated method, will be deleted soon
  // use SetTRDGeometryName
  void SetInputFile(TString infile) { };
  
  //should be "mb" or "st"
  //set the geometry, you use "mb" for Munster-Buharest; "st" for standard
  void SetTRDGeometryType(TString trdGeometryType) { fTRDGeometryType = trdGeometryType;}



 private:

  CbmTrdSetTracksPidANN& operator=(const CbmTrdSetTracksPidANN&);
  CbmTrdSetTracksPidANN(const CbmTrdSetTracksPidANN&);

  TClonesArray* fTrackArray;    // Input array of TRD tracks
  TClonesArray* fTrdHitArray;   // Input array of TRD Hits

  Int_t fNofTracks;             // Number of tracks successfully fitted
  Double_t fANNPar1;             // Parameter1 for Wkn method
  Double_t fANNPar2;             // Parameter2 for Wkn method
  
  std::vector<TMultiLayerPerceptron*> fNN;   // Neural Network  

  TString fTRDGeometryType; // name of the TRD geometry

  Bool_t ReadData();            // Read the weights needed for ANN

  ClassDef(CbmTrdSetTracksPidANN,2);

};

#endif
