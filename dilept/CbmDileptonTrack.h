/******************************************************************************
 *  $Id: CbmDileptonTrack.h,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrack
 *  Descripton: This is the base class for identified electrons for 
 *              dilepton pair analysis. Two classes for real data and/or
 *              simulated data will be derived from this.
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrack.h,v $
 *****************************************************************************/

#ifndef CBM_DILEPTON_TRACK_H
#define CBM_DILEPTON_TRACK_H

#include "TVector3.h"
#include "TObject.h"

class CbmDileptonTrack : public TObject{

protected:

    Short_t  fCharge;
    TVector3 fMomentum;

public:

    /** Default constructor **/
    CbmDileptonTrack(); //!constructor

    /** Destructor **/
    virtual ~CbmDileptonTrack(); //!destructor

    /* member functions */

    Short_t  GetCharge() const;   //! Get charge of particle
    TVector3 GetMomentum() const; //! get momentum of particle

    void SetCharge(Short_t);
    void SetMomentum(TVector3);


    ClassDef(CbmDileptonTrack,1);

};

inline Short_t  CbmDileptonTrack::GetCharge() const {return fCharge;}
inline TVector3 CbmDileptonTrack::GetMomentum() const {return fMomentum;}

inline void CbmDileptonTrack::SetCharge(Short_t var){fCharge=var;}
inline void CbmDileptonTrack::SetMomentum(TVector3 var){fMomentum=var;}

#endif
