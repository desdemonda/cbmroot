
#ifndef CBMRICHTRBRECOQASTUDYREPORT_H
#define CBMRICHTRBRECOQASTUDYREPORT_H

#include "CbmStudyReport.h"
#include <string>
using std::string;
class TH1;


class CbmRichTrbRecoQaStudyReport : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
	CbmRichTrbRecoQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmRichTrbRecoQaStudyReport();

   void SetFitHist(Bool_t b) {fFitHist = b;}

protected:

   Bool_t fFitHist; // if true -> histograms will be fitted by gaus
    /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create();

	/**
	* \brief Inherited from CbmLitStudyReport.
	*/
	void Draw();


	void FitGausAndDrawH1(
			const string& histName,
			const string& canvasName);

	void DrawEfficiency();


   ClassDef(CbmRichTrbRecoQaStudyReport, 1)
};

#endif
