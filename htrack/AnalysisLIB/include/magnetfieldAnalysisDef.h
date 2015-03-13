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
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MAGNETFIELDANALYSISDEF_H
#define _MAGNETFIELDANALYSISDEF_H


#define DIMXMODIFIER                "x"					/**< Defines the text to display in the header of a histogram to display the constant values of the constant dimension */
#define DIMYMODIFIER                "y"					/**< Defines the text to display in the header of a histogram to display the constant values of the constant dimension */
#define DIMZMODIFIER                "z"					/**< Defines the text to display in the header of a histogram to display the constant values of the constant dimension */
#define XXAXITITLE                  "x [cm]"			/**< Defines the text to display for the title of the x-axi for the B_x */
#define YXAXITITLE                  "y [cm]"			/**< Defines the text to display for the title of the x-axi for the B_y */
#define ZXAXITITLE                  "z [cm]"			/**< Defines the text to display for the title of the x-axi for the B_z */
#define BXYAXITITLE                 "B_{x} [kG]"		/**< Defines the text to display for the title of the y-axi for the B_x */
#define BYYAXITITLE                 "B_{y} [kG]"		/**< Defines the text to display for the title of the y-axi for the B_y */		
#define BZYAXITITLE                 "B_{z} [kG]"		/**< Defines the text to display for the title of the y-axi for the B_z */


#define STSMAGNETFIELDANALYSISDIR   "StsAnalysis"		/**< This defines the name of the directory where the analysis is placed. */
#define MAGNETFIELDSAVINGDIRECTORY  "Magnetfield"		/**< Defines the name of the saving directory */
#define XSAVINGINSUBDIRECOTRY       true				/**< Defines if the x analysis is stored in a subdirectory of the saving directory */
#define YSAVINGINSUBDIRECOTRY       true				/**< Defines if the y analysis is stored in a subdirectory of the saving directory */
#define ZSAVINGINSUBDIRECOTRY       true				/**< Defines if the z analysis  is stored in a subdirectory of the saving directory */
#define XSAVINGSUBDIRECTORY         "X"					/**< Defines the name of the subdirectory for the x analysis */
#define YSAVINGSUBDIRECTORY         "Y"					/**< Defines the name of the subdirectory for the y analysis */
#define ZSAVINGSUBDIRECTORY         "Z"					/**< Defines the name of the subdirectory for the z analysis */


#define xBxDisplayName              "MagnetfieldxBx"	/**< Defines the name for the xBx display */
#define xByDisplayName              "MagnetfieldxBy"	/**< Defines the name for the xBy display */
#define xBzDisplayName              "MagnetfieldxBz"	/**< Defines the name for the xBz display */
#define yBxDisplayName              "MagnetfieldyBx"	/**< Defines the name for the yBx display */
#define yByDisplayName              "MagnetfieldyBy"	/**< Defines the name for the yBy display */
#define yBzDisplayName              "MagnetfieldyBz"	/**< Defines the name for the yBz display */
#define zBxDisplayName              "MagnetfieldzBx"	/**< Defines the name for the zBx display */
#define zByDisplayName              "MagnetfieldzBy"	/**< Defines the name for the zBy display */
#define zBzDisplayName              "MagnetfieldzBz"	/**< Defines the name for the zBz display */

#define xBxDisplayTitle             "Magnetfield B_{x}(x)"	/**< Defines the title for the xBx display */
#define xByDisplayTitle             "Magnetfield B_{y}(x)"	/**< Defines the title for the xBy display */
#define xBzDisplayTitle             "Magnetfield B_{z}(x)"	/**< Defines the title for the xBz display */
#define yBxDisplayTitle             "Magnetfield B_{x}(y)"	/**< Defines the title for the yBx display */
#define yByDisplayTitle             "Magnetfield B_{y}(y)"	/**< Defines the title for the yBy display */
#define yBzDisplayTitle             "Magnetfield B_{z}(y)"	/**< Defines the title for the yBz display */
#define zBxDisplayTitle             "Magnetfield B_{x}(z)"	/**< Defines the title for the zBx display */
#define zByDisplayTitle             "Magnetfield B_{y}(z)"	/**< Defines the title for the zBy display */
#define zBzDisplayTitle             "Magnetfield B_{z}(z)"	/**< Defines the title for the zBz display */


#endif
