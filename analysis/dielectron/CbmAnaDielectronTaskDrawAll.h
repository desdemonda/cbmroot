/** CbmAnaDielectronTaskDrawAll.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 2.0
 **/

#ifndef CBM_ANA_DIELECTRON_TASK_DRAW_ALL
#define CBM_ANA_DIELECTRON_TASK_DRAW_ALL

#include <vector>
#include <string>
#include <map>

#include "CbmLmvmHist.h"

#include "TObject.h"

class TH1;
class TH2D;
class TH1D;
class TFile;
class TCanvas;
class CbmHistManager;

using namespace std;

enum SignalType {
   kRho0 = 0,
   kOmega = 1,
   kPhi = 2,
   kOmegaD = 3
};

class CbmAnaDielectronTaskDrawAll: public TObject {

public:

   /**
   * \brief Default constructor.
   */
 CbmAnaDielectronTaskDrawAll() 
   : TObject(),
    fCanvas(),
    fUseMvd(false),
    fHM(),
    fh_mean_bg_minv(),
    fh_mean_eta_minv(),
    fh_mean_pi0_minv(),
    fh_mean_eta_minv_pt(),
    fh_mean_pi0_minv_pt(),
    fh_sum_s_minv(),
    fOutputDir("")
      {
	 ;
      }

   /**
    * \brief Destructor.
    */
   virtual ~CbmAnaDielectronTaskDrawAll(){;}

   /**
   * \brief Implement functionality of drawing histograms in the macro
   * from the specified files, this function should be called from macro.
   * \param[in] fileNameRho0 Name of the file with rho0 histograms.
   * \param[in] fileNameOmega Name of the file with omega histograms.
   * \param[in] fileNamePhi Name of the file with phi histograms.
   * \param[in] fileNameOmegaDalitz Name of the file with omegaDalitz histograms.
   * \param[in] outputDir Output directory for figures and .json file.
   * \param useMvd draw histograms related to the MVD detector?
   **/
   void DrawHistosFromFile(
         const string& fileNameRho0,
         const string& fileNameOmega,
         const string& fileNamePhi,
         const string& fileNameOmegaDalitz,
         const string& outputDir = "",
         Bool_t useMvd = false);

private:
   static const int fNofSignals = 4;

   vector<TCanvas*> fCanvas; // store all pointers to TCanvas -> save to images

   Bool_t fUseMvd; // do you want to draw histograms related to the MVD detector?

   //[0]=rho0, [1]=omega, [2]=phi, [3]=omegaDalitz
   vector<CbmHistManager*> fHM;

   // index: AnalysisSteps
   vector<TH1D*> fh_mean_bg_minv; //mean histograms from 4 files
   vector<TH1D*> fh_mean_eta_minv;
   vector<TH1D*> fh_mean_pi0_minv;
   vector<TH2D*> fh_mean_eta_minv_pt;
   vector<TH2D*> fh_mean_pi0_minv_pt;

   // index: AnalysisSteps
   vector<TH1D*> fh_sum_s_minv; // sum of all signals

   string fOutputDir; // output directory for figures

   TCanvas* CreateCanvas(
         const string& name,
         const string& title,
         int width,
         int height);

   TH1D* H1(
         int signalType,
         const string& name);

   TH2D* H2(
         int signalType,
         const string& name);


   /**
    * \brief Draw invariant mass histograms.
    */
   void DrawMinvAll();

   /**
    * \brief Draw invariant mass spectra for all signal types for specified analysis step.
    * \param[in] step Analysis step.
    */
   void DrawMinv(
         CbmLmvmAnalysisSteps step);

   /**
    * \brief Draw invariant mass vs Pt histograms.
    */
   void DrawMinvPtAll();

   /**
    * \brief Draw invariant mass spectra vs Pt for all signal types for specified analysis step.
    * \param[in] step Analysis step.
    */
   void DrawMinvPt(
         CbmLmvmAnalysisSteps step);

   /**
    * \brief It creates a mean histogram from 4 files.
    */
   void FillMeanHist();

   /**
    * \brief Fill sum signals.
    */
   void FillSumSignalsHist();

   /**
    * \brief Calculate cut efficiency in specified invariant mass region.
    * \param[in] min Minimum invariant mass.
    * \param[in] max Maximum invariant mass.
    */
   void CalcCutEffRange(
         Double_t minMinv,
         Double_t maxMinv);

   /**
    * \brief Create S/BG vs cuts for specified invariant mass range.
    * \param[in] min Minimum invariant mass.
    * \param[in] max Maximum invariant mass.
    */
   TH1D* SBgRange(
         Double_t minMinv,
         Double_t maxMinv);

   /**
    * \brief Draw S/BG vs plots for different mass ranges.
    */
   void SBgRangeAll();

   /**
    * \brief Draw S/BG vs plots for different signals.
    */
   void DrawSBgSignals();

   /**
    * \brief Save all created canvases to images.
    */
   void SaveCanvasToImage();


   CbmAnaDielectronTaskDrawAll(const CbmAnaDielectronTaskDrawAll&);
   CbmAnaDielectronTaskDrawAll operator=(const CbmAnaDielectronTaskDrawAll&);

   ClassDef(CbmAnaDielectronTaskDrawAll, 1);
};

#endif
