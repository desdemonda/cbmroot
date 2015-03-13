/** @file CbmStsSensorType.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#ifndef CBMSTSSENSORTYPE_H
#define CBMSTSSENSORTYPE_H 1

#include <vector>
#include "TNamed.h"
#include "CbmStsCluster.h"

class CbmStsSensor;
class CbmStsSensorPoint;


/** @class CbmStsSensorType
 ** @brief Abstract base class for description of the functional
 ** behaviour of sensors in the STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** Concrete classes deriving from CbmStsSensorType describe the
 ** production of charge by a CbmStsSensorPoint (charged particle
 ** traversing the sensor). The must implement the abstract
 ** method ProcessPoint.
 **/
class CbmStsSensorType : public TNamed
{

  public:

    /** Constructor  **/
    CbmStsSensorType() : fTypeId(-1) { };


    /** Destructor  **/
    virtual ~CbmStsSensorType() { };


    /** Find hits from clusters
     ** @param clusters  vector with clusters
     ** @param sensor    pointer to sensor object
     ** @return Number of found hits
     **
     ** Perform the hit finding operation on an array of clusters
     ** and deliver the found hits to the sensor object. To be
     ** implemented for each concrete sensor type.
     **/
    virtual Int_t FindHits(std::vector<CbmStsCluster*>& clusters,
    		                   CbmStsSensor* sensor) = 0;


    /** Type identifier **/
    Int_t GetTypeId() const { return fTypeId; }


    /** Perform response simulation for one MC Point
     **
     ** @param point   Pointer to CbmStsSensorPoint with relevant parameters
     ** @param sensor  Pointer to CbmStsSensor object
     ** @param link    Link to MCPoint
     ** @return  Status variable, depends on concrete type
     **
     ** Perform the appropriate action for a particle trajectory in the
     ** sensor characterised by the CbmStsSensorPoint object.
     **/
    virtual Int_t ProcessPoint(CbmStsSensorPoint* point,
                               const CbmStsSensor* sensor) const = 0;


  private:

    Int_t fTypeId;  ///< Unique type identifier



  ClassDef(CbmStsSensorType,1);

};




#endif
