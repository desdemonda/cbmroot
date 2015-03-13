//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl�
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
/// $Date: 2007-12-28 14:40:06 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TOTALANALYSISDEF_H
#define _TOTALANALYSISDEF_H


#define CORRECTIONCOUNTERNAME       "CCDistribution"												/**< Defines the name of the display */
#define CORRECTIONCOUNTERTITLE      "Correction Counter Distribution"								/**< Defines the title for the display */
#define CORRECTIONCOUNTERXAXITITLE  "Dim1/Dim2-ratio"												/**< Defines the text to display for the title of the x-axi */
#define CORRECTIONCOUNTERYAXITITLE  "Number of corrected transformations"							/**< Defines the text to display for the title of the y-axi */

#define numberOfCorrectionCounters 15
const double correctionCounterSteps[numberOfCorrectionCounters]  = {1.5, 1.75, 2.0, 2.25 , 2.5, 2.75, 3.0, 3.25, 3.50, 3.75, 4.00, 4.25, 4.50, 4.75, 5.00};
const double correctionCounterValues[numberOfCorrectionCounters] = {  0,    0,   0,    0,    0,    0, 129,  363, 2578, 4270, 4548, 8334, 8334, 8334, 8336};


#define NOTFINDABLETRACKSNAME          "NFTDistribution"											/**< Defines the name of the display */
#define NOTFINDABLETRACKSTITLE         "Not Findable Tracks Distribution"							/**< Defines the title for the display */
#define NOTFINDABLETRACKSXAXITITLE     "Quantization identifier"									/**< Defines the text to display for the title of the x-axi */
#define NOTFINDABLETRACKSYAXITITLE     "Number of not findable tracks"								/**< Defines the text to display for the title of the y-axi */

#define numberOfTracksWhichArenotFindable 15
const double tracksWhichAreNotFindableSteps[numberOfTracksWhichArenotFindable]  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const double tracksWhichAreNotFindableValues[numberOfTracksWhichArenotFindable] = {0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0};


#define TRACKSWITHGOODPRELUTSIGNATURENAME          "TWGPSDistribution"								/**< Defines the name of the display */
#define TRACKSWITHGOODPRELUTSIGNATURETITLE         "Tracks With Good Prelut Signature Distribution"	/**< Defines the title for the display */
#define TRACKSWITHGOODPRELUTSIGNATUREXAXITITLE     "Dim3/Dim2-ratio"								/**< Defines the text to display for the title of the x-axi */
#define TRACKSWITHGOODPRELUTSIGNATUREYAXITITLE     "Number of tracks"								/**< Defines the text to display for the title of the y-axi */

#define numberOfTracksWithAGoodPrelutSignature 15
const double tracksWithAGoodPrelutSignatureSteps[numberOfTracksWithAGoodPrelutSignature]  = {1.5, 1.75, 2.0, 2.25, 2.5, 2.75, 3.0, 3.25, 3.50, 3.75, 4.00, 4.25, 4.50, 4.75, 5.00};
const double tracksWithAGoodPrelutSignatureValues[numberOfTracksWithAGoodPrelutSignature] = {341,  340, 340,  340, 339,  340, 339,  340,  340,  340,  337,  337,  336,  340,  342};


#define TRACKSWITHGOODLUTSIGNATURENAME          "TWGLSDistribution"									/**< Defines the name of the display */
#define TRACKSWITHGOODLUTSIGNATURETITLE         "Tracks With Good Lut Signature Distribution"		/**< Defines the title for the display */
#define TRACKSWITHGOODLUTSIGNATUREXAXITITLE     "Dim1/Dim2-ratio"									/**< Defines the text to display for the title of the x-axi */
#define TRACKSWITHGOODLUTSIGNATUREYAXITITLE     "Number of tracks"									/**< Defines the text to display for the title of the y-axi */

#define numberOfTracksWithAGoodLutSignature 15
const double tracksWithAGoodLutSignatureSteps[numberOfTracksWithAGoodLutSignature]  = {1.5, 1.75, 2.0, 2.25, 2.5, 2.75, 3.0, 3.25, 3.50, 3.75, 4.00, 4.25, 4.50, 4.75, 5.00};
const double tracksWithAGoodLutSignatureValues[numberOfTracksWithAGoodLutSignature] = {325,  323, 321,  321, 324,  325, 319,  322,  322,  320,  314,  313,  303,  298,  293};


#define GOODPRELUTSIGNATURESNAME          "GPSDistribution"											/**< Defines the name of the display */
#define GOODPRELUTSIGNATURESTITLE         "Good Prelut Signature Distribution"						/**< Defines the title for the display */
#define GOODPRELUTSIGNATURESXAXITITLE     "Dim3/Dim2-ratio"											/**< Defines the text to display for the title of the x-axi */
#define GOODPRELUTSIGNATURESYAXITITLE     "Number of tracks"										/**< Defines the text to display for the title of the y-axi */

#define numberOfGoodPrelutSignatures 15
const double goodPrelutSignaturesSteps[numberOfGoodPrelutSignatures]  = {1.5, 1.75, 2.0, 2.25, 2.5, 2.75, 3.0, 3.25, 3.50, 3.75, 4.00, 4.25, 4.50, 4.75, 5.00};
const double goodPrelutSignaturesValues[numberOfGoodPrelutSignatures] = {  2,    2,   2,    2,   2,    2,   2,    2,    2,    2,    2,    2,    2,    2,    3};


#define GOODLUTSIGNATURESNAME          "GLSDistribution"											/**< Defines the name of the display */
#define GOODLUTSIGNATURESTITLE         "Good Lut Signature Distribution"							/**< Defines the title for the display */
#define GOODLUTSIGNATURESXAXITITLE     "Dim1/Dim2-ratio"											/**< Defines the text to display for the title of the x-axi */
#define GOODLUTSIGNATURESYAXITITLE     "Number of tracks"											/**< Defines the text to display for the title of the y-axi */

#define numberOfGoodLutSignatures 15
const double goodLutSignaturesSteps[numberOfGoodLutSignatures]  = {1.5, 1.75, 2.0, 2.25, 2.5, 2.75, 3.0, 3.25, 3.50, 3.75, 4.00, 4.25, 4.50, 4.75, 5.00};
const double goodLutSignaturesValues[numberOfGoodLutSignatures] = {  3,    3,    3,   3,   3,    3,   3,    3,    3,    3,    2,    2,    1,    1,    1};


#define GOODPRELUTSIGNATURETABLENAME       "GPSTDistribution"								/**< Defines the name of the display */
#define GOODPRELUTSIGNATURETABLETITLE      "Good Prelut Signature Table Distribution"		/**< Defines the title for the display */
#define GOODPRELUTSIGNATURETABLEXAXITITLE  "Signature"										/**< Defines the text to display for the title of the x-axi */
#define GOODPRELUTSIGNATURETABLEYAXITITLE  "Frequency of occurence"							/**< Defines the text to display for the title of the y-axi */


#define GOODLUTSIGNATURETABLENAME          "GLSTDistribution"								/**< Defines the name of the display */
#define GOODLUTSIGNATURETABLETITLE         "Good Lut Signature Table Distribution"			/**< Defines the title for the display */
#define GOODLUTSIGNATURETABLEXAXITITLE     "Signature"										/**< Defines the text to display for the title of the x-axi */
#define GOODLUTSIGNATURETABLEYAXITITLE     "Frequency of occurence"							/**< Defines the text to display for the title of the y-axi */


#define CODINGTABLENAME                    "UCTDistribution"								/**< Defines the name of the display */
#define CODINGTABLETITLE                   "Used CodingTable Distribution"					/**< Defines the title for the display */
#define CODINGTABLEXAXITITLE               "Signature"										/**< Defines the text to display for the title of the x-axi */
#define CODINGTABLEYAXITITLE               "Classification"									/**< Defines the text to display for the title of the y-axi */


#define GRADINGPTABLENAME                  "UGPTDistribution"								/**< Defines the name of the display */
#define GRADINGPTABLETITLE                 "Used GradingPTable Distribution"				/**< Defines the title for the display */
#define GRADINGPTABLEXAXITITLE             "Signature"										/**< Defines the text to display for the title of the x-axi */
#define GRADINGPTABLEYAXITITLE             "Classification"									/**< Defines the text to display for the title of the y-axi */


#define GRADINGRTABLENAME                  "UGRTDistribution"								/**< Defines the name of the display */
#define GRADINGRTABLETITLE                 "Used GradingRTable Distribution"				/**< Defines the title for the display */
#define GRADINGRTABLEXAXITITLE             "Signature"										/**< Defines the text to display for the title of the x-axi */
#define GRADINGRTABLEYAXITITLE             "Classification"									/**< Defines the text to display for the title of the y-axi */


#endif
