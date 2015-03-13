/** @file CbmStsSensorTypeDssdIdeal.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 20.05.2014
 **/

#include <iomanip>

#include "TMath.h"

#include "FairLogger.h"

#include "digitize/CbmStsSensorTypeDssdIdeal.h"
#include "setup/CbmStsSensorPoint.h"


using namespace std;


// --- Energy for creation of an electron-hole pair in silicon [GeV]  ------
const double kPairEnergy = 3.6e-9;



// -----   Constructor   ---------------------------------------------------
CbmStsSensorTypeDssdIdeal::CbmStsSensorTypeDssdIdeal()
    : CbmStsSensorTypeDssd()
{
}
// -------------------------------------------------------------------------



// -----   Produce charge on the strips   ----------------------------------
Int_t CbmStsSensorTypeDssdIdeal::ProduceCharge(CbmStsSensorPoint* point,
																					 		 Int_t side,
                                               const CbmStsSensor* sensor)
																						 	 const {

  // --- Protect against being called without parameters being set
  if ( ! fIsSet ) LOG(FATAL) << "Parameters of sensor " << fName
                             << " are not set!" << FairLogger::endl;

  // This implementation projects all charge on one strip on front and
  // back side, which correspond to the horizontal projections of
  // the mid-point of the trajectory in the sensor.

  // Check side
  if ( side < 0 || side > 1) {
    LOG(ERROR) << "Illegal side qualifier " << side << FairLogger::endl;
    return -1;
 }

  // Calculate trajectory mid-point
  Double_t xP = 0.5 * ( point->GetX1() + point->GetX2() );
  Double_t yP = 0.5 * ( point->GetY1() + point->GetY2() );

  // Check for being in sensitive area
  if ( TMath::Abs(xP) > fDx/2. || TMath::Abs(yP) > fDy/2. )
  	return 0;

  // Get strip number
	Int_t iStrip = GetStripNumber(xP, yP, side);
	if ( iStrip < 0 || iStrip >= fNofStrips[side] ) {
		LOG(ERROR) << GetName() << ": Illegal strip number " << iStrip
				       << FairLogger::endl;
		return -1;
	}

  // Total produced charge
  Double_t qtot = point->GetELoss() / kPairEnergy;

  // Register charge to module, if inside active area.
  if ( iStrip >= 0 )
  	RegisterCharge(sensor, side, iStrip, qtot, point->GetTime() );

  return 1;
}
// -------------------------------------------------------------------------


ClassImp(CbmStsSensorTypeDssdIdeal)
