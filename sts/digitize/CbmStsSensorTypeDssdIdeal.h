/** @file CbmStsSensorTypeDssdIdeal.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#ifndef CBMSTSSENSORTYPEDSSDIDEAL_H
#define CBMSTSSENSORTYPEDSSDIDEAL_H 1


#include "CbmStsSensorTypeDssd.h"


/** @class CbmStsSensorTypeDssdIdeal
 ** @brief Class describing double-sided silicon strip sensors with ideal response.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** This class describes the ideal response of double-sided silicon
 ** strip sensors in the STS. All charge is collected on one strip on
 ** both the front and the back side. For more information, see
 ** the mother class CbmStsSensorTypeDssd.
 **
 **/



class CbmStsSensorTypeDssdIdeal : public CbmStsSensorTypeDssd
{

  public:

    /** Constructor  **/
    CbmStsSensorTypeDssdIdeal();


    /** Destructor  **/
    virtual ~CbmStsSensorTypeDssdIdeal() { };




  private:

    /** Produce charge on front or back side from a CbmStsPoint
     ** @param point  Pointer to CbmStsSensorType object
     ** @param side   0 = front, 1 = back side
     ** @param sensor Pointer to sensor object
     ** @return  Number of generated charge signals (active strips)
     **
     ** Re-implements CbmStsSensorTypeDssd::ProduceCharge
     **/
    virtual Int_t ProduceCharge(CbmStsSensorPoint* point, Int_t side,
                                const CbmStsSensor* sensor) const;




    ClassDef(CbmStsSensorTypeDssdIdeal,1);

};


#endif
