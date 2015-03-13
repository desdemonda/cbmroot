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


#ifndef _MOMENTUMEFGCANALYSISDEF_H
#define _MOMENTUMEFGCANALYSISDEF_H


#define MOMENTUMEFGCEVENTMODIFIER   "Event"				/**< Defines the text to display in the header of a histogram for an event */
#define PZXAXITITLE                 "p_{z} [GeV/c]"		/**< Defines the text to display for the title of the x-axi for the momentum */
#define PTXAXITITLE                 "p_{t} [GeV/c]"		/**< Defines the text to display for the title of the x-axi for the transversal impulse */
#define YAXIMIN                     0					/**< Defines the minimal value for the y-axi */
#define YAXIMAX                     100					/**< Defines the maximal value for the y-axi */
#define YAXIOVERSIZE                2					/**< Defines the value to oversize the y-axi. This oversized regions consists of all values out of the ordinary range */
#define EMYAXITITLE                  "Efficiency [%]"	/**< Defines the text to display for the title of the y-axi for the efficiency */		
#define FMYAXITITLE                  "Fakes [%]"			/**< Defines the text to display for the title of the y-axi for the fakes */
#define GMYAXITITLE                  "Ghosts [%]"		/**< Defines the text to display for the title of the y-axi for the ghosts */
#define CMYAXITITLE                  "Clones [%]"		/**< Defines the text to display for the title of the y-axi for the clones */


#define MOMENTUMEFGCSTSANALYSISDIR   "StsAnalysis"		/**< This defines the name of the directory where the analysis is placed. */
#define EVENTPZSAVINGDIRECOTRY      "EventPz"			/**< Defines the name of the saving directory for the momentum for each event analysis */
#define TOTALPZSAVINGDIRECOTRY      "TotalPz"			/**< Defines the name of the saving directory for the momentum for the accumulated event analysis */
#define EVENTPTSAVINGDIRECOTRY      "EventPt"			/**< Defines the name of the saving directory for the transversale impulse for each event analysis */
#define TOTALPTSAVINGDIRECOTRY      "TotalPt"			/**< Defines the name of the saving directory for the transversale impulse for the accumulated event analysis */
#define EVENTPZSAVINGINSUBDIRECOTRY true				/**< Defines if the momentum analysis for each event is stored in a subdirectory of in the main directory */
#define TOTALPZSAVINGINSUBDIRECOTRY true				/**< Defines if the accumulated momentum analysis for all events is stored in a subdirectory of in the main directory */
#define EVENTPTSAVINGINSUBDIRECOTRY true				/**< Defines if the transversale impulse analysis for each event is stored in a subdirectory of in the main directory */
#define TOTALPTSAVINGINSUBDIRECOTRY true				/**< Defines if the accumulated transversale impulse analysis for all events is stored in a subdirectory of in the main directory */
#define EMSAVINGSUBDIRECTORY        "EM"				/**< Defines the name of the subdirectory for the efficiency analysis */
#define FMSAVINGSUBDIRECTORY        "FM"				/**< Defines the name of the subdirectory for the fakse analysis */
#define GMSAVINGSUBDIRECTORY        "GM"				/**< Defines the name of the subdirectory for the ghosts analysis */
#define CMSAVINGSUBDIRECTORY        "CM"				/**< Defines the name of the subdirectory for the clones analysis */


//#define NOIDENTIFICATION		/**< The title for all displays are the same */
#undef NOIDENTIFICATION


#define emDisplayNameEventPz       "HoughTransformPzEME"	/**< Defines the name for the efficiency display of the momentum for each event */
#define fmDisplayNameEventPz       "HoughTransformPzFME"	/**< Defines the name for the fakes display of the momentum for each event */
#define gmDisplayNameEventPz       "HoughTransformPzGME"	/**< Defines the name for the ghosts display of the momentum for each event */
#define cmDisplayNameEventPz       "HoughTransformPzCME"	/**< Defines the name for the clones display of the momentum for each event */
#define emDisplayNameEventPt       "HoughTransformPtEME"	/**< Defines the name for the efficiency display of the transversale impulse for each event */
#define fmDisplayNameEventPt       "HoughTransformPtFME"	/**< Defines the name for the fakes display of the transversale impulse for each event */
#define gmDisplayNameEventPt       "HoughTransformPtGME"	/**< Defines the name for the ghosts display of the transversale impulse for each event */
#define cmDisplayNameEventPt       "HoughTransformPtCME"	/**< Defines the name for the clones display of the transversale impulse for each event */

#ifdef NOIDENTIFICATION
#define emDisplayTitleEventPz      "35 AGeV"				/**< Defines the title for the efficiency display of the momentum for each event */
#define fmDisplayTitleEventPz      emDisplayTitleEventPz	/**< Defines the title for the fakes display of the momentum for each event */
#define gmDisplayTitleEventPz      emDisplayTitleEventPz	/**< Defines the title for the ghosts display of the momentum for each event */
#define cmDisplayTitleEventPz      emDisplayTitleEventPz	/**< Defines the title for the clones display of the momentum for each event */
#define emDisplayTitleEventPt      "35 AGeV"				/**< Defines the title for the efficiency display of the transversale impulse for each event */
#define fmDisplayTitleEventPt      emDisplayTitleEventPt	/**< Defines the title for the fakes display of the transversale impulse for each event */
#define gmDisplayTitleEventPt      emDisplayTitleEventPt	/**< Defines the title for the ghosts display of the transversale impulse for each event */
#define cmDisplayTitleEventPt      emDisplayTitleEventPt	/**< Defines the title for the clones display of the transversale impulse for each event */
#else
#define emDisplayTitleEventPz      emDisplayNameEventPz		/**< Defines the title for the efficiency display of the momentum for each event */
#define fmDisplayTitleEventPz      fmDisplayNameEventPz		/**< Defines the title for the fakes display of the momentum for each event */
#define gmDisplayTitleEventPz      gmDisplayNameEventPz		/**< Defines the title for the ghosts display of the momentum for each event */
#define cmDisplayTitleEventPz      cmDisplayNameEventPz		/**< Defines the title for the clones display of the momentum for each event */
#define emDisplayTitleEventPt      emDisplayNameEventPt		/**< Defines the title for the efficiency display of the transversale impulse for each event */
#define fmDisplayTitleEventPt      fmDisplayNameEventPt		/**< Defines the title for the fakes display of the transversale impulse for each event */
#define gmDisplayTitleEventPt      gmDisplayNameEventPt		/**< Defines the title for the ghosts display of the transversale impulse for each event */
#define cmDisplayTitleEventPt      cmDisplayNameEventPt		/**< Defines the title for the clones display of the transversale impulse for each event */
#endif

#define emDisplayNameTotalPz       "HoughTransformPzEMT"	/**< Defines the name for the efficiency display of the accumulated momentum for all events */
#define fmDisplayNameTotalPz       "HoughTransformPzFMT"	/**< Defines the name for the fakes display of the accumulated momentum for all events */
#define gmDisplayNameTotalPz       "HoughTransformPzGMT"	/**< Defines the name for the ghosts display of the accumulated momentum for all events */
#define cmDisplayNameTotalPz       "HoughTransformPzCMT"	/**< Defines the name for the clones display of the accumulated momentum for all events */
#define emDisplayNameTotalPt       "HoughTransformPtEMT"	/**< Defines the name for the efficiency display of the accumulated transversale impulse for all events */
#define fmDisplayNameTotalPt       "HoughTransformPtFMT"	/**< Defines the name for the fakes display of the accumulated transversale impulse for all events */
#define gmDisplayNameTotalPt       "HoughTransformPtGMT"	/**< Defines the name for the ghosts display of the accumulated transversale impulse for all events */
#define cmDisplayNameTotalPt       "HoughTransformPtCMT"	/**< Defines the name for the clones display of the accumulated transversale impulse for all events */

#ifdef NOIDENTIFICATION
#define emDisplayTitleTotalPz      "35 AGeV"				/**< Defines the title for the efficiency display of the accumulated momentum for all events */
#define fmDisplayTitleTotalPz      emDisplayTitleTotalPz	/**< Defines the title for the fakes display of the accumulated momentum for all events */
#define gmDisplayTitleTotalPz      emDisplayTitleTotalPz	/**< Defines the title for the ghosts display of the accumulated momentum for all events */
#define cmDisplayTitleTotalPz      emDisplayTitleTotalPz	/**< Defines the title for the clones display of the accumulated momentum for all events */
#define emDisplayTitleTotalPt      "35 AGeV"				/**< Defines the title for the efficiency display of the accumulated transversale impulse for all events */
#define fmDisplayTitleTotalPt      emDisplayTitleTotalPt	/**< Defines the title for the fakes display of the accumulated transversale impulse for all events */
#define gmDisplayTitleTotalPt      emDisplayTitleTotalPt	/**< Defines the title for the ghosts display of the accumulated transversale impulse for all events */
#define cmDisplayTitleTotalPt      emDisplayTitleTotalPt	/**< Defines the title for the clones display of the accumulated transversale impulse for all events */
#else
#define emDisplayTitleTotalPz      emDisplayNameTotalPz		/**< Defines the title for the efficiency display of the accumulated momentum for all events */
#define fmDisplayTitleTotalPz      fmDisplayNameTotalPz		/**< Defines the title for the fakes display of the accumulated momentum for all events */
#define gmDisplayTitleTotalPz      gmDisplayNameTotalPz		/**< Defines the title for the ghosts display of the accumulated momentum for all events */
#define cmDisplayTitleTotalPz      cmDisplayNameTotalPz		/**< Defines the title for the clones display of the accumulated momentum for all events */
#define emDisplayTitleTotalPt      emDisplayNameTotalPt		/**< Defines the title for the efficiency display of the accumulated transversale impulse for all events */
#define fmDisplayTitleTotalPt      fmDisplayNameTotalPt		/**< Defines the title for the fakes display of the accumulated transversale impulse for all events */
#define gmDisplayTitleTotalPt      gmDisplayNameTotalPt		/**< Defines the title for the ghosts display of the accumulated transversale impulse for all events */
#define cmDisplayTitleTotalPt      cmDisplayNameTotalPt		/**< Defines the title for the clones display of the accumulated transversale impulse for all events */
#endif


#endif
