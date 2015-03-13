/** @file CbmStsSensorTypeDssd.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#include "digitize/CbmStsSensorTypeDssd.h"

#include <iomanip>

#include "TMath.h"

#include "FairLogger.h"


#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorPoint.h"
#include "setup/CbmStsSensor.h"


using std::fixed;
using std::setprecision;



// --- Energy for creation of an electron-hole pair in silicon [GeV]  ------
const double kPairEnergy = 3.57142e-9;



// -----   Constructor   ---------------------------------------------------
CbmStsSensorTypeDssd::CbmStsSensorTypeDssd()
    : CbmStsSensorType(), 
      fDx(-1.), fDy(-1.), fDz(-1.),
      fPitch(), fStereo(), fIsSet(kFALSE),
      fNofStrips(), fTanStereo(), fStripShift()
{
}
// -------------------------------------------------------------------------



// -----   Find hits   -----------------------------------------------------
Int_t CbmStsSensorTypeDssd::FindHits(vector<CbmStsCluster*>& clusters,
		                                 CbmStsSensor* sensor) {

	Int_t nHits = 0;
	Int_t nClusters = clusters.size();

	Int_t nClustersF = 0;
	Int_t nClustersB = 0;
	Double_t xF = -1.;   // Cluster position on sensor edge
	Double_t xB = -1.;   // Cluster position on sensor edge

	// --- Sort clusters into front and back side
	vector<Int_t> frontClusters;
	vector<Int_t> backClusters;
	Double_t xCluster = -1.;  // cluster position on read-out edge
	Int_t side  = -1;         // front or back side
	for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
		side = GetSide( clusters[iCluster]->GetCentre() );
		if ( side == 0) {
			frontClusters.push_back(iCluster);
			nClustersF++;
		}
		else if ( side == 1 ) {
			backClusters.push_back(iCluster);
			nClustersB++;
		}
		else
			LOG(FATAL) << GetName() << ": Illegal side qualifier "
			           << side << FairLogger::endl;
	}  // Loop over clusters in module

	// --- Loop over front and back side clusters
	Double_t xClusterF = -1.;  // Front cluster position at r/o edge
	Double_t xClusterB = -1.;  // Back cluster position at r/o edge
	for (Int_t iClusterF = 0; iClusterF < nClustersF; iClusterF++) {
		CbmStsCluster* clusterF = clusters[frontClusters[iClusterF]];
		for (Int_t iClusterB = 0; iClusterB < nClustersB;	iClusterB++) {
			CbmStsCluster* clusterB = clusters[backClusters[iClusterB]];

			// --- Calculate intersection points
			Int_t nOfHits = IntersectClusters(clusterF, clusterB, sensor);
			LOG(DEBUG4) << GetName() << ": Cluster front " << iClusterF
					        << ", cluster back " << iClusterB
					        << ", intersections " << nOfHits << FairLogger::endl;
			nHits += nOfHits;

		}  // back side clusters

	}  // front side clusters

  LOG(DEBUG3) << GetName() << ": Clusters " << nClusters << " ( "
  		        << nClustersF << " / " << nClustersB << " ), hits: "
  		        << nHits << FairLogger::endl;
	return nHits;
}
// -------------------------------------------------------------------------



// -----   Get cluster position at read-out edge   -------------------------
// Same as GetStrip, but with float instead of channel number
void CbmStsSensorTypeDssd::GetClusterPosition(Double_t centre,
		                                          Int_t sensorId,
                                              Double_t& xCluster,
                                              Int_t& side) {

	// Take integer channel
	Int_t iChannel = Int_t(centre);
	Double_t xDif = centre - Double_t(iChannel);

	// Calculate corresponding strip on sensor
	Int_t iStrip = -1;
	GetStrip(iChannel, sensorId, iStrip, side);

	// Re-add difference to integer channel. Convert channel to
	// coordinate
	xCluster = (Double_t(iStrip) + xDif + 0.5 ) * fPitch[side];

	LOG(DEBUG4) << GetName() << ": Cluster centre " << centre
			        << ", sensor index " << sensorId << ", side "
			        << side << ", cluster position " << xCluster
			        << FairLogger::endl;
	return;
}
// -------------------------------------------------------------------------



// -----   Get channel number in module   ----------------------------------
Int_t CbmStsSensorTypeDssd::GetModuleChannel(Int_t strip, Int_t side,
                                             Int_t sensorId) const {

  // --- Check side
  if ( side < 0 || side > 1 ) {
    LOG(ERROR) << "Illegal side qualifier " << side << FairLogger::endl;
    return -1;
  }

  // --- Account for offset due to stereo angle
  Int_t channel = strip - sensorId * fStripShift[side];

  // --- Account for horizontal cross-connection of strips
  while ( channel < 0 ) channel += fNofStrips[side];
  while ( channel >= fNofStrips[side] ) channel -= fNofStrips[side];

  // --- Account for front or back side
  if ( side ) channel += fNofStrips[0];

  return channel;
}
// -------------------------------------------------------------------------



// -----  Get the strip pitch    -------------------------------------------
Double_t CbmStsSensorTypeDssd::GetPitch(Int_t iSide) const {
	if ( iSide < 0 || iSide > 1 ) {
		LOG(FATAL) << GetName() << ": illegal side identifier!"
				       << FairLogger::endl;
	}
	return fPitch[iSide];
}
// -------------------------------------------------------------------------



// -----  Get the stereo angle    ------------------------------------------
Double_t CbmStsSensorTypeDssd::GetStereoAngle(Int_t iSide) const {
	if ( iSide < 0 || iSide > 1 ) {
		LOG(FATAL) << GetName() << ": illegal side identifier!"
				       << FairLogger::endl;
	}
	return fStereo[iSide];
}
// -------------------------------------------------------------------------



// -----   Get strip number from coordinates   -----------------------------
Int_t CbmStsSensorTypeDssd::GetStripNumber(Double_t x, Double_t y,
		                                       Int_t side) const {

	// Cave: This implementation assumes that the centre of the sensor volume
	// is also the centre of the active area, i.e. that the inactive borders
	// (guard ring) are symmetric both and x and y (not necessarily the same
	// in y and y).

	// Check side
	if ( side < 0 || side > 1) {
		LOG(ERROR) << "Illegal side qualifier " << side << FairLogger::endl;
		return -1;
	}

	// Check whether in active area (should have been caught before)
	if ( TMath::Abs(x) > fDx / 2. ) {
		LOG(ERROR) << GetName() << ": Outside active area : x = "
				       << x << " cm"<< FairLogger::endl;
    return -1;
	}
	if ( TMath::Abs(y) > fDy / 2. ) {
		LOG(ERROR) << GetName() << ": Outside active area : y = "
			         << y << " cm"<< FairLogger::endl;
    return -1;
	}

  // Stereo angle and strip pitch
  Double_t tanphi = fTanStereo[side];
  Double_t pitch  = fPitch[side];
  Int_t nStrips   = fNofStrips[side];

  // Calculate distance from lower left corner of the active area.
  // Note: the coordinates are given w.r.t. the centre of the volume.
  Double_t xdist = x + 0.5 * fDx;
  Double_t ydist = y + 0.5 * fDy;

  // Project coordinates to readout (top) edge
  Double_t xro = xdist - ( fDy - ydist ) * tanphi;

  // Calculate corresponding strip number
  Int_t iStrip = TMath::FloorNint( xro / pitch );

  // Account for horizontal cross-connection of strips
  // not extending to the top edge
  while ( iStrip < 0 )        iStrip += nStrips;
  while ( iStrip >= nStrips ) iStrip -= nStrips;

  if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG4)) Print();
  LOG(DEBUG3) << setprecision(6) << GetName() << ": (x,y) = (" << x << ", " << y
  		        << ") cm, x(ro) = " << xro << " cm, strip "
  		        << iStrip << FairLogger::endl;
  return iStrip;
}
// -------------------------------------------------------------------------



// -----   Get strip and side from channel number   ------------------------
void CbmStsSensorTypeDssd::GetStrip(Int_t channel, Int_t sensorId,
                                    Int_t& strip, Int_t& side) {

  Int_t stripNr = -1;
  Int_t sideNr  = -1;


  // --- Determine front or back side
  if ( channel < fNofStrips[0] ) {          // front side
    sideNr = 0;
    stripNr = channel;
  }
  else {
    sideNr = 1;
    stripNr = channel - fNofStrips[0];      // back side
  }
  side = sideNr;

  // --- Offset due to stereo angle
  stripNr += sensorId * fStripShift[side];

  // --- Horizontal cross-connection
  while ( stripNr < 0 ) stripNr += fNofStrips[side];
  while ( stripNr >= fNofStrips[side] ) stripNr -= fNofStrips[side];

  strip = stripNr;

 return;
}
// -------------------------------------------------------------------------



// -----   Intersection of two lines along the strips   --------------------
Bool_t CbmStsSensorTypeDssd::Intersect(Double_t xF, Double_t xB,
		                                   Double_t& x, Double_t& y) {

	// In the coordinate system with origin at the bottom left corner,
	// a line along the strips with coordinate x0 at the top edge is
	// given by the function y(x) = Dy - ( x - x0 ) / tan(phi), if
	// the stereo angle phi does not vanish. Two lines yF(x), yB(x) with top
	// edge coordinates xF, xB intersect at
	// x = ( tan(phiB)*xF - tan(phiF)*xB ) / (tan(phiB) - tan(phiF)
	// y = Dy + ( xB - xF ) / ( tan(phiB) - tan(phiF) )
	// For the case that one of the stereo angles vanish (vertical strips),
	// the calculation of the intersection is straightforward.

	// --- First check whether stereo angles are different. Else there is
	// --- no intersection.
	if ( TMath::Abs(fStereo[0]-fStereo[1]) < 0.5 ) {
		x = -1000.;
		y = -1000.;
		return kFALSE;
	}

	// --- Now treat vertical front strips
	if ( TMath::Abs(fStereo[0]) < 0.001 ) {
		x = xF;
		y = fDy - ( xF - xB ) / fTanStereo[1];
		return IsInside(x, y);
	}

	// --- Maybe the back side has vertical strips?
	if ( TMath::Abs(fStereo[1]) < 0.001 ) {
		x = xB;
		y = fDy - ( xB - xF ) / fTanStereo[0];
		return IsInside(x, y);
	}

	// --- OK, both sides have stereo angle
	x = ( fTanStereo[1] * xF - fTanStereo[0] * xB ) /
			( fTanStereo[1] - fTanStereo[0]);
	y = fDy + ( xB - xF ) / ( fTanStereo[1] - fTanStereo[0]);
	return IsInside(x, y);

}
// -------------------------------------------------------------------------



// -----  Intersect two clusters (front / back)   --------------------------
Int_t CbmStsSensorTypeDssd::IntersectClusters(CbmStsCluster* clusterF,
		                                          CbmStsCluster* clusterB,
		                                          CbmStsSensor* sensor) {

	// --- Check pointer validity
	if ( ! clusterF ) {
		LOG(FATAL) << GetName() << ": invalid front cluster pointer!"
				        << FairLogger::endl;
		return 0;
	}
	if ( ! clusterB ) {
		LOG(FATAL) << GetName() << ": invalid back cluster pointer!"
				        << FairLogger::endl;
		return 0;
	}
	if ( ! sensor ) {
		LOG(FATAL) << GetName() << ": invalid sensor pointer!"
				        << FairLogger::endl;
		return 0;
	}

	// --- Calculate cluster centre position on readout edge
	Int_t side  = -1;
	Double_t xF = -1.;
	Double_t xB = -1.;
	GetClusterPosition(clusterF->GetCentre(), sensor->GetIndex(), xF, side);
	if ( side != 0 )
		LOG(FATAL) << GetName() << ": Inconsistent side qualifier " << side
		           << " for front side cluster! " << FairLogger::endl;
	GetClusterPosition(clusterB->GetCentre(), sensor->GetIndex(), xB, side);
	if ( side != 1 )
		LOG(FATAL) << GetName() << ": Inconsistent side qualifier " << side
		           << " for back side cluster! " << FairLogger::endl;

	// --- Should be inside active area
	if ( ! ( xF >= 0. || xF <= fDx) ) return 0;
	if ( ! ( xB >= 0. || xB <= fDx) ) return 0;

	// --- Hit counter
	Int_t nHits = 0;

	// --- Calculate number of line segments due to horizontal
	// --- cross-connection. If x(y=0) does not fall on the bottom edge,
	// --- the strip is connected to the one corresponding to the line
	// --- with top edge coordinate xF' = xF +/- Dx. For odd combinations
	// --- of stereo angle and sensor dimensions, this could even happen
	// --- multiple times. For each of these lines, the intersection with
	// --- the line on the other side is calculated. If inside the active area,
	// --- a hit is created.
	Int_t nF = Int_t( (xF + fDy * fTanStereo[0]) / fDx );
	Int_t nB = Int_t( (xB + fDy * fTanStereo[1]) / fDx );

	// --- If n is positive, all lines from 0 to n must be considered,
	// --- if it is negative (phi negative), all lines from n to 0.
	Int_t nF1 = TMath::Min(0, nF);
	Int_t nF2 = TMath::Max(0, nF);
	Int_t nB1 = TMath::Min(0, nB);
	Int_t nB2 = TMath::Max(0, nB);

	// --- Double loop over possible lines
	Double_t xC = -1.;   // x coordinate of intersection point
	Double_t yC = -1.;   // y coordinate of intersection point
	for (Int_t iF = nF1; iF <= nF2; iF++) {
		Double_t xFi = xF - Double_t(iF) * fDx;
		for (Int_t iB = nB1; iB <= nB2; iB++) {
			Double_t xBi = xB - Double_t(iB) * fDx;

			// --- Intersect the two lines
			Bool_t found = Intersect(xFi, xBi, xC, yC);
			LOG(DEBUG4) << GetName() << ": Trying " << xFi << ", " << xBi
					        << ", intersection ( " << xC << ", " << yC
					        << " ) " << ( found ? "TRUE" : "FALSE" )
					        << FairLogger::endl;
			if ( found ) {

				// --- Transform into sensor system with origin at sensor centre
				xC -= 0.5 * fDx;
				yC -= 0.5 * fDy;
				// --- Send hit information to sensor
				sensor->CreateHit(xC, yC, clusterF, clusterB);
				nHits++;

			}  //? Intersection of lines
		}  // lines on back side
	}  // lines on front side

	return nHits;
}
// -------------------------------------------------------------------------



// -----   Check whether a point is inside the active area   ---------------
Bool_t CbmStsSensorTypeDssd::IsInside(Double_t x, Double_t y) {
	if ( x > 0. && x < fDx && y > 0. && y < fDy) return kTRUE;
	return kFALSE;
}
// -------------------------------------------------------------------------



// -----   Print parameters   ----------------------------------------------
void CbmStsSensorTypeDssd::Print(Option_t* opt) const {

  LOG(INFO) << "Properties of sensor type " << GetName() << ": "
            << FairLogger::endl
            << "\t  Dimensions: (" << fixed << setprecision(4)
            << fDx << ", " << fDy << ", " << fDz << ") cm"
            << FairLogger::endl
            << "\t  Front side: pitch = "
            << setprecision(0) << fPitch[0]*1.e4 << " mum, "
            << fNofStrips[0] << " strips, stereo angle "
            << setprecision(1) << fStereo[0] << " degrees"
            << FairLogger::endl
            << "\t  Back side:  pitch = "
            << setprecision(0) << fPitch[1]*1.e4 << " mum, "
            << fNofStrips[1] << " strips, stereo angle "
            << setprecision(1) << fStereo[1] << " degrees"
            << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   Process an MC Point  --------------------------------------------
Int_t CbmStsSensorTypeDssd::ProcessPoint(CbmStsSensorPoint* point,
                                         const CbmStsSensor* sensor) const {

  // --- Catch if parameters are not set
  if ( ! fIsSet ) {
    LOG(FATAL) << fName << ": parameters are not set!"
               << FairLogger::endl;
    return -1;
  }

  // --- Debug
  LOG(DEBUG4) << GetName() << ": processing sensor point at ( "
		      << point->GetX1() << ", " << point->GetX2()
		      << " ) cm at " << point->GetTime() << " ns, energy loss "
		      << point->GetELoss() << ", PID " << point->GetPid()
		      << ", By = " << point->GetBy() << " T"
		      << FairLogger::endl;

  // --- Check for being in sensitive area
  // --- Note: No charge is produced if either entry or exit point
  // --- (or both) are outside the active area. This is not an exact
  // --- description since the track may enter the sensitive area
  // --- if not perpendicular to the sensor plane. The simplification
  // --- was chosen to avoid complexity. :-)
  if ( TMath::Abs(point->GetX1()) > fDx/2. ||
	   TMath::Abs(point->GetY1()) > fDy/2. ||
	   TMath::Abs(point->GetX2()) > fDx/2. ||
	   TMath::Abs(point->GetY2()) > fDy/2. ) return 0;

  // --- Number of created charge signals (coded front/back side)
  Int_t nSignals = 0;

  // --- Produce charge on front and back side
  nSignals += 1000 * ProduceCharge(point, 0, sensor); // front
  nSignals +=        ProduceCharge(point, 1, sensor); // back

  return nSignals;
}
// -------------------------------------------------------------------------



// -----   Produce charge on the strips   ----------------------------------
Int_t CbmStsSensorTypeDssd::ProduceCharge(CbmStsSensorPoint* point,
                                          Int_t side,
                                          const CbmStsSensor* sensor)
                                          const {

  // --- Protect against being called without parameters being set
  if ( ! fIsSet ) LOG(FATAL) << "Parameters of sensor " << fName
                             << " are not set!" << FairLogger::endl;

  // This implementation assumes a straight trajectory in the sensor
  // and a constant charge distribution along it.

  // Check for side qualifier
  if ( side < 0 || side > 1 )  {
    LOG(ERROR) << "Illegal side qualifier!" << FairLogger::endl;
    return -1;
  }

  // Total produced charge
  Double_t qtot = point->GetELoss() / kPairEnergy;

  // Stereo angle and strip pitch
  Double_t tanphi = fTanStereo[side];
  Double_t pitch  = fPitch[side];
  Int_t nStrips   = fNofStrips[side];

  // Debug output
  LOG(DEBUG4) << GetName() << ": Side " << side << ", dx = " << fDx
  		        << " cm, dy = " << fDy << " cm, stereo " << fStereo[side]
  		        << " degrees, strips " << fNofStrips[side] << ", pitch "
  		        << pitch << " mum" << FairLogger::endl;

  // Project point coordinates (in / out) along strips to readout (top) edge
  // Keep in mind that the SensorPoint gives coordinates with
  // respect to the centre of the active area.
  Double_t x1 = point->GetX1() + 0.5 * fDx
                - ( 0.5 * fDy - point->GetY1() ) * tanphi;
  Double_t x2 = point->GetX2() + 0.5 * fDx
                - ( 0.5 * fDy - point->GetY2() ) * tanphi;
  LOG(DEBUG4) << GetName() << ": R/O x coordinates are " << x1 << " "
  		        << x2 << FairLogger::endl;


  // Calculate corresponding strip numbers
  // This can be negative or larger than the number of channels if the
  // strip does not extend to the top edge.
  Int_t i1 = TMath::FloorNint( x1 / pitch );
  Int_t i2 = TMath::FloorNint( x2 / pitch );


  // --- More than one strip: sort strips
  if ( i1 > i2 ) {
    Int_t tempI = i1;
    i1 = i2;
    i2 = tempI;
    Double_t tempX = x1;
    x1 = x2;
    x2 = tempX;
  }


  // --- Loop over fired strips
  Int_t nSignals = 0;
  for (Int_t iStrip = i1; iStrip <= i2; iStrip++) {

    // --- Calculate charge in strip
    Double_t y1 = TMath::Max(x1, Double_t(iStrip) * pitch);  // start in strip
    Double_t y2 = TMath::Min(x2, Double_t(iStrip+1) * pitch); // stop in strip
    Double_t charge = (y2 - y1) * qtot / ( x2 - x1 );

    // --- Register charge to module
    RegisterCharge(sensor, side, iStrip, charge, point->GetTime());
    nSignals++;

  } // Loop over fired strips

  return nSignals;
}
// -------------------------------------------------------------------------



// -----   Register charge   -----------------------------------------------
void CbmStsSensorTypeDssd::RegisterCharge(const CbmStsSensor* sensor,
                                          Int_t side, Int_t strip,
                                          Double_t charge,
                                          Double_t time) const {

  // --- Determine module channel for given sensor strip
  Int_t channel = GetModuleChannel(strip, side, sensor->GetSensorId() );

  // --- Debug output
  LOG(DEBUG4) << fName << ": Registering charge: side " << side
              << ", strip " << strip << ", time " << time
              << ", charge " << charge
              << " to channel " << channel
              << " of module " << sensor->GetModule()->GetName()
              << FairLogger::endl;

  // --- Get the MC link information
  Int_t index = -1;
  Int_t entry = -1;
  Int_t file  = -1;
  if ( sensor->GetCurrentLink() ) {
  	index = sensor->GetCurrentLink()->GetIndex();
  	entry = sensor->GetCurrentLink()->GetEntry();
  	file  = sensor->GetCurrentLink()->GetFile();
  }

  // --- Send signal to module
  sensor->GetModule()->AddSignal(channel, time, charge, index, entry, file);

}
// -------------------------------------------------------------------------



// -----   Self test   -----------------------------------------------------
Bool_t CbmStsSensorTypeDssd::SelfTest() {

  for (Int_t sensorId = 0; sensorId < 3; sensorId++ ) {
    for (Int_t side = 0; side < 2; side ++ ) {
      for (Int_t strip = 0; strip < fNofStrips[side]; strip++ ) {
        Int_t channel = GetModuleChannel(strip, side, sensorId);
        Int_t testStrip, testSide;
        GetStrip(channel, sensorId, testStrip, testSide);
        if ( testStrip != strip || testSide != side ) {
          LOG(ERROR) << fName << "Self test failed! Sensor " << sensorId
                     << " side " << side << " strip " << strip
                     << " gives channel " << channel << " gives strip "
                     << testStrip << " side " << testSide
                     << FairLogger::endl;
          return kFALSE;
        }
      } // strip loop
    } // side loop
  } // sensor loop

  //LOG(DEBUG2) << fName << ": self test passed" << FairLogger::endl;
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Set the parameters   --------------------------------------------
void CbmStsSensorTypeDssd::SetParameters(Double_t dx, Double_t dy,
                                         Double_t dz, Int_t nStripsF,
                                         Int_t nStripsB, Double_t stereoF,
                                         Double_t stereoB) {

  // --- Check stereo angles
  if ( TMath::Abs(stereoF) > 85. )
    LOG(FATAL) << "Stereo angle for front side " << stereoF
               << "outside allowed limits! "
               << "Must be between -85 and 85 degrees."
               << FairLogger::endl;
  if ( TMath::Abs(stereoB) > 85. )
    LOG(FATAL) << "Stereo angle for back side " << stereoB
               << "outside allowed limits! "
               << "Must be between -85 and 85 degrees."
               << FairLogger::endl;

  // --- Set members
  fDx           = dx;
  fDy           = dy;
  fDz           = dz;
  fNofStrips[0] = nStripsF;
  fNofStrips[1] = nStripsB;
  fStereo[0]    = stereoF;
  fStereo[1]    = stereoB;

  // --- Calculate parameters for front and back
  for (Int_t side = 0; side < 2; side++) {
    fPitch[side] = fDx / Double_t(fNofStrips[side]);
    fTanStereo[side] = TMath::Tan( fStereo[side] * TMath::DegToRad() );
    fStripShift[side] = TMath::Nint(fDy * fTanStereo[side] / fPitch[side]);
  }

  // --- Flag parameters to be set if test is ok
  fIsSet = SelfTest();

}
// -------------------------------------------------------------------------



ClassImp(CbmStsSensorTypeDssd)
