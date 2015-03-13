/**
 * \file CbmMuchPixelHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Class for pixel hits in MUCH detector.
 **/
#ifndef CBMMUCHPIXELHIT_H_
#define CBMMUCHPIXELHIT_H_

#include "CbmPixelHit.h"

class TVector3;

class CbmMuchPixelHit :public CbmPixelHit
{
public:
   /**
    * \brief Default constructor.
    */
	CbmMuchPixelHit();

	/**
	 * Standard constructor.
	 * \param address detector unique identifier
	 * \param x X pposition of the hit [cm]
	 * \param y Y position of the hit [cm]
	 * \param z Z position of the hit [cm]
	 * \param dx X position error of the hit [cm]
	 * \param dy Y position error of the hit [cm]
	 * \param dz Z position error of the hit [cm]
	 * \param dxy XY correlation of the hit
	 * \param refId some reference ID
	 * \param planeId detector plane identifier
	 **/
	CbmMuchPixelHit(
         Int_t address,
         Double_t x,
         Double_t y,
         Double_t z,
         Double_t dx,
         Double_t dy,
         Double_t dz,
         Double_t dxy,
         Int_t refId,
         Int_t planeId,
         Double_t time,
         Double_t dtime);

	/**
	 * \brief Standard constructor
	 * \param address  Unique detector ID (including module number)
	 * \param pos Position in global c.s. [cm]
    * \param err Errors of position in global c.s. [cm]
	 * \param dxy XY correlation of the hit
	 * \param refId Index of digi or cluster
	 * \param planeId Detector plane identifier
	 **/
	CbmMuchPixelHit(
         Int_t address,
         const TVector3& pos,
         const TVector3& err,
         Double_t dxy,
         Int_t refId,
         Int_t planeId);

	/** Standard constructor
	  *\param address     Unique detector ID (including module number)
	  *\param pos       Position in global c.s. [cm]
	  *\param err       Errors of position in global c.s. [cm]
	  *\param dxy       Covariance of x and y
	  *\param refId     index of digi or cluster
	  *\param planeId   detectror plane identifier
	  *\param time      Time since event start [ns]
	  *\param dTime     Time resolution [ns]
	**/
	CbmMuchPixelHit(
			Int_t address,
			const TVector3& pos,
			const TVector3& err,
			Double_t dxy,
			Int_t refId,
			Int_t planeId,
			Double_t time,
			Double_t dtime);

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmMuchPixelHit();

	/**
	 * \brief Inherited from CbmBaseHit.
	 */
	virtual Int_t GetPlaneId() const { return fPlaneId; }

	/** Setters **/
	Double_t GetTime() const { return fTime; }
	Double_t GetDTime() const { return fDTime; }
	Int_t GetFlag() const { return fFlag; }

	/** Accessors **/
	void SetTime(Double_t time) { fTime = time; }
	void SetDTime(Double_t dtime) { fDTime = dtime; }
	void SetFlag(Int_t flag) { fFlag = flag; }

private:
   Int_t fPlaneId; // Plane number
   Double_t fTime; // Time since event start [ns]
   Double_t fDTime; // Time resolution [ns]
   Int_t fFlag; // Flag

	ClassDef(CbmMuchPixelHit, 2);
};

#endif /* CBMMUCHPIXELHIT_H_ */
