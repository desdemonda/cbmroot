/** @file CbmStsSetup.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 10.06.2013
 **/

#ifndef CBMSETUP_H
#define CBMSETUP_H 1


#include <map>
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

class FairModule;
class FairRunSim;
class CbmFieldMap;

using namespace std;



class CbmSetup : public TObject
{
  public:


    /** Default constructor **/
 CbmSetup() 
   : TObject(), 
    fGeoTags(), 
    fActive(), 
    fFieldTag(""), 
    fFieldScale(1.0),
    fFieldPosition(0., 0., 0.)
    { };


    /** Constructor with pre-defined setup
     ** @param  setup   Name of setup
     **/
 CbmSetup(const char* setup) 
   : TObject(), 
    fGeoTags(), 
    fActive(), 
    fFieldTag(""), 
    fFieldScale(1.0),
    fFieldPosition(0., 0., 0.)
    {
      ReadSetup(setup);
    };
  

    /** Destructor **/
    ~CbmSetup() { };


    /** Initialisation
     ** @param run  Pointer to FairRunSim instance
     ** The availability of the input files is checked, and if given,
     ** modules and field are instantiated and registered to the run.
     ** If the argument is not specified or NULL, only a check of the
     ** availability of the input files is performed.
     **/
    void Init(FairRunSim* run = NULL);


    /** Output to screen  **/
    virtual void Print(Option_t* opt = "");


    /** Remove a module from the current setup
     ** @param moduleId  Module identifier (enum SystemId or kMagnet etc.)
     **/
    void RemoveModule(Int_t moduleId);


    /** Activate a module (detector)
     ** @param moduleId  Module identifier. SystemId for detectors, or kMagnet, kPipe, kTarget
     ** @param active    Activity tag for module (only in case of detectors)
     **
     ** Activate or deactivate a detector already present in the setup.
     ** The method will have no effect if called for a passive module (target,
     ** pipe, magnet). If a detector is flagged active,
     ** its ProcessHits method will be called during the transport simulation.
     **/
    void SetActive(Int_t moduleId, Bool_t active = kTRUE);


    /** Set the magnetic field map
     ** @param tag   Field map tag
     ** @param scale Field scaling factor
     **
     ** The magnetic field map is automatically selected according to the
     ** magnet geometry version. The user can, however, override this by
     ** choosing a different field map. In this case, consistency between
     ** field map and magnet geometry is within the responsibility of the
     ** user.
     **/
    void SetField(const char* tag, Double_t scale = 1.);


    /** Set the field scaling factor
     ** @param scale  Field scaling factor
     **
     ** The currently selected field map will be scaled by the specified
     ** factor.
     **/
    void SetFieldScale(Double_t scale) { fFieldScale = scale; }


    /** Add a module to the setup
     ** @param moduleId  Module identifier. SystemId for detectors, or kMagnet, kPipe, kTarget
     ** @param geoTag    Geometry version for module
     ** @param active    Activity tag for module (only in case of detectors)
     **
     ** The module / detector will be added to the setup. If a detector is flagged active,
     ** its ProcessHits method will be called during the transport simulation.
     **/
    void SetModule(Int_t moduleId, const char* geoTag, Bool_t active = kTRUE);



  private:

    map<Int_t, TString> fGeoTags;           ///< Geometry tags for the modules
    map<Int_t, Bool_t>  fActive;            ///< Activity tags for the modules
    TString             fFieldTag;          ///< Version tag for magnetic field
    Double_t            fFieldScale;        ///< Field scaling factor
    TVector3            fFieldPosition;     ///< Position of field map origin


    CbmFieldMap* CreateFieldMap();

    FairModule* CreateModule(Int_t moduleId, Bool_t active);

    Bool_t GetFile(Int_t moduleId, TString& fileName);

    Bool_t GetMagnetPosition(const char* geoTag,
                             Double_t& x, Double_t& y, Double_t& z);


    /** Read a predefined setup from file
     ** @param setupName  Name tag of setup
     ** The setup file defines the geometry flags for all modules
     ** in the setup.
     **/
    void ReadSetup(const char* setupName);


    ClassDef(CbmSetup, 1);

};

#endif /* CBMSETUP_H */
