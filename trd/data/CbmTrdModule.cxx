#include "CbmTrdModule.h"

#include "CbmTrdPoint.h"
#include "CbmTrdAddress.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TMath.h"

#include <iomanip>

CbmTrdModule::CbmTrdModule() 
  : TNamed(),
    fModuleAddress(0),
    fOrientation(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fSizeX(0.),
    fSizeY(0.),
    fSizeZ(0.),
    fAnodeWireOffset(0),
    fAnodeWireSpacing(0),
    fAnodeWireToPadPlaneDistance(0),
    fNofSectors(0),  
    fSectorX(0),    
    fSectorY(0),    
    fSectorZ(0),    
    fSectorBeginX(0),
    fSectorBeginY(0),
    fSectorEndX(0),
    fSectorEndY(0),
    fSectorSizeX(0),
    fSectorSizeY(0),
    fPadSizeX(0),
    fPadSizeY(0),
    fTriangularPads(false),
    fAsicMap(),
    fNofAsics(0)
{
}
CbmTrdModule::CbmTrdModule(
			   Int_t address, Int_t orientation, Double_t x, Double_t y, Double_t z,
			   Double_t sizex, Double_t sizey, Double_t sizez, Int_t nofSectors,
			   const TArrayD& sectorSizeX, const TArrayD& sectorSizeY,
			   const TArrayD& padSizeX, const TArrayD& padSizeY,
			   const Bool_t padGeoTriangular)
  : TNamed(),
    fModuleAddress(address),
    fOrientation(orientation),
    fX(x),
    fY(y),
    fZ(z),
    fSizeX(sizex),
    fSizeY(sizey),
    fSizeZ(sizez),
    fAnodeWireOffset(0.375),
    fAnodeWireSpacing(0.25),
    fAnodeWireToPadPlaneDistance(0.35),
    fNofSectors(nofSectors),
    fSectorX(nofSectors),
    fSectorY(nofSectors),
    fSectorZ(nofSectors),
    fSectorBeginX(nofSectors),
    fSectorBeginY(nofSectors),
    fSectorEndX(nofSectors),
    fSectorEndY(nofSectors),
    fSectorSizeX(sectorSizeX),
    fSectorSizeY(sectorSizeY),
    fPadSizeX(padSizeX),
    fPadSizeY(padSizeY) ,
    fTriangularPads(padGeoTriangular),
    fAsicMap(),
    fNofAsics(0)
{
  CbmTrdModule(address, orientation, x, y, z,
	       sizex, sizey, sizez, nofSectors,
	       sectorSizeX, sectorSizeY,
	       padSizeX, padSizeY);
}


CbmTrdModule::CbmTrdModule(
			   Int_t address, Int_t orientation, Double_t x, Double_t y, Double_t z,
			   Double_t sizex, Double_t sizey, Double_t sizez, Int_t nofSectors,
			   const TArrayD& sectorSizeX, const TArrayD& sectorSizeY,
			   const TArrayD& padSizeX, const TArrayD& padSizeY)
  : TNamed(),
    fModuleAddress(address),
    fOrientation(orientation),
    fX(x),
    fY(y),
    fZ(z),
    fSizeX(sizex),
    fSizeY(sizey),
    fSizeZ(sizez),
    fAnodeWireOffset(0.375),
    fAnodeWireSpacing(0.25),
    fAnodeWireToPadPlaneDistance(0.35),
    fNofSectors(nofSectors),
    fSectorX(nofSectors),
    fSectorY(nofSectors),
    fSectorZ(nofSectors),
    fSectorBeginX(nofSectors),
    fSectorBeginY(nofSectors),
    fSectorEndX(nofSectors),
    fSectorEndY(nofSectors),
    fSectorSizeX(sectorSizeX),
    fSectorSizeY(sectorSizeY),
    fPadSizeX(padSizeX),
    fPadSizeY(padSizeY) ,
    fTriangularPads(false),
    fAsicMap(),
    fNofAsics(0)
{
  // Calculate the coordinates of the begin and the end of each sector
  // as well as the coordinates of the center of the sector
  // In the moment it is assumed that there are sectors which have either
  // in x- or y-direction the size of the chamber.

  if (nofSectors > 1) {
    Double_t beginx, beginy, endx, endy;
    Double_t summed_sectsize;
    if (sectorSizeX.GetAt(0) == 2 * sizex) { //substructure only in y-direction
      beginx = 0;
      endx = 2 * sizex;
      summed_sectsize = 0;
      for (Int_t i = 0; i < fNofSectors; i++) {
	if (0 == i) {
	  beginy = 0.;
	  endy = sectorSizeY.GetAt(i);
	  summed_sectsize += sectorSizeY.GetAt(i);
	} else {
	  beginy = summed_sectsize;
	  endy = summed_sectsize += sectorSizeY.GetAt(i);
	}
	fSectorBeginX.AddAt(beginx, i);
	fSectorBeginY.AddAt(beginy, i);
	fSectorEndX.AddAt(endx, i);
	fSectorEndY.AddAt(endy, i);
	fSectorX.AddAt(beginx + (sectorSizeX.GetAt(i) / 2.), i);
	fSectorY.AddAt(beginy + (sectorSizeY.GetAt(i) / 2.), i);
	fSectorZ.AddAt(fZ, i);
      }
    } else {
      beginy = 0;
      endy = 2 * sizey;
      summed_sectsize = 0;
      for (Int_t i = 0; i < fNofSectors; i++) {
	if (0 == i) {
	  beginx = 0.;
	  endx = sectorSizeX.GetAt(i);
	  summed_sectsize += sectorSizeX.GetAt(i);
	} else {
	  beginx = summed_sectsize;
	  endx = summed_sectsize += sectorSizeX.GetAt(i);
	}
	fSectorBeginX.AddAt(beginx, i);
	fSectorBeginY.AddAt(beginy, i);
	fSectorEndX.AddAt(endx, i);
	fSectorEndY.AddAt(endy, i);
	fSectorX.AddAt(beginx + (sectorSizeX.GetAt(i) / 2.), i);
	fSectorY.AddAt(beginy + (sectorSizeY.GetAt(i) / 2.), i);
	fSectorZ.AddAt(fZ, i);
      }
    }
  } else {
    fSectorBeginX.AddAt(0., 0);
    fSectorBeginY.AddAt(0., 0);
    fSectorEndX.AddAt(sectorSizeX.GetAt(0), 0);
    fSectorEndY.AddAt(sectorSizeY.GetAt(0), 0);
    fSectorX.AddAt(x, 0);
    fSectorY.AddAt(y, 0);
    fSectorZ.AddAt(z, 0);
  }
  InitAsics();
}

CbmTrdModule::~CbmTrdModule() 
{
  std::map<Int_t, CbmTrdAsic*>::iterator AsicMapIt;
  for (AsicMapIt = fAsicMap.begin(); AsicMapIt != fAsicMap.end(); AsicMapIt++)
    delete AsicMapIt->second;
  fAsicMap.clear();
}

void CbmTrdModule::InitAsics() {
  Int_t iAsic = 0;
  Int_t nChannels = 32;
  Int_t iFebGroup = 0; // 1; 2;  // normal, super, ultimate
  Int_t gRow[3] = {  2, 2, 3 };  // re-ordering on the feb -> same mapping for normal and super
  Int_t gCol[3] = { 16,16,10 };  // re-ordering on the feb -> same mapping for normal and super
//  Int_t gRow[3] = {  2, 4, 6 };  // intuitive, but wrong mapping
//  Int_t gCol[3] = { 16, 8, 5 };  // intuitive, but wrong mapping
  Double_t sizeAsicX = 6.0; // 2.0; // in cm
  Double_t sizeAsicY = 3.0; // 2.0; // in cm
  Double_t sizeAsicZ = 0.5;         // in cm
  Double_t xAsic = 0;  // x position of Asic
  Double_t yAsic = 0;  // y position of Asic

  Int_t rowId = 0;
  Int_t isecId = 0;
  Int_t irowId = 0;

  for (Int_t s = 0; s < fNofSectors; s++) { //new    
    for (Int_t r = 0; r < GetNofRowsInSector(s); r++){ //new
      for (Int_t c = 0; c < GetNofColumnsInSector(s); c++){ //new
	//for (Int_t r = 0; r < GetNofRows(); r++){
	//for (Int_t c = 0; c < GetNofColumns(); c++){
	// ultimate density 6 rows,  5 pads
	// super    density 4 rows,  8 pads
	// normal   density 2 rows, 16 pads
	if ((rowId % gRow[iFebGroup]) == 0){
	  if ((c % gCol[iFebGroup]) == 0){
	    xAsic =     c + gCol[iFebGroup] / 2.;
	    yAsic =     r + gRow[iFebGroup] / 2.;

	    Double_t local_point[3];
	    Double_t padsizex = fPadSizeX.At(s);
	    Double_t padsizey = fPadSizeY.At(s);

	    // calculate position in sector coordinate system
	    // with the origin in the lower left corner (looking upstream)
	    local_point[0] = ((Int_t)(xAsic + 0.5) * padsizex);
	    local_point[1] = ((Int_t)(yAsic + 0.5) * padsizey);

	    // calculate position in module coordinate system
	    // with the origin in the lower left corner (looking upstream)
	    local_point[0] += fSectorBeginX.GetAt(s);
	    local_point[1] += fSectorBeginY.GetAt(s);

	    // local_point[i] must be >= 0 at this point      Double_t local_point[3];

	    fAsicMap[iAsic] = new CbmTrdAsic(
					     iAsic, nChannels, iFebGroup,
					     local_point[0] - fSizeX, local_point[1] - fSizeY, 0.,
					     sizeAsicX, sizeAsicY, sizeAsicZ);
	    if (local_point[0] > 2*fSizeX)     LOG(ERROR) << "CbmTrdModule::InitAsics: asic position x=" << local_point[0] << " is out of bounds [0," << 2*fSizeX<< "]!" << FairLogger::endl;
	    if (local_point[1] > 2*fSizeY)     LOG(ERROR) << "CbmTrdModule::InitAsics: asic position y=" << local_point[1] << " is out of bounds [0," << 2*fSizeY<< "]!" << FairLogger::endl;

	    for (Int_t ir = rowId; ir < rowId + gRow[iFebGroup]; ir++) {
	      for (Int_t ic = c; ic < c + gCol[iFebGroup]; ic++) {
		if (ir >= GetNofRows() )     LOG(ERROR) << "CbmTrdModule::InitAsics: ir " << ir << " is out of bounds!" << FairLogger::endl;
		if (ic >= GetNofColumns() )  LOG(ERROR) << "CbmTrdModule::InitAsics: ic " << ic << " is out of bounds!" << FairLogger::endl;
		isecId = GetSector((Int_t)ir, irowId);
		fAsicMap[iAsic]->fChannelAddresses.push_back(CbmTrdAddress::GetAddress(CbmTrdAddress::GetLayerId(fModuleAddress),
										       CbmTrdAddress::GetModuleId(fModuleAddress),
										       isecId, irowId, ic));
		//s, ir, ic));//new
		if (false)
		  printf("               M:%10i(%4i) s: %i  irowId: %4i  ic: %4i r: %4i c: %4i   address:%10i\n",fModuleAddress,
			 CbmTrdAddress::GetModuleId(fModuleAddress),
			 isecId, irowId, ic, r, c,
			 CbmTrdAddress::GetAddress(CbmTrdAddress::GetLayerId(fModuleAddress),
						   CbmTrdAddress::GetModuleId(fModuleAddress),
						   isecId, irowId, ic));
	      } 
	    } 
	    
	    iAsic++;  // next Asic
	  }
	}
      }
      rowId++;
    }
  }
  // Self Test 
  for (Int_t s = 0; s < fNofSectors; s++){
    const Int_t nRow = GetNofRowsInSector(s);
    const Int_t nCol = GetNofColumnsInSector(s);
    for (Int_t r = 0; r < nRow; r++){
      for (Int_t c = 0; c < nCol; c++){
	Int_t channelAddress = CbmTrdAddress::GetAddress(CbmTrdAddress::GetLayerId(fModuleAddress),
							 CbmTrdAddress::GetModuleId(fModuleAddress), 
							 s, r, c);
	if (GetAsicAddress(channelAddress) == -1)
	  LOG(ERROR) << "CbmTrdModule::InitAsics: Channel address:" << channelAddress << " is not or multible initialized in module " << fModuleAddress << "(ID:" << CbmTrdAddress::GetModuleId(fModuleAddress) << ")" << "(s:" << s << ", r:" << r << ", c:" << c << ")" << FairLogger::endl;
      }
    }
  }
}
  // 20131209 - CB 
  CbmTrdAsic *CbmTrdModule::GetAsic(Int_t AsicAddress) {
    if (fAsicMap.size() == 0){ // Init ASICs
      InitAsics();
    }
    return fAsicMap[AsicAddress];
  }
  Int_t CbmTrdModule::GetAsicAddress(Int_t channelAddress) {
    if (fAsicMap.size() == 0){ // Init ASICs
      InitAsics();
    }
    Int_t counter(0), AsicAddress(-1);
    std::map<Int_t, CbmTrdAsic*>::iterator it;
    for (it = fAsicMap.begin(); it != fAsicMap.end(); it++)
      for (Int_t ch = 0; ch < it->second->fChannelAddresses.size(); ch++) {
	if (it->second->fChannelAddresses[ch] == channelAddress){
	  counter++;
	  AsicAddress = it->first;
	  if (counter > 1)
	    LOG(ERROR) << "CbmTrdModule::InitAsics: Channel address:" << channelAddress << " is found " << counter << " times in module " << fModuleAddress << " for ASIC address: " << AsicAddress << FairLogger::endl;
	  //return it->first;
	}
      }
    if (counter > 1){
      LOG(ERROR) << "CbmTrdModule::InitAsics: Channel address:" << channelAddress << " is found " << counter << " times in module " << fModuleAddress << FairLogger::endl;
      return -1;
    } 
    return AsicAddress;
  }
  Int_t CbmTrdModule::GetNofAsics(){
    if (fAsicMap.size() == 0)
      InitAsics(); 
    fNofAsics = fAsicMap.size();
    return fNofAsics; 
  }

  std::vector<Int_t> CbmTrdModule::GetAsicAddresses(){
    if (fAsicMap.size() == 0){ // Init ASICs
      InitAsics();
    }
    std::vector<Int_t> addresses;
    std::map<Int_t, CbmTrdAsic*>::iterator fAsicMapIt;
    for (fAsicMapIt = fAsicMap.begin(); fAsicMapIt != fAsicMap.end(); fAsicMapIt++)
      addresses.push_back(fAsicMapIt->first);
    return addresses;
  }
  // 20131009 - DE - checked OK
  void CbmTrdModule::ProjectPositionToNextAnodeWire(
						    Double_t* local_point) const
  {

    // Move the local point along y to the next anode wire position.
    if (fAnodeWireOffset > 0.0 && fAnodeWireSpacing > 0.0) {  // wires must be defined defined

      // check, if the input is within the allowed range
      if ( fabs(local_point[1]) > fSizeY )
        LOG(ERROR) << "CbmTrdModule::ProjectPositionToNextAnodeWire - local point must be within gas volume, y=" << std::setprecision(5) << local_point[1] << FairLogger::endl;

      Double_t ypos = local_point[1];

      LOG(DEBUG2) << "local y before projection: " << std::setprecision(5) << local_point[1] << " mm" << FairLogger::endl;

      // make sure, local_point[1] is not negative (due to module center coordinate)
      // therefore transform to local corner first and then back at the end of operation
      local_point[1] += fSizeY;   // transform to module corner coordinates

      local_point[1] = Int_t(((local_point[1] - fAnodeWireOffset) / fAnodeWireSpacing) + 0.5) * fAnodeWireSpacing + fAnodeWireOffset;   // find closest anode wire

      if ( local_point[1] > 2 * fSizeY - fAnodeWireOffset )
	local_point[1] = 2 * fSizeY - fAnodeWireOffset;   // move inwards to the last anode wire

      local_point[1] -= fSizeY;   // transform back to module center coordinates

      LOG(DEBUG2) << "local y after projection: " << std::setprecision(5) << local_point[1] << " mm" << FairLogger::endl;

      // check, if we have left the anode wire grid
      if ( fabs(local_point[1]) > fSizeY - fAnodeWireOffset )
        LOG(ERROR) << "CbmTrdModule::ProjectPositionToNextAnodeWire - local point projected outside anode wire plane, from " 
                   << std::setprecision(5) << ypos << " to "
                   << std::setprecision(5) << local_point[1] << " - last anode wire at "
                   << std::setprecision(5) << fSizeY - fAnodeWireOffset << FairLogger::endl;

    } else {
      LOG(ERROR) << "CbmTrdModule::ProjectPositionToNextAnodeWire: fAnodeWireOffset and fAnodeWireSpacing not set. ProjectPositionToNextAnodeWire can not be used." << FairLogger::endl;
    }
  }


  Int_t CbmTrdModule::GetSector(
				const Double_t* local_point) const
  {
    // Calculate the position in the chamber 
    // with the origin of the local coordinate system 
    // in the lower left corner of the chamber (looking upstream)
    // x goes to the left, looking in beam direction, y goes upward

    Double_t posx, posy;

    TransformToLocalCorner(local_point, posx, posy);

    for (Int_t i = 0; i < fNofSectors; i++) {
      if (posx >= fSectorBeginX.GetAt(i) && posx < fSectorEndX.GetAt(i)
	  && posy >= fSectorBeginY.GetAt(i) && posy < fSectorEndY.GetAt(i)) {
	return i;
      } 
    }

    LOG(ERROR) << "CbmTrdModule::GetSector: Could not find local point in any of the sectors" << FairLogger::endl;
    return -1; // This should be never reached
  }


  Int_t CbmTrdModule::GetSector(
				Int_t npady,
				Int_t& rowId) const
  {
    // Calculate the pad coordinates in this sector
    // from pad coordinates in the module
    // e.g. in which sector is pad (20/28)

    if ( (npady < 0) || (npady > GetNofRows()-1) ) {
      LOG(ERROR) << "CbmTrdModule::GetSector - there is no such row number: " << npady << FairLogger::endl;
      return -1;
    }

    Int_t secRows = 0;   // number of rows in sector
    Int_t nofRows = 0;   // number of rows in total

    if (fSectorSizeY.At(0) < fSizeY)  // if there are sectors in y direction
      for (Int_t iSector = 0; iSector < fNofSectors; iSector++) {  // for each sector
        secRows = (Int_t)(fSectorSizeY.At(iSector) / fPadSizeY.At(iSector) + 0.5);     // need to round for correct result
        if ( npady < nofRows + secRows ) {  // if we are in the sector containing the pad
                                            // note nypad = 12 is not in sector 0, with rowIds 0..11
          rowId = npady - nofRows;

	  LOG(DEBUG2) << "npady   : " << npady << " <= " << nofRows + secRows 
		      << "  rowId " << rowId << "  nRows(sec-1) " << nofRows 
		      << "  sec " << iSector << FairLogger::endl;
   
          if ( (rowId < 0) || (rowId > GetNofRowsInSector(iSector)-1) )
            LOG(FATAL) << "CbmTrdModule::GetModuleRow rowId "   << rowId     << " of " << GetNofRowsInSector(iSector)-1
                       << " in moduleAddress " << fModuleAddress << " is out of bounds!" << FairLogger::endl;

          return iSector;  // return sector
        }
	nofRows += secRows;  // sum up new total number of rows
      }

    LOG(ERROR) << "CbmTrdModule::GetSector: Could not find pad in any of the sectors" << FairLogger::endl;
    return -1; // This should be never reached
  }


  Int_t CbmTrdModule::GetNofColumns() const  // get total number of pad columns in module
  {
    Int_t nofColumns = 0;
    if (fSectorSizeX.At(0) < fSizeX) {
      for (Int_t i = 0; i < fNofSectors; i++) {
	nofColumns += (Int_t)(fSectorSizeX.At(i) / fPadSizeX.At(i) + 0.5);  // need to round for correct result
      }
    } else {
      nofColumns = (Int_t)(fSectorSizeX.At(0) / fPadSizeX.At(0) + 0.5);       // need to round for correct result
    }
    return nofColumns;
  }


  Int_t CbmTrdModule::GetNofRows() const  // get total number of pad rows in module
  {
    Int_t nofRows = 0;
    if (fSectorSizeY.At(0) < fSizeY) {
      for (Int_t i = 0; i < fNofSectors; i++) {
	nofRows += (Int_t)(fSectorSizeY.At(i) / fPadSizeY.At(i) + 0.5);     // need to round for correct result
      }
    } else {
      nofRows = (Int_t)(fSectorSizeY.At(0) / fPadSizeY.At(0) + 0.5);         // need to round for correct result
    }
    return nofRows;
  }


  Int_t CbmTrdModule::GetNofColumnsInSector(Int_t i) const
  {
    return (Int_t)(fSectorSizeX.At(i) / fPadSizeX.At(i) + 0.5);  // need to round for correct result
  }


  Int_t CbmTrdModule::GetNofRowsInSector(Int_t i) const
  {
    return (Int_t)(fSectorSizeY.At(i) / fPadSizeY.At(i) + 0.5);  // need to round for correct result
  }


  Int_t CbmTrdModule::GetModuleRow(
				   Int_t& sectorId,
				   Int_t& rowId) const
  {

    // check limits
    if ( (sectorId < 0) || (sectorId > GetNofSectors()-1) )
      LOG(FATAL) << "CbmTrdModule::GetModuleRow sectorId " << sectorId << " of " << GetNofSectors()-1
		 << " in moduleAddress " << fModuleAddress << " is out of bounds!" << FairLogger::endl;

    // check limits
    if ( (rowId < 0) || (rowId > GetNofRowsInSector(sectorId)-1) )
      LOG(FATAL) << "CbmTrdModule::GetModuleRow rowId "   << rowId     << " of " << GetNofRowsInSector(sectorId)-1
		 << " in moduleAddress " << fModuleAddress << " is out of bounds!" << FairLogger::endl;

    Int_t moduleRowId = rowId;

    // calculate row number within module
    for (Int_t iSector = 0; iSector < sectorId; iSector++)
      {
	moduleRowId += (Int_t)(fSectorSizeY.At(iSector) / fPadSizeY.At(iSector) + 0.5);
	//     LOG(INFO) << "adding sector "   << iSector << " of " << sectorId << FairLogger::endl;
      }

    //   if (sectorId == 0)
    //     // do nothing
    //   if (sectorId >= 1)
    //     moduleRowId += (Int_t)(fSectorSizeY.At(0) / fPadSizeY.At(0) + 0.5);
    //   if (sectorId >= 2)
    //     moduleRowId += (Int_t)(fSectorSizeY.At(1) / fPadSizeY.At(1) + 0.5);

    return moduleRowId;
  }


  Bool_t CbmTrdModule::GetPadInfo(
				  const Double_t *local_point,
				  Int_t& sectorId,
				  Int_t& columnId,
				  Int_t& rowId) const
  {
    // check, if the input is within the allowed range
    if ( fabs(local_point[0]) > fSizeX )
      {
	LOG(ERROR) << "CbmTrdModule::GetPadInfo - local point x must be within gas volume, x=" 
		   << std::setprecision(8) << local_point[0] << " fSizeX="
		   << std::setprecision(8) << fSizeX << FairLogger::endl;
	return kFALSE;
      }
    if ( fabs(local_point[1]) > fSizeY )
      {
	LOG(ERROR) << "CbmTrdModule::GetPadInfo - local point y must be within gas volume, y=" 
		   << std::setprecision(8) << local_point[1] << " fSizeY="
		   << std::setprecision(8) << fSizeY << FairLogger::endl;
	return kFALSE;
      }

    Double_t posX, posY;
    TransformToLocalSector(local_point, posX, posY);

    // calculate in which sector the point is
    sectorId = GetSector(local_point);

    columnId = (Int_t)(posX / fPadSizeX.At(sectorId));
    rowId    = (Int_t)(posY / fPadSizeY.At(sectorId));

    return kTRUE;
  }


  void CbmTrdModule::GetPadInfo(
				const CbmTrdPoint* trdPoint,
				Int_t& sectorId,
				Int_t& columnId,
				Int_t& rowId) const
  {
    // Calculate point in the middle of the detector. This is
    // for safety reasons, because the point at exit is slightly
    // outside of the active volume. If one does not use a point
    // in the detector one will get a wrong volume from the
    // geomanager. Navigate to the correct volume to get all
    // necessary information about this volume

    Double_t x_mean = 0.5 * (trdPoint->GetXIn() + trdPoint->GetXOut());
    Double_t y_mean = 0.5 * (trdPoint->GetYIn() + trdPoint->GetYOut());
    Double_t z_mean = 0.5 * (trdPoint->GetZIn() + trdPoint->GetZOut());
    gGeoManager->FindNode(x_mean, y_mean, z_mean);

    TString curNode = gGeoManager->GetPath();
    if (! curNode.Contains("gas")) {
      // if the point is not in the gas volume print warning
      LOG(WARNING) << "This point is not in the trd gas" << FairLogger::endl;
      /*
	LOG(WARNING) << "x: " << std::setprecision(7) << trdPoint->GetXIn() << ", " << trdPoint->GetXOut()<< FairLogger::endl;
	LOG(WARNING) << "y: " << std::setprecision(7) << trdPoint->GetYIn() << ", " << trdPoint->GetYOut()<< FairLogger::endl;
	LOG(WARNING) << "z: " << std::setprecision(7) << trdPoint->GetZIn() << ", " << trdPoint->GetZOut()<< FairLogger::endl;
      */
      sectorId=-1;
      return;
    }

    // Get the local point in local MC coordinates from
    // the geomanager. This coordinate system is rotated
    // if the chamber is rotated. This is corrected in
    // GetModuleInformation to have a
    // the same local coordinate system in all the chambers
    const Double_t* global_point = gGeoManager->GetCurrentPoint();
    Double_t local_point[3];

    gGeoManager->MasterToLocal(global_point, local_point);

    // 20131009 - DE - debuging output to check module orientation 0,1,2,3 with box generator
    if ( gLogger->IsLogNeeded(DEBUG2) )
      {

	// print module orientation
	LOG(DEBUG2) << "module orientation: " << std::setprecision(5) << fOrientation << FairLogger::endl;
  
	// print global coordinate
	LOG(DEBUG2) << "global x: " << std::setprecision(5) << global_point[0] 
		    << " y: " << std::setprecision(5) << global_point[1] 
		    << " z: " << std::setprecision(5) << global_point[2] << FairLogger::endl;
  
	// print local coordinate - relative to module center
	LOG(DEBUG2) << "local  x: " << std::setprecision(5) << local_point[0] 
		    << " y: " << std::setprecision(5) << local_point[1] 
		    << " z: " << std::setprecision(5) << local_point[2] << FairLogger::endl;
  
	// test projection to next anode wire
	Double_t proj_point[3];
	proj_point[0] = local_point[0];
	proj_point[1] = local_point[1];
	proj_point[2] = local_point[2];
  
	ProjectPositionToNextAnodeWire(proj_point);
  
	// print local coordinate - relative to module center
	LOG(DEBUG2) << "proj   x: " << std::setprecision(5) << proj_point[0] 
		    << " y: " << std::setprecision(5) << proj_point[1] 
		    << " z: " << std::setprecision(5) << proj_point[2] << FairLogger::endl;
  
	Double_t corner_point[3];
	corner_point[2] = local_point[2];
	TransformToLocalCorner(local_point, corner_point[0], corner_point[1]);
  
	// print local coordinate - relative to module corner
	LOG(DEBUG2) << "corner x: " << std::setprecision(5) << corner_point[0] 
		    << " y: " << std::setprecision(5) << corner_point[1] 
		    << " z: " << std::setprecision(5) << corner_point[2] << FairLogger::endl;
  
	LOG(DEBUG2) << "pos    x: " << std::setprecision(5) << fX 
		    << " y: " << std::setprecision(5) << fY 
		    << " z: " << std::setprecision(5) << fZ 
		    << " ori: " << std::setprecision(5) << fOrientation << FairLogger::endl;
  
	LOG(DEBUG2) << "size/2 x: " << std::setprecision(5) << fSizeX 
		    << " y: " << std::setprecision(5) << fSizeY 
		    << " z: " << std::setprecision(5) << fSizeZ << FairLogger::endl;
  
	Double_t sector_point[3];
	sector_point[2] = local_point[2];
	TransformToLocalSector(local_point, sector_point[0], sector_point[1]);
  
	// print local coordinate - relative to module sector
	LOG(DEBUG2) << "sector x: " << std::setprecision(5) << sector_point[0] 
		    << " y: " << std::setprecision(5) << sector_point[1] 
		    << " z: " << std::setprecision(5) << sector_point[2] << FairLogger::endl;
  
	// calculate in which sector the point is
	sectorId = GetSector(local_point);
	LOG(DEBUG2) << "sectornr: " << std::setprecision(5) << sectorId << FairLogger::endl;
  
	LOG(DEBUG2) << "ncol    : " << std::setprecision(5) << GetNofColumns() << FairLogger::endl;
	LOG(DEBUG2) << "nrow    : " << std::setprecision(5) << GetNofRows()    << FairLogger::endl;
  
	// print local coordinate - relative to module sector
	LOG(DEBUG2) << "sec2   x: " << std::setprecision(5) << fSectorBeginX.GetAt(2)
		    << " y: " << std::setprecision(5) << fSectorBeginY.GetAt(2) << FairLogger::endl;
	LOG(DEBUG2) << "sec1   x: " << std::setprecision(5) << fSectorBeginX.GetAt(1)
		    << " y: " << std::setprecision(5) << fSectorBeginY.GetAt(1) << FairLogger::endl;
	LOG(DEBUG2) << "sec0   x: " << std::setprecision(5) << fSectorBeginX.GetAt(0)
		    << " y: " << std::setprecision(5) << fSectorBeginY.GetAt(0) << FairLogger::endl;

	// get pad information
	Int_t s,c,r;
	GetPadInfo(local_point, s, c,r);
	LOG(DEBUG2) << "pad sec: " << s 
                    << " col: " << c
                    << " row: " << r << FairLogger::endl << FairLogger::endl;

      }  // debug2

    Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(trdPoint->GetDetectorID());
    GetModuleInformation(moduleAddress, local_point, sectorId, columnId, rowId);
  }


  // 20131009 - DE - checked OK for module orientations 0,1,2,3
  void CbmTrdModule::TransformToLocalCorner(
					    const Double_t* local_point,
					    Double_t& posX,
					    Double_t& posY) const
  {
    // Transformation from local coordinate system 
    // with origin in the middle of the module 
    // into a system with the origin in the lower left corner (looking upstream).
    // Since for both coordinate systems the orientation is the same 
    // this is only a shift by the half size of the module in x- and y-direction
    posX = local_point[0] + fSizeX;
    posY = local_point[1] + fSizeY;
  }


  // 20131009 - DE - checked OK for module orientations 0,1,2,3
  void CbmTrdModule::TransformToLocalSector(
					    const Double_t* local_point,
					    Double_t& posX,
					    Double_t& posY) const
  {
    // Transformation of the module coordinate system 
    // with origin in the middle of the module 
    // into a system with the origin in the lower left corner (looking upstream)
    // of the sector the point is in. 
    // First, transform in a system with origin in the lower left corner.
    TransformToLocalCorner(local_point, posX, posY);
    Int_t sector = GetSector(local_point);
    posX -= fSectorBeginX.GetAt(sector);
    posY -= fSectorBeginY.GetAt(sector);
  }


  void CbmTrdModule::TransformToLocalPad(
					 const Double_t* local_point,
					 Double_t& posX,
					 Double_t& posY) const
  {
    // Transformation of the local module coordinate system 
    // with origin in the middle of the module  
    // into a system with the origin in the local pad center
    // of the pad below the point.
    // First, transform in a system with origin in the lower left sector.

    Double_t sector_point[2];
    TransformToLocalSector(local_point, sector_point[0], sector_point[1]);

    //   TransformToLocalCorner(local_point, posX, posY);
    Int_t sectorId = GetSector(local_point);

    Double_t padx = (Int_t(sector_point[0] / fPadSizeX.At(sectorId)) + 0.5) * fPadSizeX.At(sectorId);  // x position of pad center 
    Double_t pady = (Int_t(sector_point[1] / fPadSizeY.At(sectorId)) + 0.5) * fPadSizeY.At(sectorId);  // y position of pad center 

    posX = sector_point[0] - padx;
    posY = sector_point[1] - pady;

    /*
    // print debug info
    LOG(INFO) << "sector x: " << std::setprecision(5) << sector_point[0] 
    << " y: " << std::setprecision(5) << sector_point[1] << FairLogger::endl;

    LOG(INFO) << "pad    x: " << std::setprecision(5) << padx 
    << " y: " << std::setprecision(5) << pady << FairLogger::endl;

    LOG(INFO) << "diff   x: " << std::setprecision(5) << posX 
    << " y: " << std::setprecision(5) << posY << FairLogger::endl;

    LOG(INFO) << "npad   x: " << std::setprecision(5) << padx / fPadSizeX.At(sectorId)
    << " y: " << std::setprecision(5) << pady / fPadSizeY.At(sectorId) << FairLogger::endl << FairLogger::endl;
    */
  }


  void CbmTrdModule::TransformHitError(
				       TVector3& hitErr) const
  {
    Double_t x,y; // ,z; 
    x = hitErr.X();
    y = hitErr.Y();
    //  z = hitErr.Z();

    //  LOG(INFO) << "ori : " << fOrientation << FairLogger::endl;

    if ((fOrientation == 1) || (fOrientation == 3))  // for orientations 1 or 3
      {
	hitErr.SetX(y);  // swap errors
	hitErr.SetY(x);  // swap errors

	//    LOG(INFO) << " swapped x and y error" << FairLogger::endl;
	//    LOG(INFO) << "ori : " << fOrientation << " swapped x and y error" << FairLogger::endl;
      }

  }


  void CbmTrdModule::GetModuleInformation(
					  Int_t moduleAddress,
					  const Double_t* local_point,
					  Int_t& sectorId,
					  Int_t& columnId,
					  Int_t& rowId) const
  {
    // safety check. Up to now always correct, so could be removed.
    if (fModuleAddress != moduleAddress) {
      LOG(ERROR) << "CbmTrdModule::GetModuleInformation: This is wrong!" << FairLogger::endl;
    }

    Double_t posX, posY;
    TransformToLocalSector(local_point, posX, posY);

    // calculate in which sector the point is
    sectorId = GetSector(local_point);

    columnId = (Int_t)(posX / fPadSizeX.At(sectorId));
    rowId    = (Int_t)(posY / fPadSizeY.At(sectorId));
  }


  void CbmTrdModule::GetPadPosition(
				    const Int_t padAddress,
				    TVector3& padPos,
				    TVector3& padPosErr) const
  {
    // get address of a pad, return position relative to module center

    Double_t posX = 0;
    Double_t posY = 0;
    Double_t posZ = 0;

    Int_t sectorId = CbmTrdAddress::GetSectorId(padAddress);
    Int_t rowId    = CbmTrdAddress::GetRowId(padAddress);
    Int_t columnId = CbmTrdAddress::GetColumnId(padAddress);

    Double_t padsizex = fPadSizeX.At(sectorId);
    Double_t padsizey = fPadSizeY.At(sectorId);

    // calculate position in sector coordinate system 
    // with the origin in the lower left corner (looking upstream)
    posX = (((Double_t)columnId + 0.5) * padsizex);
    posY = (((Double_t)rowId    + 0.5) * padsizey);

    // calculate position in module coordinate system
    // with the origin in the lower left corner (looking upstream)
    posX += fSectorBeginX.GetAt(sectorId);
    posY += fSectorBeginY.GetAt(sectorId);

    // calculate position in the module coordinate system
    // with origin in the middle of the module
    posX -= fSizeX;
    posY -= fSizeY;
    posZ  = 0; // fSizeZ;

    // check limits
    if ( fabs(posX) > fSizeX )
      LOG(FATAL) << "CbmTrdModule::GetPadPosition posX=" << posX << " is out of bounds!" << FairLogger::endl;
    // check limits
    if ( fabs(posY) > fSizeY )
      LOG(FATAL) << "CbmTrdModule::GetPadPosition posY=" << posY << " is out of bounds!" << FairLogger::endl;

    padPos.SetXYZ(posX, posY, posZ);
    padPosErr.SetXYZ(padsizex/2.,padsizey/2., 0.);
  }


  void CbmTrdModule::GetPosition(
				 Int_t moduleAddress,
				 Int_t sectorId,
				 Int_t columnId,
				 Int_t rowId,
				 TVector3& padPos,
				 TVector3& padSize) const
  {
    // calculate position in global coordinates 
    // from digi information (sectorId, columnId, rowId).
    // Returns two TVector3. One with the position
    // and one with the padsize of the fired pad

    if (fModuleAddress != moduleAddress) {
      LOG(ERROR) << "CbmTrdModule::GetPosition This is wrong!" << FairLogger::endl;
    }

    // check limits
    if ( (sectorId < 0) || (sectorId > GetNofSectors()-1) )
      LOG(FATAL) << "CbmTrdModule::GetPosition sectorId " << sectorId << " of " << GetNofSectors()-1                 << " in moduleAddress " << moduleAddress << " is out of bounds!" << FairLogger::endl;

    // check limits
    if ( (columnId < 0) || (columnId > GetNofColumnsInSector(sectorId)-1) )
      LOG(FATAL) << "CbmTrdModule::GetPosition columnId " << columnId << " of " << GetNofColumnsInSector(sectorId)-1 << " in moduleAddress " << moduleAddress << " is out of bounds!" << FairLogger::endl;

    // check limits
    if ( (rowId < 0) || (rowId > GetNofRowsInSector(sectorId)-1) )
      LOG(FATAL) << "CbmTrdModule::GetPosition rowId "   << rowId     << " of " << GetNofRowsInSector(sectorId)-1    << " in moduleAddress " << moduleAddress << " is out of bounds!" << FairLogger::endl;

    Double_t local_point[3];
    Double_t padsizex = fPadSizeX.At(sectorId);
    Double_t padsizey = fPadSizeY.At(sectorId);

    // calculate position in sector coordinate system 
    // with the origin in the lower left corner (looking upstream)
    local_point[0] = (((Double_t)columnId + 0.5) * padsizex);
    local_point[1] = (((Double_t)rowId    + 0.5) * padsizey);

    // calculate position in module coordinate system
    // with the origin in the lower left corner (looking upstream)
    local_point[0] += fSectorBeginX.GetAt(sectorId);
    local_point[1] += fSectorBeginY.GetAt(sectorId);
    // local_point[i] must be >= 0 at this point 

    // check limits
    if ( (local_point[0]< 0) || (local_point[0] > 2 * fSizeX) )
      LOG(FATAL) << "CbmTrdModule::GetPosition local_point[0]=" << local_point[0] << " in moduleAddress " << moduleAddress << " is out of bounds!" << FairLogger::endl;
    // check limits
    if ( (local_point[1]< 0) || (local_point[1] > 2 * fSizeY) )
      LOG(FATAL) << "CbmTrdModule::GetPosition local_point[1]=" << local_point[1] << " in moduleAddress " << moduleAddress << " is out of bounds!" << FairLogger::endl;

    // calculate position in the module coordinate system
    // with origin in the middle of the module
    local_point[0] -= fSizeX;
    local_point[1] -= fSizeY;
    local_point[2]  = fSizeZ;

    // navigate to the correct module. (fX,fY,fZ)
    gGeoManager->FindNode(fX, fY, fZ);

    // get the local point in local MC coordinates from
    // the geomanager. This coordinate system is rotated,
    // if the chamber is rotated. This is corrected in
    // GetModuleInformation to have the same 
    // local coordinate system in all the chambers
    Double_t global_point[3];  // global_point[3];
    gGeoManager->LocalToMaster(local_point, global_point);

    // calculate the position in the global coordinate system
    // with the origin in target
    Double_t posX = global_point[0];
    Double_t posY = global_point[1];
    Double_t posZ = global_point[2];

    padPos.SetXYZ(posX, posY, posZ);
    padSize.SetXYZ(padsizex,padsizey, 0.);
  }
  ClassImp(CbmTrdModule)
