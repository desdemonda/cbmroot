// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidWkn header file          -----
// -----                  Created 25/02/2007  by F. Uhlig              -----
// -------------------------------------------------------------------------


/** CbmTrdSetTracksPidLike
 **
 *@author F.Uhlig <F.Uhlig@gsi.de>
 **
 ** Task class for PID calculation in the TRD using the 
 ** Likelihood method.
 ** Input: TClonesArray of CbmTrdTrack
 ** Parameters: The parameters for this class are histograms 
 ** with the energy loss information for five particle species
 ** (electrons, pions, kaons, protons and muons) at 12 different
 ** momenta bins. Between these momenta a interpoaltion is done.
 **
 **/


#ifndef CBMTRDSETTRACKSPIDLIKE_H
#define CBMTRDSETTRACKSPIDLIKE_H 1

#include "FairTask.h"

class TClonesArray;

class CbmTrdSetTracksPidLike : public FairTask
{

 public:

  /** Default constructor **/
  CbmTrdSetTracksPidLike();


  /** Standard constructor 
   **
   *@param name   Name of class
   *@param title  Task title
   **/
  CbmTrdSetTracksPidLike(const char* name, const char* title = "FairTask");


  /** Destructor **/
  virtual ~CbmTrdSetTracksPidLike();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** SetParContainers **/
  virtual void SetParContainers();


  /** Accessors **/
  Int_t GetNofTracks() const { return fNofTracks; };

  /** Setters **/
  //  void SetInputFile(Char_t* infile) {InputFile = infile;};

private:

  CbmTrdSetTracksPidLike& operator=(const CbmTrdSetTracksPidLike&);
  CbmTrdSetTracksPidLike(const CbmTrdSetTracksPidLike&);

  TClonesArray* fTrackArray;       /** Input array of TRD tracks */
  TClonesArray* fTrdHitArray;      /** Input array of TRD Hits */
  TClonesArray* fglobalTrackArray; /** Input array of global tracks */
  TClonesArray* fstsTrackArray;    /** Input array of STS tracks */

  TObjArray* fHistdEdx;          // Prob. of dEdx for 5 particle species
                                 // and several momenta

  Int_t fNofTracks;              // Number of tracks successfully fitted

  static const Int_t fgkNMom=13;   // number of momentum spectra
  Double_t *fTrackMomentum;        // [fgkNMom] Track momenta for which spectra
                                   // are available
  Int_t fNBins;                    // number of energy bins
  Double_t fBinSize;               // size of energy bin
  static const Int_t fgkNParts=5;  // numer of different particle species


  enum EParticleType {
    kElectron = 0,
    kPion = 1,
    kKaon = 2,
    kProton = 3,
    kMuon = 4,
  };

  /** Get the correct histogramnumber for particle typ and momentum bin **/
  Int_t  GetHistID(Int_t part, Int_t mom) const {return part*fgkNMom + mom;}

  /** Read the histograms from file **/
  Bool_t ReadData(); 

  /** Calculate probability for particle with momentum and dEdx **/
   Double_t   GetProbability(Int_t iType, Double_t mom, Double_t dedx) const;


  ClassDef(CbmTrdSetTracksPidLike,1);

};

#endif
