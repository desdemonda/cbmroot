/**
 * \file CbmLink.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 **/

#include "CbmLink.h"

#include <sstream>

using std::stringstream;

CbmLink::CbmLink() :
 TObject(),
 fFile(-1),
 fEntry(-1),
 fIndex(-1),
 fWeight(-1)
{

}

CbmLink::CbmLink(Float_t weight, Int_t index, Int_t entry, Int_t file) :
      TObject(),
      fFile(file),
      fEntry(entry),
      fIndex(index),
      fWeight(weight)
{

}

CbmLink::~CbmLink()
{

}

string CbmLink::ToString() const
{
   stringstream ss;
   ss << "CbmLink: weight=" << fWeight << " index=" << fIndex << " entry="
         << fEntry << " file=" << fFile << "\n";
   return ss.str();
}

ClassImp(CbmLink)
