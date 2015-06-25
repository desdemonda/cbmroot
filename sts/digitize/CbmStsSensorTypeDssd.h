/** @file CbmStsSensorTypeDssd.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#ifndef CBMSTSSENSORTYPEDSSD_H
#define CBMSTSSENSORTYPEDSSD_H 1

#include <vector>
#include "CbmStsSensorType.h"


using std::vector;



/** @class CbmStsSensorTypeDssd
 ** @brief Class describing double-sided silicon strip sensors.
 ** @author V.Friese <v.friese@gsi.de>
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
 ** The response to charged particles is modelled by a uniform charge
 ** distribution along the particle trajectory in the active volume,
 ** which is projected to the readout edge, where it is discretised
 ** on the active strips. The charge is then delivered to the corresponding
 ** channel of the readout module (CbmStsModule).
 **
 ** The mapping of strip number and module channel is trivial in the case
 ** of just one sensor per module. In case of several daisy-chained sensors,
 ** the top-edge strip is vertically connected vertically to the corresponding
 ** strip on the bottom edge of the sensor above. This results in an offset
 ** of strip number to channel number which depends on the position of the'
 ** sensor in the daisy chain. The behaviour is implemented in the methods
 ** GetStrip and GetModuleChannel.
 **/
class CbmStsSensorTypeDssd : public CbmStsSensorType
{

  public:

    /** Constructor  **/
    CbmStsSensorTypeDssd();


    /** Destructor  **/
    virtual ~CbmStsSensorTypeDssd() { };


    /** Find hits from clusters
     ** Abstract from CbmStsSensorType
     **/
    virtual Int_t FindHits(std::vector<CbmStsCluster*>&,
    		                   CbmStsSensor* sensor);


    /** Get parameters
     ** @param[out] dX         Size of active area in x [cm]
     ** @param[out] dY         Size of active area in y [cm]
     ** @param[out] dZ         Active thickness [cm]
     ** @param[out] nStripsF   Number of strips on front side
     ** @param[out] nStripsB   Number of strips on back side
     ** @param[out] stereoF    Stereo angle on front side [degrees]
     ** @param[out] stereoB    Stereo angle on back side [degrees]
     **/
    void GetParameters(Double_t& dX, Double_t& dY, Double_t& dZ,
    		               Int_t& nStripsF, Int_t& nStripsB,
    		               Double_t& stereoF, Double_t& stereoB) const {
    	dX       = fDx;
    	dY       = fDy;
    	dZ       = fDz;
    	nStripsF = fNofStrips[0];
    	nStripsB = fNofStrips[1];
    	stereoF  = fStereo[0];
    	stereoB  = fStereo[1];
    }


    /** Get pitch on front or back side
     ** @param side 0 = front side, 1 = back side
     ** @value Strip pitch [cm]
     **/
    Double_t GetPitch(Int_t iSide) const;


    /** Get the side of the sensor from the module channel number
     ** The channel number can also be the cluster position, so it needs
     ** not be integer.
     ** @param channel  Channel number
     ** @return Sensor side ( 0 = front, 1 = back)
     **/
    Int_t GetSide(Double_t channel) const {
    	return ( channel < Double_t(fNofStrips[0]) ? 0 : 1 );
    }


    /** Get stereo angle on front or back side
     ** @param side 0 = front side, 1 = back side
     ** @value Stereo angle [degrees]
     **/
    Double_t GetStereoAngle(Int_t iSide) const;


    /** Indicate parameters are set
     ** @value kTRUE if parameters are set, kFALSE else
     **/
    Bool_t IsSet() const { return fIsSet; }


    /** Print parameters **/
    virtual void Print(Option_t* opt = "") const;


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
     **/
    void SetParameters(Double_t dx, Double_t dy, Double_t dz,
                       Int_t nStripsF, Int_t nStripsB,
                       Double_t stereoF, Double_t stereoB);


  protected:

    Double_t fDx;             ///< Dimension of active area in x [cm]
    Double_t fDy;             ///< Dimension of active area in y [cm]
    Double_t fDz;             ///< Thickness in z [cm]
    Int_t    fNofStrips[2];   ///< Number of strips on front/back side
    Double_t fStereo[2];      ///< Stereo angle front/back side [degrees]
    Bool_t   fIsSet;          ///< Flag whether parameters are set


    /** Temporary variables to avoid frequent calculations **/
    Double_t fPitch[2];     //! Strip pitch front/back side [cm]
    Double_t fTanStereo[2]; //! tangent of stereo angle front/back side
    Int_t   fStripShift[2]; //! Shift in number of strips from bottom to top


    /** Get the cluster position at the top edge of the sensor.
     ** @param[in]  centre    Cluster centre in (module) channel units
     ** @param[in]  sensor    Pointer to sensor object
     ** @param[out] xCluster  Cluster position at readout edge
     ** @param[out] side      Sensor side [0 = front, 1 = back]
     **
     ** A correction for the Lorentz shift is applied.
     **/
    void GetClusterPosition(Double_t centre, CbmStsSensor* sensor,
    		                    Double_t& xCluster, Int_t& side);


    /** Get the readout channel in the module for a given strip and side
     ** @param strip     Strip number
     ** @param side      Side (0 = front, 1 = back)
     ** @param sensorId  Index of sensor within module
     ** @return  Channel number in module
     **
     ** Note: This encodes the mapping of sensor strip to module
     ** channel, i.e. defines the physical meaning of the latter.
     **/
    Int_t GetModuleChannel(Int_t strip, Int_t side, Int_t sensorId) const;


    /** Get strip and side from module channel.
     ** @param[in] channel   Channel number in module
     ** @param[in] sensorId  Sensor index in module
     ** @param[out]  strip   Strip number in sensor
     ** @param[out]  side    Sensor side [0 = front, 1 = back]
     **
     ** Note: This must be the inverse of GetModuleChannel.
     **/
    void GetStrip(Int_t channel, Int_t sensorId, Int_t& strip, Int_t& side);


    /** Get strip number from point coordinates
     ** @param x     x coordinate [cm]
     ** @param y     y coordinate [cm]
     ** @param side  0 = front side, 1 = back side
     ** @return strip number on selected side
     **/
    Int_t GetStripNumber(Double_t x, Double_t y, Int_t side) const;


    /** Intersection point of two strips / cluster centres
     ** @param xF  x coordinate on read-out edge, front side [cm]
     ** @param xB  x coordinate on read-out edge, back side  [cm]
     ** @param x (return)  x coordinate of crossing
     ** @param y (return)  y coordinate of crossing
     ** @return kTRUE if intersection is inside active area.
     **
     ** This function calculates the intersection point of two
     ** lines starting at xF and xB at the top edge with slopes
     ** corresponding to the respective stereo angle.
     **
     ** All coordinates are in the sensor frame with the origin in the
     ** bottom left corner of the active area.
     **/
    Bool_t Intersect(Double_t xF, Double_t xB, Double_t& x, Double_t& y);


    /** Find the intersection points of two clusters.
     ** For each intersection point, a hit is created.
     ** @param clusterF    Pointer to cluster on front side
     ** @param clusterB    Pointer to cluster on back side
     ** @param sensor      Pointer to sensor object
     ** @return Number of intersection points inside active area
     **/
    Int_t IntersectClusters(CbmStsCluster* clusterF,
    		                    CbmStsCluster* clusterB,
    		                    CbmStsSensor* sensor);


    /** Check whether a point (x,y) is inside the active area.
     ** Note that the coordinates have to be given in w.r.t. the bottom
     ** left corner of the active area of the sensor.
     **
     ** @param x  x coordinate in sensor frame (w.r.t. bottom left corner)
     ** @param y  y coordinate in sensor frame (w.r.t. bottom left corner)
     ** @return  kTRUE if inside active area.
     **/
    Bool_t IsInside(Double_t x, Double_t y);


    /** Produce charge on front or back side from a CbmStsSensorPoint
     ** @param point  Pointer to CbmStsSensorType object
     ** @param side   0 = front, 1 = back side
     ** @param sensor Pointer to sensor object
     ** @return  Number of generated charge signals (active strips)
     **/
    virtual Int_t ProduceCharge(CbmStsSensorPoint* point, Int_t side,
                                const CbmStsSensor* sensor) const;


    /** Register produced charge in one strip
     ** @param side  0 = front, 1 = back
     ** @param strip strip number
     ** @param charge  charge in strip [e]
     ** @param time    time of registration [ns]
     **/
    void RegisterCharge(const CbmStsSensor* sensor, Int_t side, Int_t strip,
                        Double_t charge, Double_t time) const;


    /** Test the consistent implementation of GetModuleChannel and
     ** GetStrip. The latter should be the reverse of the former.
     ** @return kTRUE if successful
     **/
    Bool_t SelfTest();



    ClassDef(CbmStsSensorTypeDssd,1);

};


#endif
