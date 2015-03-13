/**
 * \file CbmMvdGeoHandler.cxx
 * \author Philipp Sitzmann <p.sitzmann@gsi.de>
 * \brief addapted from TrdGeoHandler by Florian Uhlig <f.uhlig@gsi.de>
 */

#include "CbmMvdGeoHandler.h"


#include "FairLogger.h"

#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TVirtualMC.h"

#include <string>
#include <cstdlib>
#include <iostream>
#include <iomanip>   
using std::cout;
using std::endl;
using std::string;
using std::atoi;


//--------------------------------------------------------------------------
CbmMvdGeoHandler::CbmMvdGeoHandler() 
  : TObject(),
  fGeoPathHash(),   //!
  fCurrentVolume(), //!
  fVolumeShape(),   //!
  fGlobal(),     //! Global center of volume
  fGlobalMatrix(),  //!
  fLayerId(),       //!
  fModuleId(),     //!
  fModuleType(),    //!
  fStation(),       //! StationID
  fMother(),
  fGeoTyp(),
  fStationName(),
  fDetectorName(),
  fSectorName(),
  fQuadrantName(),
  fSensorHolding(),
  fSensorName(),
  fnodeName(),
  fVolId(),
  fStationNumber(),
  fWidth(),
  fHeight(),
  fRadLength(),
  fBeamwidth(),
  fBeamheight(),
  fThickness(),
  fXres(),
  fYres(),
  fIsSimulation(),
  fStationMap(),
  fStationPar(),
  fDetector()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
CbmMvdGeoHandler::~CbmMvdGeoHandler()
{

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void CbmMvdGeoHandler::Init(Bool_t isSimulation)
{
   fIsSimulation = isSimulation;
   GetPipe();
   GetGeometryTyp();
   if(!isSimulation)
	{
   	fStationPar = new CbmMvdStationPar(fGeoTyp);
	fStationPar->Init();
	}
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Int_t CbmMvdGeoHandler::GetSensorAddress()
{
// In the simulation we can not rely on the TGeoManager information
// In the simulation we get the correct information only from gMC 

  return 1;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Int_t CbmMvdGeoHandler::GetSensorAddress(const TString& path)
{
  if (fGeoPathHash != path.Hash()) {
    NavigateTo(path);
  }
  return GetSensorAddress();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Double_t CbmMvdGeoHandler::GetSizeX(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDX();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Double_t CbmMvdGeoHandler::GetSizeY(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDY();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Double_t CbmMvdGeoHandler::GetSizeZ(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Double_t CbmMvdGeoHandler::GetZ(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[2];
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Double_t CbmMvdGeoHandler::GetY(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[1];
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Double_t CbmMvdGeoHandler::GetX(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[0];
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
Int_t CbmMvdGeoHandler::GetStation(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fStation;
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void CbmMvdGeoHandler::NavigateTo(
      const TString& path)
{
  //   cout << "path : " << path.Data() << endl;
   if (fIsSimulation) {
      LOG(FATAL) << "This method is not supported in simulation mode" << FairLogger::endl;
   } else {
      gGeoManager->cd(path.Data());
      fGeoPathHash = path.Hash();
      fCurrentVolume = gGeoManager->GetCurrentVolume();
      TString name = fCurrentVolume->GetName();
      //cout << endl << "this volume#include "SensorDataSheets/CbmMvdMimosa26AHR.h" is " << name << endl;
      fVolumeShape = (TGeoBBox*)fCurrentVolume->GetShape();
      Double_t local[3] = {0., 0., 0.};  // Local center of volume
      gGeoManager->LocalToMaster(local, fGlobal);
      fGlobalMatrix = gGeoManager->GetCurrentMatrix();
	if(name.Contains("S0"))
	fStationNumber = 0;
	else if(name.Contains("S1"))
	fStationNumber = 1;
	else if(name.Contains("S2"))
	fStationNumber = 2;
	else if(name.Contains("S3"))
	fStationNumber = 3;
        else
	LOG(FATAL) <<  "couldn't find Station in volume name, somthing seems fishy " << FairLogger::endl;
	local[0] = fVolumeShape->GetDX();
        local[1] = fVolumeShape->GetDY();
	Double_t fGlobalMax[3]; 
        gGeoManager->LocalToMaster(local, fGlobalMax);

	local[0] = -1*fVolumeShape->GetDX();
        local[1] = -1*fVolumeShape->GetDY();
	Double_t fGlobalMin[3]; 
        gGeoManager->LocalToMaster(local, fGlobalMin);

     	if(fGlobalMax[0] > fGlobalMin[0]){fWidth = fGlobalMax[0]; fBeamwidth = fGlobalMin[0];}else {fWidth = fGlobalMin[0]; fBeamwidth = fGlobalMax[0];}
  	if(fGlobalMax[1] > fGlobalMin[1]){fHeight = fGlobalMax[1]; fBeamheight = fGlobalMin[1];}else {fHeight = fGlobalMin[1]; fBeamheight = fGlobalMax[1];}

	// TODO: hard coded numbers, find other way only for Mvd_v14a / v14b
 	if(fStationNumber == 0) fRadLength = 0.24; 
	else if(fStationNumber == 1) fRadLength = 0.31;
	else if(fStationNumber == 2) fRadLength = 0.47; 
	else fRadLength = 0.49;  

  	fXres = 3.8; // TODO: pixelSizeX / sqrt{12}
  	fYres = 3.8; // TODO: pixelSizeY / sqrt{12}
   }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void CbmMvdGeoHandler::GetPipe()
{
TString pipeName = "pipevac1";
Int_t pipeID;
TGeoNode* pipeNode;
TString motherName;

fMother = "cave_1/pipevac1_0";

	if (!gGeoManager->CheckPath(fMother.Data()))
	         {
		pipeID = gGeoManager->GetUID(pipeName);
	 	pipeNode = gGeoManager->GetNode(pipeID);
	        gGeoManager->CdTop();
		gGeoManager->CdDown(0);
		motherName=gGeoManager->GetPath();
		fMother = motherName;
		fMother += "/";
		fMother += pipeName;
		fMother += "_0";
		gGeoManager->CdTop();
		}
      else
		fMother = "cave_1/pipevac1_0";
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

void CbmMvdGeoHandler::GetGeometryTyp()
{
if ( gGeoManager->CheckPath(fMother + "/Beamtimeosetupoobgnum_0"))
	{
	cout << endl << "Found Beamtimesetup" << endl;
	fGeoTyp = 2;
	}
else if (gGeoManager->CheckPath(fMother + "/MVDoMistraloquero012oStationo150umodigi_0"))
	{
	LOG(INFO) << "Found MVD with 3 Stations" << FairLogger::endl;
	fGeoTyp = 3;
	}
else if (gGeoManager->CheckPath(fMother + "/MVDo0123ohoFPCoextoHSoSo0123_0"))
	{
	LOG(INFO) << "Found MVD with 4 Stations" << FairLogger::endl;
	fGeoTyp = 4;
	fDetectorName = "/MVDo0123ohoFPCoextoHSoSo0123_0";
	}
else 
	{
	cout << endl << "Try standard Geometry" << endl;
	fGeoTyp = 1;
	}
 
}
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
void CbmMvdGeoHandler::Fill()
{
if(fIsSimulation)
	FillStationMap();
else
	FillDetector();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void CbmMvdGeoHandler::FillDetector()
{
if(fGeoTyp == 1)
	LOG(FATAL) << "Using old Geometry files within the new Digitizer is not supported, "
		   << "please use CbmMvdDigitizeL if you want to use this Geometry" << FairLogger::endl;
if(fGeoTyp != 4)
	LOG(FATAL) <<  "Geometry format of MVD file " 
		   <<  " not yet supported." << FairLogger::endl;
else
{
fDetector = CbmMvdDetector::Instance();

if(!fDetector)
	LOG(FATAL) <<  "GeometryHandler coulden't find a valide Detector"
		   << FairLogger::endl;

fDetector->SetParameterFile(fStationPar);

Int_t iStation = 0;
  	for(Int_t StatNr = 0; StatNr < 4; StatNr++)
      	{
	if(StatNr == 0)
		fStationName = "/MVDo0ohoFPCoHSoS_1";
	else
		fStationName = Form("/MVDo%iohoFPCoextoHSoS_1",StatNr);
		for(Int_t QuadNr = 0; QuadNr < 4; QuadNr++)
	    	{
		if(StatNr == 0)
	        fQuadrantName = Form("/St0Q%iohoFPC_1", QuadNr);
		else
		fQuadrantName = Form("/St%iQ%iohoFPCoext_1",StatNr, QuadNr);
	     		for(Int_t Layer = 0; Layer < 2; Layer++)
		 	 {
		  	
		     		 for(Int_t SensNr = 0; SensNr < 50; SensNr++)
			  	{
				fSensorHolding = Form("/MVD-S%i-Q%i-L%i-C%02i-P0oPartAss_1",  StatNr, QuadNr, Layer, SensNr);
			    	fSensorName = Form("MVD-S%i-Q%i-L%i-C%02i-P0", StatNr, QuadNr, Layer, SensNr);
			  	fVolId = gGeoManager->GetUID(fSensorName);
				if(fVolId > -1)
				for(Int_t SegmentNr = 0; SegmentNr < 50; SegmentNr++)
			       {
				fSectorName = Form("/S%iQ%iS%i_1", StatNr, QuadNr, SegmentNr);
				fnodeName = fMother + fDetectorName + fStationName + fQuadrantName + fSectorName + fSensorHolding + "/" + fSensorName + "_1";
				//cout << endl << "looking for " << fnodeName << endl;
				Bool_t nodeFound = gGeoManager->CheckPath(fnodeName.Data());
				        if(nodeFound)
					{
					fDetector->AddSensor(fSensorName, fSensorName, fnodeName, new CbmMvdMimosa26AHR, iStation, fVolId, 0.0, StatNr);
					iStation++;
					FillParameter();
					}
					}
				}
			}
		}
	}
}
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void CbmMvdGeoHandler::FillParameter()
{
   
if (fGeoPathHash != fnodeName.Hash()) {
      NavigateTo(fnodeName);
}
fStationPar->SetZPosition(fStationNumber, fGlobal[2]);
fStationPar->SetWidth(fStationNumber, fWidth);
fStationPar->SetHeight(fStationNumber, fHeight);
fStationPar->SetThickness(fStationNumber, fGlobal[2]);
fStationPar->SetXRes(fStationNumber, fXres);
fStationPar->SetYRes(fStationNumber, fYres);
fStationPar->SetRadLength(fStationNumber, fRadLength);
if(fBeamheight > 0)fStationPar->SetBeamHeight(fStationNumber, fBeamheight);
if(fBeamwidth > 0)fStationPar->SetBeamWidth(fStationNumber, fBeamwidth);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void CbmMvdGeoHandler::FillStationMap()
{
if(fGeoTyp == 4)
{
Int_t iStation = 0;
  	for(Int_t StatNr = 0; StatNr < 4; StatNr++)
      	{
	if(StatNr == 0)
		fStationName = "/MVDo0ohoFPCoHSoS_1";
	else
		fStationName = Form("/MVDo%iohoFPCoextoHSoS_1",StatNr);
		for(Int_t QuadNr = 0; QuadNr < 4; QuadNr++)
	    	{
		if(StatNr == 0)
	        fQuadrantName = Form("/St0Q%iohoFPC_1", QuadNr);
		else
		fQuadrantName = Form("/St%iQ%iohoFPCoext_1",StatNr, QuadNr);
	     		for(Int_t Layer = 0; Layer < 2; Layer++)
		 	 {
		  	
		     		 for(Int_t SensNr = 0; SensNr < 50; SensNr++)
			  	{
				fSensorHolding = Form("/MVD-S%i-Q%i-L%i-C%02i-P0oPartAss_1",  StatNr, QuadNr, Layer, SensNr);
			    	fSensorName = Form("MVD-S%i-Q%i-L%i-C%02i-P0", StatNr, QuadNr, Layer, SensNr);
			  	fVolId = gGeoManager->GetUID(fSensorName);
				if(fVolId > -1)
				for(Int_t SegmentNr = 0; SegmentNr < 50; SegmentNr++)
			       {
				fSectorName = Form("/S%iQ%iS%i_1", StatNr, QuadNr, SegmentNr);
				fnodeName = fMother + fDetectorName + fStationName + fQuadrantName + fSectorName + fSensorHolding + "/" + fSensorName + "_1";
				//cout << endl << "looking for " << fnodeName << endl;
				Bool_t nodeFound = gGeoManager->CheckPath(fnodeName.Data());
				        if(nodeFound)
					{
					fStationMap[fVolId] = iStation;
					iStation++;
					
					}
					}
				}
			}
		}

}
}
else if(fGeoTyp == 1)
{
  Int_t iStation =  1;
  Int_t volId    = -1;
  do {
    TString volName = Form("mvdstation%02i", iStation);
    volId = gGeoManager->GetUID(volName);
    if (volId > -1 ) {
      fStationMap[volId] = iStation;
      LOG(INFO) << GetName() << "::ConstructAsciiGeometry: "
           << "Station No. " << iStation << ", volume ID " << volId 
	   << ", volume name " << volName << FairLogger::endl;
      iStation++;
    }
  } while ( volId > -1 );


}
else
	LOG(FATAL) << "You tried to use an unsoported Geometry" << FairLogger::endl;
}
//--------------------------------------------------------------------------

ClassImp(CbmMvdGeoHandler)
