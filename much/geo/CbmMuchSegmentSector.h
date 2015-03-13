/** CbmMuchSegmentSector.h
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 05.02.12
 *@version 1.0
 *
 * class for making parameter file for MUCH digitizer
 * sector version
 */

#ifndef CbmMuchSegmentSector_H
#define CbmMuchSegmentSector_H 1

#include "FairTask.h"

#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>


using std::map;
using std::vector;
using std::string;
using std::stringstream;
using std::istream_iterator;

class CbmMuchModuleGemRadial;
class CbmMuchSectorRadial;
class CbmMuchLayerSide;
class CbmGeoMuchPar;
class TObjArray;

class CbmMuchSegmentSector : public FairTask {

  public:
    /** Default constructor. */
    CbmMuchSegmentSector();

    /** Destructor. */
    virtual ~CbmMuchSegmentSector();

    /** Standard constructor.
     * @param inputFile     Name of the input parameters file
     * @param digiFileName  Name of the output file with segmentation parameters
     */
    CbmMuchSegmentSector(char* inputFile, char* digiFileName);

    /** Initialization. */
    virtual InitStatus Init();

    /** Performs the segmentation of all stations. */
    void SegmentMuch();

    void DebugSwitchOn() {fDebug = 1;}

  private:
    CbmGeoMuchPar*                fGeoPar;         // Geometry parameters container
    Int_t                         fNStations;      // Number of stations
    TObjArray*                    fStations;       // Array of stations
    Char_t*                       fInputFileName;  // Name of the file with input parameters
    Char_t*                       fDigiFileName;   // Name of the file with segmentation parameters
    map<Int_t, Int_t>             fNRegions;       // Map from a station index to a number of circled regions in the station
    map<Int_t, vector<Double_t> > fRadii;          // Map from a station index to a vector of circled regions radii
    map<Int_t, vector<Double_t> > fAngles;         // Map from a station index to a vector of single pad angles for given regions
    map<Int_t, vector<Double_t> > fSecLx;          // Map from a station index to a vector of sector widths for each region
    map<Int_t, vector<Double_t> > fSecLy;          // Map from a station index to a vector of sector lengths for each region
    map<Int_t, Int_t>             fNChannels;      // Map from a station index to a number of channels per sector
    map<Int_t, vector<Int_t> >    fNCols;          // Map from a station index to a vector of number of columns in a sector
    map<Int_t, vector<Int_t> >    fNRows;          // Map from a station index to a vector of number of rows in a sector
    Int_t fDebug;  // Debug info switch

    /** Get parameter containers **/
    virtual void SetParContainers();

    /**
     * Performs segmentation of the given layer side.
     * @param layerSide layer side to segment
     * @return number of segmented sectors
     */
    Int_t SegmentLayerSide(CbmMuchLayerSide* layerSide);

    /**
     * Performs segmentation of the given module.
     * @param module          Module to segment
     * @param useModuleDesign Whether module design is used
     */
    Int_t SegmentModule(CbmMuchModuleGemRadial* module, Bool_t useModuleDesign);

    /**
     * Performs segmentation of the given sector in the module.
     * @param module  Module which contains the given sector
     * @param sector  Sector to segment
     */
    void SegmentSector(CbmMuchModuleGemRadial* module, CbmMuchSectorRadial* sector);

    /** Reads input parameters for the segmentation. */
    void ReadInputFile();

    /** Draw segmentation */
    void DrawSegmentation();

    // -------------------- Methods for working with strings --------------------------
    void Trim(string& str)
    {
      string::size_type pos1 = str.find_first_not_of(' ');
      string::size_type pos2 = str.find_last_not_of(' ');
      str = str.substr(pos1 == string::npos ? 0 : pos1,
        pos2 == string::npos ? str.length() - 1 : pos2 - pos1 + 1);
    }

    Bool_t IsDummyLine(string& str){
      Trim(str);
      return str[0] == '#' || str.length() == 0 || str[0] == '\0' || str[0] == '\n';
    }

    void OmitDummyLines(std::ifstream &infile, string &str){
      getline(infile, str);
      while(IsDummyLine(str)) getline(infile, str);
    }

    vector<std::string> &Split(const string &s, char delim, vector<string> &elems) {
        stringstream ss(s);
        string item;
        while(getline(ss, item, delim)) {
          if(item.length() != 0) elems.push_back(item);
        }
        return elems;
    }


    vector<string> Split(const string &s, char delim) {
        vector<string> elems;
        return Split(s, delim, elems);
    }

    template<class T>
    void StrToNum(string &str, T &number){
      try
      {
          stringstream ss(str);
          if ((ss >> number).fail() || !(ss >> std::ws).eof())
              throw std::bad_cast();
      }
      catch(std::bad_cast exc)
      {
          Fatal("","Invalid cast.\n");
      }
    }
    // --------------------------------------------------------------------------------

    CbmMuchSegmentSector(const CbmMuchSegmentSector&);
    CbmMuchSegmentSector& operator=(const CbmMuchSegmentSector&);

    ClassDef(CbmMuchSegmentSector,1)
};

#endif
