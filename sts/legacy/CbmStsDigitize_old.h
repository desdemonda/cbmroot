//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize header file             -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

/** Legacy software. This class is replaced by CbmStsDigitize (completely
 ** new implementation).
 **/

/** CbmStsDigitise
 *@author Volker Friese <v.friese@gsi.de>
 *@since 30.08.06
 *@version 1.0
 **
 ** CBM task class for digitising STS
 ** Task level SIM
 ** Produces objects of type CbmStsDigi out of CbmStsPoint.
 **/

#ifndef CBMSTSDIGITIZE_OLD_H
#define CBMSTSDIGITIZE_OLD_H 1

#include <set>
#include "TStopwatch.h"
#include "FairTask.h"

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsStation;
class CbmStsSensor_old;
class CbmStsDigiScheme;

class CbmStsPoint;

using std::set;

class CbmStsDigitize_old : public FairTask
{

public:
    /** Default constructor **/
    CbmStsDigitize_old();

    /** Destructor **/
    virtual ~CbmStsDigitize_old();

    /** Execution **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();

    virtual void SetRealisticResponse(Bool_t real = kTRUE)
    {
        fRealistic = real;
    }

    void SetStep(Double_t tStep)
    {
        fStep = tStep;
    }

//    void FindFiredStrips(CbmStsPoint* pnt, Int_t& nofStr, Int_t*& strips, Double_t*& signals, Int_t side);
    void ProduceHitResponse(CbmStsSensor_old* sensor);

    void SetFrontThreshold(Double_t frontThr = 0.)
    {
        fFThreshold = frontThr;
    }
    void SetBackThreshold(Double_t backThr = 0.)
    {
        fBThreshold = backThr;
    }
    void SetFrontNoiseWidth(Double_t frontNoW = 0.)
    {
        fFNoiseWidth = frontNoW;
    }
    void SetBackNoiseWidth(Double_t backNoW = 0.)
    {
        fBNoiseWidth = backNoW;
    }

    void SetFrontNofBits(Int_t frontNB = 0)
    {
        fFNofBits = frontNB;
    }
    void SetBackNofBits(Int_t backNB = 0)
    {
        fBNofBits = backNB;
    }
    void SetFrontNofElPerAdc(Double_t frontMS = 0.)
    {
        fFNofElPerAdc = frontMS;
    }
    void SetBackNofElPerAdc(Double_t backMS = 0.)
    {
        fBNofElPerAdc = backMS;
    }

    void SetStripDeadTime(Double_t StripDeadTime = 0.)
    {
        fStripDeadTime = StripDeadTime;
    }

private:
    CbmGeoStsPar* fGeoPar;         /** Geometry parameter container **/
    CbmStsDigiPar* fDigiPar;       /** Digitisation parameter container **/
    CbmStsDigiScheme* fDigiScheme; /** Digitisation scheme **/
    TClonesArray* fPoints;         /** Input array of CbmStsPoint **/
    TClonesArray* fDigis;          /** Output array of CbmStsDigi **/
    TClonesArray* fDigiMatches;    /** Output array of CbmMatch**/

    Double_t fNEvents;
    Double_t fNPoints;
    Double_t fNDigisFront;
    Double_t fNDigisBack;
    Double_t fTime;

    // settings
    Double_t fStep;

    TStopwatch fTimer;

    Bool_t fRealistic;
    Double_t fEnergyLossToSignal;

    Double_t fFThreshold;
    Double_t fBThreshold;
    Double_t fFNoiseWidth;
    Double_t fBNoiseWidth;

    Double_t fStripDeadTime;

    Int_t fFNofBits;
    Int_t fBNofBits;
    Double_t fFNofElPerAdc;
    Double_t fBNofElPerAdc;
    Int_t fFNofSteps;
    Int_t fBNofSteps;

    Double_t* fStripSignalF; //!
    Double_t* fStripSignalB; //!

    // map of the point indices contributing to a fired strip
    std::map<Int_t, set<Int_t> > fFChannelPointsMap;
    std::map<Int_t, set<Int_t> > fBChannelPointsMap;

    std::map<CbmStsSensor_old*, set<Int_t> > fPointMap; /** sensor points **/
    Float_t occupancy[10][1000][20];

    /** Make sensorwise set for points **/
    void MakeSets();
    void MakeSets1();

    /** Get parameter containers **/
    virtual void SetParContainers();

    /** Intialisation **/
    virtual InitStatus Init();

    /** Reinitialisation **/
    virtual InitStatus ReInit();

    /** Reset eventwise counters **/
    void Reset();

    CbmStsDigitize_old(const CbmStsDigitize_old&);
    CbmStsDigitize_old operator=(const CbmStsDigitize_old&);

    ClassDef(CbmStsDigitize_old, 1);
};

#endif
