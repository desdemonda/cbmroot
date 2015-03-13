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
///   class:
///     - class for evaluating the histogram
///     - class computes the tracks
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-03 14:36:07 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDEF_H
#define _FILTERDEF_H


/*
 * Defines all different general filter types
 */

enum {NOFILTER, MAXMORPHSEARCHFILTER, ERASERFILTER, FILEFILTER, AUTOMATICFIRSTEVENTFILTER, AUTOMATICEACHEVENTFILTER, AUTOMATICUPDATEEVENTFILTER};

/**
 * Defines the filter's geometrie which should be used:
 *
 * FIRSTFILTERGEOMETRIETYPE |            FILTEROBJECTS            | FILTERSTRUCTURE | FILTERSIZE
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |                 |
 *                          | firstFilter  = NULL                 |                 |
 *             0            | secondFilter = NULL                 |   no structure  | 0
 *                          | combiFilter  = NULL                 |                 |
 *                          |                                     |                 |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |  X              |
 *                          | firstFilter  = filterDim2()         |  X              | f2
 *             1            | secondFilter = filterDim1()         |  X + X X X X X  | f1
 *                          | combiFilter  = NULL                 |  X              |
 *                          |                                     |  X              |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |      X    X     |
 *                          | firstFilter  = filterDim12()        |     X     X     | f1
 *             2            | secondFilter = filterDim2()         |    X   +  X     | f2
 *                          | combiFilter  = NULL                 |   X       X     |
 *                          |                                     |  X        X     |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |     XXXX        |
 *                          | firstFilter  = NULL                 |     XXXX        |
 *             3            | secondFilter = NULL                 |     X           |
 *                          | combiFilter  = filterDim1Dim2()     |  XXXX           | 2 * { S[i=0;f2/2-1] (f1+1)/2 } + 1 = (f2/2)*(f1+1)+1
 *                          |                                     |  XXXX           |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |     XXXX        |
 *                          | firstFilter  = NULL                 |     XXXX        |
 *             4            | secondFilter = NULL                 |  XXXXXXX        |
 *                          | combiFilter  = filterDim1Dim2Mod()  |  XXXX           | 2 * { S[i=0;f2/2-1] (f1+1)/2 } + f1 = (f2/2)*(f1+1)+f1
 *                          |                                     |  XXXX           |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |      XXXX       |
 *                          | firstFilter  = NULL                 |      XXX        |
 *             5            | secondFilter = NULL                 |     XXX         |
 *                          | combiFilter  = filterDim12Dim1()    |    XXX          | 2 * { S[i=0;f1/2] min(i, f2) } + f2 = 2 * { S[i=0;f1/2] i } + f2 - 2 * { S[i=0;f1/2-f2] i } = {(f1/2)*(f1/2+1) + f2} - {max(f1/2-f2,0)*(f1/2-f2+1)}
 *                          |                                     |   XXXX          |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |     XXXX        |
 *                          | firstFilter  = NULL                 |     XXXX        |
 *             6            | secondFilter = NULL                 |     X inPlace   |
 *                          | combiFilter  = firstFilterFinal()   |  XXXX           | 2 * { S[i=0;f2/2-1] (f1+1)/2 } + 1 = (f2/2)*(f1+1)+1 different localFiltersize
 *                          |                                     |  XXXX           |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *                          |                                     |     XXXX        |
 *                          | firstFilter  = NULL                 |     XXXX        |
 *             7            | secondFilter = NULL                 |     X copy      |
 *                          | combiFilter  = firstFilterFinalMod()|  XXXX           | 2 * { S[i=0;f2/2-1] (f1+1)/2 } + 1 = (f2/2)*(f1+1)+1 different localFiltersize
 *                          |                                     |  XXXX           |
 *-----------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * S[i=0;x] is an abbreviation for Sum from i = 0 to x
 * f1 is an abbreviation for filtersize1 (This value must be odd)
 * f2 is an abbreviation for filtersize2 (This value must be odd)
 */

enum {NOFIRSTGEOMETRY, FIRST21GEOMETRY, FIRST122GEOMETRY, FIRST12GEOMETRY, FIRST12MODGEOMETRY, FIRST121GEOMETRY, FIRSTFINALGEOMETRY, FIRSTFINALMODGEOMETRY};

/**
 * Defines the filter strategy which should be used:
 *
 * FIRSTFILTERHANDLINGTYPE |      FILTEROBJECT      | FILTERSTRATEGY   
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            0            | filterBasicNeutral     | no filtering
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            1            | filterBasicSimple      | delete array[element] if array[i] >= array[element]
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            2            | filterBasicSimpleMod   | delete array[element] if array[i] > array[element]
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            3            | filterBasicComplex     | delete array[element] if not global maximum or is not the local middle element
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            4            | filterBasicComplexMod  | delete array[element] if not global maximum or is not the absolute local maximum
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            5            | filterBasicSpecial     | if array[element] >= maximumClass use filterBasicSimpleMod, else use filterBasicSimple
 *--------------------------------------------------------------------------------------------------------------------------------------------
 */

enum {NOFIRSTARITHMETIC, FIRSTSIMPLEARITHMETIC, FIRSTSIMPLEMODARITHMETIC, FIRSTCOMPLEXARITHMETIC, FIRSTCOMPLEXMODARITHMETIC, FIRSTSPECIALARITHMETIC};

/**
 * SECONDFILTERGEOMETRIETYPE |            FILTEROBJECTS            | FILTERSTRUCTURE  | FILTERSIZE                                                                                                | FILTERSTRUCTURE (rotate dim3 90°)
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |                  |                                                                                                           |
 *                           |                                     |                  |                                                                                                           |
 *             0             | thirdFilter  = NULL                 |   no structure   | 0                                                                                                         |  no structure
 *                           |                                     |                  |                                                                                                           |
 *                           |                                     |                  |                                                                                                           |
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |                  |                                                                                                           |
 *                           |                                     |                  |                                                                                                           |
 *             1             | thirdFilter  = filterDim3Mod        |        X copy    | f3                                                                                                        |  X X X X X
 *                           |                                     |                  |                                                                                                           |      copy
 *                           |                                     |                  |                                                                                                           |
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |        XXXX      |                                                                                                           |     XXX
 *                           |                                     |        XXXX      |                                                                                                           |     XXX
 *             2             | thirdFilter  = secondFilterFinal    |        X inplace | (f3-1) * ( 2 * { S[i=0;f1/2-1] (f2+1)/2 } + 1) + 1 = (f3-1)*((f1/2)*(f2+1)+1)+1 different localFiltersize |     XXX
 *                           |                                     |     XXXX         |                                                                                                           |     XXX inplace
 *                           |                                     |     XXXX         |                                                                                                           |     XXX
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |        XXXX      |                                                                                                           |     XXX
 *                           |                                     |        XXXX      |                                                                                                           |     XXX
 *             3             | thirdFilter  = secondFilterFinalMod |        X copy    | (f3-1) * ( 2 * { S[i=0;f1/2-1] (f2+1)/2 } + 1) + 1 = (f3-1)*((f1/2)*(f2+1)+1)+1 different localFiltersize |     XXX
 *                           |                                     |     XXXX         |                                                                                                           |     XXX copy
 *                           |                                     |     XXXX         |                                                                                                           |     XXX
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                           |                                     |                  |                                                                                                           |
 *                           |                                     |                  |                                                                                                           |
 *             4             | thirdFilter  = filterDim3           |        X inplace | f3                                                                                                        |  X X X X X
 *                           |                                     |                  |                                                                                                           |      inplace
 *                           |                                     |                  |                                                                                                           |
 *------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * f3 is an abbreviation for filtersize3 (This value must be odd)
 */

enum {NOSECONDGEOMETRY, SECOND3MODGEOMETRY, SECONDFINALGEOMETRY, SECONDFINALMODGEOMETRY, SECOND3GEOMETRY};

/**
 * Defines the filter strategy which should be used:
 *
 * SECONDFILTERHANDLINGTYPE |      FILTEROBJECT      | FILTERSTRATEGY   
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            0             | filterBasicNeutral     | no filtering
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            1             | filterBasicSimple      | delete array[element] if array[i] >= array[element]
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            2             | filterBasicSimpleMod   | delete array[element] if array[i] > array[element]
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            3             | filterBasicComplex     | delete array[element] if not global maximum or is not the local middle element
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            4             | filterBasicComplexMod  | delete array[element] if not global maximum or is not the absolute local maximum
 *--------------------------------------------------------------------------------------------------------------------------------------------
 *            5             | filterBasicSpecial     | if array[element] >= maximumClass use filterBasicSimpleMod, else use filterBasicSimple
 *--------------------------------------------------------------------------------------------------------------------------------------------
 */

enum {NOSECONDARITHMETIC, SECONDSIMPLEARITHMETIC, SECONDSIMPLEMODARITHMETIC, SECONDCOMPLEXARITHMETIC, SECONDCOMPLEXMODARITHMETIC, SECONDSPECIALARITHMETIC};

#endif
