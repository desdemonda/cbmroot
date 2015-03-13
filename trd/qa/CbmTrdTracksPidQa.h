// -----------------------------------------------------------------------
// -----                     CbmTrdTracksPidQa                       -----
// -----               Created 24.02.07 F. Uhlig                     -----
// -----------------------------------------------------------------------


/** CbmTrdTracksPidQa
 **
 *@author F.Uhlig <F.Uhlig@gsi.de>
 **
 ** Task class for performance checks concerning the PIDs 
 ** from different methods in the TRD Track.
 ** Fill histograms for all three different PID methods (Wkn, 
 ** Likelihood, Neural network) which are implemented so far.
 ** Fill booked spectra for each TRD Track.
 ** 
 **/


#ifndef CBMTRDTRACKSPIDQA_H
#define CBMTRDTRACKSPIDQA_H 1

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class CbmTrdTracksPidQa : public FairTask
{
public:

    /* Defaul constructor */
    CbmTrdTracksPidQa();

    /* Standard constructor */
    CbmTrdTracksPidQa(const char* name,
        		const char* title = "FairTask");

    /* Destructor */
    virtual ~CbmTrdTracksPidQa();

    /* Initialisation */
    InitStatus Init();

    /* Execution */
    virtual void Exec(Option_t* option);

    /* Finish at the end of each event */
    virtual void Finish();


private:

    /* Data branches*/
    TClonesArray* fTrdTrackCollection;
    TClonesArray* fTrdHitCollection;
    TClonesArray* fMCPointArray;
    TClonesArray* fMCTrackArray;


    /* QA histograms*/
    TH1F*         PartID;//
    TH1F*         NrTRDHits;//
    TH1F*         ELossPI;//
    TH1F*         ELossEL;//
    TH1F*         ELossALL;//
    TH1F*         MomPI;//
    TH1F*         MomEL;//
    TH1F*         MomALL;//
    TH2F*         MOMvsELossEL;
    TH2F*         MOMvsELossPI;
    TH2F*         MOMvsELossALL;

    TH1F*         WknPI;//
    TH1F*         WknEL;//
    TH1F*         WknALL;//
    TH1F*         WknLowPI;//
    TH1F*         WknLowEL;//
    TH1F*         WknLowALL;//
    TH1F*         WknHighPI;//
    TH1F*         WknHighEL;//
    TH1F*         WknHighALL;//

    TH1F*         AnnPI;//
    TH1F*         AnnEL;//
    TH1F*         AnnALL;//
    TH1F*         AnnLowPI;//
    TH1F*         AnnLowEL;//
    TH1F*         AnnLowALL;//
    TH1F*         AnnHighPI;//
    TH1F*         AnnHighEL;//
    TH1F*         AnnHighALL;//

    TH1F*         LikePI;//
    TH1F*         LikeEL;//
    TH1F*         LikeALL;//
    TH1F*         LikeHighPI;//
    TH1F*         LikeHighEL;//
    TH1F*         LikeHighALL;//
    TH1F*         LikeLowPI;//
    TH1F*         LikeLowEL;//
    TH1F*         LikeLowALL;//



    /* Prepare the QA histograms */
    void PrepareHistograms();


    /* Write the QA histograms to file*/
  void WriteHistograms();
  
  CbmTrdTracksPidQa(const CbmTrdTracksPidQa&);
  CbmTrdTracksPidQa& operator=(const CbmTrdTracksPidQa&);

    ClassDef(CbmTrdTracksPidQa, 1)
};

#endif
