/**
 * \file CbmLitClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQAREPORT_H_
#define CBMLITCLUSTERINGQAREPORT_H_

#include "CbmSimulationReport.h"
#include <string>
using std::string;

/**
 * \class CbmLitClusteringQaReport
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaReport();

   static Double_t CalcEfficiency(
      const TH1* histRec,
      const TH1* histAcc,
      Double_t scale);

private:
   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw();

   /**
    * \brief Print number of objects table.
    */
   string PrintNofObjects() const;

   void DrawNofObjectsHistograms(
      const string& detName,
      const string& parameter);

   void DrawResidualsAndPulls(
      const string& detName);

//   void DrawAccAndRec(
//         const string& canvasName,
//         const string& histNamePattern);

//   void DrawEfficiency(
//         const string& canvasName,
//         const string& histNamePattern,
//         string (*labelFormatter)(const string&, const CbmHistManager*));

   void ScaleAndShrinkHistograms();

   void DivideHistos(
      TH1* histo1,
      TH1* histo2,
      TH1* histo3,
      Double_t scale);

   void CalculateEfficiencyHistos(
         const string& acc,
         const string& rec,
         const string& eff);

   ClassDef(CbmLitClusteringQaReport, 1)
};

#endif /* CBMLITCLUSTERINGQAREPORT_H_ */
