/** @file CbmStsSetup.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/


#ifndef CBMSTSSETUP_H
#define CBMSTSSETUP_H 1


#include <vector>
#include "setup/CbmStsSensor.h"
#include "setup/CbmStsElement.h"

class TGeoManager;
class CbmStsDigitize;



/** @class CbmStsSetup
 ** @brief Class representing the top level of the STS setup
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The CbmStsSetup is a singleton class and represents the
 ** interface to the CBM geometry and its elements.
 **/
class CbmStsSetup : public CbmStsElement
{

  public:

    /** Destructor **/
    virtual ~CbmStsSetup() { };


    /** Define available sensor types
     ** Poor man's sensor database, with hardcoded values
     ** @value Number of available sensor types
     **/
    Int_t DefineSensorTypes();


    /** Get digitiser task **/
    CbmStsDigitize* GetDigitizer() const  { return fDigitizer; }


    /** Get an STS element by address
     ** @param address  Unique element address (see CbmStsAddress)
     ** @param level    Element level (EStsElementLevel)
     ** @return Pointer to STS element
     **/
    CbmStsElement* GetElement(UInt_t address, Int_t level);


    /** Get the name of an element level
     ** @param level    Element level (EStsElementLevel)
     ** @return  Name of element level (station, ladder, etc.)
     **/
    static const char* GetLevelName(Int_t level) {
      if ( level < 0 || level >= kStsNofLevels ) return "";
      return (fgkLevelName[level]).Data();
    }


    /** Get number of modules in setup **/
    Int_t GetNofModules() const { return fModules.size(); }


    /** Get number of sensors in setup **/
    Int_t GetNofSensors() const { return fSensors.size(); }


    /** Get a module from the module array.
     ** For convenient loops over all modules.
     ** Note that the index of the module is meaningless.
     ** @param  index  Index of module in the array
     ** @return  Pointer to module
     **/
    CbmStsModule* GetModule(Int_t index) { return fModules[index]; }


    /** Get a sensor from the array.
     ** For convenient loops over all sensors.
     ** Note that the index of the sensor is meaningless.
     ** @param  index  Index of sensor in the array
     ** @return  Pointer to sensor
     **/
    CbmStsSensor* GetSensor(Int_t index) { return fSensors[index]; }


    /** Initialise setup from geometry
     ** @param geo  Instance of TGeoManager
     ** @return kTRUE if successfully initialised; kFALSE else
     **/
    Bool_t Init(TGeoManager* geo);


    /** Static instance of CbmStsSetup **/
    static CbmStsSetup* Instance();


    /** Set the digitiser task  **/
    void SetDigitizer(CbmStsDigitize* digitizer) {
    	fDigitizer = digitizer; }


    /** Set sensor parameters
     ** Set the sensor parameters that are not contained in the geometry,
     ** but required for digitisation and reconstruction, like strip pitch,
     ** stereo angle etc.
     ** @value Number of sensors the type of which was set.
     **/
    Int_t SetSensorTypes();



  private:

    static CbmStsSetup* fgInstance;    ///< Static instance of this class
    CbmStsDigitize* fDigitizer;   ///< Pointer to digitizer task

    static const TString fgkLevelName[kStsNofLevels];  ///< Level names

    /** These arrays allow convenient loops over all modules or sensors. **/
    vector<CbmStsModule*> fModules;   ///< Array of modules
    vector<CbmStsSensor*> fSensors;   ///< Array of sensors

    /** Available sensor types **/
    map<Int_t, CbmStsSensorType*> fSensorTypes;

    /** Default constructor  **/
    CbmStsSetup();

    CbmStsSetup(const CbmStsSetup&);
    CbmStsSetup operator=(const CbmStsSetup&);

    ClassDef(CbmStsSetup,1);

};

#endif /* CBMSTSSETUP_H */
