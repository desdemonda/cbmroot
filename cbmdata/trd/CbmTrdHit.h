/**
 * \file CbmTrdHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Class for hits in TRD detector.
 **/

#ifndef CBMTRDHIT_H_
#define CBMTRDHIT_H_

#include "CbmTrdAddress.h"
#include "CbmPixelHit.h"

class TVector3;

class CbmTrdHit : public CbmPixelHit
{
public:
	/**
	 * \brief Default constructor.
	 */
	CbmTrdHit();

	/**
	 * \brief Standard constructor.
	  *\param address Unique detector ID
	  *\param pos Position in global c.s. [cm]
     *\param dpos Errors of position in global c.s. [cm]
	  *\param dxy XY correlation of the hit
	  *\param refId Index of digi or cluster
	  *\param eLossTR Energy losses of electron via TR
	  *\param eLossdEdx Energy deposition in the detector without TR
	  *\param eLoss TR + dEdx
	  **/
	CbmTrdHit(
			Int_t address,
			const TVector3& pos,
			const TVector3& dpos,
			Double_t dxy,
			Int_t refId,
			Double_t eLossTR,
			Double_t eLossdEdx,
			Double_t eLoss);

	/**
	 * \breif Destructor.
	 */
	virtual ~CbmTrdHit();

	/**
	 * \brief Inherited from CbmBaseHit.
	 */
   Int_t GetPlaneId() const { return CbmTrdAddress::GetLayerId(GetAddress()); }

   /**
    * \brief Inherited from CbmBaseHit.
    **/
   virtual string ToString() const;

	/** Accessors **/
	Double_t GetELoss() const { return fELoss; }
	Double_t GetELossdEdX() const { return fELossdEdx; }
	Double_t GetELossTR() const { return fELossTR; }

	/** Setters **/
	void SetELoss(Double_t loss) { fELoss = loss; }
	void SetELossTR(Double_t loss) { fELossTR = loss; }
	void SetELossdEdx(Double_t loss) { fELossdEdx = loss; }

private:
   Double32_t fELossTR; // energy losses of electron via TR
   Double32_t fELossdEdx; // energy deposition in the detector without TR
   Double32_t fELoss; // TR + dEdx

   ClassDef(CbmTrdHit, 2);
};

#endif
