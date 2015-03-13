#ifndef CBMSTSXYTERRAWHITMESSAGE_H
#define CBMSTSXYTERRAWHITMESSAGE_H 1

#include "CbmRawMessage.h"

class CbmStsxyterRawHitMessage : public CbmRawMessage
{

public:
    CbmStsxyterRawHitMessage();
    CbmStsxyterRawHitMessage(Int_t EquipmentID, Int_t SourceAdress, Int_t ChannelId,
            Int_t EpochMarker, Int_t Time, Int_t AdcValue, Bool_t ParityError );
    virtual   ~CbmStsxyterRawHitMessage() { };
    Int_t     GetAdcValue() const {return fAdcValue;}
    Bool_t    GetParityError() const {return fParityError;}

private:
    Int_t     fAdcValue;
    Bool_t    fParityError;

    ClassDef(CbmStsxyterRawHitMessage,2);
};

#endif // CBMSTSXYTERRAWHITMESSAGE_H
