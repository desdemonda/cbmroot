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

// -----   Cross-talk   -----------------------------------------------
void CbmStsSensorTypeDssdReal::CrossTalk(Double_t * stripCharge, Double_t * stripChargeCT, 
	Int_t nStrips, Double_t tanphi, Double_t CTcoef) const {

    for (Int_t iStrip = 1; iStrip < nStrips - 1; iStrip ++) // loop over all strips except first and last one
	stripChargeCT[iStrip] = stripCharge[iStrip] * (1 - 2 * CTcoef) + 
				(stripCharge[iStrip - 1] + stripCharge[iStrip + 1]) * CTcoef;

    if (tanphi != 0){// there are cross-connections
	stripChargeCT[0] = stripCharge[0] * (1 - 2 * CTcoef) + 
			   (stripCharge[nStrips - 1] + stripCharge[1]) * CTcoef;//first strip
	stripChargeCT[nStrips - 1] = stripCharge[nStrips - 1] * (1 - 2 * CTcoef) + 
			    (stripCharge[0] + stripCharge[nStrips - 2]) * CTcoef; // last strip
    }
    if (tanphi == 0){// no cross-connections
	stripChargeCT[0]           = stripCharge[0] * (1 - CTcoef) 
				   + stripCharge[1] * CTcoef;//first strip
	stripChargeCT[nStrips - 1] = stripCharge[nStrips - 1] * (1 - CTcoef) 
		        	    + stripCharge[nStrips - 2] * CTcoef; // last strip
    }
}
// -------------------------------------------------------------------------


// -----   Diffusion and Lorentz shift  ------------------------------------
void CbmStsSensorTypeDssdReal::DiffusionAndLorentzShift (Double_t delta, Double_t * stripCharge, Double_t chargeLayer, 
	Double_t locX1, Double_t locX2, Double_t locY1, Double_t locY2, Double_t locZ1, Double_t locZ2,
	Double_t trajLength, Int_t iLayer, Int_t side, Double_t kTq, Double_t Vdepletion, Double_t Vbias,
	Double_t mField, Double_t * hallMobilityParameters) const {

    // Stereo angle and strip pitch
    Double_t tanphi = fTanStereo[side];
    Double_t pitch  = fPitch[side];
    Int_t nStrips   = fNofStrips[side];

    Double_t yLayer = locY1 + (iLayer + 0.5) * delta * (locY2 - locY1) / trajLength;
    Double_t xLayer = locX1 + (iLayer + 0.5) * delta * (locX2 - locX1) / trajLength;
    Double_t zLayer;

    if (locZ1 < 0) zLayer = fDz - (iLayer + 0.5) * delta * fDz / trajLength;//inverted coordinates
    else zLayer = (iLayer + 0.5) * delta * fDz / trajLength;

    Double_t muHall, muLow, vSat, beta, rHall;

    if (fLorentzShift) {
	Double_t eField = (Vbias - Vdepletion) / fDz + 2 * Vdepletion / fDz * (1 - (fDz - zLayer) / fDz);
	muLow = hallMobilityParameters[0];
	beta  = hallMobilityParameters[1];
	vSat  = hallMobilityParameters[2];
	rHall = hallMobilityParameters[3];
	muHall = rHall * muLow / pow ((1 + pow(muLow * eField / vSat, beta)), 1 / beta);
    }

    Double_t tau, sigmaLayer;
    if (fDiffusion){
	if (side == 0) {
	    tau = fDz * fDz / (2. * Vdepletion) * log ((Vbias + Vdepletion) / (Vbias - Vdepletion) * 
		    (1. - 2. * zLayer / fDz * Vdepletion / (Vbias + Vdepletion)));
	} else {
	    tau = fDz * fDz / (2. * Vdepletion) * log ((Vbias + Vdepletion) / (Vbias - Vdepletion) * 
		    (1. - 2. * (fDz - zLayer) / fDz * Vdepletion / (Vbias + Vdepletion))); 
	}
	sigmaLayer = kTq * sqrt(tau); 
    }
    if (fLorentzShift)
	if (side == 0) xLayer += muHall * mField * (fDz - zLayer) * 1.e-4;
	else           xLayer += muHall * mField * zLayer         * 1.e-4;

    Double_t roX = xLayer - (fDy - yLayer) * tanphi; //read-out coordinate
    Int_t roI = Int_t(roX / pitch); //read-out channel
  
    std::map<Int_t, Double_t> stripChargeMap;
    if ( (xLayer >= 0) && (xLayer <= fDx) ){//layer inside sensor

	if (!fDiffusion) stripChargeMap[roI] = chargeLayer; 
	else {
	    Double_t chargePrev = 0., chargeNext = 0.; 

	    Double_t edgeLeft = roI * pitch + tanphi * (fDy - yLayer);
	    Double_t edgeRight = (roI + 1) * pitch + tanphi * (fDy - yLayer);

	    if ( (xLayer - 20. * 1e-4) < edgeLeft) {
		chargePrev = chargeLayer * 0.5 * (1. + TMath::Erf(( edgeLeft - xLayer) / (sqrt(2.) * sigmaLayer) ));
		if (edgeLeft >= pitch / 2. && chargePrev > 0) stripChargeMap[roI - 1] = chargePrev;
	    }
	    if ( (xLayer + 20. * 1e-4) > edgeRight ) {
		chargeNext = chargeLayer * 0.5 * TMath::Erfc( (edgeRight - xLayer) / (sqrt(2.) * sigmaLayer));
		if (edgeRight <= fDx - pitch / 2. && chargeNext > 0) stripChargeMap[roI + 1] = chargeNext;
	    } 
	    stripChargeMap[roI] = chargeLayer - chargePrev - chargeNext;
	}//if fDiffusion
    }//layer inside sensor

    for (Int_t iStrip = roI - 1; iStrip <= roI + 1 ; iStrip ++){
	if (stripChargeMap[iStrip] > 0){
	    Int_t iStripReal = iStrip;
	    if (iStrip < 0) iStrip += nStrips;
	    if (iStrip >= nStrips) iStrip -= nStrips;
	    stripCharge[iStrip] += stripChargeMap[iStrip];
	}
    }
}
// --------------------------------------------------------------------


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
    LOG(DEBUG4) << GetName() << " dssdreal : processing sensor point at ( "
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
    Double_t qtot = point->GetELoss() / kPairEnergy;//in e
   if (side == 0) LOG(DEBUG4) << "Total produced charge (from Geant) =  " << 
	point -> GetELoss() * 1.e6 << " keV" << FairLogger::endl;

    // Stereo angle and strip pitch
    Double_t tanphi = fTanStereo[side];
    Double_t pitch  = fPitch[side];
    Int_t nStrips   = fNofStrips[side];

    // Debug output
    LOG(DEBUG4) << GetName() << ": Side " << side << ", dx = " << fDx
	<< " cm, dy = " << fDy << " cm, dz = " << fDz << ", stereo " << fStereo[side]
	<< " degrees, strips " << fNofStrips[side] << ", pitch "
	<< pitch << " mum" << FairLogger::endl;

    //coordinates of point in local coordinate system of sensor
    Double_t locX1 = point->GetX1() + 0.5 * fDx;
    Double_t locX2 = point->GetX2() + 0.5 * fDx;
    Double_t locY1 = point->GetY1() + 0.5 * fDy;
    Double_t locY2 = point->GetY2() + 0.5 * fDy;
    Double_t locZ1 = point->GetZ1();
    Double_t locZ2 = point->GetZ2();
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

    // --- More than one strip: sort strips
    if ( i1 > i2 ) {
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
     LOG(DEBUG4) << GetName() << ": Coordinates are inverted." << FairLogger::endl;
    }

    Double_t * stripCharge = new Double_t[nStrips]; 
    memset(stripCharge, 0, nStrips * sizeof(Double_t) );
    Double_t * stripChargeCT;
    Double_t totalLossCharge = 0.;

    // thickness of trajectory layer in cm, should be > 2 mum and < 10 mum. 
    // The trajectory is divided on whole number of steps with step length about 3 mum 
    Double_t delta = trajLength / Int_t(trajLength / 3.e-4);
    LOG(DEBUG4) << GetName() << ": delta = " << delta << " cm, trajLength  = " 
	<< trajLength << " cm" << FairLogger::endl;

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
    Double_t dedx = fPhysics->StoppingPower(eKin, mass, charge, isElectron);//in GeV/cm

    LOG(DEBUG3) << GetName() << ": PID " << point -> GetPid() << ", mass "
	<< mass << " GeV, charge " << charge << ", Ekin " << eKin
	<< " GeV, from table <dE/dx> = " << dedx << " GeV/cm ~= " << 1.e6* dedx / 33 
	<< " keV / 300 um"<< FairLogger::endl;


    Double_t chargeLayerUni = qtot / (Int_t (trajLength / delta));
    //calculate some value for thermal diffusion
    Double_t kTq = sqrt (2. * sensor -> GetConditions().GetTemperatur() * 1.38e-4 / 1.6);
    Double_t Vdepletion = sensor -> GetConditions().GetVfd();
    Double_t Vbias = sensor -> GetConditions().GetVbias();
    //Double_t mField = sensor -> GetConditions().GetBy();

    Double_t hallMobilityParameters[4];
    sensor -> GetConditions().GetHallMobilityParametersInto(hallMobilityParameters, side);
    LOG(DEBUG4) << GetName() << ": muLow = " << hallMobilityParameters[0] 
	<< ", beta = " << hallMobilityParameters[1] << ", vSat = " << hallMobilityParameters[2] 
	<< ", rHall = " << hallMobilityParameters[3] 
	<< FairLogger::endl;
    
    Double_t mField = point -> GetBy();

    for (Int_t iLayer = 0; iLayer < Int_t(trajLength / delta); iLayer ++) {
	if (side == 0){
	    // energy loss calculation in eV; m, Ekin - in GeV, dEdx - in GeV/cm
	    if (fNonUniformity) { 
		Double_t lossInLayer = fPhysics -> EnergyLoss(delta, mass, eKin, dedx); //eV
		ELossLayerArray.push_back(lossInLayer);
	    }
	    else ELossLayerArray.push_back(chargeLayerUni);
	}

	if (fNonUniformity)totalLossCharge += ELossLayerArray[iLayer];// in eV 
	// thermal diffusion calculation, result in eV
	DiffusionAndLorentzShift(delta, stripCharge, ELossLayerArray[iLayer], 
		locX1, locX2, locY1, locY2, locZ1, locZ2, 
		trajLength, iLayer, side, kTq, Vdepletion, Vbias,
		mField, hallMobilityParameters); 
    }
    if (!fNonUniformity)totalLossCharge = qtot;
    LOG(DEBUG4) << GetName() << ": qtot = " << qtot << " e, totalProducedCharge = " 
	<< totalLossCharge << " eV" << FairLogger::endl;

    for (Int_t iStrip = 0; iStrip < nStrips; iStrip++) {
	if (stripCharge[iStrip] > 0) LOG(DEBUG4) << GetName() << ": charge after thermal diffusion at strip#" 
	    << iStrip << " = " << stripCharge[iStrip] << FairLogger::endl;
    }
    if (fCrossTalk) {
	stripChargeCT = new Double_t[nStrips];
	memset(stripChargeCT, 0, nStrips * sizeof(Double_t) );
	CrossTalk (stripCharge, stripChargeCT, nStrips, tanphi);
    }
    else stripChargeCT = stripCharge;

    // --- Loop over fired strips
    Int_t nSignals = 0;
    for (Int_t iStrip = 0; iStrip < nStrips; iStrip++) {
	if (stripChargeCT[iStrip] > 0){
	    // --- Calculate charge in strip
	    // normalize registered charge to the total charge from simulation - qtot
	    Double_t chargeNorm = stripChargeCT[iStrip] * qtot / totalLossCharge;// in e
	    LOG(DEBUG4) << GetName() << ": charge at strip#" << iStrip << " = " << stripChargeCT[iStrip] 
		<< ", after norm (*qtot/totalProducedCharge) = " << chargeNorm << FairLogger::endl;
	    // --- Register charge to module
	    RegisterCharge(sensor, side, iStrip, chargeNorm,
		    (point -> GetTime()));
	    nSignals++;
	}
    } // Loop over fired strips

    //deleting pointers
    delete stripCharge;
    delete stripChargeCT;
    
    return nSignals;
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
