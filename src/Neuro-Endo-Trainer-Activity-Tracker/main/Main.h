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
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include "params.h"
#include <string>
#include <sstream>

using namespace std;
using namespace nets;
using namespace cv;
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	( std::ostringstream() << std::dec << x ) ).str()

typedef map<int, unsigned int> CounterMap;

enum Retval
{
    PROGRAM_EXIT = 0,
    SUCCESS = 1
};

class Main
{
public:
	// gui and image acquisiton objects;
	ImAcq *imAcq;
	nets::Gui *gui;
	int seed;
	bool showOutput;
	Rect pegGroupROI;
	double hueThreshVal_ring;
	int pegThresh_valSaturation;
	string status;
	cv::Mat element[1];
	
	pegBox _pegBox;
	RingBox _ringBox;


	CounterMap count_pegCode_contour;
	CounterMap count_contour_pegCode;
public:
	Main()
    {
		imAcq = NULL;
		gui = NULL;
		seed = 0;
		showOutput = true;
		pegGroupROI = Rect(PEGBOUNDINGBOX_X, PEGBOUNDINGBOX_Y, PEGBOUNDINGBOX_WIDTH, PEGBOUNDINGBOX_HEIGHT);
		hueThreshVal_ring = HUE_THRESHVAL_RING;
		pegThresh_valSaturation = PEGSATURATION_MINVALUE;
		status = STATIONARY;
		element[0] = getStructuringElement(MORPH_ELLIPSE, Size(8, 8), Point(0, 0));
	}

    ~Main()
    {
	}

	void run();
	void initializeBoundingBox(IplImage* img);
	void initializePegBox(IplImage* img);
	void initializeRingBox(IplImage* img);
	void activityDetection(IplImage* img);
	void ringSegmentation(IplImage* img, Mat &fgmask);
	void ringSegmentation(const Mat &img, Mat &fgmask);
	
	
};

#endif /* MAIN_H_ */
