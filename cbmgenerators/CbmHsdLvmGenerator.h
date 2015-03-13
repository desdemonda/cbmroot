/** CbmHsdGenerator.h
 **@author V.Friese <v.friese@gsi.de>
 **@author A.Kiseleva <a.kiseleva@gsi.de>
 **@since 12.05.2009 
 ** The FairHsdGenerator reads a special output file from HSD
 ** containing vector mesons. Their momenta are given in the CM system.
 ** It performs the decay into electron or muon pairs as input
 ** for transport simulation.
 **/

#ifndef CBMHSDLVMGENERATOR_H
#define CBMHSDLVMGENERATOR_H 1


#include <fstream>

#include "FairGenerator.h"

class FairPrimaryGenerator;


class CbmHsdLvmGenerator : public FairGenerator
{

 public:

  /** Default constructor without arguments should not be used. **/
  CbmHsdLvmGenerator();


  /** Standard constructor.
   **@param fileName   Input file name
   **@param pBeam      Beam momentum (GeV/u) 
   **@param decayMode  Deacy Mode (1 = e+e-, 2 = mu+mu-)
   **/
  CbmHsdLvmGenerator(const char* fileName, Double_t pBeam, Int_t decayMode);


  /** Destructor. **/
  virtual ~CbmHsdLvmGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen    Pointer to PrimaryGenerator class
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);



 private:

  FILE* fInputFile;                   //!  Input file
  const Char_t * fFileName;           //   Input file name
  Int_t fDecayMode;                   //   Decay mode
  Double_t fBetaCm;                   //   beta  for transformation CM -> lab
  Double_t fGammaCm;                  //   gamma for transformation CM -> lab
  Double_t fMd;                       //   Daughter mass (GeV)
  Int_t fPid1;                        //   PDG ID of daughter 1
  Int_t fPid2;                        //   PDG ID of daughter 2
  Int_t fCurrentEvent;                //   Current event ID

  CbmHsdLvmGenerator(const CbmHsdLvmGenerator&);
  CbmHsdLvmGenerator& operator=(const CbmHsdLvmGenerator&);

  ClassDef(CbmHsdLvmGenerator,1);
 
};

#endif
   

