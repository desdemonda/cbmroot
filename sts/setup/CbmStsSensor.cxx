/** @file CbmStsSensor.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 03.05.2013
 **/


// Include class header
#include "CbmStsSensor.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TGeoMatrix.h"
#include "TMath.h"

// Includes from FairRoot
#include "FairField.h"
#include "FairRunAna.h"

// Includes from CbmRoot
#include "CbmLink.h"
#include "CbmStsHit.h"
#include "CbmStsPoint.h"

// Includes from STS
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorConditions.h"
#include "setup/CbmStsSensorPoint.h"
#include "setup/CbmStsSetup.h"



// -----   Constructor   ---------------------------------------------------
CbmStsSensor::CbmStsSensor() : CbmStsElement(),
                               fType(NULL),
                               fConditions(),
                               fCurrentLink(NULL),
                               fHits(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsSensor::CbmStsSensor(const char* name, const char* title,
                           TGeoPhysicalNode* node) :
                           CbmStsElement(name, title, kStsSensor, node),
                           fType(NULL),
                           fConditions(),
                           fCurrentLink(NULL),
                           fHits(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Create a new hit   ----------------------------------------------
void CbmStsSensor::CreateHit(Double_t xLocal, Double_t yLocal,
		                         CbmStsCluster* clusterF,
		                         CbmStsCluster* clusterB) {

  // ---  Check clusters and output array
	if ( ! fHits ) {
		LOG(FATAL) << GetName() << ": Hit output array not set!"
				       << FairLogger::endl;
		return;
	}
	if ( ! clusterF ) {
		LOG(FATAL) << GetName() << ": Invalid pointer to front cluster!"
				       << FairLogger::endl;
	}
	if ( ! clusterB ) {
		LOG(FATAL) << GetName() << ": Invalid pointer to back cluster!"
				       << FairLogger::endl;
	}

	// --- Transform into global coordinate system
	Double_t local[3] = { xLocal, yLocal, 0.};
	Double_t global[3];
	// TODO: Set proper errors and covariances. For the time being, pitch/sqrt(12)
	// is used as an approximation.
	Double_t error[3] = { 0.0017, 0.0017, 0.0017 };
	fNode->GetMatrix()->LocalToMaster(local, global);

	// --- Calculate hit time (average of cluster times)
	Double_t hitTime = 0.5 * ( clusterF->GetTime() + clusterB->GetTime());

	// --- Create hit
	Int_t nHits = fHits->GetEntriesFast();
	new ( (*fHits)[nHits] )
			CbmStsHit(GetAddress(),          // address
					      global,                // coordinates
					      error,                 // coordinate error
					      0.,                    // covariance xy
					      clusterF->GetIndex(),  // front cluster index
					      clusterB->GetIndex(),  // back cluster index
					      0,                     // front digi index
					      -1,                    // back digi index
					      -1,                    // sectorNr
					      hitTime);              // hit time

	LOG(DEBUG2) << GetName() << ": Creating hit at (" << global[0] << ", "
			        << global[1] << ", " << global[2] << ")" << FairLogger::endl;
	return;
}
// -------------------------------------------------------------------------



// -----   Find hits in sensor   -------------------------------------------
Int_t CbmStsSensor::FindHits(vector<CbmStsCluster*>& clusters,
		                         TClonesArray* hitArray) {
	fHits = hitArray;
	Int_t nHits = fType->FindHits(clusters, this);
	LOG(DEBUG2) << GetName() << ": Clusters " << clusters.size()
			        << ", hits " << nHits << FairLogger::endl;
	return nHits;
}
// -------------------------------------------------------------------------



// -----  Get the mother module   ------------------------------------------
CbmStsModule* CbmStsSensor::GetModule() const {
	 return dynamic_cast<CbmStsModule*> ( GetMother() );
}
// -------------------------------------------------------------------------



// -----   Process a CbmStsPoint  ------------------------------------------
Int_t CbmStsSensor::ProcessPoint(const CbmStsPoint* point, CbmLink* link) {

	// Check whether type is assigned
	if ( ! fType ) {
		LOG(FATAL) << GetName() << ": No sensor type assigned!"
				       << FairLogger::endl;
		return -1;
	}

	// --- Set current link
	fCurrentLink = link;

  // --- Debug output of transformation matrix
  if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG4) ) {
  	LOG(DEBUG4) << GetName() << ": Transformation matrix is:"
  			        << FairLogger::endl;
  	fNode->GetMatrix()->Print();
  }

  // --- Transform entry coordinates into local C.S.
  Double_t global[3];
  Double_t local[3];
  global[0] = point->GetXIn();
  global[1] = point->GetYIn();
  global[2] = point->GetZIn();
  fNode->GetMatrix()->MasterToLocal(global, local);
  Double_t x1 = local[0];
  Double_t y1 = local[1];
  Double_t z1 = local[2];

  // --- Transform exit coordinates into local C.S.
  global[0] = point->GetXOut();
  global[1] = point->GetYOut();
  global[2] = point->GetZOut();
  fNode->GetMatrix()->MasterToLocal(global, local);
  Double_t x2 = local[0];
  Double_t y2 = local[1];
  Double_t z2 = local[2];

  // --- Momentum magnitude
  Double_t px = point->GetPx();
  Double_t py = point->GetPy();
  Double_t pz = point->GetPz();
  Double_t p = TMath::Sqrt( px*px + py*py + pz*pz );

  // --- Get magnetic field
  global[0] = 0.5 * ( point->GetXIn() + point->GetXOut() );
  global[1] = 0.5 * ( point->GetYIn() + point->GetYOut() );
  global[2] = 0.5 * ( point->GetZIn() + point->GetZOut() );
  Double_t bField[3] = { 0., 0., 0.};
  if ( FairRunAna::Instance() )
  	FairRunAna::Instance()->GetField()->Field(global, bField);

  // --- Create SensorPoint
  // Note: there is a conversion from kG to T in the field values.
  CbmStsSensorPoint* sPoint = new CbmStsSensorPoint(x1, y1, z1, x2, y2, z2, p,
                                                    point->GetEnergyLoss(),
                                                    point->GetTime(),
                                                    bField[0] / 10.,
                                                    bField[1] / 10.,
                                                    bField[2] / 10.,
                                                    point->GetPid());
  LOG(DEBUG2) << GetName() << ": Local point coordinates are (" << x1
  		        << ", " << y1 << "), (" << x2 << ", " << y2 << ")"
  		        << FairLogger::endl;
  LOG(DEBUG2) << GetName() << ": Sensor type is " << fType->GetName()
  		        << " " << fType->GetTitle() << FairLogger::endl;

  // --- Call ProcessPoint method from sensor type
  Int_t result = fType->ProcessPoint(sPoint, this);
  delete sPoint;

  return result;
}
// -------------------------------------------------------------------------


ClassImp(CbmStsSensor)
