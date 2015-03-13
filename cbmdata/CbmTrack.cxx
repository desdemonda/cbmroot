// -------------------------------------------------------------------------
// -----                        CbmTrack source file                   -----
// -----                  Created 29/11/07  by V. Friese               -----
// -----                  Modified 26/05/09  by A. Lebedev             -----
// -------------------------------------------------------------------------

#include "CbmTrack.h"

#include <sstream>
using std::stringstream;
using std::endl;

CbmTrack::CbmTrack() :
    TObject(),
    fHitIndex(),
    fHitType(),
    fPidHypo(0),
    fParamFirst(),
    fParamLast(),
    fFlag(0),
    fChiSq(0.),
    fNDF(0),
    fPreviousTrackId(-1),
    fMatch(NULL)
{
}

// Only shallow copy needed
CbmTrack::CbmTrack(const CbmTrack& rhs)
 : TObject(rhs),
   fHitIndex(rhs.fHitIndex),
   fHitType(rhs.fHitType),
   fPidHypo(rhs.fPidHypo),
   fParamFirst(rhs.fParamFirst),
   fParamLast(rhs.fParamLast),
   fFlag(rhs.fFlag),
   fChiSq(rhs.fChiSq),
   fNDF(rhs.fNDF),
   fPreviousTrackId(rhs.fPreviousTrackId),
   fMatch(NULL)
{
}

// Only shallow copy needed
CbmTrack& CbmTrack::operator=(const CbmTrack& rhs)
{

  if (this != &rhs) {
    TObject::operator=(rhs);
    fHitIndex = rhs.fHitIndex;
    fHitType = rhs.fHitType;
    fPidHypo = rhs.fPidHypo;
    fParamFirst = rhs.fParamFirst;
    fParamLast = rhs.fParamLast;
    fFlag = rhs.fFlag;
    fChiSq = rhs.fChiSq;
    fNDF = rhs.fNDF;
    fPreviousTrackId = rhs.fPreviousTrackId;
    fMatch = rhs.fMatch;
  }
  return *this;
}

CbmTrack::~CbmTrack()
{
}

void CbmTrack::AddHit(
		Int_t index,
		HitType type)
{
	fHitIndex.push_back(index);
	fHitType.push_back(type);
}

string CbmTrack::ToString() const
{
   stringstream ss;
	ss << "CbmTrack: nof hits=" << fHitIndex.size() << ", chiSq=" << fChiSq
		<< ", NDF=" << fNDF << ", pidHypo=" << fPidHypo
		<< ", previousTrackId=" << fPreviousTrackId << ", flag=" << fFlag << "\n";
//	fParamFirst.Print();
//	fParamLast.Print();
	return ss.str();
}

ClassImp(CbmTrack);
