/** @file CbmDaq.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 20 July 2012
 **/


#ifndef CBMDAQ_H
#define CBMDAQ_H 1

#include "TStopwatch.h"
#include "FairTask.h"


class CbmDaqBuffer;
class CbmDigi;
class CbmTimeSlice;


/** @class CbmDaq
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 20 July 2012
 ** @brief CBM task class for filling digis from the CbmDaqBuffer into
 ** timeslice containers
 **/
class CbmDaq : public FairTask
{
  public:

    /**   Constructor
     ** @param timeSliceSize  Duration of time slices [ns]
     **/
    CbmDaq(Double_t timeSliceSize = 1000.);


    /**   Destructor   **/
    ~CbmDaq();


    /**   Task execution   **/
    virtual void Exec(Option_t* opt);


    /**   Task initialisation   **/
    virtual InitStatus Init();


  private:

    /** Start time of current time slice [ns] **/
    Double_t fCurrentStartTime;

    /** Duration of time slice [ns] **/
    Double_t fDuration;

    /** Flag whether empty timeslices should be filled to the tree **/
    // TODO: Implement functionality
    Bool_t fStoreEmptySlices;

    /** Timer **/
    TStopwatch fTimer;

    /** Counters **/
    Int_t fNofSteps;             ///< Number of execute steps
    Int_t fNofDigis;             ///< Total number of processed digis
    Int_t fNofTimeSlices;        ///< Number of time slices
    Int_t fNofTimeSlicesEmpty;   ///< Number of empty time slices
    Double_t fTimeDigiFirst;     ///< Time of first digi
    Double_t fTimeDigiLast;      ///< Time of last digi
    Double_t fTimeSliceFirst;    ///< Start time of first time slice
    Double_t fTimeSliceLast;     ///< Stop time of last time slice


    /** Pointer to current time slice **/
    CbmTimeSlice* fTimeSlice;

    /** Pointer to CbmDaqBuffer instance  **/
    CbmDaqBuffer* fBuffer;
    

    /** Close the current time slice
     ** The current slice is filled to the tree. It is then reset
     ** to the next time slice interval.
     */
    void CloseTimeSlice();


    /** Fill data from the buffer into the current time slice
     ** @return Number if digis filled into the timeslice
     **/
    Int_t FillTimeSlice();


    /** At end of run: Process the remaining data in the CbmDaqBuffer  **/
    virtual void Finish();

    CbmDaq(const CbmDaq&);
    CbmDaq& operator=(const CbmDaq&);
    
    ClassDef (CbmDaq,1);

};

#endif /* CBMDAQ_H */
