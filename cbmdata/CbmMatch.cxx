/**
 * \file CbmMatch.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 **/

#include "CbmMatch.h"

#include <sstream>

using std::make_pair;
using std::stringstream;

CbmMatch::CbmMatch() :
      fLinks(),
      fTotalWeight(0.),
      fMatchedIndex(-1)
{

}

CbmMatch::~CbmMatch()
{

}

string CbmMatch::ToString() const
{
   stringstream ss;
   ss << "CbmMatch: ";
   Int_t nofLinks = GetNofLinks();
   ss << "nofLinks=" << nofLinks << "\n";
   for (Int_t i = 0; i < nofLinks; i++) {
      const CbmLink& link = fLinks[i];
      ss << link.ToString();
   }
   ss << " totalWeight=" << fTotalWeight << ", matchedIndex="
         << fMatchedIndex << std::endl;
   return ss.str();
}

void CbmMatch::AddLinks(const CbmMatch& match)
{
   Int_t nofLinks = match.GetNofLinks();
   for (Int_t i = 0; i < nofLinks; i++) {
      AddLink(match.GetLink(i));
   }
}

void CbmMatch::AddLink(const CbmLink& newLink)
{
   Int_t addedIndex = -1;
   Int_t nofLinks = GetNofLinks();
   for (Int_t i = 0; i < nofLinks; i++) {
      CbmLink& link = fLinks[i];
      if (link == newLink) {
         link.AddWeight(newLink.GetWeight());
         addedIndex = i;
         break;
      }
   }
   if (addedIndex < 0) {
      fLinks.push_back(newLink);
      addedIndex = fLinks.size() - 1;
   }

   fTotalWeight += newLink.GetWeight();
   if (fMatchedIndex < 0) {
      fMatchedIndex = addedIndex;
   } else {
      if (fLinks[addedIndex].GetWeight() > fLinks[fMatchedIndex].GetWeight()) {
         fMatchedIndex = addedIndex;
      }
   }
}

void CbmMatch::AddLink(Double_t weight, Int_t index, Int_t entry, Int_t file) {
	CbmLink link(weight, index, entry, file);
	AddLink(link);
}


void CbmMatch::ClearLinks()
{
   fLinks.clear();
   fTotalWeight = 0.;
   fMatchedIndex = -1;
}

ClassImp(CbmMatch);
