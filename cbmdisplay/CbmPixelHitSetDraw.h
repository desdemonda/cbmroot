/*
 * CbmMCPointsDraw.h
 *
 *  Created on: Sep. 30, 2009
 *      Author: stockman
 */

#ifndef CBMPIXELHITSETDRAW_H_
#define CBMPIXELHITSETDRAW_H_

#include "FairPointSetDraw.h"           // for FairPointSetDraw
#include "Rtypes.h"                     // for CbmPixelHitSetDraw::Class, etc

class TVector3;
class TObject;

class CbmPixelHitSetDraw: public FairPointSetDraw
{
public:
	CbmPixelHitSetDraw();
	CbmPixelHitSetDraw(const char* name, Color_t color ,Style_t mstyle, Int_t iVerbose = 1):FairPointSetDraw(name, color, mstyle, iVerbose){};
	virtual ~CbmPixelHitSetDraw();

protected:
	TVector3 GetVector(TObject* obj);

	ClassDef(CbmPixelHitSetDraw,1);
};

#endif /* CBMPIXELHITSETDRAW_H_ */
