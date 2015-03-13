// -------------------------------------------------------------------------
// -----                  CbmEcalStructure header file                 -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalInf.h
 *@author Mikhail Prokudin
 **
 ** Container of ECAL geometry parameters
 **/

/* $Id: CbmEcalInf.h,v 1.13 2006/07/29 00:33:53 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalInf.h,v $
 * Revision 1.13  2006/07/29 00:33:53  prokudin
 * Commenting. Bringing code to coding conventions. New way of parameter file processing
 *
 * Revision 1.12  2006/07/24 21:45:02  prokudin
 * Now we write information to parameter file
 *
 * Revision 1.11  2006/07/12 16:26:36  prokudin
 * Corrected according to ROOT requirements
 *
 * Revision 1.10  2006/07/12 12:04:59  prokudin
 * CbmEcalInf now singleton.
 *
 * Revision 1.9  2006/07/11 13:08:43  prokudin
 * Magic number 48 removed. Using atoi instead.
 *
 * Revision 1.8  2006/07/10 14:39:38  kharlov
 * Correction for type cell
 *
 * Revision 1.7  2006/07/10 07:43:25  kharlov
 * Conversion of cell type from string to a number
 *
 * Revision 1.6  2006/06/30 06:17:17  kharlov
 * Optimization, commenting
 *
 * Revision 1.5  2006/06/22 06:52:13  kharlov
 * Compilation warnings removed
 *
 * Revision 1.4  2006/04/02 16:55:23  kharlov
 * Merging full and fast MC
 *
 * Revision 1.2  2006/01/31 17:07:17  kharlov
 * Correction for CbmEcalv1
 *
 * Revision 1.1  2006/01/27 17:26:30  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */

#ifndef CBMECALINF_H
#define CBMECALINF_H

#include "TObjArray.h"
#include "TString.h"
#include "TObjString.h"

#include <list>
#include <stdlib.h>

class TMap;
class CbmGeoEcalPar;

class CbmEcalInf:public TObject
{
public:
  /** This is ROOT requirement **/
 CbmEcalInf() : TObject(), fVariables(NULL), fEcalStr(), fXPos(0.), fYPos(0.),
    fZPos(0.), fNLayers(0), fXSize(0), fYSize(0), fModuleSize(0.), fPSLead(0.),
    fPSScin(0.), fPSGap(0.), fLead(0.), fScin(0.), fTyveec(0.), fThicknessLayer(0.),
    fCellSize(0.), fEcalSize(), fEcalHole(), fECut(0.), fHCut(0.), fFastMC(-1),
    fSuccess(-1), fFileName("") 
    {};

  static CbmEcalInf* GetInstance(const char* filename);
  /** Getters **/
  inline Double_t GetXPos() const {return fXPos;} 
  inline Double_t GetYPos() const {return fYPos;} 
  inline Double_t GetZPos() const {return fZPos;}
	
  inline Double_t GetModuleSize() const {return fModuleSize;}
  
  inline Double_t GetPSLead()  const {return fPSLead;}
  inline Double_t GetPSScin()  const {return fPSScin;}
  inline Double_t GetPSGap()   const {return fPSGap; }

  inline Int_t    GetNLayers() const {return fNLayers;}
  inline Double_t GetLead()    const {return fLead;}
  inline Double_t GetScin()    const {return fScin;}
  inline Double_t GetTyveec()  const {return fTyveec;}
  inline Double_t GetThicknessLayer() const {return fThicknessLayer;}

  /** Size of ECAL in super modules **/
  inline Int_t GetXSize() const {return fXSize;}
  inline Int_t GetYSize() const {return fYSize;}

  /** Geant cuts information **/
  inline Double_t GetElectronCut() const {return fECut;}
  inline Double_t GetHadronCut()   const {return fHCut;}

  inline Double_t GetCellSize()    const {return fCellSize;}
  inline Double_t GetEcalHole(Int_t num) const {
    if (num>-1&&num<3) return fEcalHole[num];
    return -1;
  }
  inline Double_t GetEcalSize(Int_t num) const {
    if (num>-1&&num<3) return fEcalSize[num];
    return -1;
  }
  char GetType(Int_t x, Int_t y) const; //returns type of (X,Y) supercell
  inline Int_t GetFastMC() const {return fFastMC;}
  void DumpContainer() const;
  
  void FreeInstance();

  /** key must be lower case. For example, if have in
   ** geo file AaaA=90, then you should call 
   ** GetVariableStrict("aaaa").
   ** If variable not found, will return -1111 **/
  Double_t GetVariable(const char* key);
  /** key must be lower case. For example, if have in
   ** geo file AaaA=90, then you should call 
   ** GetVariableStrict("aaaa").
   ** If variable not found, will generate Fatal **/
  Double_t GetVariableStrict(const char* key);
  TString GetStringVariable(const char* key);

  void AddVariable(const char* key, const char* value);
  /** Writes Inf to par, if write_str==1 fEcalStr also
   ** writen **/
  void FillGeoPar(CbmGeoEcalPar* par, Int_t write_str) const;
  /** If data from Parameter file differs from ours **/
  void CheckVariables();
protected:

  /** Text file constructor **/
  CbmEcalInf(const char* filename);	
  void CalculateHoleSize();
  virtual ~CbmEcalInf();

  static CbmEcalInf* fInf;
  static Int_t fRefCount;

  
private:
  /** Init all other variables from fVariables
   ** and fEcalStr**/
  void InitVariables();
  /** Ignore a parameter during comparision **/
  Bool_t ExcludeParameter(TString parname);
  /** A map containing all variables
   ** This variable should be saved in parameter file **/
  TMap* fVariables;
  /** Structure of ECAL as array of strings
   ** This variable should be saved in parameter file **/
  TObjArray fEcalStr;
  /** x-position of ECAL center [cm]	 **/
  Double_t fXPos;
  /** y-position of ECAL center [cm] **/
  Double_t fYPos;
  /** z-position of ECAL front [cm] **/
  Double_t fZPos;

  /** Number of ECAL layers **/
  Int_t fNLayers;

  /** x-size of ecal in supermodules **/
  Int_t fXSize;
  /** y-size of ecal in supermodules **/
  Int_t fYSize;

  /** transverse supermodule size in cm **/
  Double_t fModuleSize;

  /** Thickness of PS lead [cm] **/
  Double_t fPSLead;
  /** Thickness of PS scintillator [cm] **/
  Double_t fPSScin;
  /** Gap between PS and ECAL [cm] **/
  Double_t fPSGap;

  /**Thickness of lead in one layer [cm] **/
  Double_t fLead;
  /**Thickness of scintillator in one layer [cm] **/
  Double_t fScin;
  /**Thickness of tyvec in one layer [cm] **/
  Double_t fTyveec;
  /**Total thickness of layer	[cm] **/
  Double_t fThicknessLayer;
  /**transverse size of ECAL cell for simulation [cm] **/
  Double_t fCellSize;
  /**Size of ECAL container [cm] **/
  Double_t fEcalSize[3];
  /**Size of ECAL beam hole [cm] **/
  Double_t fEcalHole[3];

  /**Electron cut for ECAL **/
  Double_t fECut;
  /**Hadron cut for ECAL **/
  Double_t fHCut;

  /**Flag to run Fast (1) or Full (0) MC code **/
  Int_t fFastMC;

  /** 1 if evething Ok **/
  Int_t fSuccess;

  TString fFileName;

  CbmEcalInf(const CbmEcalInf&);
  CbmEcalInf& operator=(const CbmEcalInf&);

  ClassDef(CbmEcalInf,2);
};

inline char CbmEcalInf::GetType(Int_t x, Int_t y) const
{
  /** Returns the type of the module with position (x,y) **/
  if (x<0||y<0||y>fEcalStr.GetLast()) return 0;
  TObjString* str=(TObjString*)fEcalStr.At(y);
  if (str->GetString().Length()<x) return 0;
  char stri[2]={str->GetString()[x],0};
  return atoi(stri);
}

inline void CbmEcalInf::FreeInstance()
{
	fRefCount--;
	if (fRefCount==0)
	{
		delete this;
		fInf=NULL;
	}
}
#endif
