/** @file CbmStsSensorConditions.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 28.06.2014
 **/

#include <iomanip>
#include <sstream>
#include "FairLogger.h"
#include "CbmStsSensorConditions.h"

using namespace std;


// -----   Default constructor   -------------------------------------------
CbmStsSensorConditions::CbmStsSensorConditions(Double_t vFd,
		                                           Double_t vBias,
		                                           Double_t temperature,
		                                           Double_t cCoupling,
		                                           Double_t cInterstrip,
		                                           Double_t bX,
		                                           Double_t bY,
		                                           Double_t bZ) :
		                                           TObject(),
		                                           fVfd(vFd),
		                                           fVbias(vBias),
		                                           fTemperature(temperature),
		                                           fCcoupling(cCoupling),
		                                           fCinterstrip(cInterstrip),
		                                           fCrossTalk(0.),
		                                           fBx(bX),
		                                           fBy(bY),
		                                           fBz(bZ)
{
	if ( fCinterstrip + fCcoupling != 0. )
		fCrossTalk = cInterstrip / (cInterstrip + cCoupling);
	SetHallMobilityParameters();
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmStsSensorConditions::~CbmStsSensorConditions() { }
// -------------------------------------------------------------------------

// -----   Set parameters for Hall mobility calculation   ------------------
void CbmStsSensorConditions::SetHallMobilityParameters() {

    Double_t muLow[2], vSat[2], beta[2], rHall[2], muHall[2];

    // electrons
    muLow[0] = 1417.  * pow (fTemperature / 300., -2.2);// cm^2 / (V s)
    beta[0]  = 1.109  * pow (fTemperature / 300., 0.66);
    vSat[0]  = 1.07e7 * pow (fTemperature / 300., 0.87);// cm / s 
    rHall[0] = 1.15;

    fHallMobilityParametersE[0] = muLow[0];
    fHallMobilityParametersE[1] = beta[0];
    fHallMobilityParametersE[2] = vSat[0];
    fHallMobilityParametersE[3] = rHall[0];

    // holes
    muLow[1] = 470.5   * pow (fTemperature / 300., -2.5);// cm^2 / (V s)
    beta[1]  = 1.213   * pow (fTemperature / 300., 0.17);
    vSat[1]  = 0.837e7 * pow (fTemperature / 300., 0.52);// cm / s 
    rHall[1] = 0.7;

    fHallMobilityParametersH[0] = muLow[1];
    fHallMobilityParametersH[1] = beta[1];
    fHallMobilityParametersH[2] = vSat[1];
    fHallMobilityParametersH[3] = rHall[1];

    //calculate mean shift
    Double_t dZ = 0.03, E = (fVbias - fVfd) / dZ + 2 * fVfd / dZ;//dZ - sensor thickness, E - el field [V/cm]
    Int_t nSteps = 1000;
    Double_t deltaZ = dZ / nSteps; 
    Double_t dxMean[2];
    dxMean[0] = dxMean[1] = 0.;

    for (Int_t j = 0; j <= nSteps; j++){
	E -= 2 * fVfd / dZ * deltaZ / dZ;//V / cm
	for (Int_t i = 0; i < 2; i++){
	    muHall[i] = rHall[i] * muLow[i] / pow ((1 + pow (muLow[i] * E / vSat[i], beta[i])), 1 / beta[i]);
	    if (i == 1) dxMean[i] += muHall[i] * j * deltaZ; 
	    if (i == 0) dxMean[i] += muHall[i] * (dZ - j * deltaZ); 
	}
    }
    for (Int_t i = 0; i < 2; i++) dxMean[i] /= nSteps;
     fMeanLorentzShift[0] = dxMean[0] * fBy * 1.e-4;
     fMeanLorentzShift[1] = dxMean[1] * fBy * 1.e-4;

}
// -------------------------------------------------------------------------


// -----   Get parameters for Hall mobility calculation into array  --------
void CbmStsSensorConditions::GetHallMobilityParametersInto(Double_t * hallMobilityParameters, Int_t chargeType) const {

   if (chargeType == 0) { // electrons
       for (Int_t i = 0; i < 4; i++) hallMobilityParameters[i] = fHallMobilityParametersE[i];

   } else if (chargeType == 1) { // holes
       for (Int_t i = 0; i < 4; i++) hallMobilityParameters[i] = fHallMobilityParametersH[i];

   } else LOG(ERROR) << GetName() << "Cannot get parameter for Hall mobility. Unknown type of charge carriers. Must be 0 or 1!" << FairLogger::endl;

}
// -------------------------------------------------------------------------




// -----   String output   -------------------------------------------------
string CbmStsSensorConditions::ToString() const {
	stringstream ss;
	ss << "VFD = " << fVfd << " V, V(bias) = " << fVbias << " V, T = "
		 << fTemperature << " K, C(coupl.) = " << fCcoupling
		 << " pF, C(int.) = " << fCinterstrip
		 << " pF, cross-talk coeff. = " << fCrossTalk <<  "B = ("
		 << setprecision(3) << fixed << fBx << ", " << fBy << ", " << fBz
		 << ") T";
	return ss.str();
}
// -------------------------------------------------------------------------


ClassImp(CbmStsSensorConditions)
