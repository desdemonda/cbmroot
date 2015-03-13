/****************************************************************************
 *  $Id: CbmDileptonTrackSimCollection.h,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrackSimCollection
 *  Descripton: This is the container to hold the data class 
 *              CbmDileptonTrackSim. 
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrackSimCollection.h,v $
 *****************************************************************************/

#ifndef CBM_DILEPTON_TRACK_SIM_COLLECTION_H
#define CBM_DILEPTON_TRACK_SIM_COLLECTION_H

#include "TObject.h"

class CbmDileptonTrackSim;

class TClonesArray;
class TVector3;

class CbmDileptonTrackSimCollection : public TObject{
  
 private:
  
  Int_t fNTracksSim;
  TClonesArray*  fTracksSim;
  
  CbmDileptonTrackSimCollection(const CbmDileptonTrackSimCollection&);
  CbmDileptonTrackSimCollection& operator=(const CbmDileptonTrackSimCollection&);

 public:
  
  CbmDileptonTrackSimCollection();
  virtual ~CbmDileptonTrackSimCollection();


  Int_t     GetNoOfTracks() const;
  TClonesArray* DileptonTracksSim();

  virtual void addTrack(TVector3, Bool_t, Bool_t, Int_t, Int_t, Int_t);
  virtual void deleteTrack();
  virtual void clearTrack();

  ClassDef(CbmDileptonTrackSimCollection,1)

    };
    
inline Int_t CbmDileptonTrackSimCollection::GetNoOfTracks() const {return fNTracksSim;}
inline  TClonesArray* CbmDileptonTrackSimCollection::DileptonTracksSim() {return fTracksSim;}
#endif
    
