/*
 * CbmMCResult.cpp
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#include "CbmMCResult.h"

ClassImp(CbmMCResult);

CbmMCResult::CbmMCResult() 
  : CbmMCObject(),
    fStartType(Cbm::kUnknown),
    fStopType(Cbm::kUnknown)
{
}

CbmMCResult::~CbmMCResult() {
}

CbmMCResult::CbmMCResult(Cbm::DataType start, Cbm::DataType stop)
  : CbmMCObject(),
    fStartType(start), 
    fStopType(stop)
{
}

