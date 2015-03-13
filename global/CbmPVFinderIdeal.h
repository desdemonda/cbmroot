// -------------------------------------------------------------------------
// -----                   CbmPVFinderIdeal header file                -----
// -----                  Created 28/11/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmPVFinderIdeal
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Ideal primary vertex finder. It takes the start vertex of primary
 ** MCTracks.
 ** Warning: This works only if the MC input tracks really start from
 ** the primary vertex and are not displaced by hand.
 ** The primary vertex should ideally be taken from the appropriate
 ** MC class, which is not yet implemented.
 **/


#ifndef CBMPVFINDERIDEAL_H
#define CBMPVFINDERIDEAL_H 1


#include "CbmPrimaryVertexFinder.h"


class CbmPVFinderIdeal : public CbmPrimaryVertexFinder
{

 public:

  /** Default constructor  **/
  CbmPVFinderIdeal();


  /** Destructor **/
  virtual ~CbmPVFinderIdeal();


  /** Initialisation **/
  virtual void Init();


  /** Primary vertex finding **/
  virtual Int_t FindPrimaryVertex(TClonesArray* tracks,
				  CbmVertex* vertex);


 private:

  /** Array of MCTracks **/
  TClonesArray* fMCTracks;

  CbmPVFinderIdeal(const CbmPVFinderIdeal&);
  CbmPVFinderIdeal& operator=(const CbmPVFinderIdeal&);

  ClassDef(CbmPVFinderIdeal,1);

};

#endif
