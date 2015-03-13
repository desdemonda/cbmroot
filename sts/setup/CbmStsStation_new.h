/** @file CbmStsStation_new.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 4.03.2015
 **/

#ifndef CBMSTSSTATION_NEW_H
#define CBMSTSSTATION_NEW_H 1


#include "CbmStsElement.h"


/** @class CbmStsStation_new
 ** @brief Class representing a station of the StsSystem.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The StsStation class represents one level in the STS element hierarchy.
 ** It holds station-wide parameters like z position, sensor thickness,
 ** and material budget for use in reconstruction tasks.
 **/
class CbmStsStation_new : public CbmStsElement {

	public:

		/** Default constructor **/
		CbmStsStation_new();


		/** Standard constructor
		 ** @param name   Station name
		 ** @param title  Station type
		 ** @param node   Pointer to corresponding TGeoPhysicalNode
		 */
		CbmStsStation_new(const char* name, const char* title,
						TGeoPhysicalNode* node = NULL);


		/** Destructor **/
		virtual ~CbmStsStation_new();


		/** Minimal and maximal x and y coordinates **/
		Double_t GetXmin() const { return fXmin; }
		Double_t GetXmax() const { return fXmax; }
		Double_t GetYmin() const { return fYmin; }
		Double_t GetYmax() const { return fYmax; }


		/** Sensor thickness **/
		Double_t GetSensorD() const { return fSensorD; }


		/** Get sensor strip pitch
		 ** @param iSide  Sensor side (0 = front, 1 = back)
		 ** @value Strip pitch [cm]
		 **/
		Double_t GetSensorPitch(Int_t iSide) const;


		/** Get sensor stereo angle
		 ** @param iSide  Sensor side (0 = front, 1 = back)
		 ** @value Stereo angle [degrees]
		 **/
		Double_t GetSensorStereoAngle(Int_t iSide) const;


		/** Sensor rotation **/
		// TODO: Unclear which rotation angle should be delivered. To discuss with L1.
		Double_t GetSensorRot() const { return 0.; }


		/** Station z position in global c.s. **/
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
    CbmStsStation_new(const CbmStsStation_new&);
    CbmStsStation_new& operator=(const CbmStsStation_new&);


		ClassDef(CbmStsStation_new, 1);
};

#endif /* CBMSTSSTATION_NEW_H_ */
