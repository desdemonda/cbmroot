/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - implements methods to visualize tracks
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-10-19 14:32:16 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../include/analysisWarningMsg.h"
#include "../include/analysisError.h"
#include "../include/visualAnalysis.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoPainter.h"
#include "TStyle.h"


/* **************************************************************
 * CLASS visualAnalysis					 						*
 * **************************************************************/


/****************************************************************
 * This method initializes the global style for each display.	*
 ****************************************************************/

void visualAnalysis::initWindowStyle() {

	/* setting some globals for the display */
	if (gStyle != NULL) {

		gStyle->SetCanvasBorderMode(-1);
		gStyle->SetCanvasBorderSize(1);
		gStyle->SetCanvasColor(10);

		gStyle->SetFrameFillColor(10);
		gStyle->SetFrameBorderSize(1);
		gStyle->SetFrameBorderMode(-1);
		gStyle->SetFrameLineWidth(2);
		gStyle->SetFrameLineColor(1);

		gStyle->SetHistFillColor(0);
		gStyle->SetHistLineWidth(2);
		gStyle->SetHistLineColor(1);

		gStyle->SetPadColor(10);
		gStyle->SetPadBorderSize(1);
		gStyle->SetPadBorderMode(-1);

		gStyle->SetStatColor(10);
		gStyle->SetStatFont(22);

		gStyle->SetTitleColor(1, "X");
		gStyle->SetTitleOffset(1.0, "X");
		gStyle->SetTitleFont(22, "X");
		gStyle->SetLabelFont(22, "X");

		gStyle->SetTitleColor(1, "Y");
		gStyle->SetTitleOffset(0.8, "Y");
		gStyle->SetTitleFont(22, "Y");
		gStyle->SetLabelFont(22, "Y");

		gStyle->SetOptDate(0);
		gStyle->SetOptTitle(1);
		gStyle->SetOptStat(0);
		gStyle->SetOptFit(1);

	}
	else {

		gStyleNotFoundWarningMsg* gStyleNotFound = new gStyleNotFoundWarningMsg(ANALYSISLIB);
		gStyleNotFound->warningMsg();
		if(gStyleNotFound != NULL) {
			delete gStyleNotFound;
			gStyleNotFound = NULL;
		}

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

visualAnalysis::visualAnalysis() 
  : window(NULL),
    tracks(NULL),
    numberOfTracks(0),
    actualTrack(NULL)

{
  /*
	window         = NULL;
	tracks         = NULL;
	numberOfTracks = 0;
	actualTrack    = NULL;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

visualAnalysis::~visualAnalysis() {

	if (window != NULL) {

		delete window;
		window = NULL;

	}

	if (tracks != NULL) {

		for (unsigned short j = 0; j < numberOfTracks; j++) {

			if (tracks[j].track != NULL) {

				delete tracks[j].track;
				tracks[j].track = NULL;

			}

			if (tracks[j].hitIndex != NULL) {

				delete tracks[j].hitIndex;
				tracks[j].hitIndex = NULL;

			}

			if (tracks[j].momentum != NULL) {

				delete tracks[j].momentum;
				tracks[j].momentum = NULL;

			}

			if (tracks[j].coordinates != NULL) {

				delete tracks[j].coordinates;
				tracks[j].coordinates = NULL;

			}

		}

		delete tracks;
		tracks = NULL;

	}

}

/****************************************************************
 * method sets the visibility for the TGeoManager volumes to	*
 * display														*
 ****************************************************************/

void visualAnalysis::setupVolumeVisibility(std::string* volumes, unsigned short numberOfVolumes) {

	TObjArray*  listOfVolumes;
	TGeoVolume* actualVolume;
	std::string actualVolumeName;
	bool        makeVolumeInvisible;

	if (gGeoManager != NULL) {

		listOfVolumes = gGeoManager->GetListOfVolumes();

		if (listOfVolumes != NULL) {

			for (int i = 0; i < listOfVolumes->GetEntries(); i++) {

				actualVolume = (TGeoVolume*)listOfVolumes->At(i);
				
				if (actualVolume != NULL) {

					actualVolumeName  = actualVolume->GetName();

					makeVolumeInvisible = true;
					if (volumes != NULL) {

						for (unsigned short j = 0; j < numberOfVolumes; j++) {
					
							if (actualVolumeName.find(volumes[j], 0) != std::basic_string<char>::npos) {

								makeVolumeInvisible = false;
								break;

							}

						}

					}

					actualVolume->InvisibleAll(makeVolumeInvisible);

				}
				else {

					volumeNotFoundWarningMsg* volumeNotFound = new volumeNotFoundWarningMsg();
					volumeNotFound->warningMsg();
					if(volumeNotFound != NULL) {
						delete volumeNotFound;
						volumeNotFound = NULL;
					}

				}

			}

		}
		else {

			listOfVolumesNotFoundWarningMsg* listOfVolumesNotFound = new listOfVolumesNotFoundWarningMsg();
			listOfVolumesNotFound->warningMsg();
			if(listOfVolumesNotFound != NULL) {
				delete listOfVolumesNotFound;
				listOfVolumesNotFound = NULL;
			}

		}

	}
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

}

/****************************************************************
 * method resets the object										*
 ****************************************************************/

void visualAnalysis::reset() {

	if (window != NULL) {

		window->Close();
		delete window;
		window = NULL;

	}

	if (tracks != NULL) {

		for (unsigned short j = 0; j < numberOfTracks; j++) {

			if (tracks[j].track != NULL) {

				delete tracks[j].track;
				tracks[j].track = NULL;

			}

			if (tracks[j].hitIndex != NULL) {

				delete tracks[j].hitIndex;
				tracks[j].hitIndex = NULL;

			}

			if (tracks[j].momentum != NULL) {

				delete tracks[j].momentum;
				tracks[j].momentum = NULL;

			}

			if (tracks[j].coordinates != NULL) {

				delete tracks[j].coordinates;
				tracks[j].coordinates = NULL;

			}

		}

		delete tracks;
		tracks = NULL;

	}

	numberOfTracks = 0;
	actualTrack    = NULL;

}

/****************************************************************
 * method sets the track for actual track						*
 ****************************************************************/

void visualAnalysis::setActualTrack(unsigned int index) {

	if (index < numberOfTracks)
		actualTrack = &tracks[index];
	else
		throw indexIsOutOfTrackRangeError(index, numberOfTracks);

}

/****************************************************************
 * method adds the track and sets it for actual track			*
 ****************************************************************/

void visualAnalysis::addActualTrack(int trackIndex, double px, double py, double pz, int dim1, int dim2, int dim3, int pdgCode) {

	numberOfTracks++;

	if (tracks == NULL)
		tracks = (visualTrackInformation*)calloc(numberOfTracks, sizeof(visualTrackInformation));
	else
		tracks = (visualTrackInformation*)realloc(tracks, numberOfTracks * sizeof(visualTrackInformation));

	if (trackIndex == INVALIDTRACKINDEX)
		tracks[numberOfTracks - 1].track = new TGeoTrack(numberOfTracks - 1, pdgCode, NULL, NULL);
	else
		tracks[numberOfTracks - 1].track = new TGeoTrack(trackIndex, pdgCode, NULL, NULL);

	tracks[numberOfTracks - 1].hitIndex = NULL;

	if ((dim1 != 0) && (dim2 != 0) && (dim3 != 0)) {

		tracks[numberOfTracks - 1].momentum = new trackMomentum();
		tracks[numberOfTracks - 1].momentum->set(px, PX);
		tracks[numberOfTracks - 1].momentum->set(py, PY);
		tracks[numberOfTracks - 1].momentum->set(pz, PZ);

	}
	else
		tracks[numberOfTracks - 1].momentum = NULL;

	if ((dim1 > 0) && (dim2 > 0) && (dim3 > 0)) {

		tracks[numberOfTracks - 1].coordinates = new trackCoordinates();
		tracks[numberOfTracks - 1].coordinates->set(dim1, DIM1);
		tracks[numberOfTracks - 1].coordinates->set(dim2, DIM2);
		tracks[numberOfTracks - 1].coordinates->set(dim3, DIM3);

	}
	else
		tracks[numberOfTracks - 1].coordinates = NULL;

	setActualTrack(numberOfTracks - 1);

}

/****************************************************************
 * method adds a point to the actual track						*
 ****************************************************************/

void visualAnalysis::addPointToActualTrack(double x, double y, double z, double time, int hitIndex) {

	double     xLast;
	double     yLast;
	double     zLast;
	double     timeLast;

	if (actualTrack != NULL) {

		if (actualTrack->track->GetNpoints() > 0)
			actualTrack->track->GetLastPoint(xLast, yLast, zLast, timeLast);
		else
			timeLast = 0;

		if (time < 0) {

			actualTrack->track->AddPoint(x, y, z, timeLast + 1);

		}
		else {

			if (time > timeLast) {

				actualTrack->track->AddPoint(x, y, z, time);

			}
			else {

				timeViolationWarningMsg* timeViolation = new timeViolationWarningMsg(timeLast, time);
				timeViolation->warningMsg();
				if(timeViolation != NULL) {
					delete timeViolation;
					timeViolation = NULL;
				}

				actualTrack->track->AddPoint(x, y, z, timeLast + 0.1 * timeLast);

			}

		}

		if (actualTrack->hitIndex == NULL)
			actualTrack->hitIndex = (int*)calloc(actualTrack->track->GetNpoints(), sizeof(int));
		else
			actualTrack->hitIndex = (int*)realloc(actualTrack->hitIndex, actualTrack->track->GetNpoints() * sizeof(int));

		actualTrack->hitIndex[actualTrack->track->GetNpoints() - 1] = hitIndex;

	}
	else {

		actualTrackNotFoundWarningMsg* actualTrackNotFound = new actualTrackNotFoundWarningMsg();
		actualTrackNotFound->warningMsg();
		if(actualTrackNotFound != NULL) {
			delete actualTrackNotFound;
			actualTrackNotFound = NULL;
		}

	}

}

/****************************************************************
 * Method draws all tracks including the visible volumes.		*
 * If no track is set, just the visible volumes would be drawn.	*
 ****************************************************************/

void visualAnalysis::drawSetup(std::string name, std::string title, int trackIndex) {

	if (window == NULL) {

		initWindowStyle();
		window = new TCanvas(name.c_str(), title.c_str(), 600, 400);

	}
	else {

		window->Clear();

	}

	window->cd();

	if (gGeoManager != NULL) {

		((TGeoPainter*)gGeoManager->GetGeomPainter())->Draw();

		gGeoManager->SetAnimateTracks(true);

		for (unsigned int i = 0; i < numberOfTracks; i++) {

			if (tracks[i].track != NULL) {

				if ((trackIndex == INVALIDTRACKINDEX) || (trackIndex == tracks[i].track->GetId()))
					tracks[i].track->Draw();

			}

		}

		gGeoManager->SetAnimateTracks(false);
		gGeoManager->ModifiedPad();

	}
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

}

/****************************************************************
 * Method converts all tracks into a string representation.		*
 ****************************************************************/

std::string visualAnalysis::toString(std::string title, int trackIndex) {

	std::string returnValue;
	char        intBuffer[intConversionDigits+1];
	char        doubleBuffer[doubleConversion+1];
	double      x;
	double      y;
	double      z;
	double      time;

	returnValue  = title;
	returnValue += ":\n";

	for (unsigned int i = 0; i < numberOfTracks; i++) {

		if (tracks[i].track != NULL) {

			if ((trackIndex == INVALIDTRACKINDEX) || (trackIndex == tracks[i].track->GetId())) {

				if (tracks[i].momentum != NULL) {

					returnValue += "px = ";
					dtos(tracks[i].momentum->get(PX), doubleBuffer, doubleConversionDigits);
					returnValue += doubleBuffer;
					returnValue += ", py = ";
					dtos(tracks[i].momentum->get(PY), doubleBuffer, doubleConversionDigits);
					returnValue += doubleBuffer;
					returnValue += ", pz = ";
					dtos(tracks[i].momentum->get(PZ), doubleBuffer, doubleConversionDigits);
					returnValue += doubleBuffer;

					if (tracks[i].coordinates != NULL)
						returnValue += ", dim1 = ";
					else
						returnValue += "\n";

				}

				if (tracks[i].coordinates != NULL) {

					if (tracks[i].momentum == NULL)
						returnValue += "dim1 = ";

					itos(tracks[i].coordinates->get(DIM1), intBuffer, 10, intConversionDigits);
					returnValue += intBuffer;
					returnValue += ", dim2 = ";
					itos(tracks[i].coordinates->get(DIM2), intBuffer, 10, intConversionDigits);
					returnValue += intBuffer;
					returnValue += ", dim3 = ";
					itos(tracks[i].coordinates->get(DIM3), intBuffer, 10, intConversionDigits);
					returnValue += intBuffer;
					returnValue += "\n";

				}

				for (int j = 0; j < tracks[i].track->GetNpoints(); j++) {

					tracks[i].track->GetPoint(j, x, y, z, time);
					returnValue += "  ";

					if (tracks[i].hitIndex != NULL) {

						returnValue += "hitId = ";
						itos(tracks[i].hitIndex[j], intBuffer, 10, intConversionDigits);
						returnValue += intBuffer;
						returnValue += ", trackId = ";

					}
					else
						returnValue += "trackId = ";

					itos(tracks[i].track->GetId(), intBuffer, 10, intConversionDigits);
					returnValue += intBuffer;
					returnValue += ", x = ";
					dtos(x, doubleBuffer, doubleConversionDigits);
					returnValue += doubleBuffer;
					returnValue += ", y = ";
					dtos(y, doubleBuffer, doubleConversionDigits);
					returnValue += doubleBuffer;
					returnValue += ", z = ";
					dtos(z, doubleBuffer, doubleConversionDigits);
					returnValue += doubleBuffer;
					returnValue += ", time = ";
					dtos(time, doubleBuffer, doubleConversionDigits);
					returnValue += doubleBuffer;
					returnValue += "\n";

				}

			}

		}

	}

	return returnValue;

}
