/*
 * CbmStsPhysics.cxx
 *
 *  Created on: 02.12.2014
 *      Author: vfriese
 */

#include <fstream>
#include <iomanip>
#include "CbmStsPhysics.h"
#include "TMath.h"
#include "TRandom.h"
#include "TSystem.h"
#include "FairLogger.h"


using std::ifstream;
using std::right;
using std::setw;


// -----   Initialisation of static instance   -----------------------------
CbmStsPhysics* CbmStsPhysics::fgInstance = NULL;
// -------------------------------------------------------------------------


const Double_t CbmStsPhysics::fgkSiCharge = 14.;
const Double_t CbmStsPhysics::fgkR = 0.4;
const Double_t CbmStsPhysics::fgkEmax = 1.e6;
const Double_t CbmStsPhysics::fgI = 16. * TMath::Power(fgkSiCharge, 0.9);
const Double_t CbmStsPhysics::fgF1 = 1. - 2. / fgkSiCharge;
const Double_t CbmStsPhysics::fgF2 = 2. / fgkSiCharge;
const Double_t CbmStsPhysics::fgE2 = 10. * fgkSiCharge * fgkSiCharge;
const Double_t CbmStsPhysics::fgE1 = TMath::Power(fgI / TMath::Power(fgE2, fgF2), 1./fgF1);




// -----   Constructor   ---------------------------------------------------
CbmStsPhysics::CbmStsPhysics() 
  : fStoppingElectron(),
    fStoppingProton()
{
	LOG(INFO) << "Instantiating STS Physics... " << FairLogger::endl;
	ReadDataTables();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsPhysics::~CbmStsPhysics() {
}
// -------------------------------------------------------------------------



// ----- Get static instance   ---------------------------------------------
CbmStsPhysics* CbmStsPhysics::Instance() {
	  if ( ! fgInstance ) fgInstance = new CbmStsPhysics();
	  return fgInstance;
}
// -------------------------------------------------------------------------



// -----   Read data tables for stopping power   ---------------------------
void CbmStsPhysics::ReadDataTables() {

	TString dir = gSystem->Getenv("VMCWORKDIR");
	TString eFileName = dir + "/sts/digitize/dEdx_Si_e.txt";
	TString pFileName = dir + "/sts/digitize/dEdx_Si_p.txt";

	ifstream inFile;
	Double_t e, dedx;

	// --- Read electron stopping power
	inFile.open(eFileName);
	if ( inFile.is_open() ) {
		while (true) {
			inFile >> e;
			inFile >> dedx;
			if ( inFile.eof() ) break;
			e    *= 1.e-3;  // MeV -> GeV
			dedx *= 1.e-3;  // MeV -> GeV
			fStoppingElectron[e] = dedx;
		}
		inFile.close();
		LOG(INFO) << "StsPhysics: " << setw(5) << right
				      << fStoppingElectron.size() << " values read from "
				      << eFileName << FairLogger::endl;
	}
	else
		LOG(FATAL) << "StsPhysics: Could not read from " << eFileName
		           << FairLogger::endl;

	// --- Read proton stopping power
	inFile.open(pFileName);
	if ( inFile.is_open() ) {
		while (true) {
			inFile >> e;
			inFile >> dedx;
			if ( inFile.eof() ) break;
			e    *= 1.e-3;  // MeV -> GeV
			dedx *= 1.e-3;  // MeV -> GeV
			fStoppingProton[e] = dedx;
		}
		inFile.close();
		LOG(INFO) << "StsPhysics: " << setw(5) << right
				      << fStoppingProton.size() << " values read from "
				      << pFileName << FairLogger::endl;
	}
	else
		LOG(FATAL) << "StsPhysics: Could not read from " << pFileName
		           << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   Stopping power   ------------------------------------------------
Double_t CbmStsPhysics::StoppingPower(Double_t energy, Double_t mass,
		                                  Double_t charge, Bool_t isElectron) {

	// --- Calculate equivalent proton energy for particles other than electron
	Double_t eEquiv = energy;
	if ( ! isElectron ) eEquiv = energy * 0.938 / mass;
	Double_t stopPower = -1.;

	// --- For energies > 10 GeV use asymptotic values
	if ( eEquiv > 10. ) {
		if ( isElectron ) stopPower = 100. * 1.e-3;
		else              stopPower =   2. * 1.e-3;
	} //? E > 10 GeV

	// --- For lower energies: get largest value smaller than this energy
	// --- from data tables
	else {
		std::map<Double_t, Double_t>::iterator it;
		if ( isElectron ) it = fStoppingElectron.lower_bound(eEquiv);
		else              it = fStoppingProton.lower_bound(eEquiv);
		stopPower = it->second; // specific stopping power [GeV*g/cm^2]
	} //? E < 10 GeV

	// --- Calculate stopping power (from specific SP and density of silicon)
	stopPower *= 2.33;    // density of silicon is 2.33 g/cm^3

	// --- For non-electrons: scale with squared charge
	if ( ! isElectron ) stopPower *= charge * charge;

	return stopPower;
}
// -------------------------------------------------------------------------



Double_t CbmStsPhysics::EnergyLoss(Double_t dz, Double_t mass, Double_t eKin,
		                               Double_t dedx) {

	// Gamma and beta
	Double_t gamma = (eKin + mass) / mass;
	Double_t beta2 = 1. - 1. / ( gamma * gamma );

	// Normalisation
	Double_t c = dedx * 1.e9;

	// Auxiliary
	Double_t xLog = TMath::Log( 2. * mass * beta2 * gamma * gamma);

	// Mean energy losses
	Double_t sigma1 = c * fgF1 / fgE1 * ( xLog - TMath::Log(fgE1) - beta2 )
			            / ( ( xLog - TMath::Log(fgI) - beta2 ) * (1. - fgkR) );
	Double_t sigma2 = c * fgF2 / fgE2 * ( xLog - TMath::Log(fgE2) - beta2 )
			            / ( ( xLog - TMath::Log(fgI) - beta2 ) * (1. - fgkR) );
	Double_t sigma3 = c * fgkEmax * fgkR /
			( fgI * ( fgkEmax + fgI ) * TMath::Log( (fgkEmax + fgI) / fgI ) );

	// Sample from Poissonian energy loss distribution
	Int_t n1 = gRandom->Poisson( sigma1 * dz );
	Int_t n2 = gRandom->Poisson( sigma2 * dz );
	Int_t n3 = gRandom->Poisson( sigma3 * dz );

	// Ion energy loss
	Double_t eLossIon = 0.;
	for (Int_t j = 1; j <= n3; j++) {
		Double_t uni = gRandom->Uniform(1.);
		eLossIon += fgI / ( 1. - uni * fgkEmax / ( fgkEmax + fgI ) );
	}

	// Total energy loss
	return n1 * fgE1 + n2 * fgE2 + eLossIon;


}

