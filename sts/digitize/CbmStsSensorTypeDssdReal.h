/** @file CbmStsSensorTypeDssdReal.h
 ** @authors: Volker Friese <v.friese@gsi.de>, Hanna Malygina <h.malygina@gsi.de>
 ** @date 25.11.2014
 **/

#ifndef CBMSTSSENSORTYPEDSSDREAL_H
#define CBMSTSSENSORTYPEDSSDREAL_H 1

#include <vector>
#include "TRandom3.h"
#include "CbmStsSensorTypeDssd.h"

class CbmStsPhysics;


/** @class CbmStsSensorTypeDssdReal
 ** @brief Class describing double-sided silicon strip sensors.
 ** @authors: Volker Friese <v.friese@gsi.de>, Hanna Malygina <h.malygina@gsi.de>
 ** @version 1.0
 **
 ** This class describes the response of double-sided silicon
 ** strip sensors in the STS. Its free parameters are the dimensions
 ** of the active area, the number of strips at the
 ** top (readout) edge and the stereo angles on front and back side.
 **
 ** The active area does not necessarily coincide with the geometric
 ** dimensions of the sensor. It is, however, centred in the latter,
 ** meaning that the width of inactive regions (guard ring) are
 ** the same on the left and on the right side and also the same at
 ** the top and and the bottom.
 **
 ** The stereo angle is defined with respect to the y (vertical) axis.
 ** Readout is performed at the top edge of the sensor. In case of
 ** finite stereo angle, the corner strips not reaching the top edge are
 ** connected horizontally to the corresponding strip in the other corner.
 **
 ** The response to charged particles is modelled by a nonuniform charge
 ** distribution along the particle trajectory in the active volume 
 ** (Urban theory). Thermal diffusion, Lorentz shift and cross-talk due 
 ** to interstrip capacitances are took into account.
 **
 ** The mapping of strip number and module channel is trivial in the case
 ** of just one sensor per module. In case of several daisy-chained sensors,
 ** the top-edge strip is vertically connected vertically to the corresponding
 ** strip on the bottom edge of the sensor above. This results in an offset
 ** of strip number to channel number which depends on the position of the'
 ** sensor in the daisy chain. The behaviour is implemented in the methods
 ** GetStrip and GetModuleChannel.
 **/
class CbmStsSensorTypeDssdReal : public CbmStsSensorTypeDssd
{

    public:

	/** Constructor  **/
	CbmStsSensorTypeDssdReal();


	/** Destructor  **/
	virtual ~CbmStsSensorTypeDssdReal() { };


	/** Process one STS Point
	 **
	 ** @param point   Pointer to CbmStsSensorPoint with relevant parameters
	 ** @param sensor  Pointer to CbmStsSensor object
	 ** @return  1000* # signals on front side + # signals on back side
	 **
	 ** Perform the appropriate action for a particle trajectory in the
	 ** sensor characterised by the CbmStsSensorPoint object.
	 **/
	virtual Int_t ProcessPoint(CbmStsSensorPoint* point,
		const CbmStsSensor* sensor) const;



	/** Set the parameters
	 ** @param dx,dy,dz          Size in x,y,z [cm]
	 ** @param pitchF,pitchB     Strip pitch foint and back side [cm]
	 ** @param stereoF,stereoB   Strip stereo angle front and back side [degrees]
	 ** @param nStripsF,nStripsB Number of strips for front and back side
	 ** @param Vdep, Vbias       Depletion and bias (operational) voltage for sensor [V] 
	 ** @param temperature       Temperature of sensor [K]
	 ** @param Ccoup, Cinter     Coupling and interstrip capacitances [same unit]
	 **/
	virtual	void SetParameters(Double_t dx, Double_t dy, Double_t dz,
		Int_t nStripsF, Int_t nStripsB,
		Double_t stereoF, Double_t stereoB,
		Double_t Vdep, Double_t Vbias, Double_t temperature,
		Double_t Ccoup, Double_t Cinter);


    protected:
	Double_t fVdepletion;     ///< Depletion voltage of sensor [V]
	Double_t fVbias;          ///< Bias voltage of sensor [V]
	Double_t fTemperature;    ///< Environment temperature [K]
	Double_t fCcoup;          ///< Coupling capacitance
	Double_t fCinter;         ///< Interstreip capacitance


	/** Temporary variables to avoid frequent calculations **/
	Double_t fkTq;          //! = sqrt(2 * k * T / q)
	Double_t fCTcoef;       //! cross-talk coefficient due to capacitances 

	/** Instance of StsPhysics **/
	CbmStsPhysics* fPhysics;  //!

	/** Produce charge on front or back side from a CbmStsSensorPoint
	 ** @param point  Pointer to CbmStsSensorType object
	 ** @param side   0 = front, 1 = back side
	 ** @param sensor Pointer to sensor object
	 ** @return  Number of generated charge signals (active strips)
	 **/
	//virtual Int_t ProduceCharge(CbmStsSensorPoint* point, Int_t side,
	//	const CbmStsSenzor* sensor, Double_t * ELossLayerArray) const;

	virtual Int_t ProduceCharge(CbmStsSensorPoint* point, Int_t side,
		const CbmStsSensor* sensor, std::vector<Double_t> &ELossLayerArray) const;

	/** Simulate energy loss in small piece of trajectory in sensor.
	 ** @ param delta size of piece along track [cm]
	 ** @ param m     mass of the incident particle [GeV]
	 ** @ param Ekin  kinetic energy of the incident particle [GeV]
	 ** @ param dEdx  stopping power [eV/cm]
	 **/
	Double_t EnergyLossLayer (TRandom3 * ELossRandom, Double_t delta, Double_t m, Double_t Ekin, Double_t dEdx) const;

	/** Simulate thermal diffusion and take into account Lorentz shift.
	 ** @ param delta size of piece along track [cm]
	 ** @ stripCharge -- array of resulting charge distribution over the strips
	 ** @ chargeLayer -- charge, produced in current layer of trajectory
	 ** @ i1, i2 --  
	 ** @ locY1, locY2 -- in and out coordinates of trajectory 
	 ** @ locX1, locX2 -- in and out coordinates of trajectory 
	 ** @ locZ1 -- in coordinate of trajectory 
	 ** @ trajLength -- length of trajectory in sensor
	 ** @ iLayer -- number of trajectory layer
	 ** @ side -- 0 or 1 (p or n) -- to know type of charge carriers
	 **/
	void ThermalSharing (Double_t delta, Double_t * stripCharge, Double_t chargeLayer, 
		Double_t locX1, Double_t locX2, Double_t locY1, Double_t locY2, Double_t locZ1, 
		Int_t i1, Int_t i2, Double_t trajLength, Int_t iLayer, Int_t side ) const;

	/** Calculate charge sharing due to capacitance
	 ** @ stripCharge -- input charge distribution over the strips
	 ** @ stripChargeCT -- output charge distribution over the strip
	 ** nStrips -- number of strips on current side
	 **/ 
	void CrossTalk(Double_t * stripCharge, Double_t * stripChargeCT, Int_t nStrips, Double_t tanphi) const;

	/** Calculate stopping power for particles
	 ** @ fElectron -- is particle electron?
	 ** @ mx -- particle mass [GeV]
	 ** @ Ex -- particle energy [GeV]
	 ** @ zx -- particle charge [e]
	 **/
	Double_t StoppingPower(Bool_t fElectron, Double_t mx, Double_t Ex, Int_t zx) const;

 private:
	CbmStsSensorTypeDssdReal(const CbmStsSensorTypeDssdReal&);
	CbmStsSensorTypeDssdReal operator=(const CbmStsSensorTypeDssdReal&);

	ClassDef(CbmStsSensorTypeDssdReal,1);

};


#endif
