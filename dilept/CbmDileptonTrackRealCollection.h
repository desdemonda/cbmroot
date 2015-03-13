/****************************************************************************
 *  $Id: CbmDileptonTrackRealCollection.h,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrackRealCollection
 *  Descripton: This is the container to hold the data class
 *              CbmDileptonTrackReal.
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrackRealCollection.h,v $
 *****************************************************************************/

#ifndef CBM_DILEPTON_TRACK_REAL_COLLECTION_H
#define CBM_DILEPTON_TRACK_REAL_COLLECTION_H

#include "TObject.h"

class CbmDileptonTrackReal;

class TClonesArray;
class TVector3;

class CbmDileptonTrackRealCollection : public TObject{
  
 private:
  
  Int_t fNTracks;
  TClonesArray*  fTracks;
  
 public:
  
  CbmDileptonTrackRealCollection();
  virtual ~CbmDileptonTrackRealCollection();


  Int_t     GetNoOfTracks() const;
  TClonesArray* DileptonTracksReal();

  virtual void addTrack(Int_t, Short_t, TVector3, Double_t, Bool_t, Bool_t, Bool_t);
  virtual void deleteTrack();
  virtual void clearTrack();

  CbmDileptonTrackRealCollection(const CbmDileptonTrackRealCollection&);
  CbmDileptonTrackRealCollection& operator=(const CbmDileptonTrackRealCollection&);

  ClassDef(CbmDileptonTrackRealCollection,1)

    };
    
inline Int_t CbmDileptonTrackRealCollection::GetNoOfTracks() const {return fNTracks;}
inline  TClonesArray* CbmDileptonTrackRealCollection::DileptonTracksReal() {return fTracks;}
#endif
    
