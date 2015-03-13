/** @file CbmStsSensorTypeDssd.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#include "digitize/CbmStsSensorTypeDssdReal.h"

#include <iomanip>
#include <fstream>
#include <cstring>
//#include <vector>

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"
#include "TSystem.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TParticle.h"

#include "FairLogger.h"

#include "digitize/CbmStsPhysics.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorPoint.h"
#include "setup/CbmStsSensor.h"


using std::fixed;
using std::setprecision;
using std::ifstream;
using std::vector;


// --- Energy for creation of an electron-hole pair in silicon [GeV]  ------
const double kPairEnergy = 3.57142e-9;

// --- Flags to switch on/off different physical processes in sensor -------
static Bool_t fDiffusion = 1;
static Bool_t fNonUniformity = 1;
static Bool_t fCrossTalk = 1;
static Bool_t fField = 1;

// -----   Constructor   ---------------------------------------------------
    CbmStsSensorTypeDssdReal::CbmStsSensorTypeDssdReal()
: CbmStsSensorTypeDssd(), 
    fVdepletion (), fVbias (), fkTq (), fTemperature (),
    fCcoup (), fCinter (), fCTcoef (), fPhysics(CbmStsPhysics::Instance())
{
}
// -------------------------------------------------------------------------

/*
// -----   Print parameters   ----------------------------------------------
void CbmStsSensorTypeDssdReal::Print(Option_t* opt) const {

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
*/


// -----   Process an MC Point  --------------------------------------------
Int_t CbmStsSensorTypeDssdReal::ProcessPoint(CbmStsSensorPoint* point,
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
	<< " ) cm at " << point->GetTime() << " ns, energy loss (Geant) "
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

    // --- Created charge signals
    Int_t nSignals = 0;

    vector <Double_t> ELossLayerArray;
    // --- Produce charge on front and back side
    nSignals += 1000 * ProduceCharge(point, 0, sensor, ELossLayerArray);
    nSignals += ProduceCharge(point, 1, sensor, ELossLayerArray);

    return nSignals;
}
// -------------------------------------------------------------------------


// -----   Produce charge on the strips   ----------------------------------
//Int_t CbmStsSensorTypeDssdReal::ProduceCharge(CbmStsSensorPoint* point,
//	Int_t side,
//	const CbmStsSensor* sensor, Double_t * ELossLayerArray)
Int_t CbmStsSensorTypeDssdReal::ProduceCharge(CbmStsSensorPoint* point,
	Int_t side,
	const CbmStsSensor* sensor, vector<Double_t> &ELossLayerArray)
const {

    // --- Protect against being called without parameters being set
    if ( ! fIsSet ) LOG(FATAL) << "Parameters of sensor " << fName
	<< " are not set!" << FairLogger::endl;

    // This implementation assumes a straight trajectory in the sensor
    // and a non-uniform charge distribution along it.

    // Check for side qualifier
    if ( side < 0 || side > 1 )  {
	LOG(ERROR) << "Illegal side qualifier!" << FairLogger::endl;
	return -1;
    }

    // Total produced charge
    Double_t qtot = point->GetELoss() / kPairEnergy;
    LOG(DEBUG4) << "Total produced charge (from Geant) =  " << 
	qtot <<  FairLogger::endl;

    // Stereo angle and strip pitch
    Double_t tanphi = fTanStereo[side];
    Double_t pitch  = fPitch[side];
    Int_t nStrips   = fNofStrips[side];

    // Debug output
    LOG(DEBUG4) << GetName() << ": Side " << side << ", dx = " << fDx
	<< " cm, dy = " << fDy << " cm, stereo " << fStereo[side]
	<< " degrees, strips " << fNofStrips[side] << ", pitch "
	<< pitch << " mum" << FairLogger::endl;

    //coordinates of point in local coordinate system of sensor
    Double_t locX1 = point->GetX1() + 0.5 * fDx;
    Double_t locX2 = point->GetX2() + 0.5 * fDx;
    Double_t locY1 = point->GetY1() + 0.5 * fDy;
    Double_t locY2 = point->GetY2() + 0.5 * fDy;
    Double_t locZ1 = point->GetZ1();// + 0.5 * fDz;
    Double_t locZ2 = point->GetZ2();// + 0.5 * fDz;
    // Debug output
    LOG(DEBUG4) << GetName() << ": Side " << side 
	<< ", x1 = " << locX1 << " cm, x2 = " << locX2 
	<< " cm, y1 = " << locY1 << " cm, y2 = " << locY2
	<< " cm, z1 = " << locZ1 << " cm, z2 = " << locZ2
	<< " cm." << FairLogger::endl;

    // Calculate the length of trajectiry inside sensor 
    // (without taking into account the band in magnetic field)
    Double_t trajLength = sqrt ((locX2 - locX1) * (locX2 - locX1) + 
	    fDz * fDz + (locY2 - locY1) * (locY2 - locY1));


    // shift in (or out) coordinate due to magnetic field      
    Double_t tanLor = 0.;
    Double_t field = point -> GetBy();
    if (!fField)   field = 0;
    if (side == 0) {// n-side
	tanLor = 1650e-4 * field;// electrons
	if (locZ1 < 0)	locX1 += fDz * tanLor;
	if (locZ2 < 0)	locX2 += fDz * tanLor;
    }
    if (side == 1) {// p-side
	tanLor = 310e-4 * field;// holes
	if (locZ1 > 0) locX1 += fDz * tanLor;
	if (locZ2 > 0) locX2 += fDz * tanLor;
    }


    // Project point coordinates (in / out) along strips to readout (top) edge
    // Keep in mind that the SensorPoint gives coordinates with
    // respect to the centre of the active area.
    Double_t x1 = locX1	- ( fDy - locY1 ) * tanphi;
    Double_t x2 = locX2	- ( fDy - locY2 ) * tanphi;
    LOG(DEBUG4) << GetName() << ": R/O x coordinates are " << x1 << " "
	<< x2 << FairLogger::endl;

    // Calculate corresponding strip numbers
    // This can be negative or larger than the number of channels if the
    // strip does not extend to the top edge.
    //Int_t i1 = TMath::FloorNint( x1 / pitch );
    //Int_t i2 = TMath::FloorNint( x2 / pitch );
    Int_t i1 = int( x1 / pitch );
    Int_t i2 = int( x2 / pitch );
    LOG(DEBUG4) << GetName() << ": R/O strips are " << i1 << " "
	<< i2 << FairLogger::endl;

    Bool_t invert = 0;
    // --- More than one strip: sort strips
    if ( i1 > i2 ) {
	invert = 1;
	Int_t tempI = i1;
	i1 = i2;
	i2 = tempI;
	Double_t tempX = x1;
	x1 = x2;
	x2 = tempX;
	Double_t templocX = locX1;
	locX1 = locX2;
	locX2 = templocX;
	Double_t templocY = locY1;
	locY1 = locY2;
	locY2 = templocY;
	Double_t templocZ = locZ1;
	locZ1 = locZ2;
	locZ2 = templocZ;
    }
    if (invert) LOG(DEBUG4) << "Coordinates are inverted." << FairLogger::endl;

    Double_t * stripCharge = new Double_t[nStrips]; 
    memset(stripCharge, 0, nStrips * sizeof(Double_t) );
    Double_t * stripChargeCT;
    Double_t totalLossCharge = 0.;

    // thickness of trajectory layer in cm, should be > 2 mum and < 10 mum
    Double_t delta = 3e-4;

    // --- Get mass and charge
    Double_t mass   = 0.;
    Double_t charge = 0.;
    Int_t pid = point->GetPid();
    // First look in TDatabasePDG
    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(pid);
    if ( particle ) {
    	mass   = particle->Mass();
    	charge = particle->Charge() / 3.;
    } //? found in database
    else if (pid > 1000000000 && pid < 1010000000 ) {  // ion
    	pid -= 1000000000;
    	Int_t iCharge = pid / 10000;
    	pid -= iCharge * 10000;
    	charge = Double_t (iCharge);
    	mass   = Double_t ( pid / 10 );
    } //? ion
    else    // not found
    	LOG(FATAL) << GetName() << ": could not determine properties for PID "
    			       << point->GetPid() << FairLogger::endl;

    // --- Calculate kinetic energy; determine whether it is an electron
    Double_t p = point->GetP();
    Double_t eKin = TMath::Sqrt( p*p + mass*mass ) - mass;
    Bool_t isElectron = ( point->GetPid() == 11 || point->GetPid() == -11 );

    // --- Get stopping power
    Double_t dedx = fPhysics->StoppingPower(eKin, mass, charge, isElectron);

    LOG(DEBUG3) << GetName() << ": PID " << point->GetPid() << ", mass "
    		        << mass << " GeV, charge " << charge << ", Ekin " << eKin
    		        << " GeV, <dE/dx> = " << 1000. * dedx << " MeV "
    		        << FairLogger::endl;


    Double_t chargeLayerUni = qtot / (Int_t (trajLength / delta));


    TRandom3 * ELossRandom = new TRandom3();
    for (Int_t iLayer = 0; iLayer < Int_t(trajLength / delta); iLayer ++) {
	if (side == 0){
	    // energy loss calculation in eV; m, Ekin - in GeV, dEdx - in GeV/cm
	    if (fNonUniformity) ELossLayerArray.push_back(EnergyLossLayer(ELossRandom, delta, mass, eKin, dedx));
	    else ELossLayerArray.push_back(chargeLayerUni);
	}

	totalLossCharge += ELossLayerArray[iLayer];// in eV 
	// thermal diffusion calculation, result in eV
	ThermalSharing(delta, stripCharge, ELossLayerArray[iLayer], 
		locX1, locX2, locY1, locY2, locZ1, i1, i2, trajLength, iLayer, side); 
    }
    LOG(DEBUG4) << "Total produced charge after energy loss = " << totalLossCharge * 1e-3 << " keV" << FairLogger::endl;

    if (fCrossTalk) {
	stripChargeCT = new Double_t[nStrips];
	memset(stripChargeCT, 0, nStrips * sizeof(Double_t) );
	CrossTalk (stripCharge, stripChargeCT, nStrips, tanphi);
    }
    else stripChargeCT = stripCharge;

    // --- Loop over fired strips
    Int_t nSignals = 0;
    for (Int_t iStrip = 0; iStrip < nStrips; iStrip++) {
	if (stripChargeCT[iStrip] > 0){//charge > 1 eV

	    // --- Calculate charge in strip
	    // normalize registered charge to total charge from simulation - qtot
	    Double_t chargeNorm = stripChargeCT[iStrip] * qtot / totalLossCharge;// in e
	    // --- Register charge to module
	    RegisterCharge(sensor, side, iStrip, chargeNorm,
		    (point -> GetTime()));
	    nSignals++;
	}
    } // Loop over fired strips

    return nSignals;
}
// -------------------------------------------------------------------------


// -----   EnergyLoss calculation for 1 layer   -------------------------------
Double_t CbmStsSensorTypeDssdReal::EnergyLossLayer (TRandom3 * ELossRandom, 
	Double_t delta, Double_t m, Double_t Ekin, Double_t dEdx) const {

    Double_t gamma = (Ekin + m) / m, beta2 = 1. - 1. / (gamma * gamma);
    Double_t SiCharge = 14.;

    // Model's parameters
    Double_t r = 0.4, C = dEdx * 1e9;//eV
    Double_t I = 16. * pow (SiCharge, 0.9), Emax = 1e6;//eV
    Double_t f2 = 2. / SiCharge, f1 = 1. - f2;
    Double_t E2 = 10. * SiCharge * SiCharge, E1 = pow (I / pow (E2, f2), 1. / f1);//eV

    //Model's variables' definition
    Double_t sigma1 = C * f1 / E1 * ( log (2.0 * m * beta2 * gamma * gamma / E1) - beta2) /
	( log (2.0 * m * beta2 * gamma * gamma / I) - beta2) * (1.0 - r);
    Double_t sigma2 = C * f2 / E2 * ( log (2.0 * m * beta2 * gamma * gamma / E2) - beta2) /
	( log (2.0 * m * beta2 * gamma * gamma / I) - beta2) * (1.0 - r);
    Double_t sigma3 = C * Emax / (I * (Emax + I) * log( (Emax + I) / I)) * r;


    //   LOG(INFO) << "before " << FairLogger::endl;
    Int_t n1 = ELossRandom -> Poisson(sigma1 * delta);
    //   LOG(INFO) << "n1 = " << n1 << FairLogger::endl;
    Int_t n2 = ELossRandom -> Poisson(sigma2 * delta);
    Int_t n3 = ELossRandom -> Poisson(sigma3 * delta);

    Double_t EnergyLossExit = n1 * E1 + n2 * E2, EnergyLossIon = 0.;

    for (Int_t j = 1; j <= n3; j++){
	Double_t u = ELossRandom -> Uniform(1);
	EnergyLossIon += I / (1 - u * Emax / (Emax + I));
    }

    return EnergyLossIon + EnergyLossExit;//eV
}
// -------------------------------------------------------------------------


// -----   Thermal diffusion    -----------------------------
void CbmStsSensorTypeDssdReal::ThermalSharing (Double_t delta, Double_t * stripCharge, Double_t chargeLayer, 
	Double_t locX1, Double_t locX2, Double_t locY1, Double_t locY2, Double_t locZ1,
	Int_t i1, Int_t i2, Double_t trajLength, Int_t iLayer, Int_t side) const {

    // Stereo angle and strip pitch
    Double_t tanphi = fTanStereo[side];
    Double_t pitch  = fPitch[side];
    Int_t nStrips   = fNofStrips[side];

    Double_t xLayer = locX1 + (iLayer + 0.5) * delta * (locX2 - locX1) / trajLength;
    Double_t yLayer = locY1 + (iLayer + 0.5) * delta * (locY2 - locY1) / trajLength;
    Double_t zLayer = (iLayer + 0.5) * delta * fDz / trajLength;


    Double_t tau, sigmaLayer;
    if (fDiffusion) {
	if ( (side == 0 && locZ1 > 0) || (side == 1 && locZ1 < 0) ) zLayer = fDz - zLayer;  
	tau = fDz * fDz / (2 * fVdepletion) * log ((fVbias + fVdepletion) / (fVbias - fVdepletion) * 
		(1 - 2 * zLayer / fDz * fVdepletion / (fVbias + fVdepletion))); 
	sigmaLayer = fkTq * sqrt(tau); 
    }

    Double_t * stripChargeSh = new Double_t[i2 - i1 + 3];
    memset(stripChargeSh, 0, (i2 - i1 + 3) * sizeof(Double_t) );
    //for (Int_t i = 0; i < (i2 - i1 + 3); i++) stripChargeSh[i] = 0.;
    if ( (xLayer >= 0) && (xLayer <= fDx) ){//layer inside sensor

	for (Int_t iStrip = i1; iStrip <= i2; iStrip++){

	    Int_t iStripShifted = iStrip - i1 + 1;// shift index to avoid negativ index of array 
	    Double_t chargePrev = 0., chargeNext = 0.; 

	    if ((xLayer >= iStrip * pitch + tanphi * (fDy - yLayer)) && 
		    (xLayer < (iStrip + 1) * pitch + tanphi * (fDy - yLayer))) {

		if (!fDiffusion) stripChargeSh[iStripShifted] += chargeLayer; 
		if (fDiffusion){
		    // calculate the coordianate of strip edge, where this layer is
		    Double_t xro = xLayer - (fDy - yLayer) * tanphi;//coordinate on the ro (top) edge of sensor
		    Int_t iro = TMath::FloorNint(xro / pitch);//number of ro strip
		    Double_t edgeLeft = iro * pitch + tanphi * (fDy - yLayer);
		    Double_t edgeRight = (iro + 1) * pitch + tanphi * (fDy - yLayer);

		    /*	    if ( (xLayer - 20 * 1e-4) < iStrip * pitch + tanphi * (fDy - yLayer)) {
			    chargePrev = chargeLayer * 0.5 * (1 + TMath::Erf( (iStrip * pitch + 
			    tanphi * (fDy - yLayer) - xLayer) / (sqrt(2) * sigmaLayer) ));
			    if (edgeLeft >= pitch / 2 && chargePrev > 0) {stripChargeSh[iStripShifted - 1] += chargePrev;
			    LOG(DEBUG4) << "chargePrev = " << chargePrev << FairLogger::endl;
			    }
			    }
			    if ( (xLayer + 20 * 1e-4) > (iStrip + 1) * pitch + tanphi * (fDy - yLayer) ) {
			    chargeNext = chargeLayer * 0.5 * TMath::Erfc( ((iStrip + 1) * pitch + 
			    tanphi * (fDy - yLayer) - xLayer) / (sqrt(2) * sigmaLayer) );
			    if (edgeRight <= fDx - pitch / 2 && chargeNext > 0) {stripChargeSh[iStripShifted + 1] += chargeNext;
			    LOG(DEBUG4) << "chargeNext = " << chargeNext <<  FairLogger::endl;
			    }
			    } */

		    if ( (xLayer - 20 * 1e-4) < edgeLeft) {
			chargePrev = chargeLayer * 0.5 * (1 + TMath::Erf(( edgeLeft - xLayer) / (sqrt(2) * sigmaLayer) ));
			if (edgeLeft >= pitch / 2 && chargePrev > 0) {stripChargeSh[iStripShifted - 1] += chargePrev;
			}
		    }
		    if ( (xLayer + 20 * 1e-4) > edgeRight ) {
			chargeNext = chargeLayer * 0.5 * TMath::Erfc( (edgeRight - xLayer) / (sqrt(2) * sigmaLayer));
			if (edgeRight <= fDx - pitch / 2 && chargeNext > 0) {stripChargeSh[iStripShifted + 1] += chargeNext;
			}
		    } 
		    stripChargeSh[iStripShifted] += chargeLayer - chargePrev - chargeNext;
		}//if fDiffusion
	    }
	}//loop over strips
    }//layer inside sensor
    // move indexes back
    for (Int_t iStripShifted = 0; iStripShifted < (i2 - i1 + 3); iStripShifted ++){
	if (stripChargeSh[iStripShifted] > 0){
	    int iStrip = iStripShifted + i1 - 1;
	    if (iStrip < 0) iStrip += nStrips;
	    if (iStrip >= nStrips) iStrip -= nStrips;
	    stripCharge[iStrip] += stripChargeSh[iStripShifted];
	}
    }
}
// --------------------------------------------------------------------


// -----   Cross-talk   -----------------------------------------------
void CbmStsSensorTypeDssdReal::CrossTalk(Double_t * stripCharge, Double_t * stripChargeCT, 
	Int_t nStrips, Double_t tanphi) const {

    for (Int_t iStrip = 1; iStrip < nStrips - 1; iStrip ++) // loop over all strips except first and last one
	stripChargeCT[iStrip] = stripCharge[iStrip] * (1 - 2 * fCTcoef) + 
	    (stripCharge[iStrip - 1] + stripCharge[iStrip + 1]) * fCTcoef;

    if (tanphi != 0){// there are cross-connections
	stripChargeCT[0] = stripCharge[0] * (1 - 2 * fCTcoef) + 
	    (stripCharge[nStrips - 1] + stripCharge[1]) * fCTcoef;//first strip
	stripChargeCT[nStrips - 1] = stripCharge[nStrips - 1] * (1 - 2 * fCTcoef) + 
	    (stripCharge[0] + stripCharge[nStrips - 2]) * fCTcoef; // last strip
    }
    if (tanphi == 0){// no cross-connections
	stripChargeCT[0]           = stripCharge[0] * (1 - fCTcoef) 
	    + stripCharge[1] * fCTcoef;//first strip
	stripChargeCT[nStrips - 1] = stripCharge[nStrips - 1] * (1 - fCTcoef) 
	    + stripCharge[nStrips - 2] * fCTcoef; // last strip
    }
}
// -------------------------------------------------------------------------


// -----   Stopping power   -----------------------------------------------
Double_t CbmStsSensorTypeDssdReal::StoppingPower(Bool_t fElectron, 
	Double_t mx, Double_t Ex, Int_t zx) const {

    Double_t mp = 0.938, E, dEdx;//[GeV]
    ifstream inFile;
    TString dataDir = gSystem -> Getenv("VMCWORKDIR");
    dataDir += "/sts/digitize";

    //calculate dEdx
    if (fElectron){ 
    	E = Ex;
	if (E <= 10){//read from electron table, we have table only till E = 10 GeV
	    LOG(DEBUG2) << "it is ELECTRON" << FairLogger::endl;
	    inFile.open(dataDir + "/dEdx_Si_e.txt");
	    if(inFile.is_open()){
		LOG(DEBUG2) << "opened file" << FairLogger::endl;
		Double_t Etemp = 0., dEdxtemp = 0.;
		while (Etemp <= E){
		    inFile >> Etemp;//E, Mev
		    inFile >> dEdxtemp;//dEdx, MeV/...
		    Etemp *= 1e-3;
		}
		LOG(DEBUG4) << "dE/dx =  " << dEdxtemp << " MeV/..." <<  Etemp << FairLogger::endl;
		dEdx = dEdxtemp * 1e-3 * 2.33;
		inFile.close();
	    }else{
		LOG(ERROR) << "File with dE/dx for electrons WAS NOT opened." << FairLogger::endl;}

	} else {
	    dEdx = 100e-3 * 2.33;// convert to MeV/cm, rho(Si) = 2.33 g/cm^2 
	}
    } else {//read from proton table
	E = Ex * mp / mx;
	if (E >= 10) {//for energy >= 10 GeV assume dEdx to be const
	    dEdx = zx * zx * 2e-3 * 2.33;// convert to MeV/cm, rho(Si) = 2.33 g/cm^2 
	} else {
	    inFile.open(dataDir + "/dEdx_Si_p.txt");
	    if(inFile.is_open()){
		Double_t Etemp = 0., dEdxtemp = 0.;
		//int counter = 0;
		while (Etemp <= E){
		    inFile >> Etemp;//E, Mev
		    inFile >> dEdxtemp;//dEdx, MeV/...
		    Etemp *= 1e-3;
		    //	counter++;
		}
		LOG(DEBUG4) << "dE/dx =  " << dEdxtemp << " MeV/..." <<  Etemp << "  " << E << FairLogger::endl;
		dEdx = dEdxtemp * 1e-3 * zx * zx * 2.33; // convert to MeV/cm, rho(Si) = 2.33 g/cm^2
		inFile.close();
	    }else{
		LOG(ERROR) << "File with dE/dx for protons WAS NOT opened." << FairLogger::endl;
	    }
	}
    }
    return dEdx;
}
// -------------------------------------------------------------------------


// -----   Set the parameters   --------------------------------------------
void CbmStsSensorTypeDssdReal::SetParameters(Double_t dx, Double_t dy,
	Double_t dz, Int_t nStripsF,
	Int_t nStripsB, Double_t stereoF,
	Double_t stereoB,
	Double_t Vdep, Double_t Vbias, Double_t temperature,
	Double_t Ccoup, Double_t Cinter) {

    // --- Set members
    fVdepletion   = Vdep;
    fVbias        = Vbias;
    fTemperature  = temperature;
    fCcoup        = Ccoup;
    fCinter       = Cinter;
    fkTq = sqrt (2. * temperature * 1.38e-4 / 1.6);
    fCTcoef = Cinter / (Cinter + Ccoup);
    CbmStsSensorTypeDssd::SetParameters(dx, dy, dz, nStripsF, nStripsB, stereoF, stereoB);   
}
// -------------------------------------------------------------------------



ClassImp(CbmStsSensorTypeDssdReal)
