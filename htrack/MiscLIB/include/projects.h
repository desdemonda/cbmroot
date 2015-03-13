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
///     - global definition of projectnames
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 14:02:13 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PROJECTS_H
#define _PROJECTS_H


/**
 * These definitions set a std::string representation for each project.
 * It is used especially for the global errors which can occur. Each
 * project which throws a global error places one of these strings in the throwing
 * way. So one has the information which project throws this error.
 */

#define ANALYSISLIB                "AnalysisLIB"
#define DATAOBJECTLIB              "DataObjectLIB"
#define DATAROOTOBJECTLIB          "DataRootObjectLIB"
#define FILEIOLIB                  "FileioLIB"
#define ROOTFRAMEWORKLIB           "RootFrameworkLIB"
#define HISTOGRAMTRANSFORMATIONLIB "HoughTransformationLIB"
#define HOUGHTRANSFORMATIONLIB     "HoughTransformationLIB"
#define INPUTLIB                   "InputLIB"
#define LUTGENERATORLIB            "LutGeneratorLIB"
#define MISCLIB                    "MiscLIB"
#define OUTPUTDATALIB              "OutputDataLIB"
#define OUTPUTRESULTLIB            "OutputResultLIB"
#define PPUCODEEXE                 "PpuCode"
#define TRACKFINDERLIB             "TrackfinderLIB"


#endif
