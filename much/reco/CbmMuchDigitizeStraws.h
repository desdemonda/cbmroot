/**
 * \file CbmMuchDigitizeStraws.h
 * \brief Digitizer for MUCH straw detectors.
 **/

#ifndef CBMMUCHDIGITIZESTRAWS_H
#define CBMMUCHDIGITIZESTRAWS_H 1

#include "FairTask.h"
#include "TString.h"

class TClonesArray;
class CbmMuchGeoScheme;

class CbmMuchDigitizeStraws : public FairTask
{
public:
    /**
    * \brief Constructor.
    */
    CbmMuchDigitizeStraws(const char* digiFileName);

    /**
    * \brief Destructor.
    */
    virtual ~CbmMuchDigitizeStraws();

    /**
    * \brief Inherited from FairTask.
    */
    virtual void Exec(Option_t* opt);

    /**
    * \brief Inherited from FairTask.
    **/
    virtual void Finish();

    /**
    * \brief Inherited from FairTask.
    **/
    virtual InitStatus Init();

    void Exec2D(); // digitize for 2-D readout
    Int_t GetDimens() const
    {
        return fDimens;
    }
    void SetDimens(Int_t dim)
    {
        fDimens = dim;
    }

private:
    CbmMuchGeoScheme* fGeoScheme; // TODO add comments
    TString fDigiFile;            // Digitization file

    TClonesArray* fPoints;      // Input array of CbmMuchPoint
    TClonesArray* fDigis;       // Output array of CbmMuchDigi
    TClonesArray* fDigiMatches; // Output array of CbmMuchDigiMatches
    Int_t fDimens;              // 1- or 2- dimensional readout

    CbmMuchDigitizeStraws(const CbmMuchDigitizeStraws&);
    CbmMuchDigitizeStraws& operator=(const CbmMuchDigitizeStraws&);

    ClassDef(CbmMuchDigitizeStraws, 1)
};
#endif
