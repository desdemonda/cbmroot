#ifndef CBMTRDMODULE_H
#define CBMTRDMODULE_H 1

#include "TNamed.h"
#include "TArrayD.h"
#include "CbmTrdAsic.h"

#include <map>

class CbmTrdPoint;
class TVector3;

class CbmTrdModule : public TNamed
{
 public:

  /**
   * \brief Default constructor.
   **/
  CbmTrdModule();

  /**
   * \brief Constructor with parameters.
   */
  CbmTrdModule(
	       Int_t address, Int_t orientation, 
	       Double_t x, Double_t y, Double_t z,
	       Double_t sizex, Double_t sizey, Double_t sizez, Int_t nofSectors,
	       const TArrayD& sectorSizeX, const TArrayD& sectorSizeY,
	       const TArrayD& padSizeX, const TArrayD& padSizeY);

  CbmTrdModule(
	       Int_t address, Int_t orientation, Double_t x, Double_t y, Double_t z,
	       Double_t sizex, Double_t sizey, Double_t sizez, Int_t nofSectors,
	       const TArrayD& sectorSizeX, const TArrayD& sectorSizeY,
	       const TArrayD& padSizeX, const TArrayD& padSizeY,
	       const Bool_t padGeoTriangular);

  /**
   * \brief Destructor.
   **/
  virtual ~CbmTrdModule();

  /** Accessors **/
  Int_t GetModuleAddress() const { return fModuleAddress; }
  Int_t GetOrientation() const { return fOrientation; }
  Double_t GetX() const { return fX; }
  Double_t GetY() const { return fY; }
  Double_t GetZ() const { return fZ; }
  Double_t GetSizeX() const { return fSizeX; }
  Double_t GetSizeY() const { return fSizeY; }
  Double_t GetSizeZ() const { return fSizeZ; }

  Bool_t GetPadGeoTriangular() const { return fTriangularPads; }
  void SetPadGeoTriangular(Bool_t geoTriangular) { fTriangularPads = geoTriangular; }

  Int_t GetNofColumns() const;
  Int_t GetNofRows() const;

  Int_t GetNofColumnsInSector(Int_t i) const;
  Int_t GetNofRowsInSector(Int_t i) const;
  
  Double_t GetPadSizeX(Int_t i) const { return fPadSizeX.At(i); }
  Double_t GetPadSizeY(Int_t i) const { return fPadSizeY.At(i); }
  Double_t GetSectorSizeX(Int_t i) const { return fSectorSizeX.At(i); }
  Double_t GetSectorSizeY(Int_t i) const { return fSectorSizeY.At(i); }

  Double_t GetAnodeWireToPadPlaneDistance() const { return fAnodeWireToPadPlaneDistance; }
  Double_t GetAnodeWireOffset() const { return fAnodeWireOffset; }
  Double_t GetAnodeWireSpacing() const { return fAnodeWireSpacing; }

  CbmTrdAsic *GetAsic(Int_t AsicAddress);
  Int_t GetNofAsics();
  Int_t GetAsicAddress(Int_t channelAddress);
  std::vector<Int_t> GetAsicAddresses();

  Int_t GetNofSectors() const { return fNofSectors; }

  Int_t GetModuleRow(
		     Int_t& sectorId,
		     Int_t& rowId) const;

  Bool_t GetPadInfo(
		    const Double_t* local_point,
		    Int_t& sectorId,
		    Int_t& columnId,
		    Int_t& rowId) const;

  void GetPadInfo(
		  const CbmTrdPoint* trdPoint,
		  Int_t& sectorId,
		  Int_t& columnId,
		  Int_t& rowId) const;

  void GetPadPosition(
		      const Int_t digiAddress,
		      TVector3& padPos,
		      TVector3& padPosErr) const;

  void GetPosition(
		   Int_t moduleAddress,
		   Int_t sectorId,
		   Int_t columnId,
		   Int_t rowId,
		   TVector3& padPos,
		   TVector3& padSize) const;

  void TransformHitError(
			 TVector3& hitErr) const;

  void TransformToLocalPad(
			   const Double_t* local_point,
			   Double_t& posX,
			   Double_t& posY) const;

  void ProjectPositionToNextAnodeWire(
				      Double_t* local_point) const;

  Int_t GetSector(
		  Int_t npady,
		  Int_t& rowId) const;

 private:

  void GetModuleInformation(
			    Int_t moduleAddress,
			    const Double_t* local_point,
			    Int_t& sectorId,
			    Int_t& columnId,
			    Int_t& rowId) const;

  void TransformToLocalCorner(
			      const Double_t* local_point,
			      Double_t& posX,
			      Double_t& posY) const;

  void TransformToLocalSector(
			      const Double_t* local_point,
			      Double_t& posX,
			      Double_t& posY) const;

  Int_t GetSector(
		  const Double_t* local_point) const;

  void InitAsics();

  Int_t fModuleAddress; // unique detector ID
  Int_t fOrientation; // angle between long pad axis and y-axis in steps of 90 deg [0..3]
  Double_t fX; // center of module in global c.s. [cm]
  Double_t fY; // center of module in global c.s. [cm]
  Double_t fZ; // center of module in global c.s. [cm]
  Double_t fSizeX; // module half size in x [cm]
  Double_t fSizeY; // module half size in y [cm]
  Double_t fSizeZ; // module half size in z [cm]

  Double_t fAnodeWireOffset; // [cm]
  Double_t fAnodeWireSpacing; // [cm]
  Double_t fAnodeWireToPadPlaneDistance; // [cm]

  Int_t fNofSectors; // number sectors for this module
  TArrayD fSectorX; // center of sectors local c.s. [cm]
  TArrayD fSectorY; // center of sectors local c.s. [cm]
  TArrayD fSectorZ; // center of sectors local c.s. [cm]
  TArrayD fSectorBeginX; // begin of sector [cm]
  TArrayD fSectorBeginY; // begin of sector [cm]
  TArrayD fSectorEndX; // end of sector [cm]
  TArrayD fSectorEndY; // end of sector [cm]
  TArrayD fSectorSizeX; // sector size in x [cm]
  TArrayD fSectorSizeY; // sector size in y [cm]

  TArrayD fPadSizeX; // size of the readout pad in x [cm]
  TArrayD fPadSizeY; // size of the readout pad in y [cm]

  Bool_t fTriangularPads;

  std::map<Int_t, CbmTrdAsic*> fAsicMap;
  Int_t fNofAsics;

  ClassDef(CbmTrdModule, 4);
};

#endif
