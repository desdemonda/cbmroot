 /****************************************************************************
 *  
 *  Class : CbmJpsiTrackCollection
 *  Descripton: This is the container to hold the data class
 *              CbmJpsiTrackCollection.
 *
 *  Author: Alla Maevskaya
 *  E-mail: alla@inr.ru
 *****************************************************************************
 *  $Log : CbmJpsiTrackCollection.h ,v $
 ***************************************************************************/

#ifndef CBM_JPSI_TRACK_COLLECTION_H
#define CBM_JPSI_TRACK_COLLECTION_H

#include "TObjArray.h"
#include "TVector3.h"

class CbmJpsiTrackCollection;

class CbmJpsiTrackCollection : public TObject{
   
 public:
  
  CbmJpsiTrackCollection();
  virtual ~CbmJpsiTrackCollection();


  Int_t     GetNoOfTracks() const {return fNTracks;}
  TClonesArray* JpsiTracks() {return fTracks;}

  virtual void AddTrack( TVector3 mom, Int_t pdg, Double_t ann, Double_t like, Double_t wkn);
  virtual void deleteTrack();
  virtual void clearTrack();
 
 private:
  
  Int_t fNTracks;
  TClonesArray*  fTracks;

  ClassDef(CbmJpsiTrackCollection,1)

    };
    
#endif
    
