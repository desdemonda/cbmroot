/** @file CbmStsStation.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 04.03.2015
 **/

#include <sstream>
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "CbmStsSensor.h"
#include "CbmStsSensorTypeDssd.h"
#include "CbmStsStation.h"


// -----   Default constructor   -------------------------------------------
CbmStsStation::CbmStsStation()
	: CbmStsElement(),
	  fZ(0.),
	  fXmin(0.), fXmax(0.), fYmin(0.), fYmax(0.), fSensorD(0.),
	  fNofSensors(0),
	  fDiffSensorD(kFALSE),
	  fFirstSensor(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsStation::CbmStsStation(const char* name, const char* title,
		                                 TGeoPhysicalNode* node)
	: CbmStsElement(name, title, kStsStation, node),
	  fZ(0.),
    fXmin(0.), fXmax(0.), fYmin(0.), fYmax(0.), fSensorD(0.),
    fNofSensors(0),
    fDiffSensorD(kFALSE),
		fFirstSensor(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsStation::~CbmStsStation() {
}
// -------------------------------------------------------------------------



// -----   Initialise the station properties from sensors   ----------------
void CbmStsStation::CheckSensorProperties() {

	Int_t nSensors = 0;         // sensor counter
	Double_t zMin  =  999999.;  // sensor z minimum
	Double_t zMax  = -999999.;  // sensor z maximum

	// --- Loop over ladders
  for (Int_t iLad = 0; iLad < GetNofDaughters(); iLad++) {
  	CbmStsElement* ladd = GetDaughter(iLad);

  	// --- Loop over half-ladders
  	for (Int_t iHla = 0; iHla < ladd->GetNofDaughters(); iHla++) {
  		CbmStsElement* hlad = ladd->GetDaughter(iHla);

  		// --- Loop over modules
  		for (Int_t iMod = 0; iMod < hlad->GetNofDaughters(); iMod++) {
  			CbmStsElement* modu = hlad->GetDaughter(iMod);

  			// --- Loop over sensors
  			for (Int_t iSen = 0; iSen < modu->GetNofDaughters(); iSen++) {
  				CbmStsSensor* sensor =
  						dynamic_cast<CbmStsSensor*>(modu->GetDaughter(iSen));

  				// Set first sensor
  				if ( ! nSensors ) fFirstSensor = sensor;

  				// Get sensor z position
  		    TGeoPhysicalNode* sensorNode = sensor->GetPnode();
  		    // --- Transform sensor centre into global C.S.
  		    Double_t local[3] = {0., 0., 0.};  // sensor centre, local c.s.
   		    Double_t global[3];                // sensor centre, global c.s.
    		  sensorNode->GetMatrix()->LocalToMaster(local, global);
    		  if ( ! nSensors ) {  // first sensor
    		  	zMin = global[2];
    		  	zMax = global[2];
    		  }
    		  else {
    		  	zMin = TMath::Min(zMin, global[2]);
    		  	zMax = TMath::Max(zMax, global[2]);
    		  }

    		  // Get sensor thickness
    		  TGeoBBox* sBox = dynamic_cast<TGeoBBox*>(sensorNode->GetShape());
    		  if ( ! sBox )
    		  	LOG(FATAL) << GetName() << ": sensor shape is not a box!"
    		  	           << FairLogger::endl;
    		  	Double_t sD = 2. * sBox->GetDZ();
    		    if ( ! nSensors ) fSensorD = sD; // first sensor
    		    else {
    		    	if ( TMath::Abs(sD - fSensorD) > 0.0001 )
    		    		fDiffSensorD = kTRUE;
    		    }

    		  nSensors++;
  			} // # sensors
  		} // # modules
  	} // # half-ladders
  } // # ladders

  fZ = 0.5 * (zMin + zMax);
  fNofSensors = nSensors;
}
// -------------------------------------------------------------------------



// -----   Strip pitch    --------------------------------------------------
Double_t CbmStsStation::GetSensorPitch(Int_t iSide) const {
	if ( iSide < 0 || iSide > 1 ) {
		LOG(FATAL) << GetName() << ": illegal side identifier!"
				       << FairLogger::endl;
	}
	CbmStsSensorTypeDssd* type =
			dynamic_cast<CbmStsSensorTypeDssd*>(fFirstSensor->GetType());
	if ( ! type )
		LOG(FATAL) << GetName() << ": sensor type is not DSSD!"
		           << FairLogger::endl;
	return type->GetPitch(iSide);
}
// -------------------------------------------------------------------------



// -----   Rotation    -----------------------------------------------------
// N.B.: Implementation is only correct if the first sensor is only rotated
// in the x-y plane.
Double_t CbmStsStation::GetSensorRotation() const {
	TGeoPhysicalNode* sensorNode = fFirstSensor->GetNode();
	Double_t local[3] = {1., 0., 0.}; // unit vector on local x axis
	Double_t global[3];    // unit vector in global C.S.
	sensorNode->GetMatrix()->LocalToMaster(local, global);
	return atan2(global[1], global[0]); // angle from global to local x-axis
}
// -------------------------------------------------------------------------



// -----   Stereo angle    -------------------------------------------------
Double_t CbmStsStation::GetSensorStereoAngle(Int_t iSide) const {
	if ( iSide < 0 || iSide > 1 ) {
		LOG(FATAL) << GetName() << ": illegal side identifier!"
				       << FairLogger::endl;
	}
	CbmStsSensorTypeDssd* type =
			dynamic_cast<CbmStsSensorTypeDssd*>(fFirstSensor->GetType());
	if ( ! type )
		LOG(FATAL) << GetName() << ": sensor type is not DSSD!"
		           << FairLogger::endl;
	return type->GetStereoAngle(iSide);
}
// -------------------------------------------------------------------------



// -----   Initialise station parameters   ---------------------------------
void CbmStsStation::Init() {

	// Determine x and y extensions of the STS volume. This implementation
	// assumes that the shape of the latter derives from TGeoBBox and that
	// it is not rotated in the global c.s.
	TGeoBBox* box = dynamic_cast<TGeoBBox*>(fNode->GetShape());
	if ( ! box )
		LOG(FATAL) << GetName() << ": shape is not box! " << FairLogger::endl;
	Double_t local[3] = { 0., 0., 0.};
	Double_t global[3];
  fNode->GetMatrix()->LocalToMaster(local, global);
  fXmin = global[0] - box->GetDX();
  fXmax = global[0] + box->GetDX();
  fYmin = global[1] - box->GetDY();
  fYmax = global[1] + box->GetDY();

  // The z position of the station is obtained from the sensor positions,
  // not from the station node. This is more flexible, because it does not
  // assume the station to be symmetric.
	CheckSensorProperties();

	// Warning if varying sensor properties are found
	if ( fDiffSensorD )
		LOG(WARNING) << GetName() << ": Different values for sensor thickness!"
		             << FairLogger::endl;
}
// --------------------------------------------------------------------------



// -----   Info   -----------------------------------------------------------
string CbmStsStation::ToString() const
{
   stringstream ss;
   ss << GetName() << ": " << fNofSensors << " sensors, z = " << fZ
  	  << " cm, x = " << fXmin << " to " << fXmax << " cm, y = " << fYmin
  	  << " to " << fYmax << " cm " << "\n\t\t"
  	  << " sensor thickness " << fSensorD << " cm"
  	  << " pitch " << GetSensorPitch(0) << " cm / " << GetSensorPitch(1)
  	  << " cm, stereo angle " << GetSensorStereoAngle(0) << " / "
  	  << GetSensorStereoAngle(1);

   return ss.str();
}
// --------------------------------------------------------------------------

ClassImp(CbmStsStation)

