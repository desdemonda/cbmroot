/**
 * \file CbmTrdGas.cxx
 * \brief Container for gas properties of TRD.
 */

#include "CbmTrdGas.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoMaterial.h"
#include "TObjArray.h"
#include "TMath.h"

CbmTrdGas* CbmTrdGas::fgInstance = 0;

CbmTrdGas::CbmTrdGas()
  : TObject(),
    fDetType(-1),
    fGasThick(0.),
    fPercentNobleGas(0.),
    fPercentCO2(0.),  
    fNobleGasType(-1),
    fFileNameLike(""),
    fFileNameANN("")
{
   if (fgInstance) {
      LOG(FATAL) << "CbmTrdGas::CbmTrdGas Singleton instance already exists." << FairLogger::endl;
   }
   fgInstance = this;
}

CbmTrdGas::~CbmTrdGas()
{

}

void CbmTrdGas::Init()
{
   // Read MWPC gas properties from geometry. This enforce that gas
   // mixture simulation and reconstruction is the same. This is important
   // because dE/dx is calculated in the simulation and TR is calculated
   // in the reconstruction.

   // Get pointer to gas volume
   TGeoVolume* fm = (TGeoVolume*) gGeoManager->GetListOfVolumes()->FindObject("gas");
   if (!fm) {
      TObjArray* volList = gGeoManager->GetListOfVolumes();
      TObjArrayIter iter(volList);
      TGeoVolume* vol = NULL;
      LOG(ERROR) << "********** List of available volumes ************ " << FairLogger::endl;
      while( (vol = (TGeoVolume*)iter.Next()) ) {
         LOG(ERROR) << vol->GetName() << FairLogger::endl;
      }
      LOG(ERROR) << "***************** End of List ******************* " << FairLogger::endl
            << " -E- Could not find volume <gas>. " << FairLogger::endl
            << " -E- If there is no list above this text then probably "<< FairLogger::endl
            << " -E- the geometry was not loaded in the macro." << FairLogger::endl
            << " -E- Please do it with fRun->LoadGeometry().  " << FairLogger::endl
            << " -E- If you see a list probably the names of the " << FairLogger::endl
            << " -E- volumes have changed and CbmTrdRadiator has to " << FairLogger::endl
            << " -E- be changed accordingly. " << FairLogger::endl;
      LOG(FATAL) << "CbmTrdGas::Init: No volumes defined." << FairLogger::endl;
   }

    // check if detector is of GSI or Muenster/Bucarest type
    // the type is coded in the master volume name
    TGeoVolume* fm1 = (TGeoVolume*) gGeoManager->GetListOfVolumes()->FindObject("trd1mb");

    // get pointer to shape of gas volume
    TGeoBBox* shape = (TGeoBBox*) fm->GetShape();
    
    if (fm1) { // MB type
      fDetType = 1;
      // only halve thickness of gas layer tacken into account because the
      // total absorbed TR is calculated in two steps since the real chamber
      // consists of two gas volumes with both halve the thickness as from
      // geometry
      fGasThick = 2 * (shape->GetDZ());
      LOG(INFO) << "CbmTrdGas::Init: Detector type : double sided geometry (1) "<< FairLogger::endl;
      LOG(INFO) << "CbmTrdGas::Init: Gas thickness : " << fGasThick << " cm" <<FairLogger::endl;
    } else { // GSI type
      fDetType = 0;
      fGasThick = 2 * (shape->GetDZ());
      LOG(INFO) << "CbmTrdGas::Init: Detector type : standard GSI geometry (2) " << FairLogger::endl;
      LOG(INFO) << "CbmTrdGas::Init: Gas thickness : " << fGasThick << " cm" << FairLogger::endl;
    }

   // Get all the necessary properties of the gas mixture
   TGeoMixture* mixt = (TGeoMixture*) fm->GetMaterial();
   Int_t nmixt = mixt->GetNelements();
   if (nmixt != 3) {
      LOG(ERROR) << "CbmTrdGas::Init: This is not a mixture composed out of three different elements." << FairLogger::endl;
      LOG(ERROR) << "CbmTrdGas::Init: Don't know what to do, so stop execution here." << FairLogger::endl;
      LOG(FATAL) << "CbmTrdGas::Init: Unknown gas mixture." << FairLogger::endl;
   }

   Bool_t foundCarbon = kFALSE;
   Bool_t foundOxygen = kFALSE;
   Int_t carbon = 0;
   Int_t oxygen = 0;
   Int_t noblegas = 0;

   Double_t* elem = mixt->GetZmixt();
   Double_t* weight = mixt->GetWmixt();
   Double_t* amixt = mixt->GetAmixt();

   for (Int_t i = 0 ; i < nmixt; i++) {
      if (elem[i] == 6.0) {
         carbon = i;
         foundCarbon = kTRUE;
      } else if (elem[i] == 8.0) {
         oxygen = i;
         foundOxygen = kTRUE;
      } else noblegas = i;
   }
   if (!(foundCarbon && foundOxygen)) {
      LOG(ERROR) << "CbmTrdGas::Init: This gas mixture has no CO2 admixture" << FairLogger::endl
         << "CbmTrdGas::Init: If you want to use this mixture you have to change" << FairLogger::endl
         << "CbmTrdGas::Init: CbmTrdRadiator to be consistent" << FairLogger::endl;
      LOG(FATAL) << "CbmTrdGas::Init: Unknown gas mixture." << FairLogger::endl;
   }
   if (elem[noblegas] != 54) {
      LOG(ERROR) << "CbmTrdGas::Init:  This gas mixture has no Xe admixture" << FairLogger::endl
         << "CbmTrdGas::Init:  If you want to use this mixture you have to change" << FairLogger::endl
         << "CbmTrdGas::Init:  CbmTrdRadiator to be consistent" << FairLogger::endl;
      LOG(FATAL) << "CbmTrdGas::Init: Unknown gas mixture." << FairLogger::endl;
   } else {
      fNobleGasType = 1;
   }

   Double_t massC = amixt[carbon];
   Double_t massO = amixt[oxygen];
   Double_t massXe = amixt[noblegas];
   Double_t x = weight[noblegas];
   Double_t percentNoblegas = 100 * (((massC * x) + (2 * massO * x))/(massXe + massC * x + 2 * massO * x - massXe * x));

   fPercentNobleGas = TMath::Nint(percentNoblegas) / 100.;
   fPercentCO2 = 1 - fPercentNobleGas;

   if (elem[noblegas] == 54) {
      LOG(INFO) << "CbmTrdGas::Init: Percent  (Xe) : " << (fPercentNobleGas * 100) << FairLogger::endl;
   }
   LOG(INFO) << "CbmTrdGas::Init: Percent (CO2) : " << (fPercentCO2 * 100) << FairLogger::endl;;

   SetFileName();
}

TString CbmTrdGas::GetFileName(TString method) const
{
   if (method.Contains("Like")) {
      return fFileNameLike;
   } else if (method.Contains("ANN")) {
      return fFileNameANN;
   } else {
      LOG(ERROR) << "CbmTrdGas::GetFileName: Electron ID method " << method << " not known." << FairLogger::endl
         << "CbmTrdGas::GetFileName: The input must be either Like or ANN" << FairLogger::endl;
      return "";
   }
}

void CbmTrdGas::SetFileName()
{
   Int_t fraction =  TMath::Nint(fPercentNobleGas*100);
   Int_t thickness =  TMath::Nint(fGasThick*10);

   const char* detector = "";
   if (fDetType == 0) {
      detector = "GSI";
   } else if (fDetType == 1) {
      detector = "MB";
   } else {
      LOG(ERROR) << "CbmTrdGas::SetFileName: Detector type " << fDetType << " not known"<< FairLogger::endl;
      LOG(ERROR) << "CbmTrdGas::SetFileName: Stop execution of program due to initialization error." << FairLogger::endl;
      LOG(FATAL) << "CbmTrdGas::SetFileName: Unknown detector type." << FairLogger::endl;
   }
   const char* gastype = "";
   if (fNobleGasType == 1) {
      gastype = "Xenon";
   } else {
      LOG(ERROR) << "CbmTrdGas::SetFileName: Gas type " << fNobleGasType << " not known" << FairLogger::endl;
      LOG(ERROR) << "CbmTrdGas::SetFileName: Stop execution of program due to initialization error." << FairLogger::endl;
      LOG(FATAL) << "CbmTrdGas::SetFileName: Unknown gas type." << FairLogger::endl;
   }

   TString path = getenv("VMCWORKDIR");
   path = path + "/parameters/trd/";
   fFileNameLike.Form("Likelihood_%s_%d_%s_%d.root", gastype, fraction, detector, thickness);
   fFileNameANN.Form("ANN_%s_%d_%s_%d.root", gastype, fraction, detector, thickness);

   fFileNameLike = path + fFileNameLike;
   fFileNameANN = path + fFileNameANN;
}

ClassImp(CbmTrdGas)
