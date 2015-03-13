/**
* \file CbmRichProtHitProducer.h
*
* \brief Class for producing RICH hits directly from MCPoints.
*
* \author D. Kresan
* \date 2010
**/

#ifndef CBM_RICH_PROT_HIT_PRODUCER
#define CBM_RICH_PROT_HIT_PRODUCER

#include "FairTask.h"

class CbmGeoRichPar;
class TClonesArray;
class TVector3;

/**
* \class CbmRichProtHitProducer
*
* \brief Class for producing RICH hits directly from MCPoints.
*
* \author D. Kresan
* \date 2010
**/
class CbmRichProtHitProducer : public FairTask {

public:

  /**
   * \brief Default constructor.
   */
  CbmRichProtHitProducer();

  /**
   * \brief Destructor
   */
  virtual ~CbmRichProtHitProducer();

  /**
   * \brief Inherited from FairTask.
   */
  virtual void SetParContainers();

  /**
   * \brief Inherited from FairTask.
   */
  virtual InitStatus Init();

  /**
   * \brief Inherited from FairTask.
   */
  virtual void Exec(
       Option_t* option);

  /**
   * \brief Inherited from FairTask.
   */
  virtual void Finish();

  /**
   * \brief Set detector type (choose: 1=Protvino, 2=CsI, 3=Hamamatsu, 4=Hamamatsu H8500-03)
   */
  void SetDetectorType(Int_t detType){ fDetType = detType;}

  /**
   * \brief Set number of noise hits to be added.
   */
  void SetNofNoiseHits(Double_t noise) {fNofNoiseHits = noise;}

  /**
   * \brief Set collection efficiency for photoelectrons in PMT optics.
   */
  void SetCollectionEfficiency(Double_t collEff){fCollectionEfficiency = collEff;}

  /**
   * \brief Set additional smearing of MC Points due to light scattering in mirror.
   */
  void SetSigmaMirror(Double_t sigMirror) {fSigmaMirror = sigMirror;}

  /**
   * \brief Set cross talk probability.
   */
  void SetCrossTalkHitProb(Double_t ct) {fCrossTalkHitProb = ct;}

  /**
   * \brief Adds a RichHit to the HitCollection.
   * \param posHit Hit position.
   * \param posHitErr Hit errors.
   * \param address Detector ID.
   * \param pmtID PMT ID.
   * \param ampl Hit amplitude.
   * \param index Index.
   */
  void AddHit(
        TVector3 &posHit,
        TVector3 &posHitErr,
        Int_t address,
        Int_t pmtID,
        Double_t ampl,
        Int_t index);

  /* Add cross talk hits
   * @param x X position of the central hit
   * @param y Y position of the central hit
   * @param pointInd Index of the Rich Point of the central hit
   * @param RichDetID ID of the RICH detector*/
  void AddCrossTalkHits(
        Double_t x,
        Double_t y,
        Int_t pointInd,
        Int_t RichDetID);

  /**
   * \brief Finds hit position in PMT plane.
   * \param xPoint
   * \param yPoint
   * \param xHit
   * \param yHit
   * \param pmtID
   */
  void FindRichHitPositionSinglePMT(
        Double_t xPoint,
        Double_t yPoint,
        Double_t& xHit,
        Double_t& yHit,
        Int_t& pmtID);

  /**
   * \brief Finds hit position in MAMPT plane.
   * \param sigma
   * \param xPoint
   * \param yPoint
   * \param xHit
   * \param yHit
   * \param pmtID
   */
  void FindRichHitPositionMAPMT(
        Double_t sigma,
        Double_t xPoint,
        Double_t yPoint,
        Double_t& xHit,
        Double_t& yHit,
        Int_t & pmtID);

  /**
   * \brief Finds hit position in MAMPT plane.
   * \param xPoint
   * \param yPoint
   * \param xHit
   * \param yHit
   * \param pmtID
   */
  void FindRichHitPositionCsI(
        Double_t xPoint,
        Double_t yPoint,
        Double_t& xHit,
        Double_t& yHit,
        Int_t& pmtID);

  /**
   * \brief Spectrum of the PMT response to one photo-electron.
   */
  Double_t OnePhotonAmplitude(
       Double_t x);

  /**
   * \brief Generate randomly PMT amplitude according to probability density
   * provided by OnePhotonAmplitude(x).
   */
  Double_t GetAmplitude();

  /**
   * \brief Set parameters of the photodetector.
   * \param[in] det_type Type of the photodetector.
   * \param[out] lambda_min Minimum lambda for quantum efficiency table.
   * \param[out] lambda_max Maximum lambda for quantum efficiency table.
   * \param[out] lambda_step Step of the lambda for quantum efficiency table.
   * \param[out] efficiency[] Quantum efficiencies.
   */
  void SetPhotoDetPar(
        Int_t det_type,
        Double_t& lambda_min,
        Double_t& lambda_max,
        Double_t& lambda_step,
        Double_t efficiency[]);

private:

  TClonesArray* fRichPoints; // RICH MC points
  TClonesArray* fRichHits; // RICH hits
  TClonesArray* fMcTracks; // Monte-Carlo tracks

  Int_t fNHits; // Number of hits
  Int_t fNDoubleHits; // Number of double hits

  static const Double_t c; // speed of light
  static const Double_t h; // Planck constant
  static const Double_t e; // elementary charge

  Double_t fNRefrac; // refraction index
  Int_t fDetection; // flag for detection
  Int_t fNEvents; // event number

  Double_t fDetX; // X-coordinate of photodetector
  Double_t fDetY; // Y-coordinate of photodetector
  Double_t fDetZ; // Z-coordinate of photodetector
  Double_t fDetZ_org; // X-coordinate of photodetector (original from parameter file)
  Double_t fDetWidthX; // width of photodetector in x
  Double_t fDetWidthY; // width of photodetector in y


  TObjArray *fSensNodes;
  TObjArray *fPassNodes;
  CbmGeoRichPar *fPar;

  // Parameters of photodetector
  Double_t fPhotomulRadius; // radius of photomultiplier [cm], set according to detector type
  Double_t fPhotomulDist; // distance between PMT tube [cm], set according to detector type
  Int_t fDetType; // detector type
  Double_t fNofNoiseHits; // number of noise hits
  Int_t fNofNoiseHitsCounter; // counter for noise hits
  Double_t fCollectionEfficiency; // collection efficiency for photoelectrons in PMT optics
  Double_t fSigmaMirror; // additinal smearing of MC Points due to light scattering in mirror

  Double_t fTheta; // angle by which photodetector was tilted around x-axis
  Double_t fPhi; // angle by which photodetector was tilted around y-axis

  Double_t fCrossTalkHitProb;// probability of cross talk hit
  Int_t fNofCrossTalkHits; // Number of cross talk hits

  /**
   * \brief Copy constructor.
   */
  CbmRichProtHitProducer(const CbmRichProtHitProducer&);

  /**
   * \brief Assignment operator.
   */
  CbmRichProtHitProducer& operator=(const CbmRichProtHitProducer&);

  ClassDef(CbmRichProtHitProducer,1)

};

#endif
