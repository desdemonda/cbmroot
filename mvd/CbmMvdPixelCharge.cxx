// -----------------------------------------------------------------------
// -----               CbmMvdDigi source file                        -----
// -----              Created 17/04/08  by C. Dritsa                 -----
// -----------------------------------------------------------------------

#include <iostream>
#include "CbmMvdPixelCharge.h"


using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdPixelCharge::CbmMvdPixelCharge()
  : TObject(),
    fFrame(-1),
    fCharge(-1.),
    fMaxChargeContribution(0.),
    fDominatingPointX(-1.),
    fDominatingPointY(-1.),
    fContributors(0.),
    fChannelNrX(0.),
    fChannelNrY(0.),
    fTrackCharge(0.),
    fDominatorTrackId(-1.),
    fDominatorPointId(-1.),
    fTrackId(),
    fPointId(),
    fPointX(),
    fPointY(),
    fDominatorIndex(0),
    fPixelTime(-1.),
    fPointWeight()
{
    for(Int_t i=0;i<5;i++){
      fTrackId[i]=-1;
      fPointId[i]=-1;
      fPointX[i]=0;
      fPointY[i]=0;
    }
}
// -------------------------------------------------------------------------
Bool_t CbmMvdPixelCharge::TestXY(Int_t channelNrX,Int_t channelNrY)
{

    if ( (channelNrX==fChannelNrX) && (channelNrY==fChannelNrY))
    {
	return 1;
    }
    else {
	return 0;
    };
}

// -----   Constructor with parameters   -----------------------------------
CbmMvdPixelCharge::CbmMvdPixelCharge(Float_t charge, Int_t channelNrX, Int_t channelNrY, Int_t pointId, Int_t trackId,
				     Float_t pointPosX, Float_t pointPosY, Float_t time, Int_t frame)
  : TObject(),
    fFrame(frame),
    fCharge(0.),
    fMaxChargeContribution(0.),
    fDominatingPointX(-1.),
    fDominatingPointY(-1.),
    fContributors(0.),
    fChannelNrX(channelNrX),
    fChannelNrY(channelNrY),
    fTrackCharge(charge),
    fDominatorTrackId(-1.),
    fDominatorPointId(-1.),
    fTrackId(),
    fPointId(),
    fPointX(),
    fPointY(),
    fDominatorIndex(0),
    fPixelTime(time),
    fPointWeight()
{
    for(Int_t i=0;i<5;i++){
      fTrackId[i]=-1;
      fPointId[i]=-1;
      fPointX[i]=0;
      fPointY[i]=0;
      fPointWeight[i]=0;
    }
    
    fTrackId[0]   = trackId;
    fPointId[0]	  = pointId;
    fPointX[0]	  = pointPosX;
    fPointY[0]	  = pointPosY;
    fPointWeight[0] = charge;
}

// ------- DigestCharge ----------------------------------------------------#

  // Sums up the charge stored in track charge (assuming this is the summed charge from 
  // all segments of a track). Checks if a new track contributed charge to the pixel
  // Checks if the new track is dominant
  
void CbmMvdPixelCharge::DigestCharge(Float_t pointX, Float_t pointY, Int_t pointId, Int_t trackId)
{

    if (fTrackCharge>0)
    {
	if (fTrackCharge>fMaxChargeContribution)
	{
	    fDominatorIndex	   = fContributors;
	    fMaxChargeContribution = fTrackCharge;
	}

	fCharge = fCharge+fTrackCharge; // Add charge of the track
	fTrackCharge = 0;
	fTrackId[fContributors]=trackId;
	fPointId[fContributors]=pointId;
	fPointX [fContributors]=pointX;
	fPointY [fContributors]=pointY;
	fPointWeight[fContributors]=fTrackCharge;
	fContributors = fContributors+1;
	
    }
}

// -----   Destructor   ----------------------------------------------------
CbmMvdPixelCharge::~CbmMvdPixelCharge(){
}
// -------------------------------------------------------------------------

ClassImp(CbmMvdPixelCharge)
