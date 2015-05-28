/*
 * \file CbmLitShowClustersReport.cxx
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date Mai, 2015
 * \brief
 */


#include "CbmLitShowClustersReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TProfile.h"
#include "TCanvas.h"
#include <boost/assign/list_of.hpp>
#include <vector>
using std::vector;
using std::endl;
using std::stringstream;
using boost::assign::list_of;
using Cbm::NumberToString;
using Cbm::Split;
using Cbm::FindAndReplace;


CbmLitShowClustersReport::CbmLitShowClustersReport():
		CbmSimulationReport()
{
   SetReportName("clustering_qa_show_clusters");
}

CbmLitShowClustersReport::~CbmLitShowClustersReport()
{

}

void CbmLitShowClustersReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   PrintCanvases();
   Out() <<  R()->DocumentEnd();
}


void CbmLitShowClustersReport::Draw()
{
   DrawH2ByPattern("hhh_.*Clustering_visualisation.*col_H2", kLinear, kLinear, kLinear, "colz");
   DrawH2ByPattern("hhh_.*Clustering_visualisation.*cont_H2", kLinear, kLinear, kLinear, "contz");

}

ClassImp(CbmLitShowClustersReport)