/**
 * \file CbmAnaDielectronStudyReportAll.h
 * \brief Creates study report for LMVM analysis.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */
#ifndef CBM_ANA_DIELECTRON_STUDY_REPORT_ALL
#define CBM_ANA_DIELECTRON_STUDY_REPORT_ALL

#include "CbmStudyReport.h"
#include <string>
//#include "TSystem.h"
using std::string;

/**
 * \class CbmAnaDielectronStudyReportAll
 * \brief Creates study report for LMVM analysis.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 *
 */
class CbmAnaDielectronStudyReportAll : public CbmStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmAnaDielectronStudyReportAll();

   /**
    * \brief Destructor.
    */
   virtual ~CbmAnaDielectronStudyReportAll();

protected:
    /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create();

   /**
   * \brief Inherited from CbmLitStudyReport.
   */
  void Draw() {};

   /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetQaFileName() const {
       return "lmvm_results_all.json";
    }

    /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetIdealFileName() const {
       return "lmvm_results_all.json";
    }

    /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetCheckFileName() const {
       return "lmvm_results_all.json";
    }

   /**
    * \brief Print one row in a table.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return String with table row.
    */
   virtual string PrintRow(
      const string& property,
      const string& name);

   /**
    * \brief
    */
   virtual string PrintValue(
         int studyId,
         const string& valueTitle);
};

#endif
