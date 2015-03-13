/** CbmAnaLmvmDrawStudy.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2012
 * @version 1.0
 **/

#ifndef CBM_ANA_LMVM_DRAW_STUDY
#define CBM_ANA_LMVM_DRAW_STUDY

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
class CbmAnaPTree;

using namespace std;

class CbmAnaLmvmDrawStudy: public TObject {

public:

   /**
   * \brief Default constructor.
   */
   CbmAnaLmvmDrawStudy() : TObject(), fCanvas(), fNofStudies(0), fStudyNames(),
                           fHM(), fMeanFiles(), fOutputDir("")
   {;}

   /**
    * \brief Destructor.
    */
   virtual ~CbmAnaLmvmDrawStudy(){;}

   /**
   * \brief Implement functionality of drawing histograms in the macro
   * from the specified files, this function should be called from macro.
   * \param[in] fileNames Names of the file with histograms.
   * \param[in] studyNames Names of the study to be drawn in Legend.
   * \param[in] outputDir Name of the output directory.
   * \param[in] useMvd .
   **/
   void DrawFromFile(
         const vector<string>& fileNames,
         const vector<string>& fileNamesMean,
         const vector<string>& studyNames,
         const string& outputDir = "");

private:
   vector<TCanvas*> fCanvas; // store all pointers to TCanvas -> save to images
   Int_t fNofStudies;
   vector<string> fStudyNames;

   vector<CbmHistManager*> fHM; // store pointers to histogram manager for different simulations

   vector<string> fMeanFiles; // Files mean

   string fOutputDir; // output directory for figures and .json file

   TCanvas* CreateCanvas(
         const string& name,
         const string& title,
         int width,
         int height);

   void DrawTextOnHist(
         const string& text,
         Double_t x1,
         Double_t y1,
         Double_t x2,
         Double_t y2);

   TH1D* H1(
         int studyNum,
         const string& name);

   TH2D* H2(
         int studyNum,
         const string& name);

   void SaveCanvasToImage();

   void SetAnalysisStepLabels(
         TH1* h);

   void DrawMinv();
   void DrawNofBgTracks();
   void DrawBgSourceTracks();
   void DrawBgSourcePairsStep(
        int step);
   void DrawBgSourcePairs();
   void DrawBgSourceMinv();
   void DrawDistributions(
        const string& canvasName,
        const string& histName,
        int step,
        int sourceType);

   ClassDef(CbmAnaLmvmDrawStudy, 1);
};

#endif
