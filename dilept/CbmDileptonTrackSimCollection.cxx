/****************************************************************************
 *  $Id: CbmDileptonTrackSimCollection.cxx,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrackSimCollection
 *  Descripton: This is the container to hold the data class
 *              CbmDileptonTrackSim.
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrackSimCollection.cxx,v $
 *****************************************************************************/

#include "CbmDileptonTrackSimCollection.h"

#include "CbmDileptonTrackSim.h"

#include "TClonesArray.h"
#include "TVector3.h"

ClassImp(CbmDileptonTrackSimCollection)

CbmDileptonTrackSimCollection::CbmDileptonTrackSimCollection()
: TObject(),
  fNTracksSim(0),
  fTracksSim(new TClonesArray("CbmDileptonTrackSim",1000))
{
}

CbmDileptonTrackSimCollection::~CbmDileptonTrackSimCollection()
{
  deleteTrack();
}

void CbmDileptonTrackSimCollection::addTrack(TVector3 mom, Bool_t b1, Bool_t b2, Int_t b3, Int_t b4, Int_t b5)
{

  TClonesArray& clref = *fTracksSim;
  new ( clref[clref.GetEntriesFast()] ) CbmDileptonTrackSim(mom,b1,b2,b3,b4,b5);
  fNTracksSim++;
}


void CbmDileptonTrackSimCollection::deleteTrack()
{
  Option_t* opt = "ALL";
  fTracksSim->Delete(opt);
}

void CbmDileptonTrackSimCollection::clearTrack()
{
  Option_t* opt = "ALL";
  fTracksSim->Clear(opt);
}
