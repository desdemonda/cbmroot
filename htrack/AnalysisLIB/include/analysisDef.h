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
///     - class for defining the analysis definitions
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:58:06 $
/// $Revision: 1.13 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ANALYSISDEF_H
#define _ANALYSISDEF_H


//#define PEAKCONTRIBUTIONCOMPTABILITYMODE		/**< This definition enables the comptability with an older version of this software. */
#undef PEAKCONTRIBUTIONCOMPTABILITYMODE			/**< This definition switches to a new version of this software development */

//#define FOUNDTRACKISREALTRACKCOMPTABILITYMODE	/**< This definition enables the comptability with an older version of this software. */
#undef FOUNDTRACKISREALTRACKCOMPTABILITYMODE	/**< This definition switches to a new version of this software development */


#define WELLFOUNDHITFACTOR 0.7					/**< This defines that a track is well found, if more than WELLFOUNDHITFACTOR of the original hits of this track are found. */


/**
 * If defined, the trackIds of the not found tracks would be
 * written to the standard output stream.
 */

//#define WHICHTRACKIDISNOTFOUND
#undef WHICHTRACKIDISNOTFOUND


/*
 * Defining the bit positions in the parameters analysisInitAnalysisResultWarnings and analysisInitAnalysisResultDisplays
 */

#define TRACKWITHHITINCORRECTBORDERDISTRIBUTION           (1 <<  0)	/**< This definition sets the bit position for enabling the output for the prelut goodness results for the correct borders. */
#define TRACKWITHHITINBORDERDISTRIBUTION                  (1 <<  1)	/**< This definition sets the bit position for enabling the output for the prelut goodness results for the borders. */
#define MIDDLEDISTANCEOFMAXIMUMANDCORRECTLAYER            (1 <<  2)	/**< This definition sets the bit position for enabling the output of the prelut goodness results for the middle distance of the maximum and the correct cells. */
#define TRACKWITHHITWITHCORRECTSLOPEDISTRIBUTION          (1 <<  3)	/**< This definition sets the bit position for enabling the output for the prelut goodness results for the slopes. */
#define TRACKWITHYZPROJECTION                             (1 <<  4)	/**< This definition sets the bit position for enabling the output for the yz-projection of one track. */

#define TRACKWITHHITINCORRECTCELLDISTRIBUTION             (1 <<  5)	/**< This definition sets the bit position for enabling the output for the lut goodness results for the correct cells. */
#define TRACKWITHHITINCELLDISTRIBUTION                    (1 <<  6)	/**< This definition sets the bit position for enabling the output for the lut goodness results for the cells. */
#define MIDDLEDISTANCEOFMAXIMUMANDCORRECTCELL             (1 <<  7)	/**< This definition sets the bit position for enabling the output of the lut goodness results for the middle distance of the maximum and the correct cells. */
#define TRACKWITHSIGNATUREDISTRIBUTION                    (1 <<  8)	/**< This definition sets the bit position for enabling the output for the lut goodness results for the tracks' hit signature. */

#define TRACKWITHMOMENTAERRORDISTRIBUTION                 (1 <<  9)	/**< This definition sets the bit position for enabling the output for the Hough transform goodness results for the tracks. */

#define QUANTIZEDMOMENTADIMDISTRIBUTION                   (1 << 10)	/**< This definition sets the bit position for enabling the output for the quantization goodness results separate for each dimension of the momenta. */
#define QUANTIZEDHOUGHSPACEDIMDISTRIBUTION                (1 << 11)	/**< This definition sets the bit position for enabling the output for the quantization goodness results separate for each dimension of the Hough space. */
#define QUANTIZEDMOMENTADISTRIBUTION                      (1 << 12)	/**< This definition sets the bit position for enabling the output for the quantization goodness results for the cobined momenta dimensions. */
#define QUANTIZEDHOUGHSPACEDISTRIBUTION                   (1 << 13)	/**< This definition sets the bit position for enabling the output for the quantization goodness results for the combined Hough space dimensions. */
#define SAMEHOUGHSPACECELLDISTRIBUTION                    (1 << 14)	/**< This definition sets the bit position for enabling the output of the quantization goodness results for the histogram cells with more than one track. */
#define NUMBEROFTRACKSWHICHCANNOTBEFOUND                  (1 << 15)	/**< This definition sets the bit position for enabling the output of the number of tracks which cannot be found because of the quantization goodness. */

#define DIMPEAKDISTANCEDISTRIBUTION                       (1 << 16)	/**< This definition sets the bit position for enabling the output of the peak distance goodness results for the tracks. */

#define TRACKSPERCOLUMNDISTRIBUTION                       (1 << 17)	/**< This definition sets the bit position for enabling the output about the distribution of the number of found tracks per column. */
#define FIFOSPERCOLUMNDISTRIBUTION                        (1 << 18)	/**< This definition sets the bit position for enabling the output about the distribution of the fifo sizes for the found tracks per column. */

#define TRACKSPERROWDISTRIBUTION                          (1 << 19)	/**< This definition sets the bit position for enabling the output about the distribution of the number of found tracks per row. */
#define FIFOSPERROWDISTRIBUTION                           (1 << 20)	/**< This definition sets the bit position for enabling the output about the distribution of the fifo sizes for the found tracks per row. */

#define TRACKSPERLAYERDISTRIBUTION                        (1 << 21)	/**< This definition sets the bit position for enabling the output about the distribution of the number of found tracks per histogram layer. */

#define HITREADOUTDISTRIBUTION                            (1 << 22)	/**< This definition sets the bit position for enabling the output about the distribution of the number of hits with an entry in multiple histogram layers. */
#define HITREADOUTMEANDISTRIBUTION                        (1 << 23)	/**< This definition sets the bit position for enabling the output about the distribution of the mean number of histogram layers for one hit with different parallel implemented layers. */

#define MAGNETFIELDFACTORINFORMATION                      (1 << 24)	/**< This definition sets the bit position for enabling the output about the evaluated magnetfield factors. */

#define PRELUTRANGEINFORMATION                            (1 << 25)	/**< This definition sets the bit position for enabling the output about the evaluated prelut range. */

#define LUTCORRECTIONCOUNTER                              (1 << 26)	/**< This definition sets the bit position for enabling the output about the number of corrections in the lut object. */

#define GOODSIGNATURES                                    (1 << 27)	/**< This definition sets the bit position for enabling the output about the good signatures in the prelut and lut. */

#define GOODSIGNATURETABLES                               (1 << 28)	/**< This definition sets the bit position for enabling the output about the good signature tables in the prelut and lut. */

#define USEDSIGNATURETABLES                               (1 << 29)	/**< This definition sets the bit position for enabling the output about the used signature tables. */

#define HOUGHPICTURES                                     (1 << 30)	/**< This definition sets the bit position for enabling the output of some static Hough transform graphics for the thesis. */

#define FPGAHISTOGRAMPROCESSINGTIMING                     (1 << 31)	/**< This definition sets the bit position for enabling the output about the time consumption for FPGA implementations with multiple parallel implemented histogram layers. */

/*
 * Defining the bit positions in the parameters analysisInitAnalysisMoreResultWarnings and analysisInitAnalysisMoreResultDisplays
 */

#define MCTRACKVISUALIZATION                              (1 <<  0)	/**< This definition sets the bit position for enabling the output about the MCTrack distribution (hits) in the detector system. */

#define FOUNDTRACKVISUALIZATION                           (1 <<  1)	/**< This definition sets the bit position for enabling the output about the found track distribution (hits) in the detector system. */

#define PEAKFINDINGGEOMETRYVISUALIZATION                  (1 <<  2)	/**< This definition sets the bit position for enabling the output about the filter geometry which is used in the peakfinding process. */

#define PROJECTEDPEAKFINDINGGEOMETRYVISUALIZATION         (1 <<  3)	/**< This definition sets the bit position for enabling the output about the projected filter geometry which is used in the peakfinding process. */

#define COVEREDPEAKFINDINGGEOMETRYVISUALIZATION           (1 <<  4)	/**< This definition sets the bit position for enabling the output about the covered filter geometry which is used in the peakfinding process. */

#define COVEREDPROJECTEDPEAKFINDINGGEOMETRYVISUALIZATION  (1 <<  5)	/**< This definition sets the bit position for enabling the output about the covered and projected filter geometry which is used in the peakfinding process. */

#endif
