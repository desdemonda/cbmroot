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
/// $Date: 2008-08-14 12:29:25 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PROJECTIONEFGCNANALYSISDEF_H
#define _PROJECTIONEFGCNANALYSISDEF_H


#define PROJECTIONEFGCNEVENTMODIFIER   "Event"					/**< Defines the text to display in the header of a histogram for an event */
#define DIM1AXITITLE                   "Dim1"					/**< Defines the text to display for the title of the dim1-axi */
#define DIM2AXITITLE                   "Dim2"					/**< Defines the text to display for the title of the dim2-axi */
#define DIM3AXITITLE                   "Dim3"					/**< Defines the text to display for the title of the dim3-axi */

#define PROJECTIONEFGCNSTSANALYSISDIR  "StsAnalysis"			/**< This defines the name of the directory where the analysis is placed. */
#define EVENTSAVINGDIRECOTRY           "Event"					/**< Defines the name of the saving directory for the projection for each event analysis */
#define TOTALSAVINGDIRECOTRY           "Total"					/**< Defines the name of the saving directory for the projection for all event analysis */
#define EVENTSAVINGINSUBDIRECOTRY      true						/**< Defines if the event analysis for each event is stored in a subdirectory of in the main directory */
#define TOTALSAVINGINSUBDIRECOTRY      true						/**< Defines if the total analysis for all events is stored in a subdirectory of in the main directory */
#define ESAVINGSUBDIRECTORY            "E"						/**< Defines the name of the subdirectory for the efficiency analysis */
#define FSAVINGSUBDIRECTORY            "F"						/**< Defines the name of the subdirectory for the fakse analysis */
#define GSAVINGSUBDIRECTORY            "G"						/**< Defines the name of the subdirectory for the ghosts analysis */
#define CSAVINGSUBDIRECTORY            "C"						/**< Defines the name of the subdirectory for the clones analysis */
#define NSAVINGSUBDIRECTORY            "N"						/**< Defines the name of the subdirectory for the not founds analysis */


#define eDisplayNameEvent12            "HoughTransform12EE"		/**< Defines the name for the efficiency display of the projection in dim1/dim2 direction for each event */
#define fDisplayNameEvent12            "HoughTransform12FE"		/**< Defines the name for the fakes display of the projection in dim1/dim2 direction for each event */
#define gDisplayNameEvent12            "HoughTransform12GE"		/**< Defines the name for the ghosts display of the projection in dim1/dim2 direction for each event */
#define cDisplayNameEvent12            "HoughTransform12CE"		/**< Defines the name for the clones display of the projection in dim1/dim2 direction for each event */
#define nDisplayNameEvent12            "HoughTransform12NE"		/**< Defines the name for the not founds display of the projection in dim1/dim2 direction for each event */
#define eDisplayNameEvent13            "HoughTransform13EE"		/**< Defines the name for the efficiency display of the projection in dim1/dim2 direction for each event */
#define fDisplayNameEvent13            "HoughTransform13FE"		/**< Defines the name for the fakes display of the projection in dim1/dim2 direction for each event */
#define gDisplayNameEvent13            "HoughTransform13GE"		/**< Defines the name for the ghosts display of the projection in dim1/dim2 direction for each event */
#define cDisplayNameEvent13            "HoughTransform13CE"		/**< Defines the name for the clones display of the projection in dim1/dim2 direction for each event */
#define nDisplayNameEvent13            "HoughTransform13NE"		/**< Defines the name for the not founds display of the projection in dim1/dim2 direction for each event */
#define eDisplayNameEvent32            "HoughTransform32EE"		/**< Defines the name for the efficiency display of the projection in dim1/dim2 direction for each event */
#define fDisplayNameEvent32            "HoughTransform32FE"		/**< Defines the name for the fakes display of the projection in dim1/dim2 direction for each event */
#define gDisplayNameEvent32            "HoughTransform32GE"		/**< Defines the name for the ghosts display of the projection in dim1/dim2 direction for each event */
#define cDisplayNameEvent32            "HoughTransform32CE"		/**< Defines the name for the clones display of the projection in dim1/dim2 direction for each event */
#define nDisplayNameEvent32            "HoughTransform32NE"		/**< Defines the name for the not founds display of the projection in dim1/dim2 direction for each event */

#define eDisplayTitleEvent12           "HoughTransform12EE"		/**< Defines the title for the efficiency display of the projection in dim1/dim2 direction for each event */
#define fDisplayTitleEvent12           "HoughTransform12FE"		/**< Defines the title for the fakes display of the projection in dim1/dim2 direction for each event */
#define gDisplayTitleEvent12           "HoughTransform12GE"		/**< Defines the title for the ghosts display of the projection in dim1/dim2 direction for each event */
#define cDisplayTitleEvent12           "HoughTransform12CE"		/**< Defines the title for the clones display of the projection in dim1/dim2 direction for each event */
#define nDisplayTitleEvent12           "HoughTransform12NE"		/**< Defines the title for the not founds display of the projection in dim1/dim2 direction for each event */
#define eDisplayTitleEvent13           "HoughTransform13EE"		/**< Defines the title for the efficiency display of the projection in dim1/dim2 direction for each event */
#define fDisplayTitleEvent13           "HoughTransform13FE"		/**< Defines the title for the fakes display of the projection in dim1/dim2 direction for each event */
#define gDisplayTitleEvent13           "HoughTransform13GE"		/**< Defines the title for the ghosts display of the projection in dim1/dim2 direction for each event */
#define cDisplayTitleEvent13           "HoughTransform13CE"		/**< Defines the title for the clones display of the projection in dim1/dim2 direction for each event */
#define nDisplayTitleEvent13           "HoughTransform13NE"		/**< Defines the title for the not founds display of the projection in dim1/dim2 direction for each event */
#define eDisplayTitleEvent32           "HoughTransform32EE"		/**< Defines the title for the efficiency display of the projection in dim1/dim2 direction for each event */
#define fDisplayTitleEvent32           "HoughTransform32FE"		/**< Defines the title for the fakes display of the projection in dim1/dim2 direction for each event */
#define gDisplayTitleEvent32           "HoughTransform32GE"		/**< Defines the title for the ghosts display of the projection in dim1/dim2 direction for each event */
#define cDisplayTitleEvent32           "HoughTransform32CE"		/**< Defines the title for the clones display of the projection in dim1/dim2 direction for each event */
#define nDisplayTitleEvent32           "HoughTransform32NE"		/**< Defines the title for the not founds display of the projection in dim1/dim2 direction for each event */

#define eDisplayNameTotal12            "HoughTransform12ET"		/**< Defines the name for the efficiency display of the projection in dim1/dim2 direction for all events */
#define fDisplayNameTotal12            "HoughTransform12FT"		/**< Defines the name for the fakes display of the projection in dim1/dim2 direction for all events */
#define gDisplayNameTotal12            "HoughTransform12GT"		/**< Defines the name for the ghosts display of the projection in dim1/dim2 direction for all events */
#define cDisplayNameTotal12            "HoughTransform12CT"		/**< Defines the name for the clones display of the projection in dim1/dim2 direction for all events */
#define nDisplayNameTotal12            "HoughTransform12NT"		/**< Defines the name for the not founds display of the projection in dim1/dim2 direction for all events */
#define eDisplayNameTotal13            "HoughTransform13ET"		/**< Defines the name for the efficiency display of the projection in dim1/dim2 direction for all events */
#define fDisplayNameTotal13            "HoughTransform13FT"		/**< Defines the name for the fakes display of the projection in dim1/dim2 direction for all events */
#define gDisplayNameTotal13            "HoughTransform13GT"		/**< Defines the name for the ghosts display of the projection in dim1/dim2 direction for all events */
#define cDisplayNameTotal13            "HoughTransform13CT"		/**< Defines the name for the clones display of the projection in dim1/dim2 direction for all events */
#define nDisplayNameTotal13            "HoughTransform13NT"		/**< Defines the name for the not founds display of the projection in dim1/dim2 direction for all events */
#define eDisplayNameTotal32            "HoughTransform32ET"		/**< Defines the name for the efficiency display of the projection in dim1/dim2 direction for all events */
#define fDisplayNameTotal32            "HoughTransform32FT"		/**< Defines the name for the fakes display of the projection in dim1/dim2 direction for all events */
#define gDisplayNameTotal32            "HoughTransform32GT"		/**< Defines the name for the ghosts display of the projection in dim1/dim2 direction for all events */
#define cDisplayNameTotal32            "HoughTransform32CT"		/**< Defines the name for the clones display of the projection in dim1/dim2 direction for all events */
#define nDisplayNameTotal32            "HoughTransform32NT"		/**< Defines the name for the not founds display of the projection in dim1/dim2 direction for all events */

#define eDisplayTitleTotal12           "HoughTransform12ET"		/**< Defines the title for the efficiency display of the projection in dim1/dim2 direction for all events */
#define fDisplayTitleTotal12           "HoughTransform12FT"		/**< Defines the title for the fakes display of the projection in dim1/dim2 direction for all events */
#define gDisplayTitleTotal12           "HoughTransform12GT"		/**< Defines the title for the ghosts display of the projection in dim1/dim2 direction for all events */
#define cDisplayTitleTotal12           "HoughTransform12CT"		/**< Defines the title for the clones display of the projection in dim1/dim2 direction for all events */
#define nDisplayTitleTotal12           "HoughTransform12NT"		/**< Defines the title for the not founds display of the projection in dim1/dim2 direction for all events */
#define eDisplayTitleTotal13           "HoughTransform13ET"		/**< Defines the title for the efficiency display of the projection in dim1/dim2 direction for all events */
#define fDisplayTitleTotal13           "HoughTransform13FT"		/**< Defines the title for the fakes display of the projection in dim1/dim2 direction for all events */
#define gDisplayTitleTotal13           "HoughTransform13GT"		/**< Defines the title for the ghosts display of the projection in dim1/dim2 direction for all events */
#define cDisplayTitleTotal13           "HoughTransform13CT"		/**< Defines the title for the clones display of the projection in dim1/dim2 direction for all events */
#define nDisplayTitleTotal13           "HoughTransform13NT"		/**< Defines the title for the not founds display of the projection in dim1/dim2 direction for all events */
#define eDisplayTitleTotal32           "HoughTransform32ET"		/**< Defines the title for the efficiency display of the projection in dim1/dim2 direction for all events */
#define fDisplayTitleTotal32           "HoughTransform32FT"		/**< Defines the title for the fakes display of the projection in dim1/dim2 direction for all events */
#define gDisplayTitleTotal32           "HoughTransform32GT"		/**< Defines the title for the ghosts display of the projection in dim1/dim2 direction for all events */
#define cDisplayTitleTotal32           "HoughTransform32CT"		/**< Defines the title for the clones display of the projection in dim1/dim2 direction for all events */
#define nDisplayTitleTotal32           "HoughTransform32NT"		/**< Defines the title for the not founds display of the projection in dim1/dim2 direction for all events */


#endif
