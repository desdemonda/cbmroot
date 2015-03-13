/**
 * \file CbmAnaDielectronReports.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmAnaDielectronReports.h"
#include "CbmAnaDielectronStudyReportAll.h"
#include "CbmReport.h"
#include "CbmStudyReport.h"

#include "TSystem.h"

#include <fstream>

using namespace std;

CbmAnaDielectronReports::CbmAnaDielectronReports()
{
}

CbmAnaDielectronReports::~CbmAnaDielectronReports()
{
}

void CbmAnaDielectronReports::CreateStudyReport(
      const string& title,
      const vector<string>& fileNames,
      const vector<string>& studyNames,
      const string& outputDir)
{
   if (outputDir != "") gSystem->mkdir(outputDir.c_str(), true);

   CbmStudyReport* report = new CbmAnaDielectronStudyReportAll();
   report->Create(fileNames, studyNames, outputDir);
   delete report;
}

ClassImp(CbmAnaDielectronReports);
