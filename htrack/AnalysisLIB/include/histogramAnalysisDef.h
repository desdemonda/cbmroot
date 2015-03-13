//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   file:
///     - global definition of structs, defines and enums for project
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:24 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMANALYSISDEF_H
#define _HISTOGRAMANALYSISDEF_H


#define HISTOGRAMEVENTMODIFIER      "Event"					/**< Defines the text to display in the header of a histogram for the event */
#define LAYERMODIFIER               "Layer"					/**< Defines the text to display in the header of a histogram for the layer */

#define STSHISTOGRAMANALYSISDIR     "StsAnalysis"			/**< This defines the name of the directory where the analysis is placed. */
#define HISTOGRAMSAVINGDIRECTORY    "Histogram"				/**< Defines the name of the saving directory */

#define createdDisplayName          "createdHistogram"		/**< Defines the name for the display */
#define createdDisplayTitle         "createdHistogram"		/**< Defines the title for the display */
#define encodedDisplayName          "encodedHistogram"		/**< Defines the name for the display */
#define encodedDisplayTitle         "encodedHistogram"		/**< Defines the title for the display */
#define filteredDisplayName         "filteredHistogram"		/**< Defines the name for the display */
#define filteredDisplayTitle        "filteredHistogram"		/**< Defines the title for the display */

#define HISTOGRAMXAXITITLE          "Theta"					/**< Defines the text to display for the title of the x-axi for the display */
#define HISTOGRAMYAXITITLE          "Radius"				/**< Defines the text to display for the title of the y-axi for the display */

#undef SAVINGINEVENTSUBDIRECTORY							/**< Define enables the saving of the analysis separated in folders by the events */
#undef SAVINGINLAYERSUBDIRECTORY							/**< Define enables the saving of the analysis separated in folders by the layers */


#endif
