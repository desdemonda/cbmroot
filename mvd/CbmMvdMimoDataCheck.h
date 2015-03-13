// -------------------------------------------------------------------------
// -----                      CbmMvdMimoDataCheck header file           -----
// -----          Created 23.08.2011  by Q. Li, M. Deveaux             -----
// -------------------------------------------------------------------------


/** CbmMvdClusterFinder
 * @author Q. Li, M. Deveaux
 * @since 15.12.2011
 **
 ** Class for MIMOSA-26 CbmMvdMimoDataCheck
 **/

#ifndef CBMMVDMIMODATACHECK_H
#define CBMMVDMIMODATACHECK_H


#include "FairTask.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "CbmMvdHit.h"
#include "CbmMvdDigi.h"
#include "CbmMvdDigitizeL.h"


#include "TH1.h"//add by Qiyan
#include "TH2.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom3.h"

#include <iostream>
#include <fstream>
#include <vector>
#include "CbmMvdMimoData.h"
#include "CbmMvdMimoStateMatch.h"

using namespace std;
class FairLogger;





class CbmMvdMimoDataCheck : public FairTask
{

  public:

    /** Default constructor **/
    CbmMvdMimoDataCheck();


    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    CbmMvdMimoDataCheck(const char* name, Int_t iVerbose = 1);
    
    
    virtual void CompareDataWithStateMatch();
   

    /** Destructor **/
    virtual ~CbmMvdMimoDataCheck();

  protected:

   

    /** Intialisation at begin of run. To be implemented in the derived class.
    *@value  Success   If not kSUCCESS, task will be set inactive.
    **/
    virtual InitStatus Init();


    /** Reinitialisation. To be implemented in the derived class.
    *@value  Success   If not kSUCCESS, task will be set inactive.
    **/
    virtual InitStatus ReInit();
    
    

 

    
    
  /** Task execution **/
    virtual void Exec(Option_t* opt);


    /** Intialise parameter containers.
        To be implemented in the derived class.
    **/
    virtual void SetParContainers() { };

    /** Action after each event. To be implemented in the derived class **/
    virtual void Finish();

    //  /** Action after each event. To be implemented in the derived class **/
    //  virtual void FinishTask() { };
    TClonesArray* fDigiArray; // digis obtained from the digitizer
    TClonesArray* fMimoDataArray;
    TClonesArray* fMimoDataStateMatchArray;
    
     
    CbmMvdDigi* fDigi;
    CbmMvdMimoData* fMimoData;
    CbmMvdMimoStateMatch* fMimoStateMatch;
    
    vector< unsigned char > fStateVector;
    vector< Int_t > stateDigis;
    
    Int_t fEventNum;
    
       //test of the TH2 histro
    TH2* histroOfCompare;
    TCanvas* tcan;
    
    ClassDef(CbmMvdMimoDataCheck,2);

};


#endif