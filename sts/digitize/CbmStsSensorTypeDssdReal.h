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
 ** to interstrip capacitances are taken into account.
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
	virtual ~CbmStsSensorTypeDssdReal() {};


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

	/** Set switches for physical processes
	 ** @param dx,dy,dz          Size in x,y,z [cm]
	 ** @param pitchF,pitchB     Strip pitch foint and back side [cm]
	 ** @param stereoF,stereoB   Strip stereo angle front and back side [degrees]
	 **/
	virtual	void SetPhysicalProcesses(Bool_t nonUniformity, 
		Bool_t diffusion, Bool_t crossTalk, Bool_t lorentzShift) {
	    fNonUniformity = nonUniformity;
	    fDiffusion     = diffusion;
	    fCrossTalk     = crossTalk;
	    fLorentzShift  = lorentzShift;
	}


    protected:

		/** Instance of StsPhysics **/
	CbmStsPhysics* fPhysics;  //!

	/** Switches for physical processes **/
	Bool_t fNonUniformity;
	Bool_t fDiffusion;
	Bool_t fCrossTalk;
	Bool_t fLorentzShift;

	
	/** Calculate charge sharing due to capacitance
	 ** @ stripCharge    input charge distribution over the strips
	 ** @ stripChargeCT  output charge distribution over the strip
	 ** @ param nStrips  number of strips on current side
	 ** @ param tanphi   stereo-angle 
	 ** @ param CTcoef   = C_interstrip / (C_coupling + (C_bulk) + C_interstrip)
	 **/ 
	void CrossTalk(Double_t * stripCharge, Double_t * stripChargeCT, Int_t nStrips, Double_t tanphi, Double_t CTcoef) const;


	/** Simulate thermal diffusion and take into account Lorentz shift.
	 ** @ param delta           size of piece along track [cm]
	 ** @ param stripCharge     array of resulting charge distribution over the strips
	 ** @ param chargeLayer     charge, produced in current layer of trajectory
	 ** @ param locY1, locY2    in and out coordinates of trajectory 
	 ** @ param locX1, locX2    in and out coordinates of trajectory 
	 ** @ param locZ1, locZ2    in and out coordinates of trajectory 
	 ** @ param trajLength      length of trajectory in sensor
	 ** @ param iLayer          number of trajectory layer
	 ** @ param side            0 or 1 (p or n) -- to know type of charge carriers
	 ** @ param kTq             
	 ** @ param Vdepletion      full depletion voltage
	 ** @ param Vbias           bias voltage
	 ** @ param mField          magnetic field
	 ** @ param *hallMobilityParameters  array of parameters for Hall mobility calculation
	 **/
	void DiffusionAndLorentzShift (Double_t delta, Double_t * stripCharge, Double_t chargeLayer, 
		Double_t locX1, Double_t locX2, Double_t locY1, Double_t locY2, Double_t locZ1, Double_t locZ2,
        	Double_t trajLength, Int_t iLayer, Int_t side, Double_t kTq, Double_t Vdepletion, Double_t Vbias,
		Double_t mField, Double_t * hallMobilityParameters) const;


	/** Produce charge on front or back side from a CbmStsSensorPoint
	 ** @param point  Pointer to CbmStsSensorType object
	 ** @param side   0 = front, 1 = back side
	 ** @param sensor Pointer to sensor object
	 ** @return  Number of generated charge signals (active strips)
	 **/
	virtual Int_t ProduceCharge(CbmStsSensorPoint* point, Int_t side,
		const CbmStsSensor* sensor, std::vector<Double_t> &ELossLayerArray) const;

	
 private:
	CbmStsSensorTypeDssdReal(const CbmStsSensorTypeDssdReal&);
	CbmStsSensorTypeDssdReal operator=(const CbmStsSensorTypeDssdReal&);

	ClassDef(CbmStsSensorTypeDssdReal,1);

};


#endif
