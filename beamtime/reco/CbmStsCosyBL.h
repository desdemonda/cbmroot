#ifndef CBMSTSCOSYBL_H
#define CBMSTSCOSYBL_H

#include "FairTask.h"
#include "TH1F.h"
#include "TH2F.h"

#include "CbmFiberHodoAddress.h"
#include "CbmFiberHodoDigi.h"

using std::vector;
class TClonesArray;

class CbmStsCosyBL : public FairTask
{
  public:

    /** Default constructor **/
    CbmStsCosyBL();

    /** Constructor with parameters (Optional)
    //  CbmStsCosyBL(Int_t verbose);
    **/

    /** Destructor **/
    ~CbmStsCosyBL();


    /** Initiliazation of task at the beginning of a run **/
    virtual InitStatus Init();

    /** ReInitiliazation of task when the runID changes **/
    virtual InitStatus ReInit();


    /** Executed for each event. **/
    virtual void Exec(Option_t* opt);

    /** Load the parameter container from the runtime database **/
    virtual void SetParContainers();

    /** Recursive FinishEvent of subtasks **/
    virtual void FinishEvent();

    /** Finish task called at the end of the run **/
    virtual void Finish();
    //    virtual Int_t AddFile( const char* name ); 
    void BLInit();
    void BLClean();
    void BaseLine(TClonesArray* fBaselineDigis,     vector< vector < vector < double> > > base_line_array);
    void HodoBaseLine(TClonesArray* fBaselineDigis, vector< vector < vector < double> > > base_line);
    
    void SetTriggeredMode(Bool_t mode) { fTriggeredMode = mode; }
    void SetTriggeredStation(Int_t station) { fTriggeredStation = station ; }

    virtual void Reset();    

    Int_t GetEntries () { return fChain->GetEntries();}

 private:
    TClonesArray*     auxDigis;       /** Input array of CbmFiberHodoDigi **/
    TClonesArray*     fDigis;       /** Input array of CbmFiberHodoDigi **/
    TClonesArray*     fBLDigis;
    TClonesArray*     hBLDigis;
    TClonesArray*     hDigis;
    TClonesArray*     cDigis;
    TClonesArray*     chDigis;

    TChain *fChain;                   //!
    TFile *outFile;                   //!
    static Double_t GetBlPos( TH1F * hist );
    static const Int_t kBaselineMinAdc = 0;
    static const Int_t kBaselineMaxAdc = 4096;
    static const Int_t kBaselineNBins = 4096;

    TH1F *sts_station;
    TH1F *sts_hits;
    TH1F *hodo_hits;
    TH1F *hodo_layer;
    TH1F *hodo_system;
    TH1F *chanels[3][2];
    TH1F *hit_patern[3][2];
    TH2F *xy_distr[3];
    TH2F *strip_vs_adc[3][2];
    TH1F *digi_distr[3];
    TH1F *digi_side[3][2];
    TH1F *digi_tdiff[3][2];
    TH2F *baseline_ch[3][2];
    TH2F *hodo_baseline_ch[4];
    TH1F *hits_layer[3][2]; 
    TH1F *aux_digi;
    TH1F *aux_time_diff;
    TH1F *aux_time;
 

    Bool_t calib;
    Bool_t hodo_calib;

    Bool_t fTriggeredMode; ///< Flag if data is taken in triggered mode
    Int_t  fTriggeredStation;

    vector< vector < vector < double> > > base_line_array;
    vector< vector < vector < double> > > hodo_BL_array;
    /** Input array from previous already existing data level **/
    //  TClonesArray* <InputDataLevel>;

    /** Output array to  new data level**/
    //  TClonesArray* <OutputDataLevel>;

    CbmStsCosyBL(const CbmStsCosyBL&);
    CbmStsCosyBL operator=(const CbmStsCosyBL&);

    ClassDef(CbmStsCosyBL,1);
};

#endif
