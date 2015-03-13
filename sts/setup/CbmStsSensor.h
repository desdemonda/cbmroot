/** @file CbmStsSensor.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 03.05.2013
 **
 ** This class is to replace the CbmStsSensor, at which point it will be
 ** renamed to spell correctly.
 **/

#ifndef CBMSTSSENSOR_H
#define CBMSTSSENSOR_H 1


#include <vector>
#include <string>
#include "CbmStsAddress.h"
#include "CbmStsCluster.h"
#include "setup/CbmStsElement.h"
#include "setup/CbmStsSensorConditions.h"
#include "setup/CbmStsSensorType.h"

class TClonesArray;
class TGeoPhysicalNode;
class CbmLink;
class CbmStsModule;
class CbmStsPoint;
class CbmStsSensorType;

using std::vector;


/** @class CbmStsSensor
 ** @brief Class representing an instance of a sensor in the CBM-STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 2.0
 **
 ** The sensor is the smallest geometric element in the STS setup.
 ** It is the daughter node of a module, which may contain one sensor
 ** or several daisy-chained ones. The sensor class represents
 ** the physical node through its member fNode, but also performs detector
 ** response simulation through its member fType.
 ** After being instantiated by the initialisation of CbmStsSetup,
 ** the sensor type must be assigned to the sensor. This is currently done
 ** by the digitiser task.
 ** The sensor class performs the coordinate transformation from the
 ** global system to the sensor system, having the sensor midpoint as origin.
 ** The analog response is then modelled by the CbmStsSensorType object.
 **/
class CbmStsSensor : public CbmStsElement
{

  public:

    /** Constructor  **/
    CbmStsSensor();


    /** Standard constructor
     ** @param name   Name
     ** @param title  Title
     ** @param node   Pointer to relevant TGeoPhysicalNode
     */
    CbmStsSensor(const char* name, const char* title,
                 TGeoPhysicalNode* node = NULL);


    /** Destructor  **/
    virtual ~CbmStsSensor() { };


    /** Create a new hit in the output array form two clusters
     ** @param xLocal   hit x coordinate in sensor system
     ** @param yLocal   hit y coordinate in sensor system
     ** @param clusterF pointer to front side cluster
     ** @param clusterB pointer to back side cluster
     **/
    void CreateHit(Double_t xLocal, Double_t yLocal,
    		           CbmStsCluster* clusterF, CbmStsCluster* clusterB);


    /** Find hits in sensor
     ** @param clusters  Vector of clusters
     ** @param hitArray  TClonesArray to store the hits in
     ** @return Number of created hits
     **/
    Int_t FindHits(vector<CbmStsCluster*>& clusters,
    		           TClonesArray* hitArray);


    /** Sensor conditions
     ** @return Pointer to sensor condition object
     **/
    const CbmStsSensorConditions& GetConditions() const { return fConditions; }


    /** Current link object
     ** @return Pointer to current link object (to CbmStsPoint)
     **/
    CbmLink* GetCurrentLink() const { return fCurrentLink; }


    /** Get mother module **/
    CbmStsModule* GetModule() const;


  	/** Get physical node
  	 ** @return Pointer to TGeoPhysicalNode of sensor
     **/
  	TGeoPhysicalNode* GetNode() const {return fNode;}


    /** Get the sensor Id within the module  **/
    Int_t GetSensorId() const {
      return CbmStsAddress::GetElementId(fAddress, kStsSensor); }


    /** Pointer to sensor type **/
    CbmStsSensorType* GetType() const { return fType; }


    /** Process one MC Point
     ** @param point  Pointer to CbmStsPoint object
     ** @return  Status variable, depends on sensor type
     **
     ** Perform the appropriate action for a particle trajectory in the
     ** sensor characterised by the CbmStsPoint object
     **/
    Int_t ProcessPoint(const CbmStsPoint* point, CbmLink* link = NULL);


    /** Set the sensor address
     ** @param address  Sensor address
     **/
    void SetAddress(UInt_t address) { fAddress = address; }


    /** Set the sensor conditions
     ** @param conditions  Condition object
     **/
     void SetConditions(CbmStsSensorConditions& conditions) {
    	fConditions = conditions;
    }


    /** Set the sensor type
     ** @param Pointer to sensor type object
     **/
    void SetType(CbmStsSensorType* type) { fType = type; }


  private:

    CbmStsSensorType*       fType;        ///< Pointer to sensor type
    CbmStsSensorConditions  fConditions;  ///< Operating conditions
    CbmLink* fCurrentLink;  ///< Link to currently processed MCPoint
    TClonesArray* fHits;    ///<  Output array for hits. Used in hit finding.

    /** Prevent usage of copy constructor and assignment operator **/
    CbmStsSensor(const CbmStsSensor&);
    CbmStsSensor& operator=(const CbmStsSensor&);


    ClassDef(CbmStsSensor,2);

};


#endif
