/****************************************************************************
 *  
 *  Class : CbmJpsiTrackCollection
 *  Descripton: This is the container to hold the data class
 *              CbmJpsiTrackCollection.
 *
 *  Author: Alla Maevskaya
 *  E-mail: alla@inr.ru
 *****************************************************************************
 *  $Log : CbmJpsiTrackCollection.cxx,v $
 *****************************************************************************/

#include "CbmJpsiTriggerElectron.h"
#include "CbmJpsiTrackCollection.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include "iostream.h"

ClassImp(CbmJpsiTrackCollection)

CbmJpsiTrackCollection::CbmJpsiTrackCollection()
{
  fNTracks = 0;
  fTracks = new TClonesArray("CbmJpsiTriggerElectron",1000);
}

CbmJpsiTrackCollection::~CbmJpsiTrackCollection()
{
  deleteTrack();
}

void CbmJpsiTrackCollection::AddTrack(TVector3 mom, Int_t pdg, Double_t ann, Double_t like, Double_t wkn)
{

  TClonesArray& clref = *fTracks;
  new ( clref[clref.GetEntriesFast()] ) CbmJpsiTriggerElectron(mom,pdg, ann, like, wkn);
  //  cout<<"CbmJpsiTrackCollection>> "<<clref.GetEntriesFast()<<endl; 
  fNTracks++;
}


void CbmJpsiTrackCollection::deleteTrack()
{
  Option_t* opt = "ALL";
  fTracks->Delete(opt);
}

void CbmJpsiTrackCollection::clearTrack()
{
  Option_t* opt = "ALL";
  fTracks->Clear(opt);
}
