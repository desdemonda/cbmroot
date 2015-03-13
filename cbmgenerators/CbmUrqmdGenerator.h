// -------------------------------------------------------------------------
// -----                FairUrqmdGenerator header file                  -----
// -----          Created 11/06/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------


/** FairUrqmdGenerator.h
 *@ author V.Friese <v.friese@gsi.de>
 *@author D.Bertini <d.bertini@gsi.de>
 *
 The FairUrqmdGenerator reads the output file 14 (ftn14) from UrQMD. The UrQMD 
 calculation has to be performed in the CM system of the collision; Lorentz
 transformation into the lab is performed by this class.
 Derived from FairGenerator.
**/



#ifndef CBMURQMDGENERATOR_H
#define CBMURQMDGENERATOR_H 1


#include "FairGenerator.h"

#include <fstream>
#include <map>
 
class TVirtualMCStack;
class FairPrimaryGenerator;


class CbmUrqmdGenerator : public FairGenerator
{

 public:

  /** Default constructor without arguments should not be used. **/
  CbmUrqmdGenerator();


  /** Standard constructor.
   * @param fileName The input file name
   **/
  CbmUrqmdGenerator(const char* fileName);


  /** Destructor. **/
  virtual ~CbmUrqmdGenerator();


  /** Public method SetEventPlane 
   **@param phiMin   Lower limit for event plane angle [rad]
   **@param phiMax   Upper limit for event plane angle [rad]
   **If set, an event plane angle will be generated with flat
   **distrtibution between phiMin and phiMax. 
   **/
  void SetEventPlane(Double_t phiMin, Double_t phiMax);


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param pStack    pointer to the stack
   ** @param ver       not used
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);


  //Skip some events in file
  Bool_t SkipEvents(Int_t count);
   
 private:

  FILE* fInputFile;                     //!  Input file

  std::map<Int_t,Int_t> fParticleTable;      //!  Map from UrQMD PID to PDGPID

  const Char_t * fFileName;             //!  Input file name

  Double32_t fPhiMin, fPhiMax;          // Limits of event plane angle
  Bool_t     fEventPlaneSet;            // Flag whether event plane angle is used

  /** Private method ReadConversionTable. Reads the conversion table
      from UrQMD particle code to PDG particle code and fills the
      conversion map. Is called from the constructor. **/
  void ReadConversionTable();

  CbmUrqmdGenerator(const CbmUrqmdGenerator&);
  CbmUrqmdGenerator& operator=(const CbmUrqmdGenerator&);

  ClassDef(CbmUrqmdGenerator,1);
 
};

#endif
   

