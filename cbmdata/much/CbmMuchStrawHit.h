/**
 * \file CbmMuchStrawHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Class for straw hits in MUCH detector.
 **/
#ifndef CBMMUCHSTRAWHIT_H_
#define CBMMUCHSTRAWHIT_H_

#include "CbmStripHit.h"

class TVector3;

class CbmMuchStrawHit : public CbmStripHit
{
public:
	/**
	 * \breif Default constructor.
	 */
	CbmMuchStrawHit();

	/**
	 * \brief Standard constructor.
	 * \param address detector unique identifier
	 * \param u coordinate in the rotated c.s. [cm]
	 * \param phi strip rotation angle [rad]
	 * \param z Z position of the hit [cm]
	 * \param du U measurement error [cm]
	 * \param dphi PHI measurement error [rad]
	 * \param z Z position of the hit [cm]
	 * \param refId some reference ID
	 **/
	CbmMuchStrawHit(
			Int_t address,
			Double_t u,
			Double_t phi,
			Double_t z,
			Double_t du,
			Double_t dphi,
			Double_t dz,
			Int_t refId,
			Int_t planeId);

	/**
	 * \brief Standard constructor
    * \param address Unique detector ID (including module number)
	 * \param pos U coordinate, rotation angle Phi, Z position in c.s.
	 * \param err Errors c.s. [cm]
	 * \param refId Index of digi or cluster
	**/
	CbmMuchStrawHit(
			Int_t address,
			const TVector3& pos,
			const TVector3& err,
			Int_t refId);

	/**
	 * \breif Destructor.
	 */
	virtual ~CbmMuchStrawHit();

	/**
	 * \brief Inherited from CbmBaseHit.
	 */

   CbmMuchStrawHit(const CbmMuchStrawHit&);

   Int_t GetPlaneId() const { return fPlaneId; }

   /** Accessores **/
   Double_t GetX() const {return fX;}
   Double_t GetY() const {return fY;}
   Double_t* GetDouble() { return fDouble; }
   Int_t* GetInt() { return fInt; }
   Int_t GetFlag() const { return fFlag; }
   Int_t GetTube() const { return fTube; }
   Int_t GetSegment() const { return fSegment; }

   /** Modifiers **/
   void SetX(Double_t x) { fX = x; }
   void SetY(Double_t y) { fY = y; }
   void SetDouble(Int_t n, Double_t *arr) { for (Int_t i=0; i<n; ++i) fDouble[i]=arr[i]; } // set double array
   void SetInt(Int_t n, Int_t *arr) { for (Int_t i=0; i<n; ++i) fInt[i]=arr[i]; } ; // set integer array
   void SetFlag(Int_t flag) { fFlag = flag; }
   void SetPlaneId(Int_t planeId) { fPlaneId = planeId; }
   void SetTube(Int_t tube) { fTube = tube; }
   void SetSegment(Int_t segm) { fSegment = segm; }

private:
   Double32_t fX; // Global X-coord.
   Double32_t fY; // Global Y-coord.
   Double_t fDouble[5]; //! Auxiliary double array
   Int_t fInt[5]; //! Auxiliary integer array
   Int_t fFlag; // Flag
   Int_t fPlaneId; // Plane ID
   Int_t fTube; // Tube number
   Int_t fSegment; // Tube segment

	ClassDef(CbmMuchStrawHit, 1);
};

#endif /* CBMMUCHSTRAWHIT_H_ */
