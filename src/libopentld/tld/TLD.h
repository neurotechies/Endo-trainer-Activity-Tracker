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
 * TLD.h
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#ifndef TLD_H_
#define TLD_H_

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/opencv.hpp>

#include "MedianFlowTracker.h"
#include "DetectorCascade.h"

namespace tld
{

class TLD
{
    void storeCurrentData();
    void fuseHypotheses();
    void learn();
    void initialLearning();
public:
    bool trackerEnabled;
    bool detectorEnabled;
    bool learningEnabled;
    bool alternating;
	//bool storePrevBB;
	//int countDist;
	//int prevFrameNo;
	//int currFrameNo;

    MedianFlowTracker *medianFlowTracker;
    DetectorCascade *detectorCascade;
    NNClassifier *nnClassifier;
    bool valid;
    bool wasValid;
    cv::Mat prevImg;
    cv::Mat currImg;
	cv::Rect *prevBB;
	//cv::Rect *prevBB1;
    cv::Rect *currBB;
    float currConf;
    bool learning;

    TLD();
    virtual ~TLD();
	void learnPatch(const cv::Mat &img, cv::Rect *bb);
    void release();
    void selectObject(const cv::Mat &img, cv::Rect *bb);
    void processImage(const cv::Mat &img);
	void processImageWithTracker(const cv::Mat &img);
    void writeToFile(const char *path);
    void readFromFile(const char *path);
};

} /* namespace tld */
#endif /* TLD_H_ */
