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
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKPROPAGATIONANALYSISDEF_H
#define _TRACKPROPAGATIONANALYSISDEF_H


#define TRACKPROPAGATIONEVENTMODIFIER   "Event"								/**< Defines the text to display in the header of a graphic for an event */
#define DISTANCEXAXITITLE               "distance [cm]"						/**< Defines the text to display for the title of the x-axi for the distance graphic*/
#define DISTANCEYAXITITLE               "tracks [#]"						/**< Defines the text to display for the title of the y-axi for the distance graphic */


#define TRACKPROPAGATIONSTSANALYSISDIR  "StsAnalysis"						/**< This defines the name of the directory where the analysis is placed. */
#define EVENTHITSAVINGDIRECOTRY         "EventHit"							/**< Defines the name of the saving directory for the hit distance for each event analysis */
#define TOTALHITSAVINGDIRECOTRY         "TotalHit"							/**< Defines the name of the saving directory for the hit distance for the accumulated event analysis */
#define EVENTPOINTSAVINGDIRECOTRY       "EventPoint"						/**< Defines the name of the saving directory for the point distance for each event analysis */
#define TOTALPOINTSAVINGDIRECOTRY       "TotalPoint"						/**< Defines the name of the saving directory for the point distance for the accumulated event analysis */
#define EVENTHITSAVINGINSUBDIRECOTRY    true								/**< Defines if the hit distance analysis for each event is stored in a subdirectory of in the main directory */
#define TOTALHITSAVINGINSUBDIRECOTRY    true								/**< Defines if the accumulated hit distance analysis for all events is stored in a subdirectory of in the main directory */
#define EVENTPOINTSAVINGINSUBDIRECOTRY  true								/**< Defines if the point distance analysis for each event is stored in a subdirectory of in the main directory */
#define TOTALPOINTSAVINGINSUBDIRECOTRY  true								/**< Defines if the accumulated point distance analysis for all events is stored in a subdirectory of in the main directory */
#define DISTANCESAVINGSUBDIRECTORY      "Distance"							/**< Defines the name of the subdirectory for the track propagation distance analysis */


#define distanceDisplayNameEventHit     "GeaneDistanceEventHit"				/**< Defines the name for the hit distance display for each event */
#define distanceDisplayNameEventPoint   "GeaneDistanceEventPoint"			/**< Defines the name for the point distance display for each event */

#define distanceDisplayTitleEventHit    distanceDisplayNameEventHit			/**< Defines the title for the hit distance for each event */
#define distanceDisplayTitleEventPoint  distanceDisplayNameEventPoint		/**< Defines the title for the point distance for each event */

#define distanceDisplayNameTotalHit     "GeaneDistanceTotalHit"				/**< Defines the name for the accumulated hit distance display for all events */
#define distanceDisplayNameTotalPoint   "GeaneDistanceTotalPoint"			/**< Defines the name for the accumulated point display for all events */

#define distanceDisplayTitleTotalHit    distanceDisplayNameTotalHit			/**< Defines the title for the accumulated hit distance display for all events */
#define distanceDisplayTitleTotalPoint  distanceDisplayNameTotalPoint		/**< Defines the title for the accumulated point display for all events */


#endif
