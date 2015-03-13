/*
 * CbmMCPointsDraw.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: stockman
 */

#include "CbmPixelHitSetDraw.h"

#include "CbmPixelHit.h"                // for CbmPixelHit

#include "Riosfwd.h"                    // for ostream
#include "TVector3.h"                   // for TVector3

#include <iosfwd>                       // for ostream
#include <iostream>                     // for cout
#include <ostream>                      // for operator<<, basic_ostream, etc

using std::cout;
using std::endl;

CbmPixelHitSetDraw::CbmPixelHitSetDraw()
{
	// TODO Auto-generated constructor stub

}

CbmPixelHitSetDraw::~CbmPixelHitSetDraw()
{
	// TODO Auto-generated destructor stub
}

TVector3 CbmPixelHitSetDraw::GetVector(TObject* obj)
{
	CbmPixelHit* p = (CbmPixelHit*)obj;
	if (fVerbose > 1)
		cout << "-I- CbmPixelHitSetDraw::GetVector: " << p->GetX() << " " << p->GetY() << " " << p->GetZ() << endl;
	return TVector3(p->GetX(), p->GetY(), p->GetZ());
}


ClassImp(CbmPixelHitSetDraw)
