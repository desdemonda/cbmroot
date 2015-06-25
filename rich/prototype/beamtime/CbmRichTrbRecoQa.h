#ifndef CBMRICHTRBRECOQA_H
#define CBMRICHTRBRECOQA_H

#include "FairTask.h"
#include "CbmRichRingLight.h"
#include "CbmHistManager.h"
#include <vector>

using namespace std;

class TClonesArray;
class TH1D;
class TH2D;
class TCanvas;
class CbmRichRing;

class CbmRichRingFitterCOP;
class CbmRichRingFitterEllipseTau;

class CbmRichTrbRecoQa : public FairTask
{
public:
   /*
    * Constructor.
    */
   CbmRichTrbRecoQa();

   /*
    * Destractor.
    */
   virtual ~CbmRichTrbRecoQa();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(Option_t* option);

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Finish();

    /*
     * Histogram initialization.
     */
    void InitHist();

   /*
    * Draw histograms.
    */
   void DrawHist();

    /*
     * Draw current event (event display)
     * \param fitRingCircle Fitted rings with COP algorithm.
     * \param fitRingEllipse FittedRings with TAU ellipse fitter.
     */
    void DrawEvent(
    		const vector<CbmRichRingLight>& fitRingCircle,
    		const vector<CbmRichRingLight>& fitRingEllipse);

    /*
     * Fill histogramms for the circle fit.
     */
    void FillHistCircle(
          CbmRichRingLight* ring);

    /*
     * Fill histogramms for the ellipse fit.
     */
    void FillHistEllipse(
          CbmRichRingLight* ring);

    /*
     *
     */
    void FillNofHitsPmt();

    /*
     * Fit histogramm with Gauss fit and put results to the title.
     */
    void FitGaussAndDrawResults(TH1* h);

    /*
     * Draw histograms from file
     */
    void DrawHistFromFile(TString fileName);

    /*
     * Set maximum number of events to draw
     */
    void SetMaxNofEventsToDraw(UInt_t n) {fMaxNofEventsToDraw = n;}

    /*
     * Set output directory for images.
     */
    void SetOutputDir(TString dir) {fOutputDir = dir;}

    /*
     * Set run title. It is also a part of the file name of image files.
     */
    void SetRunTitle(TString title) {fRunTitle = title;}

    /*
     * Set to TRUE if you want to draw histograms.
     */
    void SetDrawHist(Bool_t b) {fDrawHist = b;}

private:

   TClonesArray* fRichHits; // Array of RICH hits
   TClonesArray* fRichRings; // Array of found RICH rings
   TClonesArray* fRichHitInfos; // Array of CbmRichHitInfo

   CbmHistManager* fHM; // histogram manager, all the histogram are manager by HM

   UInt_t fEventNum; // Event counter
   UInt_t fNofDrawnEvents; // Number of drawn events
   UInt_t fMaxNofEventsToDraw; // maximum number of events to draw
   TString fOutputDir; // output directory to store figures
   TString fRunTitle; // Title of the run
   Bool_t fDrawHist; // if TRUE histogramms are drawn

   CbmRichRingFitterCOP* fCopFit;
   CbmRichRingFitterEllipseTau* fTauFit;

   CbmRichTrbRecoQa(const CbmRichTrbRecoQa&);
   CbmRichTrbRecoQa operator=(const CbmRichTrbRecoQa&);

   ClassDef(CbmRichTrbRecoQa, 1);
};

#endif
