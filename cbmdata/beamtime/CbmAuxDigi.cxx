/** @file CbmAuxDigi.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 12.02.2014
 **/

#include "CbmAuxDigi.h"


// --- Default constructor   ------------------------------------------------
CbmAuxDigi::CbmAuxDigi()
	: CbmDigi(), fRocId(0), fChannel(0), fTime(0) {
}
// --------------------------------------------------------------------------



// --- Standard constructor   -----------------------------------------------
CbmAuxDigi::CbmAuxDigi(UShort_t rocId, UShort_t channel, ULong64_t time)
	: CbmDigi(), fRocId(rocId), fChannel(channel), fTime(time) {
}
// --------------------------------------------------------------------------


ClassImp(CbmAuxDigi)

