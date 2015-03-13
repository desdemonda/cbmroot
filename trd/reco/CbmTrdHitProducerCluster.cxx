#include "CbmTrdHitProducerCluster.h"
#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdPoint.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TGeoManager.h"
#include "TMath.h"

#include <iomanip>
#include <iostream>

using std::cout;
using std::endl;

CbmTrdHitProducerCluster::CbmTrdHitProducerCluster()
  :FairTask("CbmTrdHitProducerCluster",1),
   fRecoTriangular(0),
   fDigis(NULL),
   fClusters(NULL),
   fHits(NULL),
   fDigiPar(NULL),
   fTrianglePads(false)
{
}

CbmTrdHitProducerCluster::~CbmTrdHitProducerCluster()
{
   fHits->Delete();
   delete fHits;
}

void CbmTrdHitProducerCluster::SetParContainers()
{
   fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdHitProducerCluster::Init()
{
   FairRootManager *ioman = FairRootManager::Instance();

   fDigis = (TClonesArray *) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmTrdHitProducerCluster::Init No TrdDigi array." << FairLogger::endl;

   fClusters = (TClonesArray *) ioman->GetObject("TrdCluster");
   if (fClusters == NULL) LOG(FATAL) << "CbmTrdHitProducerCluster::Init No TrdCluster array." << FairLogger::endl;

   fHits = new TClonesArray("CbmTrdHit", 100);
   ioman->Register("TrdHit","TRD Hit",fHits,kTRUE);

   return kSUCCESS;
}
void CbmTrdHitProducerCluster::SetTriangularPads(Bool_t triangles)
{
  fTrianglePads = triangles;
}
void CbmTrdHitProducerCluster::Exec(Option_t * option)
{
  fHits->Delete();

  TStopwatch timer;
  timer.Start();
  cout << "================CbmTrdHitProducerCluster===============" << endl;
  LOG(INFO) << "CbmTrdHitProducerCluster::Exec : Triangular Pads: " << (Bool_t)fTrianglePads << FairLogger::endl;
  Int_t nofCluster = fClusters->GetEntries();
 
  for (Int_t iCluster = 0; iCluster < nofCluster; iCluster++) {
    //if(fTrianglePads)
    //TriangularPadReconstruction(iCluster);
    //else
    CenterOfGravity(iCluster);
  }

  LOG(INFO) << "CbmTrdHitProducerCluster::Exec : Digis:          " << fDigis->GetEntriesFast()  << FairLogger::endl;
  LOG(INFO) << "CbmTrdHitProducerCluster::Exec : Clusters:       " << fClusters->GetEntriesFast() << FairLogger::endl;
  LOG(INFO) << "CbmTrdHitProducerCluster::Exec : Hits:           " << fHits->GetEntriesFast() << FairLogger::endl;
  LOG(INFO) << "CbmTrdHitProducerCluster::Exec : TriangularReco.:" << fRecoTriangular << FairLogger::endl;

  timer.Stop();
  LOG(INFO) << "CbmTrdHitProducerCluster::Exec : real time=" << timer.RealTime()
	    << " CPU time=" << timer.CpuTime() << FairLogger::endl;
}
Double_t CbmTrdHitProducerCluster::CalcDisplacement(Double_t Q_left, Double_t Q_center, Double_t Q_right, Double_t padWidth){
  //printf("%E, %E, %E, pW:%f\n", Q_left, Q_center, Q_right, padWidth);
  if (Q_left > 0.0 && Q_center > 0.0 && Q_right > 0.0)
    return  0.5 * padWidth * TMath::Log(Q_right / Q_left) / TMath::Log((Q_center * Q_center) / (Q_left * Q_right));
  else
    return 0.0;
}
Double_t CbmTrdHitProducerCluster::CalcY(Double_t padWidth, Double_t padHeight, Double_t rect_Dis, Double_t para_Dis, Int_t digiRow){
  Double_t alpha = TMath::ATan(padWidth / (2*padHeight));
  //printf("alpha: %f H:%f  W:%f\n",alpha, padHeight, padWidth);
  if (digiRow%2 == 0)
    return (para_Dis - (rect_Dis - 0.5 * padWidth) * TMath::Cos(alpha)) / (-1. * TMath::Sin(alpha));
  else
    return (para_Dis - (rect_Dis + 0.5 * padWidth) * TMath::Cos(alpha)) / (-1. * TMath::Sin(alpha));
}
//Bool_t combiIdSorter(std::pair a, std::pair b)
//{return (a.first < b.first); }

void CbmTrdHitProducerCluster::TriangularPadReconstruction(Int_t clusterId){
  const CbmTrdCluster* cluster = static_cast<const CbmTrdCluster*>(fClusters->At(clusterId));
 
  TVector3 hit_posV;
  TVector3 local_pad_posV;
  TVector3 local_pad_dposV;
  Double_t totalCharge = 0;
  Int_t moduleAddress = 0;
  Int_t nofDigis = cluster->GetNofDigis();
  Int_t digiMaxId(-1), digiMaxCombiId(-1), maxRow(-1), maxCol(-1), maxDigiAddress(-1);
  Double_t maxCharge = 0;
  Int_t digiId = -1;
  Int_t combiId(-1), secId(-1), colId(-1), rowId(-1), nCol(0);
  Int_t digiAddress = -1;
  Double_t padH(0.), padW(0.);
  //std::list<std::pair<Int_t, Int_t> > digiList; // combiIds = iRow * (nCol +1) + iCol, DigiId
  std::map<Int_t, Double_t> digiMap; // combiId, DigiCharge
  CbmTrdModule *moduleInfo = NULL;
  for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
    digiId = cluster->GetDigi(iDigi);
    const CbmTrdDigi* digi = static_cast<const CbmTrdDigi*>(fDigis->At(digiId));
    digiAddress = digi->GetAddress();
    secId = CbmTrdAddress::GetSectorId(digiAddress);
    colId = CbmTrdAddress::GetColumnId(digiAddress);
    rowId = CbmTrdAddress::GetRowId(digiAddress);
    moduleInfo = fDigiPar->GetModule(CbmTrdAddress::GetModuleAddress(digiAddress));
    if (moduleInfo == NULL) {
      LOG(WARNING) << "CbmTrdHitProducerCluster::TriangularPadReconstruction No CbmTrdModule found digiAddress="
		   << digi->GetAddress() << " moduleAddress=" << CbmTrdAddress::GetModuleAddress(digi->GetAddress()) << FairLogger::endl;
      return;
    }
    rowId = moduleInfo->GetModuleRow(secId, rowId);
    nCol = moduleInfo->GetNofColumns();
    combiId = rowId * (nCol + 1) + colId;
    printf("r:%4i(r:%4i)  c:%4i   ->  %4i\n", rowId, CbmTrdAddress::GetRowId(digiAddress), colId, combiId);
    //digiList.push_back(make_pair(combiId,digiId));  
    moduleAddress = CbmTrdAddress::GetModuleAddress(digi->GetAddress());
    Double_t charge = digi->GetCharge();
    digiMap[combiId] = charge;
    totalCharge += charge;
    if (charge > maxCharge){
      maxCharge = charge;
      maxRow = rowId;
      maxCol = colId;
      digiMaxCombiId = combiId;
      maxDigiAddress = digiAddress;
      padW = moduleInfo->GetPadSizeX(secId);
      padH = moduleInfo->GetPadSizeY(secId);
      digiMaxId = cluster->GetDigi(iDigi);
    }
  }
  //digiList.sort(combiIdSorter);
  //std::map<Int_t, Int_t>::iterator it;
  moduleAddress = CbmTrdAddress::GetModuleAddress(maxDigiAddress);
  moduleInfo = fDigiPar->GetModule(moduleAddress);
  gGeoManager->FindNode(moduleInfo->GetX(), moduleInfo->GetY(), moduleInfo->GetZ());
  moduleInfo->GetPadPosition(maxDigiAddress, local_pad_posV, local_pad_dposV);

  if (maxRow % 2 == 0){
    if (digiMap.find(maxRow * (nCol + 1) + (maxCol - 1))     == digiMap.end() ) 
      printf("          left same row:            (%4i,%4i) -> %4i\n",maxRow,maxCol-1,maxRow * (nCol + 1) + (maxCol - 1));
    if (digiMap.find(maxRow * (nCol + 1) + (maxCol + 1))     == digiMap.end() ) 
      printf("          right same row:           (%4i,%4i) -> %4i\n",maxRow,maxCol+1,maxRow * (nCol + 1) + (maxCol + 1));
    if (digiMap.find((maxRow+1) * (nCol + 1) + (maxCol - 1)) == digiMap.end() ) 
      printf("          left upper row:           (%4i,%4i) -> %4i\n",(maxRow+1),maxCol - 1,(maxRow+1) * (nCol + 1) + (maxCol - 1));
    if (digiMap.find((maxRow+1) * (nCol + 1) + (maxCol))     == digiMap.end() ) 
      printf("          center upper row:         (%4i,%4i) -> %4i\n",(maxRow+1),maxCol,(maxRow+1) * (nCol + 1) + (maxCol));
    if (digiMap.find((maxRow+1) * (nCol + 1) + (maxCol + 1)) == digiMap.end() ) 
      printf("          right upper row:          (%4i,%4i) -> %4i\n",(maxRow+1),maxCol + 1,(maxRow+1) * (nCol + 1) + (maxCol + 1));
    if (digiMap.find((maxRow+1) * (nCol + 1) + (maxCol + 2)) == digiMap.end() ) 
      printf("          right of right upper row: (%4i,%4i) -> %4i\n",(maxRow+1),maxCol + 2,(maxRow+1) * (nCol + 1) + (maxCol + 2));
    if (digiMap.find(maxRow * (nCol + 1) + (maxCol - 1))     == digiMap.end() || // left same row
	digiMap.find(maxRow * (nCol + 1) + (maxCol + 1))     == digiMap.end() || // right same row
	digiMap.find((maxRow+1) * (nCol + 1) + (maxCol - 1)) == digiMap.end() || // left upper row
	digiMap.find((maxRow+1) * (nCol + 1) + (maxCol))     == digiMap.end() || // center upper row
	digiMap.find((maxRow+1) * (nCol + 1) + (maxCol + 1)) == digiMap.end() || // right upper row 
	digiMap.find((maxRow+1) * (nCol + 1) + (maxCol + 2)) == digiMap.end() ){ // right of right upper row
      CenterOfGravity(clusterId);
      LOG(WARNING) << "CbmTrdHitProducerCluster::TriangularPadReconstruction Row:" << maxRow << " Col:" << maxCol << " max:(" << moduleInfo->GetNofRows() << "," << nCol << ")" << FairLogger::endl;
      return;
    } else {
      fRecoTriangular++;
      //printf("parallel padW:%f rect padW:%f\n",padW * TMath::Cos(TMath::ATan(padW / padH)),padW);
      Double_t d_rec = CalcDisplacement(digiMap[digiMaxCombiId - 1] + digiMap[maxRow + 1 * (nCol + 1) + (maxCol - 1)], // left rectagular
					digiMap[digiMaxCombiId]     + digiMap[maxRow + 1 * (nCol + 1) + (maxCol)],     // central rectagular
					digiMap[digiMaxCombiId + 1] + digiMap[maxRow + 1 * (nCol + 1) + (maxCol + 1)], // right rectangular
					padW);
      Double_t d_par = CalcDisplacement(digiMap[digiMaxCombiId - 1] + digiMap[maxRow + 1 * (nCol + 1) + (maxCol)],     // left parallelogram
					digiMap[digiMaxCombiId]     + digiMap[maxRow + 1 * (nCol + 1) + (maxCol + 1)], // central parallelogram
					digiMap[digiMaxCombiId + 1] + digiMap[maxRow + 1 * (nCol + 1) + (maxCol + 2)], // right parallelogram
					padW * TMath::Cos(TMath::ATan(padW / padH)));
      Double_t dy = CalcY(padW, padH, d_rec, d_par, maxRow);
      local_pad_posV[0] += d_rec;
      local_pad_posV[1] += 0.5 * padH + dy;
      //printf("d_rec:%f  d_par:%f  dy:%f \n", d_rec, d_par, dy);
    }
  } else {
    if (digiMap.find(maxRow * (nCol + 1) + (maxCol - 1))     == digiMap.end() ) 
      printf("          left same row:          (%4i,%4i) -> %4i\n",maxRow,maxCol-1,maxRow * (nCol + 1) + (maxCol - 1));
    if (digiMap.find(maxRow * (nCol + 1) + (maxCol + 1))     == digiMap.end() ) 
      printf("          right same row:         (%4i,%4i) -> %4i\n",maxRow,maxCol+1,maxRow * (nCol + 1) + (maxCol + 1));
    if (digiMap.find((maxRow-1) * (nCol + 1) + (maxCol - 2)) == digiMap.end() ) 
      printf("          left of left lower row: (%4i,%4i) -> %4i\n",maxRow-1,maxCol-2,(maxRow-1) * (nCol + 1) + (maxCol - 2));
    if (digiMap.find((maxRow-1) * (nCol + 1) + (maxCol - 1)) == digiMap.end() ) 
      printf("          left lower row:         (%4i,%4i) -> %4i\n",maxRow-1,maxCol-1,(maxRow-1) * (nCol + 1) + (maxCol - 1));
    if (digiMap.find((maxRow-1) * (nCol + 1) + (maxCol))     == digiMap.end() ) 
      printf("          center lower row:       (%4i,%4i) -> %4i\n",maxRow-1,maxCol,(maxRow-1) * (nCol + 1) + (maxCol));
    if (digiMap.find((maxRow-1) * (nCol + 1) + (maxCol + 1)) == digiMap.end() ) 
      printf("          right lower row:        (%4i,%4i) -> %4i\n",maxRow-1,maxCol+1,(maxRow-1) * (nCol + 1) + (maxCol + 1));
    if (digiMap.find(digiMaxCombiId - 1)                     == digiMap.end() || // left same row
	digiMap.find(digiMaxCombiId + 1)                     == digiMap.end() || // right same row
	digiMap.find((maxRow-1) * (nCol + 1) + (maxCol - 2)) == digiMap.end() || // left of left lower row
	digiMap.find((maxRow-1) * (nCol + 1) + (maxCol - 1)) == digiMap.end() || // left lower row
	digiMap.find((maxRow-1) * (nCol + 1) + (maxCol))     == digiMap.end() || // center lower row
	digiMap.find((maxRow-1) * (nCol + 1) + (maxCol + 1)) == digiMap.end() ){ // right lower row
      CenterOfGravity(clusterId);
      LOG(WARNING) << "CbmTrdHitProducerCluster::TriangularPadReconstruction Row:" << maxRow << " Col:" << maxCol <<  " max:(" << moduleInfo->GetNofRows() << "," << nCol << ")" << FairLogger::endl;
      return;
    } else {
      fRecoTriangular++;
      //printf("parallel padW:%f rect padW:%f\n",padW * TMath::Cos(TMath::ATan(padW / padH)), padW);
      Double_t d_rec = CalcDisplacement(digiMap[digiMaxCombiId - 1] + digiMap[maxRow - 1 * (nCol + 1) + (maxCol - 1)],// left rectagular
					digiMap[digiMaxCombiId]     + digiMap[maxRow - 1 * (nCol + 1) + (maxCol)],    // central rectagular
					digiMap[digiMaxCombiId + 1] + digiMap[maxRow - 1 * (nCol + 1) + (maxCol + 1)],// right rectangular
					padW);
      Double_t d_par = CalcDisplacement(digiMap[digiMaxCombiId - 1] + digiMap[maxRow - 1 * (nCol + 1) + (maxCol - 2)],// left parallelogram
					digiMap[digiMaxCombiId]     + digiMap[maxRow - 1 * (nCol + 1) + (maxCol - 1)],// central parallelogram
					digiMap[digiMaxCombiId + 1] + digiMap[maxRow - 1 * (nCol + 1) + (maxCol + 1)],// right parallelogram
					padW * TMath::Cos(TMath::ATan(padW / padH)));
      Double_t dy = CalcY(padW, padH, d_rec, d_par, maxRow);
      local_pad_posV[0] += d_rec;
      local_pad_posV[1] += -0.5 * padH + dy;
      //printf("d_rec:%f  d_par:%f  dy:%f \n", d_rec, d_par, dy);
    }
  }
  digiMap.clear();

  if (fabs(local_pad_posV[0]) > moduleInfo->GetSizeX() || fabs(local_pad_posV[1]) > moduleInfo->GetSizeY()) return;
  Double_t hit_pos[3];
  for (Int_t iDim = 0; iDim < 3; iDim++) {
    hit_posV[iDim] = local_pad_posV[iDim];
    //hit_posV[iDim] /= totalCharge;
    hit_pos[iDim] = hit_posV[iDim];
  }
  Double_t global_hit[3];
  gGeoManager->LocalToMaster(hit_pos, global_hit);

  for (Int_t iDim = 0; iDim < 3; iDim++){
    hit_posV[iDim] = global_hit[iDim];
  }
 
  Int_t nofHits = fHits->GetEntriesFast();
  new ((*fHits)[nofHits]) CbmTrdHit(moduleAddress, hit_posV, local_pad_dposV, 0, clusterId, 0, 0, totalCharge);
}


void CbmTrdHitProducerCluster::CenterOfGravity(Int_t clusterId)
{
  const CbmTrdCluster* cluster = static_cast<const CbmTrdCluster*>(fClusters->At(clusterId));

  TVector3 hit_posV;
  TVector3 local_pad_posV;
  TVector3 local_pad_dposV;
  for (Int_t iDim = 0; iDim < 3; iDim++) {
    hit_posV[iDim] = 0.0;
    local_pad_posV[iDim] = 0.0;
    local_pad_dposV[iDim] = 0.0;
  }
  Double_t totalCharge = 0;
  Int_t moduleAddress = 0;
  Int_t nofDigis = cluster->GetNofDigis();
  for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
    const CbmTrdDigi* digi = static_cast<const CbmTrdDigi*>(fDigis->At(cluster->GetDigi(iDigi)));

    moduleAddress = CbmTrdAddress::GetModuleAddress(digi->GetAddress());
    CbmTrdModule* moduleInfo = fDigiPar->GetModule(moduleAddress);
    gGeoManager->FindNode(moduleInfo->GetX(), moduleInfo->GetY(), moduleInfo->GetZ());

    if (moduleInfo == NULL) {
      LOG(WARNING) << "CbmTrdHitProducerCluster::CenterOfCharge No CbmTrdModule found digiAddress="
		   << digi->GetAddress() << " moduleAddress=" << CbmTrdAddress::GetModuleAddress(digi->GetAddress()) << FairLogger::endl;
      return;
    }
    totalCharge += digi->GetCharge();
    //printf("DigiAddress:%i ModuleAddress:%i\n",digi->GetAddress(), CbmTrdAddress::GetModuleAddress(digi->GetAddress()));
    moduleInfo->GetPadPosition(digi->GetAddress(), local_pad_posV, local_pad_dposV);//local_pad_pos[0], local_pad_pos[1], local_pad_pos[2]);
    if (fTrianglePads){
      Double_t local_pad_pos[3];
      for (Int_t i = 0; i < 3; i++) {
	local_pad_pos[i] = local_pad_posV[i];
      }
      Int_t secId(-1), colId(-1), rowId(-1);
      moduleInfo->GetPadInfo(local_pad_pos, secId, colId, rowId);
      if (rowId%2 == 0) { // Calculate center of gravity for triangular pads 
	local_pad_posV[0] = 1./3. * ((local_pad_pos[0] - 0.5 * moduleInfo->GetPadSizeX(secId)) + 
				     (local_pad_pos[0] + 0.5 * moduleInfo->GetPadSizeX(secId)) + 
				     (local_pad_pos[0] + 0.5 * moduleInfo->GetPadSizeX(secId)));
	local_pad_posV[1] = 1./3. * ((local_pad_pos[1] - 0.5 * moduleInfo->GetPadSizeX(secId)) + 
				     (local_pad_pos[1] - 0.5 * moduleInfo->GetPadSizeX(secId)) + 
				     (local_pad_pos[1] + 0.5 * moduleInfo->GetPadSizeX(moduleInfo->GetSector(rowId+1, secId))));
      } else {
	local_pad_posV[0] = 1./3. * ((local_pad_pos[0] - 0.5 * moduleInfo->GetPadSizeX(secId)) + 
				     (local_pad_pos[0] + 0.5 * moduleInfo->GetPadSizeX(secId)) + 
				     (local_pad_pos[0] - 0.5 * moduleInfo->GetPadSizeX(secId)));
	local_pad_posV[1] = 1./3. * ((local_pad_pos[1] + 0.5 * moduleInfo->GetPadSizeX(secId)) + 
				     (local_pad_pos[1] + 0.5 * moduleInfo->GetPadSizeX(secId)) + 
				     (local_pad_pos[1] - 0.5 * moduleInfo->GetPadSizeX(moduleInfo->GetSector(rowId-1, secId))));
      }
    }
    moduleInfo->TransformHitError(local_pad_dposV);

    for (Int_t iDim = 0; iDim < 3; iDim++) {
      hit_posV[iDim] += local_pad_posV[iDim] * digi->GetCharge();
    }
  }
  Double_t hit_pos[3];
  for (Int_t iDim = 0; iDim < 3; iDim++) {
    hit_posV[iDim] /= totalCharge;
    hit_pos[iDim] = hit_posV[iDim];
  }
  Double_t global_hit[3];
  gGeoManager->LocalToMaster(hit_pos, global_hit);

  for (Int_t iDim = 0; iDim < 3; iDim++){
    hit_posV[iDim] = global_hit[iDim];
  }
  Int_t nofHits = fHits->GetEntriesFast();
  new ((*fHits)[nofHits]) CbmTrdHit(moduleAddress, hit_posV, local_pad_dposV, 0, clusterId, 0, 0, totalCharge);
}
    ClassImp(CbmTrdHitProducerCluster)
