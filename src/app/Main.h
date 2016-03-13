/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/*
 * main.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Georg Nebehay
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "TLD.h"
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
#include <string>
#include <sstream>


using namespace std;
using namespace cv;
using namespace tld;

enum Retval
{
    PROGRAM_EXIT = 0,
    SUCCESS = 1
};

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	(std::ostringstream() << std::dec << x)).str()

class Main
{
public:
    tld::TLD *tld;
    ImAcq *imAcq;
    tld::Gui *gui;
    bool showOutput;
	bool showTrajectory;
	int trajectoryLength;
    const char *printResults;
    const char *saveDir;
    double threshold;
    bool showForeground;
    bool showNotConfident;
    bool selectManually;
    int *initialBB;
    bool reinit;
    bool exportModelAfterRun;
    bool loadModel;
    const char *modelPath;
    const char *modelExportFile;
    int seed;
	
	//
	int dist, temp;

	// extra entries2
	int PEGBOUNDINGBOX_X;
	int PEGBOUNDINGBOX_Y;
	int PEGBOUNDINGBOX_WIDTH;
	int PEGBOUNDINGBOX_HEIGHT;
	int PEGSATURATION_MINVALUE;
	int MIN_PEG_SIZE;
	float HUE_THRESHVAL_RING;
	int STABLE_COUNT;
	int TLD_WITH_TRACKER_ONLY;
	int STATIONARY_FRAME_COUNT;
	int CONSECUTIVE_FRAME_COUNT;
	int NO_OF_IMAGES_HITTING_DETECTION;
	int HITTING_THRESHOLD;


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
	Result result;

	bool No_activity_start;
	bool picking_activity_start;
	bool moving_activity_start;
	bool update_startFrameAndType;
	bool RingHitting;
	bool JerKDetection;
	bool trackinReinit;
	int index;



    Main()
    {
        tld = new tld::TLD();
        showOutput = 1;
        printResults = NULL;
        saveDir = ".";
        threshold = 0.5;
        showForeground = 0;

		showTrajectory = false;
		trajectoryLength = 0;

        selectManually = 0;

        initialBB = NULL;
        showNotConfident = true;

        reinit = 0;

        loadModel = false;

        exportModelAfterRun = false;
        modelExportFile = "model";
        seed = 0;

        gui = NULL;
        modelPath = NULL;
        imAcq = NULL;

		// extra entries
		PEGBOUNDINGBOX_X = 160;
		PEGBOUNDINGBOX_Y = 240;
		PEGBOUNDINGBOX_WIDTH = 830;
		PEGBOUNDINGBOX_HEIGHT = 550;
		PEGSATURATION_MINVALUE = 120;
		MIN_PEG_SIZE = 25;
		HUE_THRESHVAL_RING = 20.0;
		STABLE_COUNT = 5;
		TLD_WITH_TRACKER_ONLY = 0;
		STATIONARY_FRAME_COUNT = 20;
		CONSECUTIVE_FRAME_COUNT = 10;
		NO_OF_IMAGES_HITTING_DETECTION = 10;
		HITTING_THRESHOLD = 30;


		pegGroupROI = Rect(PEGBOUNDINGBOX_X, PEGBOUNDINGBOX_Y, PEGBOUNDINGBOX_WIDTH, PEGBOUNDINGBOX_HEIGHT);
		hueThreshVal_ring = HUE_THRESHVAL_RING;
		pegThresh_valSaturation = PEGSATURATION_MINVALUE;
		status = STATIONARY;
		element[0] = getStructuringElement(MORPH_ELLIPSE, Size(8, 8), Point(0, 0));
		element[1] = getStructuringElement(MORPH_ELLIPSE, Size(8, 8), Point(0, 0));
		element[2] = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(0, 0));
		element[3] = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(0, 0));

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

		No_activity_start = false;
		picking_activity_start = false;
		moving_activity_start = false;
		update_startFrameAndType = true;
		RingHitting = false;
		JerKDetection = false;
		trackinReinit = false;
		index = -1;

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
	void processTrackingData();
};

#endif /* MAIN_H_ */
