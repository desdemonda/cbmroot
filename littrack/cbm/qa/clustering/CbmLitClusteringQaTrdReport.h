/*
 * \file CbmLitClusteringQaTrdReport.h
 * \author Jonathan Pieper
 * \date Dec 1, 2014
 * \brief Report Creator for ClusteringQaNew
 */

#ifndef CBMLITCLUSTERINGQATRDREPORT_H_
#define CBMLITCLUSTERINGQATRDREPORT_H_

#include "CbmSimulationReport.h"
#include <string>
using std::string;


class CbmLitClusteringQaTrdReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaTrdReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaTrdReport();

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

   ClassDef(CbmLitClusteringQaTrdReport, 1)
};

#endif /* CBMLITCLUSTERINGQATRDREPORT_H_ */
