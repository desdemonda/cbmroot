/** @file CbmStsStation.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 4.03.2015
 **/

#ifndef CBMSTSSTATION_H
#define CBMSTSSTATION_H 1


#include "CbmStsElement.h"


/** @class CbmStsStation
 ** @brief Class representing a station of the StsSystem.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The StsStation class represents one level in the STS element hierarchy.
 ** It holds station-wide parameters like z position, sensor thickness,
 ** and material budget for use in reconstruction tasks.
 **/
class CbmStsStation : public CbmStsElement {

	public:

		/** Default constructor **/
		CbmStsStation();


		/** Standard constructor
		 ** @param name   Station name
		 ** @param title  Station type
		 ** @param node   Pointer to corresponding TGeoPhysicalNode
		 */
		CbmStsStation(const char* name, const char* title,
						TGeoPhysicalNode* node = NULL);


		/** Destructor **/
		virtual ~CbmStsStation();


		/** Minimal and maximal x and y coordinates **/
		Double_t GetXmin() const { return fXmin; }
		Double_t GetXmax() const { return fXmax; }
		Double_t GetYmin() const { return fYmin; }
		Double_t GetYmax() const { return fYmax; }


		/** Sensor thickness
   	 ** @value Thickness (extension in z) of first sensor [cm]
   	 **
     ** This return the thickness of the first sensor in the station.
		 ** Needed by L1/KF.
		 ** Note that in general, sensors of different thicknesses can be
		 ** in the station. In this case, a warning message is issued when
		 ** the station properties are initialised.
	   **/
		Double_t GetSensorD() const { return fSensorD; }


		/** Get sensor strip pitch
		 ** @param iSide  Sensor side (0 = front, 1 = back)
		 ** @value Strip pitch [cm]
   	 **
     ** This return the strip pitch of the first sensor in the station.
     ** Valid only for sensors of type DSSD.
		 ** Needed by L1/KF.
		 ** Note that in general, sensors of different pitches can be
		 ** in the station.
		 **/
		Double_t GetSensorPitch(Int_t iSide) const;


		/** Get sensor stereo angle
		 ** @param iSide  Sensor side (0 = front, 1 = back)
		 ** @value Stereo angle [degrees]
   	 **
     ** This return the strip stereo angle of the first sensor in the station.
     ** Valid only for sensors of type DSSD.
		 ** Needed by L1/KF.
		 ** Note that in general, sensors of different stereo angles can be
		 ** in the station.
		 **/
		Double_t GetSensorStereoAngle(Int_t iSide) const;



		/** Sensor rotation
		 ** @value Rotation of first sensor in station in global C.S. [rad]
		 **
     ** This return the rotation angle in the x-y plane of the first sensor
     ** in the station w.r.t. the global C.S.
		 ** Needed by L1/KF.
		 ** Note that in general, the rotation angle will be different for each
		 ** sensor. In particular, half of the are upside down.
		 **/
		Double_t GetSensorRotation() const;



		/** Station z position in global c.s.
		 ** @value  z Position of station in global C.S. [cm]
		 **
		 ** Calculated from the sensor positions (mean between min z and max z)
		 **/
		Double_t GetZ() const { return fZ; }


	 /** @brief Initialise the station parameters **/
		void Init();


		/** Info  **/
		virtual string ToString() const;




	private:

		Double_t fZ;          ///< z position of station [cm]
		Double_t fXmin;       ///< minimal x coordinate [cm]
		Double_t fXmax;       ///< maximal x coordinate [cm]
		Double_t fYmin;       ///< minimal y coordinate [cm]
		Double_t fYmax;       ///< maximal y coordinate [cm]
		Double_t fSensorD;    ///< thickness of sensors [cm]
		Int_t    fNofSensors; ///< Number of sensors in station
		Bool_t fDiffSensorD;  ///< Flag for different sensor thicknesses
		CbmStsSensor* fFirstSensor; ///< Pointer to first sensor


		/** @brief Check properties of sensors (position, thickness)
		 ** The z position of the station is determined as the mean of the minimum
		 ** and maximum z positions of all sensors within the station.
		 ** The active thickness of the first sensor is defined as thickness of the
		 ** entire station. A warning is given out if there are different thicknesses
		 ** among the sensors in the stations.
		 **/
		void CheckSensorProperties();


    // --- Prevent usage of copy constructor and assignment operator
    CbmStsStation(const CbmStsStation&);
    CbmStsStation& operator=(const CbmStsStation&);


		ClassDef(CbmStsStation, 1);
};

#endif /* CBMSTSSTATION_H */
