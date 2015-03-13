/**
 * \file CbmLitClusteringQaStudyReport.h
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQASTUDYREPORT_H_
#define CBMLITCLUSTERINGQASTUDYREPORT_H_

#include "CbmStudyReport.h"
#include <string>
using std::string;
class TH1;

/**
 * \class CbmLitClusteringQaStudyReport
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaStudyReport : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaStudyReport();

private:
   /**
    * \brief Inherited from CbmStudyReport.
    */
   void Create();

   /**
    * \brief Inherited from CbmStudyReport.
    */
   void Draw();

   /**
    * \brief Print number of objects table.
    */
   string PrintNofObjects() const;

   /**
    * \brief Draw all Efficiency Histogram.
    */
   void DrawAllEfficiencyHistos();

   /**
    * \brief Draw a single Efficiency Histogram.
    */
   void DrawEfficiency(const string& canvasName, const string& histName);

   /**
    * \brief Draw all Histograms defined by a given pattern.
    * \param[in] histNamePattern given pattern.
    */
   void DrawByPattern(string histNamePattern);

   /**
    * \brief Draw a canvas with all single Acc and Rec Histograms side by side.
    */
   void DrawAccRecCanvas();

   /**
    * \brief Draw Acc/Rec/Clone Histograms compared between Studies.
    */
   void DrawAccRecClones();

   /**
     * \brief Calculate efficiency for two histograms.
     * \param[in] histReco Reconstruction histogram.
     * \param[in] histAcc Acceptance histogram.
     * \param[in] scale Scaling factor for efficiency.
    */
   Double_t CalcEfficiency(const TH1* histRec, const TH1* histAcc, Double_t scale) const;

   /**
    * \brief Divides two Histograms into the third to generate the Efficiency Histogram.
    */
   void DivideHistos(TH1* histo1, TH1* histo2, TH1* histo3, Double_t scale);

   /**
    * \brief Debugging Function
    */
   void PrintListOfHistograms();

   /**
    * \brief Draw mean efficiency lines on histogram.
    * \param[in] histos Vector of histograms.
    * \param[in] efficiencies Vector of efficiency numbers.
    */
   void DrawMeanEfficiencyLines(
      const vector<TH1*>& histos,
      const vector<Double_t>& efficiencies);

   /**
    * \brief Draw accepted and reconstructed tracks histograms.
    * \param[in] canvasName Name of canvas.
    * \param[in] histName Histogram name.
    */
   void DrawAccAndRec(
         const string& canvasName,
         const string& histName);

   ClassDef(CbmLitClusteringQaStudyReport, 1)
};

#endif /* CBMLITCLUSTERINGQASTUDYREPORT_H_ */
