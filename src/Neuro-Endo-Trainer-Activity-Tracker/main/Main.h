/*  Copyright 2016 Neurosurgery Education and Training School, AIIMS
*
*   This file is part of Neuro-Endo-Trainer-Activity-Tracker.
*
*   Neuro-Endo-Trainer-Activity-Tracker is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   Neuro-Endo-Trainer-Activity-Tracker is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with Neuro-Endo-Trainer-Activity-Tracker.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/**
* @author britty baby and vinkle srivastav
*/


#ifndef MAIN_H_
#define MAIN_H_

#include "ImAcq.h"
#include "Gui.h"
#include "RingBox.h"
#include "ContourFeature.hpp"
#include "pegBox.h"
#include "Scorer.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include "params.h"
#include "TLD.h"
#include <string>
#include <sstream>

using namespace std;
using namespace nets;
using namespace cv;
using namespace tld;

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	( std::ostringstream() << std::dec << x ) ).str()

enum Retval
{
    PROGRAM_EXIT = 0,
    SUCCESS = 1
};

class Main
{
public:
	
	tld::TLD *tld;
	ImAcq *imAcq;
	nets::Gui *gui;
	params parameters;

	bool showOutput;
	const char *printResults;
	const char *saveDir;
	double threshold;
	bool showForeground;
    bool showNotConfident;
	bool exportModelAfterRun;
    bool loadModel;
    const char *modelPath;
    const char *modelExportFile;
	int seed, dist, temp;
	bool reinit;
	int pegThresh_valSaturation;
	double hueThreshVal_ring;
	Rect pegGroupROI;
	string status;
	cv::Mat element[4];
	pegBox _pegBox;
	RingBox _ringBox;
	Mat prv_frame, curr_frame;
	cv::BackgroundSubtractor* pMOG;
	bool updateBackgroundModel;
	bool updateBackgroundModel2;
	bool firstTimeRingDetection;
	Rect mvRingROI;
	bool first_tracking_failed_detection;
	bool trackingStart;
	int fileWriteCount;
	Rect prevmvRingROI;
	bool movingROIDetection;
	int pickingCount;
	int StableFrameCount;
	int prvFrameNo_RingStability;
	int currFrameNo_RingStability;
	bool update__RingStability;
	bool update__RingStability2;
	// 
	Activity activity;
	vector<Activity> scorer;
	bool No_activity_start = false;
	bool picking_activity_start = false;
	bool moving_activity_start = false;
	bool update_startFrameAndType = true;
	bool RingHitting = false;
	bool JerKDetection = false; 
	bool trackinReinit = false;
	int index = -1;

	// 
	Result result;

public:
	Main(){}

	void init()
	{
		tld = new tld::TLD();
		imAcq = NULL;
		gui = NULL;
		seed = 0;
		showOutput = parameters.SHOW_OUTPUT;
		pegGroupROI = Rect(parameters.PEGBOUNDINGBOX_X, parameters.PEGBOUNDINGBOX_Y, parameters.PEGBOUNDINGBOX_WIDTH, parameters.PEGBOUNDINGBOX_HEIGHT);
		hueThreshVal_ring = parameters.HUE_THRESHVAL_RING;
		pegThresh_valSaturation = parameters.PEGSATURATION_MINVALUE;
		status = STATIONARY;
		element[0] = getStructuringElement(MORPH_ELLIPSE, Size(8, 8), Point(0, 0));
		element[1] = getStructuringElement(MORPH_ELLIPSE, Size(8, 8), Point(0, 0));
		element[2] = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(0, 0));
		element[3] = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(0, 0));
		printResults = parameters.PRINT_RESULT_FILE.c_str();
		saveDir = parameters.SAVE_DIRECTORY.c_str();
		threshold = parameters.THRESHOLD;
		showForeground = parameters.SHOW_FOREGROUND;
		reinit = 0;
		loadModel = parameters.LOAD_MODEL;
		showNotConfident = parameters.SHOW_NOT_CONFIDENT;
		exportModelAfterRun = parameters.EXPORT_MODEL_AFTER_RUN;
		modelExportFile = parameters.MODEL_EXPORT_FILE.c_str();
		gui = NULL;
		modelPath = parameters.MODEL_PATH.c_str();
		imAcq = NULL;
		pMOG = new BackgroundSubtractorMOG2(20, 100, false);
		updateBackgroundModel = false;
		updateBackgroundModel2 = false;
		dist = 0;
		temp = 0;
		firstTimeRingDetection = false;
		mvRingROI = Rect(0, 0, 0, 0);
		prevmvRingROI = Rect(0, 0, 0, 0);
		first_tracking_failed_detection = false;
		trackingStart = false;
		fileWriteCount = 0;
		movingROIDetection = true;
		pickingCount = 0;
		StableFrameCount = 0;
		prvFrameNo_RingStability = 0;
		currFrameNo_RingStability = 0;
		update__RingStability = true;
		update__RingStability2 = false;
	}
    ~Main()
    {
		delete tld;
        imAcqFree(imAcq);
		delete pMOG;
	}

	void run();
	void initializeBoundingBox(IplImage* img);
	void initializePegBox(IplImage* img);
	void initializeRingBox(IplImage* img);
	void ringSegmentation(IplImage* img, Mat &fgmask);
	void ringSegmentation(const Mat &img, Mat &fgmask);
	void HittingDetection(const Mat &prv_frame, const Mat &curr_frame, vector<unsigned int>  &hittingData);
	bool activityDetection(const Mat &prev_frame, const Mat &curr_frame, const Rect &trackingOut);
	bool AllRingStable(const Mat &prv_frame, const Mat &curr_frame, const Rect &movingRing);
	Rect getmovingRingROI(const Mat &curr_frame);
	void computeResult(const vector<Activity> &scorer);
	void displayResult(const Result &);
	void writeResult(const Result &);
	void testPrintScorer(const vector<Activity> &scorer);
	
};

#endif /* MAIN_H_ */
