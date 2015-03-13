/** LitCudaDetectorGeometry.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Classes for geometry description for the Littrack CUDA
 * version of the tracking.
 **/

#ifndef LITCUDADETECTORGEOMETRY_H_
#define LITCUDADETECTORGEOMETRY_H_

//#include "LitCudaTypes.h"
#include "LitCudaMaterialInfo.h"
#include "LitCudaField.cu"
//#include "base/CbmLitEnums.h"

#include <iostream>

const unsigned char MAX_NOF_STATION_GROUPS = 6;
const unsigned char MAX_NOF_STATIONS = 4;
const unsigned char MAX_NOF_SUBSTATIONS = 2;

struct LitCudaSubstation {
   float Z;
   LitCudaMaterialInfo material;
   LitCudaFieldSlice fieldSlice;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaSubstation& substation ) {
      strm << "LitCudaSubstation: " << "Z=" << substation.Z << ", material=" << substation.material;
//    strm << ", fieldSlice=" << substation.fieldSlice;
      return strm;
   }
};



struct LitCudaStation {
   LitCudaStation():nofSubstations(0) {}

   void AddSubstation(const LitCudaSubstation* substation) {
      substations[nofSubstations++] = *substation;
   }

// unsigned char GetNofSubstations() const {
//    return nofSubstations;
// }

   // Type of hits on the station
// LitHitType type;
   unsigned char type;
   // array with substations in the station
   LitCudaSubstation substations[MAX_NOF_SUBSTATIONS];
   // number of substations
   unsigned char nofSubstations;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaStation& station) {
      strm << "LitCudaStation: type" << station.type << ", nofSubstations=" << station.nofSubstations << std::endl;
      for (int i = 0; i < station.nofSubstations; i++) {
         strm << "    " << i << station.substations[i];
      }
      return strm;
   }

};



struct LitCudaAbsorber {
   float Z;
   LitCudaMaterialInfo material;
   LitCudaFieldSlice fieldSliceFront;
   LitCudaFieldSlice fieldSliceBack;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaAbsorber& absorber) {
      strm << "LitCudaAbsorber: Z" << absorber.Z << ", material=" << absorber.material;
//    strm << "fieldSliceFront=" << absorber.fieldSliceFront
//       << " fieladSliceBack=" << absorber.fieldSliceBack;
      return strm;
   }
};



struct LitCudaStationGroup {
   LitCudaStationGroup():nofStations(0) {}

// virtual ~LitCudaStationGroup() {}

   void AddStation(const LitCudaStation* station) {
      stations[nofStations++] = *station;
   }

// unsigned char GetNofStations() const {
//    return nofStations;
// }

   // array with stations in the station group
   LitCudaStation stations[MAX_NOF_STATIONS];
   // number of stations in the station group
   unsigned char nofStations;
   // absorber
   LitCudaAbsorber absorber;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaStationGroup& stationGroup) {
      strm << "LitCudaStationGroup: " << "nofStations=" << stationGroup.nofStations << std::endl;
      for (unsigned char i = 0; i < stationGroup.nofStations; i++) {
         strm << "  " << i << stationGroup.stations[i];
      }
      strm << "  " << stationGroup.absorber;
      return strm;
   }
};



struct LitCudaDetectorLayout {
   LitCudaDetectorLayout():nofStationGroups(0) {};

   void AddStationGroup(const LitCudaStationGroup& stationGroup) {
      stationGroups[nofStationGroups++] = stationGroup;
   }

// unsigned char GetNofStationGroups() const {
//    return nofStationGroups;
// }

// unsigned char GetNofStations(unsigned char stationGroup) const {
//    return stationGroups[stationGroup].GetNofStations();
// }
//
// unsigned char GetNofSubstations(unsigned char stationGroup, unsigned char station) const {
//    return stationGroups[stationGroup].stations[station].GetNofSubstations();
// }
//
// const LitCudaStationGroup& GetStationGroup(unsigned char stationGroup) {
//    return stationGroups[stationGroup];
// }
//
// const LitCudaStation& GetStation(unsigned char stationGroup, unsigned char station) {
//    return stationGroups[stationGroup].stations[station];
// }
//
// const LitCudaSubstation& GetSubstation(unsigned char stationGroup, unsigned char station, unsigned char substation){
//    return stationGroups[stationGroup].stations[station].substations[substation];
// }

   // array with station groups
   LitCudaStationGroup stationGroups[MAX_NOF_STATION_GROUPS];
   //number of station groups
   unsigned char nofStationGroups;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaDetectorLayout& layout) {
      strm << "LitCudaDetectorLayout: " << "nofStationGroups=" << layout.nofStationGroups << std::endl;
      for (unsigned char i = 0; i < layout.nofStationGroups; i++) {
         strm << i << layout.stationGroups[i];
      }
      return strm;
   }
};

#endif
