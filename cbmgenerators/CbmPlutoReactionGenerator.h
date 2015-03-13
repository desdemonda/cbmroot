// -------------------------------------------------------------------------
// -----             CbmPlutoReactionGenerator header file            -----
// -----          Created 05/07/08  by I. Froehlich                    -----
// -------------------------------------------------------------------------

/** CbmPlutoReactionGenerator.h
 *@author I. Froehlich
 *
 The CbmPlutoReactionGenerator inserts an inline PReaction
 and thus connects Pluto with the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef FAIR_PLUTOREACTIONGENERATOR_H
#define FAIR_PLUTOREACTIONGENERATOR_H

#include "FairGenerator.h"              // for FairGenerator

#include "PUtils.h"                     // for PLUTOVERSION_FOR_FAIR
/** Backward compatibility **/
#ifdef PLUTOVERSION_FOR_FAIR
#include "PFairGenerator.h"             // IWYU pragma: keep
#include "PReaction.h"                  // IWYU pragma: keep 
#else
class PReaction;
class PFairGenerator;
#endif

#include "Rtypes.h"

class FairPrimaryGenerator;

class CbmPlutoReactionGenerator : public FairGenerator
{

  public:

    /** Default constructor (should not be used) **/
    CbmPlutoReactionGenerator();


    /** Standard constructor
     ** @param reaction Pointer to PLUTO inline reaction
     **/
    CbmPlutoReactionGenerator(PReaction* reaction);


    /** Destructor **/
    virtual ~CbmPlutoReactionGenerator();


    /** Reads on event from the Pluto reaction and pushes the tracks onto
     ** the stack. Abstract method in base class.
     ** @param primGen  pointer to the FairPrimaryGenerator
     **/
    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);



  private:

    Int_t iEvent;      //! Event number
#ifdef PLUTOVERSION_FOR_FAIR
    PFairGenerator* fPlutoFairGenerator; //!Pointer to interface
#endif
    PReaction* fReaction;     //! Pointer to the Pluto inline reaction

    CbmPlutoReactionGenerator(const CbmPlutoReactionGenerator&);
    CbmPlutoReactionGenerator operator=(const CbmPlutoReactionGenerator&);

    ClassDef(CbmPlutoReactionGenerator,1);

};

#endif
