/******************************************************************************
 *  $Id: CbmDileptonTrackReal.h,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrackReal
 *  Descripton: This is the class for identified electrons for real data stream.
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrackReal.h,v $
 *****************************************************************************/

#ifndef CBM_DILEPTON_TRACK_REAL_H
#define CBM_DILEPTON_TRACK_REAL_H

#include "CbmDileptonTrack.h"

class TVector3;

class CbmDileptonTrackReal : public CbmDileptonTrack{

private:

    Int_t  indexGlobal; //Index of the global track
    Double_t fChiPrimary;
   // Bool_t isSTS;
    Bool_t isRICH;
    Bool_t isTRD;
    Bool_t isTOF;

public:

    /** Constructor **/
    CbmDileptonTrackReal();
    CbmDileptonTrackReal(Int_t, Short_t, TVector3, Double_t, Bool_t, Bool_t, Bool_t); //!constructor

    /** Destructor **/
    virtual ~CbmDileptonTrackReal();

    /** Accessors **/

    Int_t  GetGlobalIndex() const;
    Double_t GetChiPrimary() const;
    Bool_t GetRich() const;
    Bool_t GetTrd() const;
    Bool_t GetTof() const;

    /** Modifiers **/

    void SetGlobalIndex(Int_t);
    void SetRich(Bool_t);
    void SetTrd(Bool_t);
    void SetTof(Bool_t);

    ClassDef(CbmDileptonTrackReal,1);

};

inline Int_t    CbmDileptonTrackReal::GetGlobalIndex() const {return indexGlobal;}
inline Double_t CbmDileptonTrackReal::GetChiPrimary() const {return fChiPrimary;}
inline Bool_t   CbmDileptonTrackReal::GetRich() const {return isRICH;}
inline Bool_t   CbmDileptonTrackReal::GetTrd()const {return isTRD;}
inline Bool_t   CbmDileptonTrackReal::GetTof() const {return isTOF;}

inline void CbmDileptonTrackReal::SetGlobalIndex(Int_t var){indexGlobal=var;}
inline void CbmDileptonTrackReal::SetRich(Bool_t var){isRICH=var;}
inline void CbmDileptonTrackReal::SetTrd(Bool_t var){isTRD=var;}
inline void CbmDileptonTrackReal::SetTof(Bool_t var){isTOF=var;}

#endif
