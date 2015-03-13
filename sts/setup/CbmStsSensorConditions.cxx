/** @file CbmStsSensorConditions.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 28.06.2014
 **/

#include <iomanip>
#include <sstream>
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
		                                           fBx(bX),
		                                           fBy(bY),
		                                           fBz(bZ)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmStsSensorConditions::~CbmStsSensorConditions() { }
// -------------------------------------------------------------------------



// -----   String output   -------------------------------------------------
string CbmStsSensorConditions::ToString() const {
	stringstream ss;
	ss << "VFD = " << fVfd << " V, V(bias) = " << fVbias << " V, T = "
		 << fTemperature << " K, C(coupl.) = " << fCcoupling
		 << " pF, C(int.) = " << fCinterstrip << " pF, B = ("
		 << setprecision(3) << fixed << fBx << ", " << fBy << ", " << fBz
		 << ") T";
	return ss.str();
}
// -------------------------------------------------------------------------


ClassImp(CbmStsSensorConditions)
