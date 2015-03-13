/** CbmAnaDielectronTaskDraw.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#ifndef CBM_ANA_DIELECTRON_TASK_DRAW_H
#define CBM_ANA_DIELECTRON_TASK_DRAW_H

#include "CbmLmvmHist.h"
#include <string>

#include "TObject.h"

class TH1;
class TH2D;
class TH1D;
class TFile;
class TCanvas;
class CbmHistManager;

using namespace std;

class CbmAnaDielectronTaskDraw: public TObject {

public:

    CbmAnaDielectronTaskDraw();
    
    virtual ~CbmAnaDielectronTaskDraw(){;}

    /**
     * \brief Implement functionality of drawing histograms in the macro
     * from the specified file, this function should be called from macro.
     * \param[in] fileName Name of the input file with histograms.
     * \param[in] outputDir Path to the output directory (if it does not exist, it will be created automatically).
     * \param[in] useMvd draw histograms related to the MVD detector?
     * \param[in] drawSig Do you want to draw significance histograms?
     **/
    void DrawHistFromFile(
          const string& fileName,
          const string& outputDir = "",
          Bool_t useMvd = true,
          Bool_t drawSig = true);

private:
    Int_t fNofEvents;
    Bool_t fUseMvd; // do you want to draw histograms related to the MVD detector?
    Bool_t fDrawSignificance; // do you want to draw significance histograms of 1D cuts?

    // analysis cut values for drawing in the histograms
    double fTrdAnnCut;
    double fRichAnnCut;
    double fPtCut;
    double fAngleCut;
    double fChiPrimCut;
    double fGammaCut;
    double fStCutAngle;
    double fStCutPP;
    double fRtCutAngle;
    double fRtCutPP;
    double fTtCutAngle;
    double fTtCutPP;
    double fMvd1CutP;
    double fMvd1CutD;
    double fMvd2CutP;
    double fMvd2CutD;

    CbmHistManager* fHM; //histogram manager
    vector<TCanvas*> fCanvas; // store pointers to all canvas -> save as image
    string fOutputDir; // output directory for results

    /**
     * \brief Rebin minv histograms for better drawing. Should be called after
     * calculation of S/BG ratios.
     */
    void RebinMinvHist();

    /**
     * \brief Return TH1D* pointer to the specified histogram.
     * \param[in] name Histogram name.
     */
    TH1D* H1(
          const string& name);

    /**
     * \brief Return TH2D* pointer to the specified histogram.
     * \param[in] name Histogram name.
     */
    TH2D* H2(
          const string& name);

    /**
     * \brief Create Canvas with specified parameters and add pointer to vector fCanvas.
     * \param[in] name Canvas name.
     * \param[in] title Canvas title.
     * \param[in] width Canvas width.
     * \param[in] height Canvas height.
     * \param return Created canvas.
     */
    TCanvas* CreateCanvas(
          const string& name,
          const string& title,
          int width,
          int height);

    /**
     * \brief Save all created canvases to images.
     */
    void SaveCanvasToImage();

    /**
     * \brief Draw an integrated efficiency on a histogram (100.*h1->GetEntries()/h2->GetEntries()).
     * Histogram must be drawn in advance.
     * \param[in] h1 Pointer to the first histogram.
     * \param[in] h2 Pointer to the second histogram.
     * \param[in] xPos X position of the text in absolute coordinates.
     * \param[in] yPos Y position of the text in absolute coordinates.
     */
    void DrawEfficiencyOnHist(
          TH1* h1,
          TH1* h2,
          Double_t xPos,
          Double_t yPos);

    /**
     * Draw text on the histogram. Histogram must be drawn in advance.
     * \param[in] text Text you want to draw.
     * \param[in] x1
     * \param[in] y1
     * \param[in] x2
     * \param[in] y2
     */
    void DrawTextOnHist(
          const string& text,
          Double_t x1,
          Double_t y1,
          Double_t x2,
          Double_t y2);

    /**
     * Produce 1D significance histogram Significance=S/sqrt(S+BG).
     * \param[in] s Histogram with signal.
     * \param[in] bg Histogram eith background.
     * \param[in] name Name of new significance histogram.
     * \param[in] option Could be "right" or "left".
     */
    TH1D* CreateSignificanceH1D(
          TH1D* s,
          TH1D* bg,
          const string& name,
          const string& option);

    /**
     * Produce 2D significance histogram Significance=S/sqrt(S+BG).
     */
    TH2D* CreateSignificanceH2D(
          TH2D* signal,
          TH2D* bg,
          const string& name,
          const string& title);

    /**
     * \brief Fit signal histogram using Fit("gaus").
     * Calculate S/BG ratio in 2 sigma region.
     * Print summary table of the efficiency, S/BG, sigma etc for each step in cout.
     * \param[in] step Analysis step.
     */
    void SOverBg(
          CbmLmvmAnalysisSteps step);

    /**
     * Calculates S/BG ratio for each step of the analysis
     * using SOverBg method.
     */
    void SOverBgAll();

    /**
     * Draw Pt vs. Y distribution of signal for one step.
     * Print integrated efficiency using DrawEfficiencyOnHist method.
     * \param[in] step Analysis step.
     */
    void DrawPtYDistribution(
          int step,
          bool drawAnaStep = true);

    /**
     * Draw Pt vs. Y distributions of signal for all steps
     * using DrawPtYDistribution method.
     */
    void DrawPtYDistributionAll();

    /**
     * Draw Rapidity distributions of signal for all steps
     */
    void DrawRapidityDistributionAll();

    /**
     * \brief Draw efficiency in dependence on Pt and Rapidity.
     * Efficiency is normalized to the previous step.
     * \param[in] step Analysis step.
     */
    void DrawPtYEfficiency(
          int step,
		  bool drawAnaStep = true);

    /**
     * Draw efficiency in dependence on Pt and Rapidity of signal for all steps.
     */
    void DrawPtYEfficiencyAll();

    /**
     * Draw momentum distribution of signal for all steps.
     */
    void DrawMomentumDistributionAll();

    /**
     * Draw efficiency vs. momentum of pair for all steps.
     */
    void DrawMomentumEfficiencyAll();

    /**
     * Draw Mother PDG
     */
    void DrawMotherPdg();

    // Draw distribution and significance of 1D analysis cut
    void Draw1DSourceTypes(
          const string& hName,
          bool doScale = true);

     void Draw1DCut(
           const string& hName,
           const string& sigOption,
           double cutValue = -999999.);

     void DrawElPiMomHis();

     void Draw2DCutTriangle(
           double xCross,
           double yCross);

     void Draw2DCut(
           const string& hist,
           double cutCrossX = -999999.,
           double cutCrossY = -999999.);

     void DrawCutDistributions();

     void DrawMismatchesAndGhosts();

    void DrawSourcesBgPairsEpEm(
          int step,
          bool inPercent,
          bool drawAnaStep = true);

    /**
     * Draw sources of BG pairs for all steps.
     */
    void DrawSourcesBgPairsAll();

    void DrawGammaVertex();

    void Draw1DHistoForEachAnalysisStep(
          const string& hist,
          Bool_t logy = false);

    //Draw Invariant mass distributions after each cut
    void DrawMinvForEachAnalysisStep();

    void DrawMinvSandBg(
          int step);

    // Invariant mass distribution after each cut for source of BG
    void DrawMinvSandBgAll();


    void DrawMinvSource(
          int step,
          bool drawAnaStep = true);

    // Invariant mass distribution after each cut for source of BG
    void DrawMinvSourceAll();

    /*
     * \brief Remove MVD bins from histograms if MVD detector was not used.
     */
    void RemoveMvdCutBins();


    void DrawBgSource2D(
          const string& canvasName,
          const string& histName,
          const vector<string>& yLabels,
          double scale,
          const string& zTitle);


    //SOURCE TRACKS
    void DrawBgSourceTracks();

    /**
     * \brief Set labels of X axis usinf analysis steps names.
     */
    void SetAnalysisStepLabels(
          TH1* h);

    void DrawMinvPtAll();

    void DrawBgSourcesVsMomentum();

    void DrawMvdCutQa();

    void DrawMvdAndStsHist();

    void DrawPmtXY();

   CbmAnaDielectronTaskDraw(const CbmAnaDielectronTaskDraw&);
   CbmAnaDielectronTaskDraw& operator=(const CbmAnaDielectronTaskDraw&);
   
   ClassDef(CbmAnaDielectronTaskDraw, 1);
};

#endif
