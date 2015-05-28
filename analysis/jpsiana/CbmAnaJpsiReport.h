
#ifndef CBMANAJPSIREPORT_H
#define CBMANAJPSIREPORT_H

#include "CbmSimulationReport.h"
#include "TSystem.h"

class CbmAnaJpsiReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
	CbmAnaJpsiReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmAnaJpsiReport();

protected:
   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw();

   void DrawSourceTypesH1(
         const string& hName,
         bool doScale = true);

   void DrawCutH1(
         const string& hName,
         double cutValue);

   void Draw2DCut(
         const string& hist);

   void DrawCutDistributions();


	ClassDef(CbmAnaJpsiReport, 1);
};

#endif /* CBMANAJPSIREPORT_H */

