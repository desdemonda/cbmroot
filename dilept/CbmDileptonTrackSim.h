/****************************************************************************
 *  $Id: CbmDileptonTrackSim.h,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrackSim
 *  Descripton: This is the data class for identified electrons after
 *              assigining the MC info.
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrackSim.h,v $
 *****************************************************************************/

#ifndef CBM_DILEPTON_TRACK_SIM_H
#define CBM_DILEPTON_TRACK_SIM_H

#include "CbmDileptonTrack.h"

class TVector3;

class CbmDileptonTrackSim : public CbmDileptonTrack{

private:

    Bool_t isFakeTrack;
    Bool_t isFakeRing;
    Int_t  MCPdg;
    Int_t  MCMotherID;
    Int_t  MCMotherPdg;
 
public:

    CbmDileptonTrackSim(); //!constructor
    CbmDileptonTrackSim(TVector3, Bool_t, Bool_t, Int_t, Int_t, Int_t); //!constructor
  
    virtual ~CbmDileptonTrackSim(); //!destructor

    /* member function */
    Bool_t GetFakeTrack() const;
    Bool_t GetFakeRing() const;
    Int_t  GetMCPdg() const;
    Int_t  GetMCMotherID() const;
    Int_t  GetMCMotherPdg() const;

    ClassDef(CbmDileptonTrackSim,1);

};

inline Bool_t CbmDileptonTrackSim::GetFakeTrack() const {return isFakeTrack;}
inline Bool_t CbmDileptonTrackSim::GetFakeRing() const {return isFakeRing;}
inline Int_t CbmDileptonTrackSim::GetMCPdg() const {return MCPdg;}
inline Int_t CbmDileptonTrackSim::GetMCMotherID() const {return MCMotherID;}
inline Int_t CbmDileptonTrackSim::GetMCMotherPdg() const {return MCMotherPdg;}


#endif
