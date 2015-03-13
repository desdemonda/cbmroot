// -------------------------------------------------------------------------
// -----                CbmHsdGenerator header file                  -----
// -----          Created 11/06/04  by V. Friese / D.Bertini           -----
// -------------------------------------------------------------------------


/** CbmHsdGenerator.h
 *@author V.Friese <v.friese@gsi.de>
 *@author D.Bertini <d.bertini@gsi.de>
 *@author A.Kiseleva <a.kiseleva@gsi.de>
 The CbmHsdGenerator reads the output file from HSD. 
 Derived from FairGenerator.
**/



#ifndef FAIRHSDGENERATOR_H
#define FAIRHSDGENERATOR_H


#include "FairGenerator.h"

#include <fstream>
 
class TVirtualMCStack;
class FairPrimaryGenerator;


class CbmHsdGenerator : public FairGenerator
{

 public:

  /** Default constructor without arguments should not be used. **/
  CbmHsdGenerator();


  /** Standard constructor.
   * @param fileName The input file name
   **/
  CbmHsdGenerator(const char* fileName, TString particle);


  /** Destructor. **/
  ~CbmHsdGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param pStack    pointer to the stack
   ** @param ver       not used
   **/
  Bool_t ReadEvent(FairPrimaryGenerator* primGen);

 private:

  FILE* fInputFile;                     //!  Input file
  const Char_t * fFileName;             //!  Input file name
  Int_t fPid, fPdg;                     //!  pID of signal particle: fPid - HSD, fPdg - MC

  CbmHsdGenerator(const CbmHsdGenerator&);
  CbmHsdGenerator& operator=(const CbmHsdGenerator&);

  ClassDef(CbmHsdGenerator,1);
 
};

#endif
   

