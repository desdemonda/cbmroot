/**
 * \file CbmLitReconstructionQa.h
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#ifndef CBMLITGLOBALELECTRONID_H_
#define CBMLITGLOBALELECTRONID_H_

#include "TObject.h"

class CbmRichElectronIdAnn;
class TClonesArray;
class CbmGlobalTrack;

class CbmLitGlobalElectronId: public TObject
{
public:
   /**
    * \brief Constructor.
    */
	CbmLitGlobalElectronId();

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmLitGlobalElectronId();

   /**
    * \brief Initialize TClonesArrays.
    */
	void Init();

   /**
    * \brief Identify electron in RICH detector.
    * \param[in] globalTrackIndex Index of global track.
    * \param[in] momentum Momentum of track.
    * \return true if track is identified as electron otherwise return false.
    */
	Bool_t IsRichElectron(
			Int_t globalTrackIndex,
			Double_t momentum);

   /**
    * \brief Identify electron in RICH detector.
    * \param[in] globalTrackIndex Index of global track.
    * \param[in] momentum Momentum of track.
    * \return true if track is identified as electron otherwise return false.
    */
	Bool_t IsTrdElectron(
			Int_t globalTrackindex,
			Double_t momentum);

   /**
    * \brief Identify electron in RICH detector.
    * \param[in] globalTrackIndex Index of global track.
    * \param[in] momentum Momentum of track.
    * \return true if track is identified as electron otherwise return false.
    */
	Bool_t IsTofElectron(
			Int_t globalTrackIndex,
			Double_t momentum);

   /**
    * \brief Identify electron in RICH detector.
    * \param[in] globalTrackIndex Index of global track.
    * \param[in] momentum Momentum of track.
    * \return true if track is identified as electron otherwise return false.
    */
	Bool_t IsElectron(
	      Int_t globalTrackIndex,
	      Double_t momentum);

	/**
	 * \brief Set cut on TRD ANN output value.
	 */
	void SetTrdAnnCut(Double_t par){fTrdAnnCut = par;}

	/**
	 * \brief Set cut on RICH ANN output value.
	 */
	void SetRichAnnCut(Double_t par){fRichAnnCut = par;}

private:
    Double_t fRichAnnCut;
    Bool_t fRichUseAnn;
    Double_t fRichMeanA;
    Double_t fRichMeanB;
    Double_t fRichRmsA;
    Double_t fRichRmsB;
    Double_t fRichRmsCoeff;
    Double_t fRichDistCut;
    Double_t fTrdAnnCut;

    CbmRichElectronIdAnn* fRichElIdAnn;

    TClonesArray* fGlobalTracks;
    TClonesArray* fRichRings;
    TClonesArray* fTrdTracks;
    TClonesArray* fTofHits;

    CbmLitGlobalElectronId(const CbmLitGlobalElectronId&);
    CbmLitGlobalElectronId& operator=(const CbmLitGlobalElectronId&);
    
    ClassDef(CbmLitGlobalElectronId, 1);
};

#endif /* CBMLITGLOBALELECTRONID_H_ */
