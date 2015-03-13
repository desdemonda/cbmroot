/**
 * \file CbmTrdGas.h
 * \brief Container for gas properties of TRD.
 */

#ifndef CBMTRDGAS_H_
#define CBMTRDGAS_H_

#include "TObject.h"
#include "TString.h"

class CbmTrdGas : public TObject
{
public:
   CbmTrdGas();
   virtual ~CbmTrdGas();

   void Init();
   TString GetFileName(TString method) const;

   Int_t    GetDetType()      const {return fDetType;}
   Double_t GetGasThick()     const {return fGasThick;}
   Double_t GetNobleGas()     const {return fPercentNobleGas;}
   Double_t GetCO2()          const {return fPercentCO2;}
   Int_t    GetNobleGasType() const {return fNobleGasType;}

   // static access method
   static CbmTrdGas* Instance() {return fgInstance;}


   private:

   CbmTrdGas& operator=(const CbmTrdGas&);
   CbmTrdGas(const CbmTrdGas&);

       Int_t              fDetType;         /** Detector type (GSI=0, MB=1) */
   Double_t           fGasThick;        /** Thickness of the gas layer */
   Double_t           fPercentNobleGas; /** Percentage of noble gas */
   Double_t           fPercentCO2;      /** Percentage of CO2 */
   Int_t              fNobleGasType;    /** Type of noble gas (Xe=1, Ar=2)*/
   static CbmTrdGas*  fgInstance;       /** Singleton Instance */
   TString            fFileNameLike; //!   /** File name of input for likelihood method */
   TString            fFileNameANN;  //!  /** File name of input for ANN method */
   void SetFileName();


   ClassDef(CbmTrdGas, 1)
};


#endif //CBM_TRD_GAS_H
