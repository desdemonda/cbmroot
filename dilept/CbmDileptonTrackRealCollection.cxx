/****************************************************************************
 *  $Id: CbmDileptonTrackRealCollection.cxx,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrackRealCollection
 *  Descripton: This is the container to hold the data class
 *              CbmDileptonTrackReal.
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrackRealCollection.cxx,v $
 *****************************************************************************/

#include "CbmDileptonTrackRealCollection.h"

#include "CbmDileptonTrackReal.h"

#include "TClonesArray.h"
#include "TVector3.h"

ClassImp(CbmDileptonTrackRealCollection)

CbmDileptonTrackRealCollection::CbmDileptonTrackRealCollection()
: TObject(),
  fNTracks(0),
  fTracks(new TClonesArray("CbmDileptonTrackReal",1000))
{
}

CbmDileptonTrackRealCollection::~CbmDileptonTrackRealCollection()
{
  deleteTrack();
}

void CbmDileptonTrackRealCollection::addTrack(Int_t iGlobal, Short_t charge, TVector3 m, Double_t chi, Bool_t b1, Bool_t b2, Bool_t b3)
{

  TClonesArray& clref = *fTracks;
  new ( clref[clref.GetEntriesFast()] ) CbmDileptonTrackReal(iGlobal,charge,m,chi,b1,b2,b3);
  fNTracks++;
}


void CbmDileptonTrackRealCollection::deleteTrack()
{
  Option_t* opt = "ALL";
  fTracks->Delete(opt);
}

void CbmDileptonTrackRealCollection::clearTrack()
{
  Option_t* opt = "ALL";
  fTracks->Clear(opt);
}
