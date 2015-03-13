// -----------------------------------------------------------------------
// -----                     CbmTrdCreatePidLike                     -----
// -----               Created 26/02/07 by F.Uhlig                   -----
// -----------------------------------------------------------------------


/** CbmTrdCreatePidLike
 **
 *@author F. Uhlig <F.Uhlig@gsi.de>
 **
 ** Task to create the energy loss spectra needed for
 ** the likelihood method
 ** 
 **/


#ifndef CBMTRDCREATEPIDLIKE_H
#define CBMTRDCREATEPIDLIKE_H 1

#include "FairTask.h"

class TH1F;
class TClonesArray;

class CbmTrdCreatePidLike : public FairTask {
public:

    /* Defaul constructor */
    CbmTrdCreatePidLike();

    /* Standard constructor */
    CbmTrdCreatePidLike(const char* name,
                        const char* title = "FairTask");

    /* Destructor */
    virtual ~CbmTrdCreatePidLike();

    /* Initialisation */
    InitStatus Init();

    /* Execution */
    virtual void Exec(Option_t* option);

    /* Finish at the end of each event */
    virtual void Finish();


private:

    /* Data branches*/
    TClonesArray* fTrdHitCollection;
    TClonesArray* fTrdPointCollection;
    TClonesArray* fMCTrackArray;

    static const Int_t kNMom=13; // number of momentum spectra

    /* Test histograms*/
    TH1F*         h1dEdxEL[kNMom]; //
    TH1F*         h1dEdxPI[kNMom]; //
    TH1F*         h1dEdxKA[kNMom]; //
    TH1F*         h1dEdxPR[kNMom]; //
    TH1F*         h1dEdxMU[kNMom]; //
    TH1F*         h1momentum;


    /* Prepare test histograms */
    void PrepareHistograms();

    /* Write test histograms */
    void WriteHistograms();

    CbmTrdCreatePidLike(const CbmTrdCreatePidLike&);
    CbmTrdCreatePidLike& operator=(const CbmTrdCreatePidLike&);

    ClassDef(CbmTrdCreatePidLike, 1)
};

#endif
