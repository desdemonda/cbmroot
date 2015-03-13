/** CbmMuchSegmentManual.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 02.04.09
 *@version 1.0
 *
 * class for making parameter file for MUCH digitizer
 *
 */

#ifndef CBMMUCHSEGMENTMANUAL_H
#define CBMMUCHSEGMENTMANUAL_H 1

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

class CbmMuchModuleGemRectangular;
class CbmMuchSectorRectangular;
class CbmMuchLayerSide;
class CbmGeoMuchPar;
class TObjArray;

class CbmMuchSegmentManual : public FairTask {

  public:
    /** Default constructor. */
    CbmMuchSegmentManual();

    /** Destructor. */
    virtual ~CbmMuchSegmentManual();

    /** Standard constructor.
     * @param inputFile     Name of the input parameters file
     * @param digiFileName  Name of the output file with segmentation parameters
     */
    CbmMuchSegmentManual(char* inputFile, char* digiFileName);

    /** Initialization. */
    virtual InitStatus Init();

    /** Performs the segmentation of all stations. */
    void SegmentMuch();

    /** Sets number of stations.
     *@param nStations  Number of stations
     */
    void SetNStations(Int_t nStations) {fNStations = nStations;}

    /** Sets number of circled regions for the given station.
     * @param iStation  Station index
     * @param nRegions  Number of regions in the station
     */
    void SetNRegions(Int_t iStation, Int_t nRegions);

    /** Sets the radius for the given circled region in the given station.
     * @param iStation Station index
     * @param iRegion  Region index
     * @param radius   Radius of the region
     */
    void SetRegionRadius(Int_t iStation, Int_t iRegion, Double_t radius);

    /** Sets number of channels per sector for the given station.
     * @param iStation   Station index
     * @param nChannels  Number of channels
     */
    void SetNChannels(Int_t iStation, Int_t nChannels);

    /** Sets resolution for the given region of the given station.
     * @param iStation Station index
     * @param iRegion  Region index
     * @param sigmaX   Resolution in X direction
     * @param sigmaY   Resolution in Y direction
     */
    void SetSigma(Int_t iStation, Int_t iRegion, Double_t sigmaX, Double_t sigmaY);

    /** Sets pad size for the given region of the given station.
     * @param iStation Station index
     * @param iRegion  Region index
     * @param padLx    Pad width
     * @param padLy    Pad length
     */
    void SetPadSize(Int_t iStation, Int_t iRegion, Double_t padLx, Double_t padLy);

    void DebugSwitchOn() {fDebug = 1;}

  private:
    CbmGeoMuchPar*                fGeoPar;         // Geometry parameters container
    Int_t                         fNStations;      // Number of stations
    TObjArray*                    fStations;       // Array of stations
    Char_t*                       fInputFileName;  // Name of the file with input parameters
    Char_t*                       fDigiFileName;   // Name of the file with segmentation parameters
    map<Int_t, Int_t>             fNRegions;       // Map from a station index to a number of circled regions in the station
    map<Int_t, vector<Double_t> > fRadii;          // Map from a station index to a vector of circled regions radii
    map<Int_t, vector<Double_t> > fSecLx;          // Map from a station index to a vector of sector widths for each region
    map<Int_t, vector<Double_t> > fSecLy;          // Map from a station index to a vector of sector lengths for each region
    map<Int_t, Int_t>             fNChannels;      // Map from a station index to a number of channels per sector
    map<Int_t, vector<Int_t> >    fNCols;          // Map from a station index to a vector of number of columns in a sector
    map<Int_t, vector<Int_t> >    fNRows;          // Map from a station index to a vector of number of rows in a sector
    Int_t fDebug;  // Debug info switch

    /** Get parameter containers **/
    virtual void SetParContainers();

    /**
     * Checks whether the given module is intersected by the circle of the given radius.
     * @param module Module to verify
     * @param radius Radius of the circle
     * @return       0 if the module is outside the circle, 1 if it is intersected by
     *               the circle, 2 if it is contained in the circle
     */
    Int_t IntersectsRad(CbmMuchModuleGemRectangular* module, Double_t radius);

    /**
     * Checks whether the given sector is intersected by the circle of the given radius.
     * @param sector Sector to verify
     * @param radius Radius of the circle
     * @return       0 if the sector is outside the circle, 1 if it is intersected by
     *               the circle, 2 if it is contained in the circle
     */
    Int_t IntersectsRad(CbmMuchSectorRectangular* sector, Double_t radius);

    /**
     * Performs segmentation of the given layer side.
     * @param layerSide layer side to segment
     */
    void SegmentLayerSide(CbmMuchLayerSide* layerSide);

    /**
     * Performs segmentation of the given module.
     * @param module          Module to segment
     * @param useModuleDesign Whether module design is used
     */
    void SegmentModule(CbmMuchModuleGemRectangular* module, Bool_t useModuleDesign);

    /**
     * Performs segmentation of the given sector in the module.
     * @param module  Module which contains the given sector
     * @param sector  Sector to segment
     */
    void SegmentSector(CbmMuchModuleGemRectangular* module, CbmMuchSectorRectangular* sector);

    /**
     * Gets maximum sector size for the given module.
     * @param module  Module to process
     * @param side    Defines which side of a sector should be returned
     * @param iRegion Region index containing the given module
     * @return        Maximum sector length, if side="Length", maximum
     *                sector width, if side="Width"
     */
    Double_t GetSectorMaxSize(CbmMuchModuleGemRectangular* module, const TString side, Int_t &iRegion);

    /**
     * Gets maximum pad size for the given module.
     * @param module Module to process
     * @param side   Defines which side of a pad should be returned
     * @return       maximum pad length, if side="Length", maximum
     *               pad width, if side="Width"
     */
    Double_t GetPadMaxSize(CbmMuchModuleGemRectangular* module, const TString side);

    /**
     * Checks whether the given sector should be segmented in the given direction.
     * @param sector    Sector to verify
     * @param direction Direction to segment in ("X" or "Y")
     * @param iRegion   Region index of the sector
     * @return          true if segmentation is required, false otherwise
     */
    Bool_t ShouldSegment(CbmMuchSectorRectangular* sector, const TString direction, Int_t &iRegion);

    /**
     * Gets region index for the given sector.
     * @param sector Sector
     * @return Index of the region for the sector
     */
    Int_t GetRegionIndex(CbmMuchSectorRectangular* sector);

    /** Determines whether the given sector is incomplete.
     * @param sector Sector to process
     * @return true if the sector is incomplete, false otherwise
     */
    Bool_t IsIncompleteSector(CbmMuchSectorRectangular* sector);

    /** Draws segmented stations. */
    void DrawSegmentation();

    /** Prints summary. */
    void Print();

    /** Reads input parameters for the segmentation. */
    void ReadInputFile();



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

    CbmMuchSegmentManual(const CbmMuchSegmentManual&);
    CbmMuchSegmentManual& operator=(const CbmMuchSegmentManual&);

    ClassDef(CbmMuchSegmentManual,1)
};

#endif
