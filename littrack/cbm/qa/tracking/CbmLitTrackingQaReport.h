/**
 * \file CbmLitTrackingQaReport.h
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITTRACKINGQAREPORT_H_
#define CBMLITTRACKINGQAREPORT_H_

#include "CbmSimulationReport.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
class TH1;

/**
 * \class CbmLitTrackingQaReport
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitTrackingQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQaReport();

protected:
   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw();

   /**
    * \brief Return string with number of objects statistics.
    * \return String with number of objects statistics.
    */
   string PrintNofObjects() const;

   /**
    * \brief Return string with hits histogram statistics (nof all, true, fake hits in track/ring).
    * \return String with hits histogram statistics (nof all, true, fake hits in track/ring).
    */
   string PrintTrackHits() const;

   /**
    * \brief Return string with number of ghosts statistics.
    * \return String with number of ghosts statistics.
    */
   string PrintNofGhosts() const;

   /**
    * \brief Return string with tracking efficiency.
    * \param[in] includeRich True if RICH detector is included in the tracking efficiency table.
    * \param[in] isPidEfficiency True if PID efficiency is plotted.
    * \return String with tracking efficiency.
    */
   string PrintTrackingEfficiency(
		   Bool_t includeRich,
	      Bool_t isPidEfficiency) const;

   /**
    * \brief Return string with pion suppression efficiency.
    * \return String with pion suppression efficiency.
    */
   string PrintPionSuppression() const;

   /**
    * \brief Main function for drawing efficiency histograms.
    */
   void DrawEfficiencyHistos();

   /**
    * \brief Draw efficiency histogram.
    * \param[in] canvasName Name of canvas.
    * \param[in] histNamePattern Histogram name pattern.
    */
   void DrawEfficiency(
	  const string& canvasName,
	  const string& histNamePattern,
	  string (*labelFormatter)(const string&, Double_t));

   void DrawPionSuppression(
         const string& canvasName,
         const string& histNamePattern,
         string (*labelFormatter)(const string&, Double_t));

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
    * \param[in] histNamePattern Histogram name pattern.
    */
   void DrawAccAndRec(
         const string& canvasName,
         const string& histNamePattern);

   /**
    * \brief Draw histograms for hits.
    */
   void DrawHitsHistos();

   /**
    * \brief Draw histograms for hits. This function automatically
    * check the existence of histograms.
    * \param[in] canvasName Name of canvas.
    * \param[in] hist main name of hits histograms.
    */
   void DrawHitsHistos(
      const string& canvasName,
      const string& hist);

   /**
    * \brief Main function for drawing Rapidity-Pt histograms.
    */
   void DrawYPtHistos();

   /**
    * \brief Draw Rapidity-Pt histograms.
    * \param[in] canvasName Name of canvas.
    * \param[in] effHistName Name of the efficiency histogram.
    * \param[in] drawOnlyEfficiency Specify whether you want to draw only efficiency histo or including distributions.
    */
   void DrawYPt(
         const string& canvasName,
         const string& effHistName,
         Bool_t drawOnlyEfficiency = false);

   /**
    * \brief Calculate efficiency for two histograms.
    * \param[in] histReco Reconstruction histogram.
    * \param[in] histAcc Acceptance histogram.
    * \param[in] scale Scaling factor for efficiency.
    */
   Double_t CalcEfficiency(
      const TH1* histRec,
      const TH1* histAcc,
      Double_t scale = 1.) const;

   void FillGlobalTrackVariants();

   /**
    * \brief Divide two histograms.
    * \param[in] histo1 Numerator.
    * \param[in] histo2 Denominator.
    * \param[out] histo3 Output histogram.
    * \param[in] scale Scaling factor.
    */
   void DivideHistos(
      TH1* histo1,
      TH1* histo2,
      TH1* histo3,
      Double_t scale);

   /**
    * \brief Calculate efficiency histograms.
    */
   void CalculateEfficiencyHistos();

   void CalculatePionSuppressionHistos();

   vector<string> fGlobalTrackVariants;

   ClassDef(CbmLitTrackingQaReport, 1)
};

#endif /* CBMLITTRACKINGQAREPORT_H_ */
