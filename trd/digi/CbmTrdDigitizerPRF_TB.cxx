#include "CbmTrdDigitizerPRF_TB.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdGeoHandler.h"

#include "CbmMCTrack.h"
#include "CbmMatch.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "CbmMCBuffer.h"
#include "CbmDaqBuffer.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TStopwatch.h"

#include <iomanip>
#include <iostream>

using std::make_pair;
using std::max;
using std::fabs;

const Double_t CbmTrdDigitizerPRF_TB::fDetectorDeadTime = 230.;


CbmTrdDigitizerPRF_TB::CbmTrdDigitizerPRF_TB(CbmTrdRadiator *radiator)
  :FairTask("CbmTrdDigitizerPRF_TB"),
   fDebug(false),
   fTrianglePads(false),
   fTime(-1.),
   fnRow(-1),
   fnCol(-1),
   fLayerId(-1),
   fModuleId(-1),
   fMCPointId(-1),
   fnDigi(0),
   fnPoint(0),
   fPoints(NULL),
   fDigis(NULL),
   fDigiMatches(NULL),
   fMCTracks(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fRadiator(radiator),
   fDigiMap(),
   fTimeAddressMap(),
   fAddressTimeMap()
{
}

CbmTrdDigitizerPRF_TB::~CbmTrdDigitizerPRF_TB()
{
   fDigis->Delete();
   delete fDigis;
   fDigiMatches->Delete();
   delete fDigiMatches;
}

void CbmTrdDigitizerPRF_TB::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdDigitizerPRF_TB::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  if (ioman == NULL) LOG(FATAL) << "CbmTrdDigitizerPRF_TB::Init: No FairRootManager" << FairLogger::endl;

  // Check for presence of MCBuffer and DaqBuffer
  if ( ! ( CbmMCBuffer::Instance() && CbmDaqBuffer::Instance() ) ) {
    gLogger->Fatal(MESSAGE_ORIGIN, "No MCBuffer or DaqBuffer present!");
    return kFATAL;
  }
  /*
    fPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
    if (fPoints == NULL) LOG(FATAL) << "CbmTrdDigitizerPRF_TB::Init: No TrdPoint array" << FairLogger::endl;
  */
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if (fMCTracks == NULL) LOG(FATAL) << "CbmTrdDigitizerPRF_TB::Init: No MCTrack array" << FairLogger::endl;
  
  fDigis = new TClonesArray("CbmTrdDigi", 100);
  ioman->Register("TrdDigi", "TRD Digis", fDigis, kTRUE);

  fDigiMatches = new TClonesArray("CbmMatch", 100);
  ioman->Register("TrdDigiMatch", "TRD Digis", fDigiMatches, kTRUE);

  fRadiator->Init();

  return kSUCCESS;
}

void CbmTrdDigitizerPRF_TB::SetTriangularPads(Bool_t triangles)
{
	fTrianglePads = triangles;
}

void CbmTrdDigitizerPRF_TB::Exec(Option_t * option)
{
  fDigis->Delete();
  fDigiMatches->Delete();

  fDebug = false;
  TStopwatch timer;
  timer.Start();

  Int_t nofLatticeHits = 0;
  Int_t nofElectrons = 0;
  Int_t nofBackwardTracks = 0;
  Int_t nofPoints = 0;
  //const CbmTrdPoint* point = dynamic_cast< const CbmTrdPoint*> (CbmMCBuffer::Instance()->GetNextPoint(kTRD));
  CbmTrdPoint* point =  (CbmTrdPoint*) (CbmMCBuffer::Instance()->GetNextPoint(kTRD));
  while ( point ) {
    nofPoints++;
    fnPoint++;
    if (fnPoint == 5) break;
    if(NULL == point) continue;


    //const CbmMCTrack* track = dynamic_cast< const CbmMCTrack*>(fMCTracks->At(point->GetTrackID()));
    CbmMCTrack* track = (CbmMCTrack*)(fMCTracks->At(point->GetTrackID()));
    if(NULL == track) continue;

    Double_t dz = point->GetZOut() - point->GetZIn();
    if (dz < 0) {
      LOG(DEBUG2) << "CbmTrdDigitizerPRF_TB::Exec: MC-track points towards target!" << FairLogger::endl;
      nofBackwardTracks++;
    }

    TVector3 mom;
    point->Momentum(mom);

    Double_t ELoss = 0.0;
    Double_t ELossTR = 0.0;
    Double_t ELossdEdX = 0.0;
    if (TMath::Abs(track->GetPdgCode()) == 11){
      nofElectrons++;
      if (fRadiator->LatticeHit(point)){  // electron has passed lattice grid (or frame material) befor reaching the gas volume -> TR-photons have been absorbed by the lattice grid
	nofLatticeHits++;
	ELossTR = 0.0;
      } else if (dz < 0){ //electron has not passed the radiator
	ELossTR = 0.0;
      } else {
	ELossTR = fRadiator->GetTR(mom);
      }
    }
    ELossdEdX = point->GetEnergyLoss();
    ELoss = ELossTR + ELossdEdX;

    fTime = point->GetTime();

    // Find node corresponding to the point in the center between entrance and exit MC-point coordinates
    Double_t meanX = (point->GetXOut() + point->GetXIn()) / 2.;
    Double_t meanY = (point->GetYOut() + point->GetYIn()) / 2.;
    Double_t meanZ = (point->GetZOut() + point->GetZIn()) / 2.;
    gGeoManager->FindNode(meanX, meanY, meanZ);

    if (!TString(gGeoManager->GetPath()).Contains("gas")){
      LOG(ERROR) << "CbmTrdDigitizerPRF_TB::Exec: MC-track not in TRD! Node:" << TString(gGeoManager->GetPath()).Data() << " gGeoManager->MasterToLocal() failed!" << FairLogger::endl;
      point = /*dynamic_cast<const*/ (CbmTrdPoint*)/*>*/(CbmMCBuffer::Instance()->GetNextPoint(kTRD));
      continue;
    }

    fLayerId = CbmTrdAddress::GetLayerId(point->GetDetectorID());
    fModuleId = CbmTrdAddress::GetModuleId(point->GetDetectorID());

    fModuleInfo = fDigiPar->GetModule(point->GetDetectorID());
    fnCol = fModuleInfo->GetNofColumns();
    fnRow = fModuleInfo->GetNofRows();

    SplitTrackPath(point, ELoss);
    point = /*dynamic_cast<const*/ (CbmTrdPoint*)/*>*/(CbmMCBuffer::Instance()->GetNextPoint(kTRD));
  }
  // Fill data from internally used stl map into output TClonesArray
   Int_t iDigi = fDigis->GetEntries();
  std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator it;
  for (it = fDigiMap.begin() ; it != fDigiMap.end(); it++) {
    printf("          %8i digis    %8i digis in data map\n",fnDigi,(Int_t)fDigiMap.size());
     printf("                          ------> Push to DAQ\n");
    //new ((*fDigis)[iDigi]) CbmTrdDigi(*(it->second.first));
    //new ((*fDigiMatches)[iDigi]) CbmMatch(*(it->second.second));
    CbmDaqBuffer::Instance()->InsertData(it->second.first);
    delete it->second.first;
    delete it->second.second;
    iDigi++;
    fnDigi++;
  }
  fDigiMap.clear();
   
  LOG(INFO) << "CbmTrdDigitizerPRF_TB::Exec nofPoints=" << nofPoints << " nofDigis=" << fDigis->GetEntriesFast()
    //<< " digis/points=" << fDigis->GetEntriesFast() / nofPoints
            << " nofBackwardTracks=" << nofBackwardTracks << " nofLatticeHits=" << nofLatticeHits
            << " nofElectrons=" << nofElectrons << " latticeHits/electrons="
            << (Double_t) nofLatticeHits / (Double_t) nofElectrons << FairLogger::endl;
  timer.Stop();
  LOG(INFO) << "CbmTrdDigitizerPRF_TB::Exec real time=" << timer.RealTime()
            << " CPU time=" << timer.CpuTime() << FairLogger::endl;
}

Double_t CbmTrdDigitizerPRF_TB::CalcPRF(Double_t x, Double_t W, Double_t h)
{
  Float_t K3 = 0.525; 
  Double_t SqrtK3 = sqrt(K3);

  return fabs(
	      -1. / (2. * atan(SqrtK3)) * (
					   atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W + 2.* x) / (8.* h) )) + 
					   atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W - 2.* x) / (8.* h) ))
					   )
	      );
}

void CbmTrdDigitizerPRF_TB::ScanPadPlaneTriangle(const Double_t* local_point, Double_t clusterELoss)
{
  /*
  // In order to get this algorithm as perfromant as possible the pad plane in the digipar containes twice pad rows without further information on the pad geomatry itself.
  // The triangle is associated to a row by the position of the 90 deg angle.-> In even rows the smallest angle ist pointing upwards and in odd rows it is pointing downward per definition.
  // In a first step the full rectangular pad (2 rows) is calculated. This has the benifit that we need only to numericaly integrate one triangular pad which is very time consuming. 
  // The second triangle is given by the difference of the full rectangle and the first triangle.
  */
  Int_t sectorId(-1), columnId(-1), rowId(-1);//, preSecRows(0);
  fModuleInfo->GetPadInfo( local_point, sectorId, columnId, rowId);
  if (sectorId < 0 && columnId < 0 && rowId < 0) {
    printf("    x:%7.3f  y:%7.3f z:%7.3f\n",local_point[0],local_point[1],local_point[2]);
    return;
  } else {
    for (Int_t i = 0; i < sectorId; i++)
      rowId += fModuleInfo->GetNofRowsInSector(i); // local -> global row

    Double_t displacement_x(0), displacement_y(0);//cm
    Double_t h = fModuleInfo->GetAnodeWireToPadPlaneDistance();
    Double_t W(fModuleInfo->GetPadSizeX(sectorId)), H(fModuleInfo->GetPadSizeY(sectorId));
    fModuleInfo->TransformToLocalPad(local_point, displacement_x, displacement_y);  
     
    //       displacement_x = 0.1; //cm
    //       displacement_y = 0.2;   //cm
    //       H = 6;
    //       W = 4;
    

    const Int_t maxCol(5/W+0.5), maxRow(6);//5/H+3);// 7 and 3 in orig. minimum 5 times 5 cm area has to be scanned
 
    //printf("%i x %i\n",maxCol,maxRow);
    //Estimate starting column and row and limits due to chamber dimensions
    Int_t startCol(columnId-maxCol/2), stopCol(columnId+maxCol/2), startRow(rowId-maxRow/2), stopRow(rowId+maxRow/2+1), startSec(0);
    if (startRow % 2 != 0){ // It does not make sence to start scanning in odd rows for triangluar pad geometry since the triangles are later combined to rectangles and parallelograms
      startRow -= 1;
      stopRow  -= 1;
    }
    Double_t sum = 0;
    Int_t secRow(-1), targCol(-1), targRow(-1), targSec(-1), addressEven(-1), addressOdd(-1);
    if (fDebug) printf("\nhit: (%7.3f,%7.3f)\n",displacement_x,displacement_y);
    for (Int_t iCol = startCol; iCol <= stopCol; iCol++){
      Double_t chargeFractionRectangle(0.0), chargeFractionTriangle(0.0);
      for (Int_t iRow = startRow; iRow <= stopRow; iRow++){
	if (fDebug) printf("(%i): ",iRow);
	if (((iCol >= 0) && (iCol <= fnCol-1)) && ((iRow >= 0) && (iRow <= fnRow-1))){// real address	 
	  targSec = fModuleInfo->GetSector(iRow, secRow);
	  //printf("secId digi1 %i\n",targSec);
	  if (iRow % 2 == 0)
	    addressEven = CbmTrdAddress::GetAddress(fLayerId, fModuleId, targSec, secRow, iCol); 
	  else
	    addressOdd = CbmTrdAddress::GetAddress(fLayerId, fModuleId, targSec, secRow, iCol); 
	  if (fDebug) {
	    if (secRow > 11 && fModuleId == 5){
	      //printf("address %i layer %i and modId %i modAddress %i  Sec%i Row:%i Col%i\n",address,fLayerId,fModuleId,fModuleAddress,targSec,secRow,iCol);
	    }
	  }
	} else {
          targRow = iRow;
          targCol = iCol;
          if (iCol < 0)
	    targCol = 0;
	  else if (iCol > fnCol-1)
	    targCol = fnCol-1;
	  if (iRow < 0)
	    targRow = 0;
	  else if (iRow > fnRow-1)	    
	    targRow = fnRow-1;

	  targSec = fModuleInfo->GetSector(targRow, secRow);
	  //printf("secId digi2 %i\n",targSec);
	  if (iRow % 2 == 0)
	    addressEven = CbmTrdAddress::GetAddress(fLayerId, fModuleId, targSec, secRow, targCol);
	  else
	    addressOdd = CbmTrdAddress::GetAddress(fLayerId, fModuleId, targSec, secRow, targCol);
	  if (secRow > 11 && fModuleId == 5){
	    //printf("address %i layer %i and modId %i modAddress %i  Sec%i Row:%i Col%i\n",address,fLayerId,fModuleId,fModuleAddress,targSec,secRow,targCol);
	  }
	}
	/*
	  if (rowId == iRow && columnId == iCol)
	  printf("c:(%2i) r:(%2i)   W:%E   H:%E\n",iCol, iRow, W, H);
	  else
	  printf("c: %2i  r: %2i    W:%E   H:%E\n",iCol, iRow, W, H);
	*/
	if (iRow % 2 == 0) { // to calculate the full rectangle one has to sum over one even and the following odd row
	  //sum = 0;
	  sum += chargeFractionRectangle;
	  chargeFractionRectangle = 0;	
          chargeFractionTriangle  = TriangleIntegration(true,  displacement_x - (iCol - columnId) * W, W, displacement_y - (iRow - rowId) * H, H, h);
	} else {
	  //chargeFractionTriangle += TriangleIntegration(false, displacement_x - (iCol - columnId) * W, W, displacement_y - (iRow - rowId) * H, H, h);
	}

	/*
	  if (rowId == iRow && columnId == iCol) {// if pad in center of n x m array
	  //chargeFractionRectangle += CalcPRF(displacement_x, W, h) * CalcPRF(displacement_y, H, h);
	  chargeFractionRectangle += CalcPRF(displacement_x, W, h) * CalcPRF(displacement_y, H, h);	 
	  } else {
	  chargeFractionRectangle += CalcPRF((iCol - columnId) * W - displacement_x, W, h) * CalcPRF((iRow - rowId) * H - displacement_y, H, h);
	  //chargeFractionRectangle = CalcPRF((iCol - columnId) * W - displacement_x, W, h) * CalcPRF((iRow - rowId) * H - displacement_y, H, h);	  
	  }
	*/
	chargeFractionRectangle += CalcPRF((iCol - columnId) * W - displacement_x, W, h) * CalcPRF((iRow - rowId) * H - displacement_y, H, h);



	//if (iCol >= stopCol)
	if (fDebug) {
	  if (iRow % 2 == 0){
	    if (rowId == iRow && columnId == iCol){
	      printf("                even: %E        R(x:%E / y:%E) %E\n", 
		     chargeFractionTriangle, 
		     displacement_x,
		     displacement_y,
		     CalcPRF(displacement_x, W, h) * CalcPRF(displacement_y, H, h));
	      printf("                                          R:%E\n                                          T:%E\n", 
		     chargeFractionRectangle, chargeFractionTriangle);
	    } else {
	      printf("                even: %E        R(x:%E / y:%E) %E\n", 
		     chargeFractionTriangle, 
		     (iCol - columnId) * W - displacement_x,
		     (iRow - rowId) * H - displacement_y,
		     CalcPRF((iCol - columnId) * W - displacement_x, W, h) * CalcPRF((iRow - rowId) * H - displacement_y, H, h));
	      printf("                                          R:%E\n                                          T:%E\n", 
		     chargeFractionRectangle, chargeFractionTriangle);
	    }
	  } else {
	    if (rowId == iRow && columnId == iCol){
	      printf("                 odd: %E        R(x:%E / y:%E) %E\n", 
		     chargeFractionRectangle - chargeFractionTriangle, 
		     displacement_x,
		     displacement_y, 
		     CalcPRF(displacement_x, W, h) * CalcPRF(displacement_y, H, h));
	      printf("                                          R:%E\n                                          T:%E\n", 
		     chargeFractionRectangle, chargeFractionTriangle);
	    } else {
	      printf("                 odd: %E        R(x:%E / y:%E) %E\n", 
		     chargeFractionRectangle - chargeFractionTriangle, 
		     (iCol - columnId) * W - displacement_x,
		     (iRow - rowId) * H - displacement_y,
		     CalcPRF((iCol - columnId) * W - displacement_x, W, h) * CalcPRF((iRow - rowId) * H - displacement_y, H, h));
	      printf("                                          R:%E\n                                          T:%E\n", 
		     chargeFractionRectangle, 
		     chargeFractionTriangle);
	    }
	  }
	}
	if (iRow % 2 != 0){
	  if ((chargeFractionRectangle - chargeFractionTriangle) < 0.0)
	    printf("-------------------------------\nc:%i r:%i even: %E  odd %E        R: %E   T:%E\n-------------------------------\n",iCol, iRow, chargeFractionTriangle, chargeFractionRectangle - chargeFractionTriangle, chargeFractionRectangle, chargeFractionTriangle);
	  AddDigi(fMCPointId, addressEven, Double_t(chargeFractionTriangle * clusterELoss), fTime);
	  AddDigi(fMCPointId, addressOdd, Double_t((chargeFractionRectangle - chargeFractionTriangle) * clusterELoss), fTime);
	}     
      } // for iRow      
    } // for iCol
    if (sum < 0.99 || sum > 1.01){
    LOG(WARNING) << "CbmTrdDigitizerPRF_TB::ScanPadPlane: Summed charge: " << std::setprecision(5) << sum << "  hit:(" << columnId << ", " << rowId <<")   max:(" << fnCol-1 << ", " << fnRow-1 << ")" << FairLogger::endl;
    }
  }  
}

  // --------------------------------------------------------------------
Double_t CbmTrdDigitizerPRF_TB::TriangleIteration(Bool_t even, Int_t step, Double_t dis_x, Double_t W, Double_t dis_y, Double_t H, Double_t h){
  Double_t chargeFraction = 0.0;//W * H;
  
  //  chargeFractionTriangle  = TriangleIntegration(true,  (iCol - columnId) * W - displacement_x, W, (iRow - rowId) * H - displacement_y, H, h);

  //  chargeFraction = CalcPRF(dis_x, W, h) * CalcPRF(dis_y, H, h);
  chargeFraction = CalcPRF(dis_x, W, h) * CalcPRF(dis_y, H, h);
  //  printf("                                          %i H(x:%E / y:%E) (xp:%E / yp:%E)  charge:%E  Wp:1/%g, Hp:1/%g  W:%E H:%E\n", step, dis_x, dis_y, dis_x/W, dis_y/H, chargeFraction, 0.675/W, 1.50/H, W, H);
  //printf("                                          %i H(x:%E / y:%E) (xp:%E / yp:%E)  charge:%E  Wp:1/%g, Hp:1/%g\n", step, dis_x, dis_y, dis_x/W, dis_y/H, chargeFraction, 0.675/W, 1.50/H);
  
  if (step < 8) { // maximum 15 iterations -> 99.99847% accuracy
    // new size of the two smaller rectangles  
    W *= 0.5;
    H *= 0.5;
    
    if (even) {   // even = bottom triangle
      // left rectangle with a quarter of the area
      chargeFraction += TriangleIteration(even, step+1 , dis_x + 1.5 * W, W, dis_y + 0.5 * H, H, h); // bottom left
      // upper rectangle with a quarter of the area
      chargeFraction += TriangleIteration(even, step+1 , dis_x - 0.5 * W, W, dis_y - 1.5 * H, H, h); // top right
    } else {   // odd = top triangle
      // right rectangle with a quarter of the area
      chargeFraction += TriangleIteration(even, step+1 , dis_x - 1.5 * W, W, dis_y - 0.5 * H, H, h); // top right
      // lower rectangle with a quarter of the area
      chargeFraction += TriangleIteration(even, step+1 , dis_x + 0.5 * W, W, dis_y + 1.5 * H, H, h); // bottom left
    }

//// DE
//    if (even) {   // even = bottom triangle
//      // left rectangle with a quarter of the area
//      chargeFraction += TriangleIteration(even, step+1 , dis_x - 1.5 * W, W, dis_y - 0.5 * H, H, h); // bottom left
//      // upper rectangle with a quarter of the area
//      chargeFraction += TriangleIteration(even, step+1 , dis_x + 0.5 * W, W, dis_y + 1.5 * H, H, h); // top right
//    } else {  // odd = top triangle
//      // right rectangle with a quarter of the area
//      chargeFraction += TriangleIteration(even, step+1 , dis_x + 1.5 * W, W, dis_y + 0.5 * H, H, h); // top right
//      // lower rectangle with a quarter of the area
//      chargeFraction += TriangleIteration(even, step+1 , dis_x - 0.5 * W, W, dis_y - 1.5 * H, H, h); // bottom left
//    }
    
  } 
  
  return chargeFraction;
}
  // --------------------------------------------------------------------
Double_t CbmTrdDigitizerPRF_TB::TriangleIntegration(Bool_t even, Double_t dis_x, Double_t W, Double_t dis_y, Double_t H, Double_t h)
{
  /*
  // The pad height has to be doubled since the digipar provides only pad height for rectangular pads. Therefore the dis has also to be corrected for triangular pad geometry.
  // The Triangular pad will be numerically approximated by a larger number of small subrectangels. 
  */
  Double_t chargeFraction = 0.0;
  //First itteration level; biggest rectangle lower left corner (W/2, H)  
  W *= 0.5;
  H *= 1.0;

  if (even){
    chargeFraction = TriangleIteration(even, 0 , dis_x - 0.5 * W, W, dis_y, H, h);
    //    chargeFraction = TriangleIteration(even, 0 , dis_x + 0.5 * W, W, dis_y, H, h); // DE
  } else {
    chargeFraction = TriangleIteration(even, 0 , dis_x + 0.5 * W, W, dis_y, H, h);
    //    chargeFraction = TriangleIteration(even, 0 , dis_x - 0.5 * W, W, dis_y, H, h); // DE
  }
  //chargeFraction =  TriangleIteration(even, 0, dis_x - 0.25 * W, 0.5 * W, dis_y, H, h) + TriangleIteration(even, 0, dis_x + 0.25 * W, 0.5 * W, dis_y, H, h); // debug
  //printf("                                          T(x:%E / y:%E) %E (x,x)\n",dis_x,dis_y,chargeFraction); 
  return chargeFraction;
}

void CbmTrdDigitizerPRF_TB::ScanPadPlane(const Double_t* local_point, Double_t clusterELoss)
{
   Int_t sectorId(-1), columnId(-1), rowId(-1);
   fModuleInfo->GetPadInfo( local_point, sectorId, columnId, rowId);
   if (sectorId < 0 && columnId < 0 && rowId < 0) {
      printf("    x:%7.3f  y:%7.3f z:%7.3f\n",local_point[0],local_point[1],local_point[2]);
      return;
   } else {
      for (Int_t i = 0; i < sectorId; i++) {
         rowId += fModuleInfo->GetNofRowsInSector(i); // local -> global row
      }

      Double_t displacement_x(0), displacement_y(0);//mm
      Double_t h = fModuleInfo->GetAnodeWireToPadPlaneDistance();
      Double_t W(fModuleInfo->GetPadSizeX(sectorId)), H(fModuleInfo->GetPadSizeY(sectorId));
      fModuleInfo->TransformToLocalPad(local_point, displacement_x, displacement_y);
      
      const Int_t maxCol(5/W+0.5), maxRow(5/H+3);// 7 and 3 in orig. minimum 5 times 5 cm area has to be scanned
      //printf("%i x %i\n",maxCol,maxRow);
      //Estimate starting column and row and limits due to chamber dimensions
      Int_t startCol(columnId-maxCol/2), stopCol(columnId+maxCol/2), startRow(rowId-maxRow/2), stopRow(rowId+maxRow/2), startSec(0);
      Double_t sum = 0;
      Int_t secRow(-1), targCol(-1), targRow(-1), targSec(-1), address(-1);
      if (fDebug) {
         printf("\nhit: (%7.3f,%7.3f)\n",displacement_x,displacement_y);
      }
      for (Int_t iRow = startRow; iRow <= rowId+maxRow/2; iRow++) {
         if (fDebug) {
            printf("(%i): ",iRow);
         }
         for (Int_t iCol = startCol; iCol <= columnId+maxCol/2; iCol++) {
            if (((iCol >= 0) && (iCol <= fnCol-1)) && ((iRow >= 0) && (iRow <= fnRow-1))){// real adress
               targSec = fModuleInfo->GetSector(iRow, secRow);
               //printf("secId digi1 %i\n",targSec);
               address = CbmTrdAddress::GetAddress(fLayerId, fModuleId, targSec, secRow, iCol);
               if (secRow > 11 && fModuleId == 5){
                  //printf("address %i layer %i and modId %i modAddress %i  Sec%i Row:%i Col%i\n",address,fLayerId,fModuleId,fModuleAddress,targSec,secRow,iCol);
               }
            } else {
               targRow = iRow;
               targCol = iCol;
               if (iCol < 0) {
                  targCol = 0;
               } else if (iCol > fnCol-1) {
                  targCol = fnCol-1;
               }
               if (iRow < 0) {
                  targRow = 0;
               } else if (iRow > fnRow-1) {
                  targRow = fnRow-1;
               }

               targSec = fModuleInfo->GetSector(targRow, secRow);
               //printf("secId digi2 %i\n",targSec);
               address = CbmTrdAddress::GetAddress(fLayerId, fModuleId, targSec, secRow, targCol);
               if (secRow > 11 && fModuleId == 5){
                  //printf("address %i layer %i and modId %i modAddress %i  Sec%i Row:%i Col%i\n",address,fLayerId,fModuleId,fModuleAddress,targSec,secRow,targCol);
               }
            }
            Double_t chargeFraction = 0;
            if (rowId == iRow && columnId == iCol) { // if pad in center of 7x3 arrayxs
               chargeFraction = CalcPRF(displacement_x, W, h) * CalcPRF(displacement_y, H, h);
            } else {
               chargeFraction = CalcPRF((iCol - columnId) * W - displacement_x, W, h) * CalcPRF((iRow - rowId) * H - displacement_y, H, h);
            }

            sum += chargeFraction;
            //if (iCol >= stopCol)

            AddDigi(fMCPointId, address, Double_t(chargeFraction * clusterELoss), fTime);

            if (fDebug) {
               if (rowId == iRow && columnId == iCol)
                  printf("(%3i:%5.3E) ",iCol,chargeFraction);
               else
                  printf(" %3i:%5.3E  ",iCol,chargeFraction);
               if (iCol == fnCol-1)
                  std::cout << "|";
            }

         } // for iCol

         if (fDebug) {
            if (iRow == fnRow-1)
               std::cout << std::endl << "-------------------------------------" << std::endl;
            else
               std::cout << std::endl;
         }
      
      } // for iRow
      if (sum < 0.99 || sum > 1.01){
         LOG(WARNING) << "CbmTrdDigitizerPRF_TB::ScanPadPlane: Summed charge: " << std::setprecision(5) << sum << "  hit:(" << columnId << ", " << rowId <<")   max:(" << fnCol-1 << ", " << fnRow-1 << ")" << FairLogger::endl;
      }
   }
}

void CbmTrdDigitizerPRF_TB::SplitTrackPath(const CbmTrdPoint* point, Double_t ELoss)
{
   const Double_t nClusterPerCm = 1.0;
   Double_t point_in[3] = {
      point->GetXIn(),
      point->GetYIn(),
      point->GetZIn()
   };
   Double_t point_out[3] = {
      point->GetXOut(),
      point->GetYOut(),
      point->GetZOut()
   };
   Double_t local_point_out[3];// exit point coordinates in local module cs
   Double_t local_point_in[3]; // entrace point coordinates in local module cs
   gGeoManager->MasterToLocal(point_in,  local_point_in);
   gGeoManager->MasterToLocal(point_out, local_point_out);

   Double_t cluster_pos[3];   // cluster position in local module coordinate system
   Double_t cluster_delta[3]; // vector pointing in MC-track direction with length of one path slice within chamber volume to creat n cluster

   Double_t trackLength = 0;

   for (Int_t i = 0; i < 3; i++) {
      cluster_delta[i] = (local_point_out[i] - local_point_in[i]);
      trackLength += cluster_delta[i] * cluster_delta[i];
   }
   trackLength = TMath::Sqrt(trackLength);
   //if (fDebug)
   //const Int_t nCluster = 1;//trackLength / nClusterPerCm + 0.9;// Track length threshold of minimum 0.1cm track length in gas volume
   const Int_t nCluster = trackLength / nClusterPerCm + 0.9;// Track length threshold of minimum 0.1cm track length in gas volume
   if (nCluster < 1){
      LOG(WARNING) << "CbmTrdDigitizerPRF_TB::SplitTrackPath: nCluster: "<<nCluster<<"   track length: "<<std::setprecision(5)<<trackLength<<"cm  nCluster/cm: "<<std::setprecision(2)<<nClusterPerCm<<"  ELoss: "<<std::setprecision(5)<<ELoss*1e-6<<"keV " << FairLogger::endl;
      return;
   }
   for (Int_t i = 0; i < 3; i++){
      cluster_delta[i] /= Double_t(nCluster);
   }
   Double_t clusterELoss = ELoss / Double_t(nCluster);

   for (Int_t iCluster = 0; iCluster < nCluster; iCluster++){
      for (Int_t i = 0; i < 3; i++){
         cluster_pos[i] = local_point_in[i] + (0.5 + iCluster) * cluster_delta[i];// move start position 0.5 step widths into track direction
      }

      if ( fModuleInfo->GetSizeX() < fabs(cluster_pos[0]) || fModuleInfo->GetSizeY() < fabs(cluster_pos[1])){
         printf("->    nC %i/%i x: %7.3f y: %7.3f \n",iCluster,nCluster-1,cluster_pos[0],cluster_pos[1]);
         for (Int_t i = 0; i < 3; i++)
            printf("  (%i) | in: %7.3f + delta: %7.3f * cluster: %i/%i = cluster_pos: %7.3f out: %7.3f g_in:%f g_out:%f\n",
                  i,local_point_in[i],cluster_delta[i],iCluster,(Int_t)nCluster,cluster_pos[i],local_point_out[i],point_in[i],point_out[i]);
      }

      fModuleInfo->ProjectPositionToNextAnodeWire(cluster_pos);
      if (!fTrianglePads) 
         ScanPadPlane(cluster_pos, clusterELoss);
      else
         ScanPadPlaneTriangle(cluster_pos, clusterELoss);
   }
}


void CbmTrdDigitizerPRF_TB::AddDigi(Int_t pointId, Int_t address, Double_t charge, Double_t time)
{
  std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator data = fDigiMap.find(address);
  if (data == fDigiMap.end()){
    fAddressTimeMap[address] = time;
    fTimeAddressMap[Int_t(time)].push_back(address);
  }
  std::map<Int_t, Double_t > ::iterator previousAddress = fAddressTimeMap.begin();
  
  printf("____________________________________________________________________\n  Time:%8.2fns point:%8i  digi:%8i  address:%10i  time:%10i\n  %8i digis in time map\n  %8i digis in data map\n",time,fnPoint,fnDigi,address,(Int_t)fAddressTimeMap.size(),(Int_t)fTimeAddressMap.size(),(Int_t)fDigiMap.size());
  /*
    while (previousAddress != fAddressTimeMap.end()){ // look in address time stamp map
    data = fDigiMap.find((*previousAddress).first);  // find address which can be pushed to DAQ_Buffer
    //printf("          %8.2fns (delta:%8.2fns)\n",(*previousAddress).second,time - (*previousAddress).second);
    if (time - (*previousAddress).second > fDetectorDeadTime){  // readout to buffer -> detector dead time is over   
    if (data != fDigiMap.end()) { // if address is found push to buffer
    printf("          %8.2fns (delta:%8.2fns)\n",(*previousAddress).second,time - (*previousAddress).second);
    printf("                          ------> Push to DAQ\n");
    Int_t iDigi = fDigis->GetEntries();
    CbmDaqBuffer::Instance()->InsertData(data->second.first);
    delete data->second.first;
    delete data->second.second;
    fDigiMap.erase(data);   // clear map entries
    fAddressTimeMap.erase(previousAddress); // previousAddress++ is coverd by map::erase(previousAddress)
    } else { // if no address is found create new one and go to next element in address time stamp map
    printf("                          Did not find data on address %10i\n",(*previousAddress).first);
    //printf("Should not happen!!! There should be no element in the time stamp map which is not in the data map!!\n");
    previousAddress++;
    }       
    } else { // Add information to existing element in digi data map
    if ((*previousAddress).first == address) {
    printf("          %8.2fns (delta:%8.2fns)\n",(*previousAddress).second,time - (*previousAddress).second);
    printf("                          ------> Add Info to existing digi %8i\n",fnDigi);
    fAddressTimeMap[address] = time;
    //const FairMCPoint* point = static_cast<const FairMCPoint*>(fPoints->At(pointId)); 
    //FairMCPoint* point = (FairMCPoint*) (fPoints->At(pointId)); 
    if (data == fDigiMap.end()) { // Pixel not yet in map -> Add new pixel
    fnDigi++;
    printf("          %8.2fns (delta:%8.2fns)\n",(*previousAddress).second,time - (*previousAddress).second);
    printf("                          ------> Creat new digi %8i\n",fnDigi);
    CbmMatch* digiMatch = new CbmMatch();
    //digiMatch->AddLink(CbmLink(charge, pointId));
    fDigiMap[address] = make_pair(new CbmTrdDigi(address, charge, time), digiMatch);
    previousAddress++;
    } else { // Pixel already in map -> Add charge
    printf("          %8.2fns (delta:%8.2fns)\n",(*previousAddress).second,time - (*previousAddress).second);
    printf("                          ------> Add Info to existing digi %8i\n",fnDigi);
    data->second.first->AddCharge(charge);
    data->second.first->SetTime(max(time, data->second.first->GetTime()));
    data->second.second->AddLink(CbmLink(charge, pointId));
    previousAddress++;
    }
    } else
    previousAddress++;
    } 
    }
  */
  std::map<Int_t, std::vector<Int_t> > ::iterator previous = fTimeAddressMap.begin(); 
  //printf("Time: %E\n",time);
  //for (previous = fTimeAddressMap.begin(); previous != fTimeAddressMap.end(); previous++){ // look in time stamp map
  while (previous != fTimeAddressMap.end()){ 
    if ( Int_t(time) - (*previous).first > fDetectorDeadTime){  // readout to buffer -> detector dead time is over
      printf("          time:%10i\n  %8i digis in time map  %8i digis in data map\n",(Int_t)fAddressTimeMap.size(),(Int_t)fTimeAddressMap.size(),(Int_t)fDigiMap.size());
      printf("          %4ins (delta:%8.2fns)\n",(*previous).first,time - (*previous).first);
      printf("                          ------> Push to DAQ\n");
      for (Int_t i = 0; i < (*previous).second.size(); i++) { // loop over all addresses with the same time stamp
	//std::vector<Int_t> ::iterator AddressIt = (*previous).second.begin();
	//while ((*previous).second.size() > 0){
	data = fDigiMap.find((*previous).second[/*(*AddressIt)*/i]);  
	if (data != fDigiMap.end()) { 
	  Int_t iDigi = fDigis->GetEntries();
	  CbmDaqBuffer::Instance()->InsertData(data->second.first);
	  delete data->second.first;
	  delete data->second.second;
	  fDigiMap.erase(data); // erase data field from data map
	  //(*previous).second.erase(AddressIt);
	} else {
	}
      }
      (*previous).second.clear(); // Clear Address vectors for this time stamp
      fTimeAddressMap.erase(previous); // clear time stamp from map
      previous++;
      //previous = fTimeAddressMap.begin(); 
    } else {
      //const FairMCPoint* point = static_cast<const FairMCPoint*>(fPoints->At(pointId)); 
      data = fDigiMap.find(address);
      if (data == fDigiMap.end()) { // Pixel not yet in map -> Add new pixel
	fnDigi++;
	printf("          %4ins (delta:%8.2fns)\n",(*previous).first,time - (*previous).first);
	printf("                          ------> Creat new digi %8i\n",fnDigi);
	CbmMatch* digiMatch = new CbmMatch();
	//digiMatch->AddLink(CbmLink(charge, pointId));
	fDigiMap[address] = make_pair(new CbmTrdDigi(address, charge, time), digiMatch);
	//fTimeAddressMap[time].push_back(address);
      } else { // Pixel already in map -> Add charge
	printf("          %4ins (delta:%8.2fns)\n",(*previous).first,time - (*previous).first);
	printf("                          ------> Add Info to existing digi %8i\n",fnDigi);
	data->second.first->AddCharge(charge);
	data->second.first->SetTime(max(time, data->second.first->GetTime()));
	data->second.second->AddLink(CbmLink(charge, pointId));

	for (Int_t i = 0; i < (*previous).second.size(); i++) { 
	  if (address == (*previous).second[i]){
	    fTimeAddressMap[time].push_back((*previous).second[i]);
	    (*previous).second.erase((*previous).second.begin()+i);
	    break;
	  }
	}
      } 
      previous++;
    }
  }
 
  //fTimeAddressMap[time].push_back(address);
    
}

void CbmTrdDigitizerPRF_TB::Finish()
{
  Int_t iDigi = fDigis->GetEntries();
  std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator it;
  for (it = fDigiMap.begin() ; it != fDigiMap.end(); it++) {
      printf("          %8i digis in data map\n",(Int_t)fDigiMap.size());
     printf("                          ------> Push to DAQ\n");
    //new ((*fDigis)[iDigi]) CbmTrdDigi(*(it->second.first));
    //new ((*fDigiMatches)[iDigi]) CbmMatch(*(it->second.second));
    CbmDaqBuffer::Instance()->InsertData(it->second.first);
    delete it->second.first;
    delete it->second.second;
    iDigi++;
  }
  fDigiMap.clear();
  Exec(""); // Digitise the remaining points in the MCBuffer
  //fNEvents -= 1; // Correct for extra call to Exec
}
  ClassImp(CbmTrdDigitizerPRF_TB)
