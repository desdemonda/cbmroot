/*
 * \file CbmLitShowClustersReport.h
 * \author Jonathan Pieper
 * \date Dec 1, 2014
 * \brief Report Creator for CbmLitShowClusters
 */

#ifndef CBMLITSHOWCLUSTERSREPORT_H_
#define CBMLITSHOWCLUSTERSREPORT_H_

#include "CbmSimulationReport.h"
#include <string>
using std::string;


class CbmLitShowClustersReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
  CbmLitShowClustersReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitShowClustersReport();

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

   ClassDef(CbmLitShowClustersReport, 1)
};

#endif /* CBMLITSHOWCLUSTERSREPORT_H_ */
