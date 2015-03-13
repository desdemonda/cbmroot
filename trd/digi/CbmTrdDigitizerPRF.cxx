#include "CbmTrdDigitizerPRF.h"

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

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TStopwatch.h"

#include <iomanip>
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::make_pair;
using std::max;
using std::fabs;


CbmTrdDigitizerPRF::CbmTrdDigitizerPRF(CbmTrdRadiator *radiator)
  :FairTask("CbmTrdDigitizerPRF"),
   fDebug(false),
   fTrianglePads(false),
   fCbmLinkWeightDistance(false),
   fSigma_noise_keV(0.0),
   fNoise(NULL),
   fMinimumChargeTH(1.0e-06),
   fTime(-1.),
   fnClusterConst(0),
   fnScanRowConst(0),
   fnScanColConst(0),
   fnRow(-1),
   fnCol(-1),
   fLayerId(-1),
   fModuleId(-1),
   fMCPointId(-1),
   fPoints(NULL),
   fDigis(NULL),
   fDigiMatches(NULL),
   fMCTracks(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fRadiator(radiator),
   fDigiMap()
{
}

CbmTrdDigitizerPRF::~CbmTrdDigitizerPRF()
{
   fDigis->Delete();
   delete fDigis;
   fDigiMatches->Delete();
   delete fDigiMatches;
}
void CbmTrdDigitizerPRF::SetNCluster(Int_t nCluster)
{
  fnClusterConst = nCluster;
}
void CbmTrdDigitizerPRF::SetCbmLinkWeightDistance(Bool_t dist)
{
  fCbmLinkWeightDistance = dist;
}
void CbmTrdDigitizerPRF::SetPadPlaneScanArea(Int_t column, Int_t row)
{
  if (row%2 == 0) row += 1;
  fnScanRowConst = row;
  if (column%2 == 0) column += 1;
  fnScanColConst = column;
}
void CbmTrdDigitizerPRF::SetParContainers()
{
	fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}
void CbmTrdDigitizerPRF::SetTriggerThreshold(Double_t minCharge){
  fMinimumChargeTH = minCharge;//  To be used for test beam data processing
}
InitStatus CbmTrdDigitizerPRF::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  if (ioman == NULL) LOG(FATAL) << "CbmTrdDigitizerPRF::Init: No FairRootManager" << FairLogger::endl;

  fPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
  if (fPoints == NULL) LOG(FATAL) << "CbmTrdDigitizerPRF::Init: No TrdPoint array" << FairLogger::endl;

  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if (fMCTracks == NULL) LOG(FATAL) << "CbmTrdDigitizerPRF::Init: No MCTrack array" << FairLogger::endl;

  fDigis = new TClonesArray("CbmTrdDigi", 100);
  ioman->Register("TrdDigi", "TRD Digis", fDigis, kTRUE);

  fDigiMatches = new TClonesArray("CbmMatch", 100);
  ioman->Register("TrdDigiMatch", "TRD Digis", fDigiMatches, kTRUE);
  if (fRadiator != NULL)
    fRadiator->Init();

  if (fSigma_noise_keV > 0.0){
    fNoise = new TRandom3();
  }

  return kSUCCESS;
}

void CbmTrdDigitizerPRF::SetTriangularPads(Bool_t triangles)
{
	fTrianglePads = triangles;
}

void CbmTrdDigitizerPRF::SetNoiseLevel(Double_t sigma_keV)
{
	fSigma_noise_keV = sigma_keV;
}

void CbmTrdDigitizerPRF::Exec(Option_t * option)
{
  cout << "================CbmTrdDigitizerPRF===============" << endl;
 LOG(INFO) << "CbmTrdDigitizerPRF::Exec : Triangular Pads: " << (Bool_t)fTrianglePads << FairLogger::endl;
 LOG(INFO) << "CbmTrdDigitizerPRF::Exec : Noise width:     " << fSigma_noise_keV << " keV"<< FairLogger::endl;
  fDigis->Delete();
  fDigiMatches->Delete();

  fDebug = false;
  TStopwatch timer;
  timer.Start();

  Int_t nofLatticeHits = 0;
  Int_t nofElectrons = 0;
  Int_t nofBackwardTracks = 0;
  Int_t nofPointsAboveThreshold = 0;
  Int_t nofPoints = fPoints->GetEntries();
  for (Int_t iPoint = 0; iPoint < nofPoints ; iPoint++) {
    fMCPointId = iPoint;
    CbmTrdPoint* point = static_cast<CbmTrdPoint*>(fPoints->At(iPoint));
    if(NULL == point) continue;
    const CbmMCTrack* track = static_cast<const CbmMCTrack*>(fMCTracks->At(point->GetTrackID()));
    if(NULL == track) continue;

    Double_t dz = point->GetZOut() - point->GetZIn();
    if (dz < 0) {
      LOG(DEBUG2) << "CbmTrdDigitizerPRF::Exec: MC-track points towards target!" << FairLogger::endl;
      nofBackwardTracks++;
    }

    TVector3 mom;
    point->Momentum(mom);

    Double_t ELoss = 0.0;
    Double_t ELossTR = 0.0;
    Double_t ELossdEdX = 0.0;
    if (fRadiator != NULL)
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
    if (ELoss > fMinimumChargeTH) 
      nofPointsAboveThreshold++;
    fTime = point->GetTime();

    // Find node corresponding to the point in the center between entrance and exit MC-point coordinates
    Double_t meanX = (point->GetXOut() + point->GetXIn()) / 2.;
    Double_t meanY = (point->GetYOut() + point->GetYIn()) / 2.;
    Double_t meanZ = (point->GetZOut() + point->GetZIn()) / 2.;
    gGeoManager->FindNode(meanX, meanY, meanZ);

    if (!TString(gGeoManager->GetPath()).Contains("gas")){
      LOG(ERROR) << "CbmTrdDigitizerPRF::Exec: MC-track not in TRD! Node:" << TString(gGeoManager->GetPath()).Data() << " gGeoManager->MasterToLocal() failed!" << FairLogger::endl;
      continue;
    }

    fLayerId = CbmTrdAddress::GetLayerId(point->GetDetectorID());
    fModuleId = CbmTrdAddress::GetModuleId(point->GetDetectorID());

    fModuleInfo = fDigiPar->GetModule(point->GetDetectorID());
    fnCol = fModuleInfo->GetNofColumns();
    fnRow = fModuleInfo->GetNofRows();

    SplitTrackPath(point, ELoss);
  }

  // Fill data from internally used stl map into output TClonesArray
  Int_t iDigi = 0;
  std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator it;
  for (it = fDigiMap.begin() ; it != fDigiMap.end(); it++) {
    new ((*fDigis)[iDigi]) CbmTrdDigi(*(it->second.first));
    new ((*fDigiMatches)[iDigi]) CbmMatch(*(it->second.second));
    delete it->second.first;
    delete it->second.second;
    iDigi++;
  }
  fDigiMap.clear();

  Double_t digisOverPoints = (nofPoints > 0) ? fDigis->GetEntriesFast() / nofPoints : 0;
  Double_t latticeHitsOverElectrons = (nofElectrons > 0) ? (Double_t) nofLatticeHits / (Double_t) nofElectrons : 0;
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec Points:               " << nofPoints << FairLogger::endl;
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec PointsAboveThreshold: " << nofPointsAboveThreshold << FairLogger::endl;
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec Digis:                " << fDigis->GetEntriesFast() << FairLogger::endl;
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec digis/points:         " << digisOverPoints << FairLogger::endl;
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec BackwardTracks:       " << nofBackwardTracks << FairLogger::endl;
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec LatticeHits:          " << nofLatticeHits  << FairLogger::endl;
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec Electrons:            " << nofElectrons << FairLogger::endl;
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec latticeHits/electrons:" << latticeHitsOverElectrons << FairLogger::endl;
  timer.Stop();
  LOG(INFO) << "CbmTrdDigitizerPRF::Exec real time=" << timer.RealTime()
            << " CPU time=" << timer.CpuTime() << FairLogger::endl;
}

Double_t CbmTrdDigitizerPRF::CalcPRF(Double_t x, Double_t W, Double_t h)
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

void CbmTrdDigitizerPRF::ScanPadPlaneTriangle(const Double_t* local_point, Double_t clusterELoss)
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
    

    //const Int_t maxCol(5/W+0.5), maxRow(6);//5/H+3);// 7 and 3 in orig. minimum 5 times 5 cm area has to be scanned
    Int_t maxCol(5/W+0.5), maxRow(6);//5/H+3);// 7 and 3 in orig. minimum 5 times 5 cm area has to be scanned
    if (fnScanRowConst > 0)
      maxRow = fnScanRowConst;
//    const_cast<const Int_t&>(maxRow);
    if (fnScanColConst > 0)
      maxCol = fnScanColConst;
//    const_cast<const Int_t&>(maxCol);

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
      LOG(WARNING) << "CbmTrdDigitizerPRF::ScanPadPlane: Summed charge: " << std::setprecision(5) << sum << "  hit:(" << columnId << ", " << rowId <<")   max:(" << fnCol-1 << ", " << fnRow-1 << ")" << FairLogger::endl;
    }
  }  
}

  // --------------------------------------------------------------------
Double_t CbmTrdDigitizerPRF::TriangleIteration(Bool_t even, Int_t step, Double_t dis_x, Double_t W, Double_t dis_y, Double_t H, Double_t h){
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
Double_t CbmTrdDigitizerPRF::TriangleIntegration(Bool_t even, Double_t dis_x, Double_t W, Double_t dis_y, Double_t H, Double_t h)
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

void CbmTrdDigitizerPRF::ScanPadPlane(const Double_t* local_point, Double_t clusterELoss)
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

    //const Int_t maxCol(5/W+0.5), maxRow(5/H+3);// 7 and 3 in orig. minimum 5 times 5 cm area has to be scanned
    Int_t maxCol(5/W+0.5), maxRow(6);//5/H+3);// 7 and 3 in orig. minimum 5 times 5 cm area has to be scanned
    if (fnScanRowConst > 0)
      maxRow = fnScanRowConst;
//    const_cast<const Int_t&>(maxRow);
    if (fnScanColConst > 0)
      maxCol = fnScanColConst;
//    const_cast<const Int_t&>(maxCol);
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
	if (fnScanRowConst == 1 && fnScanColConst == 1)
	  chargeFraction = 1.0;

	sum += chargeFraction;
	//if (iCol >= stopCol)
	/*
	  if(iCol >= 128) printf("\n----K----\n");
	  if(CbmTrdAddress::GetColumnId(address) < 0) printf("address:%i amod:%i\n            icol:%i   irow:%i\n asec:%i     acol:%i   arow:%i\n",address,CbmTrdAddress::GetModuleId(address),iCol,secRow,CbmTrdAddress::GetSectorId(address),CbmTrdAddress::GetColumnId(address),CbmTrdAddress::GetRowId(address));
	  if(iCol >= 128) printf("\n---------\n");
	*/
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
      LOG(WARNING) << "CbmTrdDigitizerPRF::ScanPadPlane: Summed charge: " << std::setprecision(5) << sum << "  hit:(" << columnId << ", " << rowId <<")   max:(" << fnCol-1 << ", " << fnRow-1 << ")" << FairLogger::endl;
    }
  }
}

void CbmTrdDigitizerPRF::SplitTrackPath(const CbmTrdPoint* point, Double_t ELoss)
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
  //const Int_t nCluster = trackLength / nClusterPerCm + 0.9;// Track length threshold of minimum 0.1cm track length in gas volume
  Int_t nCluster = trackLength / nClusterPerCm + 0.9;// Track length threshold of minimum 0.1cm track length in gas volume
  if (fnClusterConst > 0){
    nCluster = fnClusterConst;   // Set number of cluster to constant value
  }
//  const_cast<const Int_t&>(nCluster);
  if (nCluster < 1){
    LOG(WARNING) << "CbmTrdDigitizerPRF::SplitTrackPath: nCluster: "<<nCluster<<"   track length: "<<std::setprecision(5)<<trackLength<<"cm  nCluster/cm: "<<std::setprecision(2)<<nClusterPerCm<<"  ELoss: "<<std::setprecision(5)<<ELoss*1e-6<<"keV " << FairLogger::endl;
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

void CbmTrdDigitizerPRF::AddDigi(Int_t pointId, Int_t address, Double_t charge, Double_t time)
{
  //if (address < 0)
  //printf("DigiAddress:%u ModuleAddress:%i\n",address, CbmTrdAddress::GetModuleAddress(address));
  //const FairMCPoint* point = static_cast<const FairMCPoint*>(fPoints->At(pointId));
  Double_t weighting = charge;

  if (fCbmLinkWeightDistance) {
    const CbmTrdPoint* point = static_cast<CbmTrdPoint*>(fPoints->At(pointId));
    Double_t ref_pos[3] = {point->GetXOut(), point->GetYOut(), point->GetZOut()};
    TVector3 padPos, padPosErr;
    fModuleInfo->GetPadPosition(address, padPos, padPosErr);
    Double_t distance = sqrt(pow(ref_pos[0] - padPos[0],2) + pow(ref_pos[1] - padPos[1],2));
    weighting = 1. / distance;
  }

  std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator it = fDigiMap.find(address);
  if (it == fDigiMap.end()) { // Pixel not yet in map -> Add new pixel

    if (fSigma_noise_keV > 0.0){
      Double_t noise = fNoise->Gaus(0, fSigma_noise_keV * 1.E-6);// keV->GeV // add only once per digi and event noise !!!
      charge += noise; // resulting charge can be < 0 -> possible  problems with position reconstruction
    }

    CbmMatch* digiMatch = new CbmMatch();
    digiMatch->AddLink(CbmLink(weighting, pointId));
    fDigiMap[address] = make_pair(new CbmTrdDigi(address, charge, time), digiMatch);
  } else { // Pixel already in map -> Add charge
    it->second.first->AddCharge(charge);
    it->second.first->SetTime(max(time, it->second.first->GetTime()));
    it->second.second->AddLink(CbmLink(weighting, pointId));
  }
}
  ClassImp(CbmTrdDigitizerPRF)
