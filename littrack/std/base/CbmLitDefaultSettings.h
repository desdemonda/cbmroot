/** CbmLitDefaultSettings.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Header defines constants which are used in the track reconstruction.
 **/

#ifndef CBMLITDEFAULTSETTINGS_H_
#define CBMLITDEFAULTSETTINGS_H_

namespace lit
{

class CbmLitDefaultSettings
{
public:
	// Track propagation settings
	static const litfloat LINE_EXTRAPOLATION_START_Z; // z coordinate [cm] after which linear track extrapolation starts
	static const litfloat ENERGY_LOSS_CONST; // energy lost constant [GeV/c] used in the simple energy loss calculation
	static const litfloat MINIMUM_PROPAGATION_DISTANCE; // minimum propagation distance [cm]
	static const litfloat MAXIMUM_TGEO_NAVIGATION_DISTANCE; // maximum distance used in the TGeo navigation
	// Not const because it is changed to 1 cm for track length calculation!
	//static litfloat MAXIMUM_PROPAGATION_STEP_SIZE; // maximum step size in the TGeo track propagation
};


}
                                
#endif /*CBMLITDEFAULTSETTINGS_H_*/
