/** @file CbmTimeSlice.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17 July 2012
 **/

#ifndef CBMTIMESLICE_H
#define CBMTIMESLICE_H 1

#include <string>
#include <vector>

#include "TNamed.h"
#include "CbmStsDigi.h"
#include "CbmMuchDigi.h"
#include "CbmDigi.h"

using namespace std;


/** @class CbmTimeSlice
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17 July 2012
 ** @brief Container class for CBM raw data in a given time interval
 **/
class CbmTimeSlice : public TNamed
{

  public:

    /** Default constructor **/
    CbmTimeSlice();


    /** Standard constructor
     ** @param start    Start time of time slice [ns]
     ** @param duration Duration of time slice [ns]
     */
    CbmTimeSlice(Double_t start, Double_t duration);


    /** Destructor **/
    ~CbmTimeSlice();

    /** Access to data
     ** @param iDet   detector type
     ** @param index  index of data object in array
     ** @return pointer to data object
     **/
    CbmDigi* GetData(DetectorId iDet, Int_t index);


    /** Get size of raw data container for given detector
     **
     ** @param iDet   detector type
     ** @return size of raw data container (number of digis)
     */
    Int_t GetDataSize(DetectorId iDet) const;


    /** Duration of time slice
     **
     ** @return duration [ns]
     **/
    Double_t GetDuration() const { return fDuration; }


    /** Get match object
     ** @return Match object
     **/
    const CbmMatch& GetMatch() const { return fMatch; }


    /** Start time of time slice
     ** @return start time [ns]
     **/
     Double_t GetStartTime() const { return fStartTime; }


    /** End time of time slice
     ** @return end time [ns]
     **/
    Double_t GetEndTime() const { return fStartTime + fDuration; }


    /** Copy data into the time slice
     **
     ** Data will only be copied if their time fits into the time slice
     **
     ** @param digi  pointer to data object
     **/
    void InsertData(CbmDigi* digi);


    /** Check whether timeslice contains data
     ** @return kTRUE if timeslice contains data
     **/
    Bool_t IsEmpty() const { return fIsEmpty; }


    /** Reset the time slice
     **
     ** The data vectors will be cleared and a new start time is set.
     **
     ** @param start    New start time [ns]
     ** @param duration New duration [ns]
     */
    void Reset(Double_t start, Double_t duration);


    /** Consistency check
     ** Tests data to be in defined time interval and ordered w.r.t. time.
     ** @return kTRUE if OK, else kFALSE
     **/
    Bool_t SelfTest();


    /** Status to string **/
    string ToString() const;


    /** Get vector of much digis
     **
     ** @return MUCH raw data container (vector of digis) 
     */
    vector<CbmMuchDigi> GetMuchData() {return fMuchData; } 
    vector<CbmStsDigi>  GetStsData()  {return fStsData; }
    
  private:

    Double_t fStartTime;           ///< start time [ns]
    Double_t fDuration;            ///< duration [ns]
    Bool_t   fIsEmpty;             ///< Flag for containing no data
    vector<CbmStsDigi> fStsData;   ///< raw data container for STS
    vector<CbmMuchDigi> fMuchData; ///< raw data container for MUCH
    CbmMatch fMatch;               ///< link time slice to events



    ClassDef(CbmTimeSlice,2)
};

#endif /* CBMTIMESLICE_H */
