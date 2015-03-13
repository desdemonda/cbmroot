// -------------------------------------------------------------------------
// -----                      CbmMvdMimosa34 header file               -----
// -----                  Created 10/12/14  by B. Linnik               -----
// -------------------------------------------------------------------------


/** CbmMvdMimosa34.h
 **
 ** Data base for the Properties of MIMOSA-34 
 **   
 **/


#ifndef CBMMVDMIMOSA34_H
#define CBMMVDMIMOSA34_H 1


#include "CbmMvdSensorDataSheet.h"



class CbmMvdMimosa34 : public CbmMvdSensorDataSheet
{

 public:

  /** Default constructor **/
  CbmMvdMimosa34();
    
   /** Destructor **/
  ~CbmMvdMimosa34();

 
  ClassDef(CbmMvdMimosa34,1);

};



#endif
