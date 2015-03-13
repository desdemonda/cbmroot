// -----------------------------------------------------------------------
// -----                     CbmTrdHitProducerQa                     -----
// -----               Created 13/12/05  by M. Kalisky               -----
// -----------------------------------------------------------------------


/** CbmTrdHitProducerQa
 **
 *@author M.Kalisky <M.Kalisky@gsi.de>
 **
 ** Class for performance of TRD Hit Producer
 ** Reads all hits and computes Hit pools.
 ** More to come later.
 **/


#ifndef CBMTRDHITPRODUCERQA
#define CBMTRDHITPRODUCERQA 1

#include "FairTask.h"

class TClonesArray;
class TH1F;

class CbmTrdHitProducerQa : public FairTask
{
public:

    /* Defaul constructor */
    CbmTrdHitProducerQa();

    /* Standard constructor */
    CbmTrdHitProducerQa(const char* name,
        		const char* title = "FairTask");

    /* Destructor */
    virtual ~CbmTrdHitProducerQa();

    /* Initialisation */
    InitStatus Init();

    /* Execution */
    virtual void Exec(Option_t* option);

    /* Finish at the end of each event */
    virtual void Finish();


private:

    /* Data branches*/
    TClonesArray* fTrdHitCollection;
    TClonesArray* fTrdDigiCollection;
    TClonesArray* fTrdDigiMatchCollection;
    TClonesArray* fTrdPointCollection;
    TClonesArray* fMCTrackArray;

    /** Number of TRD stations **/
    Int_t fNoTrdStations;

    /** Number of layers per station **/
    Int_t fNoTrdPerStation;


    /* Test histograms*/
    TH1F*         fHitPoolsX; // = ((Hit - Point) / HitError)  in X
    TH1F*         fHitPoolsY; // = ((Hit - Point) / HitError)  in Y

    TH1F*         S1L1eTR15; //
    TH1F*         S1L1edEdx15; // 
    TH1F*         S1L1edE15; // 
    TH1F*         S1L1edEall; // 
    TH1F*         S1L1pidE15; // 
    TH1F*         S1L1pidEall; // 

    TH1F*         S3L4eTR15; //
    TH1F*         S3L4edEdx15; // 
    TH1F*         S3L4edE15; // 
    TH1F*         S3L4edEall; // 
    TH1F*         S3L4pidE15; // 
    TH1F*         S3L4pidEall; // 

private:

    /* Prepare test histograms */
    void PrepareHistograms();

    CbmTrdHitProducerQa(const CbmTrdHitProducerQa&);
    CbmTrdHitProducerQa& operator=(const CbmTrdHitProducerQa&);

//public:

    /* Write test histograms */
    void WriteHistograms();


    ClassDef(CbmTrdHitProducerQa, 2)
};


#endif
