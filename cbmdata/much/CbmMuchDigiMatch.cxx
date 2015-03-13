/** CbmMuchDigiMatch.cxx
 **@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since   23.03.07
 **@version 1.0
 **
 ** Data class for matching CbmMuchDigi with CbmMuchPoint.
 ** The indices of CbmMuchDigi and CbmMuchDigiMatch correspond in their
 ** respective arrays.
 ** The class holds the indices of all MuchPoints corresponding to
 ** the MuchDigi.
 **/


#include "CbmMuchDigiMatch.h"
#include "CbmMuchPoint.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"
#include <iostream>
#include <limits>
#include <cassert>
#include <map>
using std::cout;
using std::endl;
using std::numeric_limits;
using std::map;
using std::pair;
using std::make_pair;

// -----   Default  constructor   ------------------------------------------
CbmMuchDigiMatch::CbmMuchDigiMatch() 
  : CbmMatch(),
  //  fRefIndex(0),
    fMCtimePerPrimaryElectron(0),
  //  fCharge(0),
    fRefIndexPerPrimaryElectron(0),
    fChargePerPrimaryElectron(0),
    fTimePerPrimaryElectron(0),
    fSignalShape(0),
    fT0(0),
    fDeadTime(0.)
{
};
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmMuchDigiMatch::CbmMuchDigiMatch(CbmMuchDigiMatch* match)
  : CbmMatch(*match),
  //  fRefIndex(match->fRefIndex),
    fMCtimePerPrimaryElectron(match->fMCtimePerPrimaryElectron),
  //  fCharge(match->fCharge),
    fRefIndexPerPrimaryElectron(match->fRefIndexPerPrimaryElectron),
    fChargePerPrimaryElectron(match->fChargePerPrimaryElectron),
    fTimePerPrimaryElectron(match->fTimePerPrimaryElectron),
    fSignalShape(match->fSignalShape),
    fT0(match->fT0),
    fDeadTime(match->fDeadTime)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMuchDigiMatch::~CbmMuchDigiMatch() { };
// -------------------------------------------------------------------------


//// -----   Public method AddPoint   ----------------------------------------
//Int_t CbmMuchDigiMatch::AddPoint(Int_t iPoint) {
//  if ( iPoint < 0 ) {
//    cout << "-W- CbmMuchDigiMatch::AddPoint: Illegal MuchPoint index "
//	 << iPoint << endl;
//    return 0;
//  }
//  Int_t n = fRefIndex.GetSize();
//  fRefIndex.Set(n+1);
//  fRefIndex.AddAt(iPoint, n);
//  return n+1;
//}
//// -------------------------------------------------------------------------


//// -----   Public method GetRefIndex   -------------------------------------
//Int_t CbmMuchDigiMatch::GetRefIndex(Int_t i) const {
//  if ( i<0 || i>GetNPoints()-1 ) {
//    cout << "-W- CbmMuchDigiMatch::GetRefIndex: Illegal index number "
//	 << i << endl;
//    return -1;
//  }
//  return fRefIndex.At(i);
//}
//// -------------------------------------------------------------------------


//// -----   Public method AddCharge  ----------------------------------------
//UInt_t CbmMuchDigiMatch::AddCharge(UInt_t iCharge) {
////  if ( iCharge < 0 ) {
////    cout << "-W- CbmMuchDigiMatch::AddCharge: Illegal charge value "
////	 << iCharge << endl;
////    return 0;
////  }
//  Int_t n = fCharge.GetSize();
//  fCharge.Set(n+1);
//  fCharge.AddAt(iCharge, n);
//  assert(fCharge.At(n) == iCharge);
//  return n+1;
//}
//// -------------------------------------------------------------------------


//// -----   Public method AddCharge  ----------------------------------------
//UInt_t CbmMuchDigiMatch::AddCharge(Int_t iPoint, UInt_t iCharge) {
//  for (Int_t i=0;i<fRefIndex.GetSize();i++){
//    if (fRefIndex[i]!=iPoint) continue;
//    fCharge[i]+=iCharge;
//    return 0;
//  }
//  AddPoint(iPoint);
//  return AddCharge(iCharge);
//}
//// -------------------------------------------------------------------------


//// -----   Public method GetCharge  ----------------------------------------
//UInt_t CbmMuchDigiMatch::GetCharge(Int_t i) const {
//  if ( i<0 || i>GetNPoints()-1 ) {
//    cout << "-W- CbmMuchDigiMatch::GetCharge: Illegal index number "
//	 << i << endl;
//    return 0;
//  }
//  return fCharge.At(i);
//}
//// -------------------------------------------------------------------------


//// -----   Public method GetTotalCharge  -----------------------------------
//UInt_t CbmMuchDigiMatch::GetTotalCharge() const {
//  UInt_t totCharge = 0;
//  for(Int_t iRef=0; iRef<GetNPoints(); ++iRef){
//    UInt_t first = totCharge;
//    totCharge += GetCharge(iRef);
//    if(totCharge < first || totCharge < GetCharge(iRef)) totCharge = numeric_limits<Int_t>::max();
//  }
//  return totCharge;
//}
//// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigiMatch::SortPointsInTime(TClonesArray* points) {
  assert(points != NULL);
  map<Double_t, CbmLink> timeMap;
  Int_t nofLinks = GetNofLinks();
  for (Int_t i = 0; i < nofLinks; i++){
    const CbmLink& link = GetLink(i);
    const CbmMuchPoint* point = static_cast<const CbmMuchPoint*>(points->At(link.GetIndex()));
    assert(point != NULL);
    timeMap[point->GetTime()] = link;
  }
  Clear();
  map<Double_t, CbmLink>::iterator it;
  for (it = timeMap.begin(); it != timeMap.end(); ++it) {
     AddLink(it->second);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigiMatch::Reset() {
   Clear();
  fRefIndexPerPrimaryElectron.Reset(0);
  fChargePerPrimaryElectron.Reset(0);
  fTimePerPrimaryElectron.Reset(0);
  fMCtimePerPrimaryElectron.Reset(0);
  fSignalShape.Reset(0);
  fRefIndexPerPrimaryElectron.Set(0);
  fChargePerPrimaryElectron.Set(0);
  fTimePerPrimaryElectron.Set(0);
  fMCtimePerPrimaryElectron.Set(0);
  fSignalShape.Set(0);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigiMatch::AddCharge(Int_t iPoint, UInt_t charge, Double_t t, TArrayD shape, Double_t mcTime) {
  AddLink(CbmLink(charge, iPoint));
  Int_t n = fRefIndexPerPrimaryElectron.GetSize();
  fRefIndexPerPrimaryElectron.Set(n+1);
  fChargePerPrimaryElectron.Set(n+1);
  fTimePerPrimaryElectron.Set(n+1);
  fMCtimePerPrimaryElectron.Set(n+1);
  fRefIndexPerPrimaryElectron.AddAt(iPoint, n);
  fChargePerPrimaryElectron.AddAt(charge, n);
  fTimePerPrimaryElectron.AddAt(t, n);
  fMCtimePerPrimaryElectron.AddAt(mcTime,n);
  // initial time is set 100 ns before arrival of the first point 
  // since later points may contribute before if drift time is different for them
  // TODO  take from digitizer
  if (n==0) fT0 = t-50;
  Int_t bin0 = Int_t((t-fT0)/gkResponseBin);
  if (t<fT0) {
    printf("=====\n");
    printf("Warning t=%f < fT0=%f\n",t,fT0);
    printf("=====\n");
  }
  Int_t nbins = bin0+shape.GetSize();
  if (fSignalShape.GetSize()<nbins) fSignalShape.Set(nbins);
  for (Int_t j=0;j<shape.GetSize();j++)  fSignalShape[bin0 + j]+=charge*shape[j];
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigiMatch::AddNoise(Double_t meanNoise){
  for (Int_t i=0;i<fSignalShape.GetSize();i++){
    fSignalShape[i]+=TMath::Abs(meanNoise*gRandom->Gaus());
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmMuchDigiMatch::GetMaxCharge(){
  Double_t max_charge = -1;
  for (Int_t i=0;i<fSignalShape.GetSize();i++){
    Double_t charge = fSignalShape[i];
    if (charge>max_charge) max_charge = charge;
  }
  return max_charge;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmMuchDigiMatch::GetTimeStamp(Double_t threshold){
  Int_t bin1 = -1;
  for (Int_t i=0;i<fSignalShape.GetSize();i++){
    if (bin1<0 && fSignalShape[i]>threshold) {
      bin1 = i;
      return fT0+bin1*gkResponseBin;
    }
  }
  return -1;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmMuchDigiMatch::GetTimeOverThreshold(Double_t threshold){
  Int_t bin1 = -1;
  Int_t bin2 = -1;
  for (Int_t i=0;i<fSignalShape.GetSize();i++){
    if (bin1<0 && fSignalShape[i]>threshold) bin1 = i;
    if (bin1>0 && fSignalShape[i]<threshold) {
      bin2 = i;
      return (bin2-bin1)*gkResponseBin;
    }
  }
  if (bin1>0 && bin2<0) return (fSignalShape.GetSize()-bin1)*gkResponseBin;
  return -1;
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchDigiMatch)
