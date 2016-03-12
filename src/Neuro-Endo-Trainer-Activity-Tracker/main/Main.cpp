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

#include "Main.h"

#include "ImAcq.h"
#include "Gui.h"

// Determine using the successive frame difference and moving ring 
// that the all rings are been stable or not
bool Main::AllRingStable(const Mat &prv_frame, const Mat &curr_frame, const Rect &movingRingROI)
{
	bool retVal = false;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	// vars declaration
	Mat diff, canny_output, dst, diff_hsv, mask, mask_out, out1;


	// absolute diffrence between successive frames
	cv::absdiff(prv_frame, curr_frame, diff);
	cvtColor(diff, mask, CV_BGR2GRAY);

	cv::threshold(mask, mask, 5, 255, THRESH_BINARY); //b1((b1 > 0))= 255;

	curr_frame.copyTo(out1, mask);


	cvtColor(out1, diff_hsv, CV_BGR2HSV);
	Mat channel[3];
	split(diff_hsv, channel);

	////Mat fgMaskRing;
	cv::threshold(channel[0], mask_out, 20, 255, THRESH_TOZERO_INV); //b1((b1 >= T))= 0; 
	cv::threshold(mask_out, mask_out, 1, 255, THRESH_BINARY); //b1((b1 > 0))= 255;


	erode(mask_out, mask_out, element[2]);
	dilate(mask_out, mask_out, element[0]);
	erode(mask_out, mask_out, element[2]);
	dilate(mask_out, mask_out, element[0]);
	if (movingRingROI.x > 0)
	{
		mask_out = mask_out(movingRingROI);
		findContours(mask_out, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		if (contours.size() == 0)
		{
			if (StableFrameCount == parameters.STATIONARY_FRAME_COUNT)
			{
				StableFrameCount = 0;
				retVal = true;
				update__RingStability2 = true;
			}
			else
			{
				if (update__RingStability)
				{
					prvFrameNo_RingStability = imAcq->currentFrame;
					update__RingStability = false;
					StableFrameCount++;
				}
				if (imAcq->currentFrame - prvFrameNo_RingStability > parameters.CONSECUTIVE_FRAME_COUNT)
				{
					StableFrameCount = 0;
					update__RingStability = true;
				}
				else
				{
					prvFrameNo_RingStability = imAcq->currentFrame;
					StableFrameCount++;
				}
			}
		}
	}
	//cout << "StableFrameCount ->" << StableFrameCount << endl;
	//cout << "prvFrameNo_RingStability ->" << prvFrameNo_RingStability << endl;
	return retVal;
}

void Main::HittingDetection(const Mat &prv_frame, const Mat &curr_frame, vector<unsigned int> &hittingData)
{
	hittingData.clear();
	// constants
	int thresh = 50;
	Mat kernel = (Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
	int smallImage_width = int(pegGroupROI.width / (float)parameters.NO_OF_IMAGES_HITTING_DETECTION);
	int smallImage_height = int(pegGroupROI.height / (float)parameters.NO_OF_IMAGES_HITTING_DETECTION);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	// vars declaration
	cv::Size smallSize(smallImage_width, smallImage_height);
	Mat diff, canny_output, dst;


	// absolute diffrence between successive frames
	cv::absdiff(prv_frame, curr_frame, diff);
	diff = diff(pegGroupROI);
	// edge detection and normalization
	cv::Canny(diff, canny_output, thresh, thresh * 4, 3);
	cv::normalize(canny_output, canny_output, 0, 1, cv::NORM_MINMAX);

	// dilation and normalization
	cv::dilate(canny_output, dst, kernel);
	cv::dilate(dst, dst, kernel);
	cv::normalize(dst, dst, 0, 255, cv::NORM_MINMAX);
	//imshow("dst", dst);

	// divide image int 10x10 blocks and calculate contours on each subwindow
	for (int y = 0; y < dst.rows - smallSize.height + 1; y += smallSize.height)
	{
		for (int x = 0; x < dst.cols - smallSize.width + 1; x += smallSize.width)
		{
			cv::Rect rect = cv::Rect(x, y, smallSize.width, smallSize.height);
			Mat temp = dst(rect);
			findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			if (contours.size())
			{
				hittingData.push_back(contours.size());
			}
		}
	}
}

Rect Main::getmovingRingROI(const Mat &curr_frame)
{
	Mat blank = cv::Mat::zeros(cv::Size(curr_frame.cols, curr_frame.rows), CV_8UC3);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Rect r = Rect(0, 0, 0, 0);
	Mat fgMask, fgMaskMovingRing;
	pMOG->operator()(curr_frame, fgMask, 0);
	if (updateBackgroundModel)
	{
		//cout << "updating Background Model when all rings stationary \n";
		//waitKey(0);
		pMOG->operator()(curr_frame, fgMask, 0.5);
		updateBackgroundModel = false;
	}
	if (updateBackgroundModel2)
	{
		//cout << "prevmvRingROI" << prevmvRingROI << endl;
		//cout << "mvRingROI" << mvRingROI << endl;
		//cout << "updating Background Model when distance between previous and current ring is more than 150 \n";
		//waitKey(0);
		Mat curr_frame2 = curr_frame.clone();
		for (int y = prevmvRingROI.y; y < prevmvRingROI.y + prevmvRingROI.height; y++)
		{
			for (int x = prevmvRingROI.x; x < prevmvRingROI.x + prevmvRingROI.width; x++)
			{
				curr_frame2.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
			}
		}
		imshow("curr_frame2", curr_frame2);
		waitKey(0);
		pMOG->operator()(curr_frame2, fgMask, 0.5);
		updateBackgroundModel2 = false;
		prevmvRingROI = Rect(0, 0, 0, 0);
	}

	Mat movingToolRing, movingToolRing_hsv;
	curr_frame.copyTo(movingToolRing, fgMask);
	//imshow("fgMask", fgMask);

	if (!fgMask.empty())
	{
		cvtColor(movingToolRing, movingToolRing_hsv, CV_BGR2HSV);

		Mat channel[3];
		split(movingToolRing_hsv, channel);

		//Mat fgMaskRing;
		cv::threshold(channel[0], fgMaskMovingRing, hueThreshVal_ring, 255, THRESH_TOZERO_INV); //b1((b1 >= T))= 0; 
		cv::threshold(fgMaskMovingRing, fgMaskMovingRing, 1, 255, THRESH_BINARY); //b1((b1 > 0))= 255;

		erode(fgMaskMovingRing, fgMaskMovingRing, element[0]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);
		erode(fgMaskMovingRing, fgMaskMovingRing, element[0]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);
		erode(fgMaskMovingRing, fgMaskMovingRing, element[0]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);
		dilate(fgMaskMovingRing, fgMaskMovingRing, element[1]);


		findContours(fgMaskMovingRing, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		vector<bool> markedContours(contours.size(), false);
		vector<Point> maxContour;
		if (contours.size() > 0)
		{
			int Maxsize = contours[0].size();
			int index = 0;
			for (int i = 1; i < contours.size(); ++i)
			{
				int size = contours[i].size();
				if (Maxsize < size)
				{
					Maxsize = size;
					index = i;
				}
			}
			if (contours[index].size() > 20)
			{
				r = boundingRect(contours[index]);
			}
		}
	}
	return r;
}


bool Main::activityDetection(const Mat &prev_frame, const Mat &curr_frame, const Rect &trackingOut)
{
	int count1 = 0, count2 = 0;
	Mat curr_frame_hsv, fgMaskRing;
	// Do the ring segmentation of the current frame;
	cvtColor(curr_frame, curr_frame_hsv, CV_BGR2HSV);
	Mat grey(curr_frame.rows, curr_frame.cols, CV_8UC1);
	cvtColor(curr_frame, grey, CV_BGR2GRAY);

	mvRingROI = getmovingRingROI(curr_frame);


	/* For two moving rings creating problem*/
	Point p1 = Point(prevmvRingROI.x + (prevmvRingROI.width / 2.0), prevmvRingROI.y + prevmvRingROI.height / 2.0);
	Point p2 = Point(mvRingROI.x + (mvRingROI.width / 2.0), mvRingROI.y + mvRingROI.height / 2.0);
	double dist1 = sqrt(((p1.x - p2.x)*(p1.x - p2.x)) + ((p1.y - p2.y)*(p1.y - p2.y)));


	if (dist1 > 150 & (status == "moving" || status == "picking"))
	{
		if (firstTimeRingDetection && prevmvRingROI.width > 0 && mvRingROI.width > 0)
		{
			// Record this event to penalize the trainee ...
			updateBackgroundModel2 = true;
			RingHitting = true;
			//ForegroundDetector *fg = tld->detectorCascade->foregroundDetector;
			//fg->bgImg = grey.clone();
			mvRingROI = getmovingRingROI(curr_frame);
		}
	}

	if (firstTimeRingDetection)
		prevmvRingROI = mvRingROI;



	Mat channel[3];
	split(curr_frame_hsv, channel);
	//Mat fgMaskRing;
	cv::threshold(channel[0], fgMaskRing, hueThreshVal_ring, 255, THRESH_TOZERO_INV); //b1((b1 >= T))= 0; 
	cv::threshold(fgMaskRing, fgMaskRing, 1, 255, THRESH_BINARY); //b1((b1 > 0))= 255;
	erode(fgMaskRing, fgMaskRing, element[2]);
	dilate(fgMaskRing, fgMaskRing, element[0]);
	erode(fgMaskRing, fgMaskRing, element[2]);
	dilate(fgMaskRing, fgMaskRing, element[0]);
	Mat maskRing = fgMaskRing.clone();


	vector<vector<Point> > pegsI;
	vector<Vec4i> hierarchy_ring;
	findContours(fgMaskRing, pegsI, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	for (int i = 0; i < pegsI.size(); i++)
	{
		if (pegsI[i].size() < 35)
		{
			Rect r = boundingRect(pegsI[i]);
			for (int y = r.y; y < r.y + r.height; y++)
			{
				for (int x = r.x; x < r.x + r.width; x++)
				{
					maskRing.at<uchar>(Point(x, y)) = 0;
				}
			}
		}
	}

	unordered_map<int, double> graySum;
	for (int i = 0; i < _pegBox.pegs.size(); ++i)
	{
		int code = _pegBox.pegs[i].code;
		Rect roi = _pegBox.pegs[i].roi;
		Mat temp = maskRing(roi);
		double sumPixels = cv::sum(cv::sum(temp)).val[0];
		graySum[code] = sumPixels;
	}



	//cout << endl << endl;
	if (status == "stationary")
	{
		for (auto it = graySum.begin(); it != graySum.end(); ++it)
		{
			if (it->second == 0)
			{
				count1++;
			}
		}
		if (count1 <= 6)
		{
			// Determine whether stationary or picking status
			if (mvRingROI.width > 0)
			{
				if (firstTimeRingDetection)
				{
					Point center_ring = Point(mvRingROI.x + (mvRingROI.width / 2.0), mvRingROI.y + mvRingROI.height / 2.0);
					Point center_tooltip = Point(trackingOut.x + trackingOut.width, trackingOut.y + trackingOut.height + 40);
					if (center_ring.x > 0 && center_ring.y > 0 && trackingOut.x > 0 && trackingOut.y > 0)
					{
						dist = sqrt(((center_ring.x - center_tooltip.x)*(center_ring.x - center_tooltip.x)) + ((center_ring.y - center_tooltip.y)*(center_ring.y - center_tooltip.y)));
						if (dist < 150)
						{
							pickingCount++;
							updateBackgroundModel = true;
							cout << "pickingCount  " << pickingCount << endl;
							if (pickingCount >= 2)
							{								
								// Find intersection of moving Ring ROI with the first half and determine which Ring trainee is trying to pick up; then change the code of the ring
								for (int i = 0; i < _pegBox.pegs.size(); ++i)
								{
									int code = _pegBox.pegs[i].code;
									Rect roi = _pegBox.pegs[i].roi;
									Rect C = roi & mvRingROI;
									if (C.width > 0)
									{
										for (int p = 0; p < _ringBox.rings.size(); ++p)
										{
											if (code == _ringBox.rings[p].code_pos)
											{
												status = "picking";
												_ringBox.rings[p].status = status;
												pickingCount = 0;
												break;
											}
										}
										break;
									}
								}
							}
						}
					}
				}
				else
				{
					firstTimeRingDetection = true;
					status = "stationary";
				}
			}
			else
			{
				status = "stationary";
			}
		}
	}
	else if (status == "picking")
	{
		int code = -1, index = -1;
		//_ringBox.print();
		for (int i = 0; i < _ringBox.rings.size(); ++i)
		{
			if (_ringBox.rings[i].status == "picking")
			{
				code = _ringBox.rings[i].code_pos;
				break;
			}
		}

		for (int i = 0; i < _pegBox.pegs.size(); ++i)
		{
			if (code == _pegBox.pegs[i].code)
			{
				index = i;
				break;
			}
		}
		if (index >= 0)
		{
			Rect r = _pegBox.pegs[index].roi;
			//Mat pegRing = 
			Mat temp = maskRing(r);
			double sumPixels = cv::sum(cv::sum(temp)).val[0];
			if (sumPixels == 0)
			{
				status = "moving";
				updateBackgroundModel = true;
				for (int i = 0; i < _ringBox.rings.size(); ++i)
				{
					if (_ringBox.rings[i].status == "picking")
					{
						_ringBox.rings[i].status = status;
						break;
					}
				}
			}
			else
			{
				Point center_ring = Point(mvRingROI.x + (mvRingROI.width / 2.0), mvRingROI.y + mvRingROI.height / 2.0);
				Point center_tooltip = Point(trackingOut.x + trackingOut.width, trackingOut.y + trackingOut.height + 40);
				if (center_ring.x > 0 && center_ring.y > 0 && trackingOut.x > 0 && trackingOut.y > 0)
				{
					dist = sqrt(((center_ring.x - center_tooltip.x)*(center_ring.x - center_tooltip.x)) + ((center_ring.y - center_tooltip.y)*(center_ring.y - center_tooltip.y)));
					if (dist < 200)
					{
						status = "picking";
					}
					else
					{
						for (int i = 0; i < _ringBox.rings.size(); ++i)
						{
							if (_ringBox.rings[i].status == "picking")
							{
								_ringBox.rings[i].status = "stationary";
								break;
							}
						}
						status = "stationary";
						updateBackgroundModel = true;
					}
				}
				
			}
		}

	}

	else if (status == "moving")
	{
		for (auto it = graySum.begin(); it != graySum.end(); ++it)
		{
			if (it->second > 0.5e6)
			{
				count2++;
			}
		}
		//cout << "count2->" << count2 << endl;
		if (count2 == 5)
		{
			status == "moving";
		}
		else if (count2 >= 6)
		{
			bool v = AllRingStable(prev_frame, curr_frame, mvRingROI);
			Point center_ring = Point(mvRingROI.x + (mvRingROI.width / 2.0), mvRingROI.y + mvRingROI.height / 2.0);
			Point center_tooltip = Point(trackingOut.x + trackingOut.width, trackingOut.y + trackingOut.height + 40);
			dist = sqrt(((center_ring.x - center_tooltip.x)*(center_ring.x - center_tooltip.x)) + ((center_ring.y - center_tooltip.y)*(center_ring.y - center_tooltip.y)));
			//cout << "Ring Stablility -> " << (v ? "true" : "false") << endl;
			if (update__RingStability2 && dist > 150 && center_ring.x > 0 && center_ring.y > 0 && trackingOut.x > 0 && trackingOut.y > 0)
			{
				int index_peg = 0;
				for (int i = 0; i < _pegBox.pegs.size(); ++i)
				{
					Rect r = _pegBox.pegs[i].roi;
					int code = _pegBox.pegs[i].code;
					Rect C = r & mvRingROI;
					if (C.width > 0)
					{
						status = "stationary";
						update__RingStability2 = false;
						updateBackgroundModel = true;
						index_peg = i;
						//uodate background model
						//ForegroundDetector *fg = tld->detectorCascade->foregroundDetector;
						//fg->bgImg = grey.clone();
						break;
					}
				}
				for (int i = 0; i < _ringBox.rings.size(); ++i)
				{
					if (_ringBox.rings[i].status == "moving")
					{
						_ringBox.rings[i].status = status;
						_ringBox.rings[i].code_pos = _pegBox.pegs[index_peg].code;
						break;
					}
				}
			}
			else
			{
				status = "moving";
			}
		}
	}
	cout << "status -> " << status << endl;
	return true;
}



void Main::ringSegmentation(IplImage* img, Mat &fgmask)
{
	IplImage* im = cvCloneImage(img);
	Mat curr_frame = Mat(im);

	Mat frame_hsv;
	// Do the ring segmentation
	cvtColor(curr_frame, frame_hsv, CV_BGR2HSV);

	Mat channel[3];
	split(frame_hsv, channel);

	cv::threshold(channel[0], fgmask, hueThreshVal_ring, 255, THRESH_TOZERO_INV); //b1((b1 >= T))= 0; 
	cv::threshold(fgmask, fgmask, 1, 255, THRESH_BINARY); //b1((b1 > 0))= 255;

	erode(fgmask, fgmask, element[0]);
	dilate(fgmask, fgmask, element[0]);
	erode(fgmask, fgmask, element[0]);
	dilate(fgmask, fgmask, element[0]);


	cvReleaseImage(&im);
}

void Main::ringSegmentation(const Mat &img, Mat &fgmask)
{

	Mat frame_hsv;
	// Do the ring segmentation
	cvtColor(img, frame_hsv, CV_BGR2HSV);

	Mat channel[3];
	split(frame_hsv, channel);

	cv::threshold(channel[0], fgmask, hueThreshVal_ring, 255, THRESH_TOZERO_INV); //b1((b1 >= T))= 0; 
	cv::threshold(fgmask, fgmask, 1, 255, THRESH_BINARY); //b1((b1 > 0))= 255;

	erode(fgmask, fgmask, element[0]);
	dilate(fgmask, fgmask, element[0]);
	erode(fgmask, fgmask, element[0]);
	dilate(fgmask, fgmask, element[0]);


}

void Main::initializeBoundingBox(IplImage* img)
{
	IplImage* im1 = cvCloneImage(img);
	string message = "Press Enter to confirm the bounding box or any other key to redraw the bounding box";
	cvRectangle(im1, pegGroupROI.tl(), pegGroupROI.br(), CV_RGB(255, 0, 0), 3);
	CvFont font1;
	cvInitFont(&font1, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
	cvPutText(im1, message.c_str(), cvPoint(0, 60), &font1, cvScalar(255, 255, 0));
	gui->showImage(im1);
	char key1 = cvWaitKey(0);
	if ((key1 != '\r' && key1 != '\n' && key1 != '\r\n') || !(pegGroupROI.x != -1) || !(pegGroupROI.y != -1))
	{
		CvRect box;
		IplImage* im2 = cvCloneImage(img);
		string message = "Draw a bounding box around the pegs and press enter";
		if (getBBFromUser(im2, box, gui, message) == PROGRAM_EXIT)
		{
			return;
		}
		pegGroupROI = Rect(box);
		cvReleaseImage(&im2);
	}
	cvReleaseImage(&im1);
}
void Main::initializePegBox(IplImage* img)
{
	IplImage* im1 = cvCloneImage(img);
	Mat fgMaskPeg;
	int thresh_val_peg = getPegthresholdFromUser(im1, gui, "Adjust the trackbar above to get all 12 pegs and press enter", pegThresh_valSaturation, pegGroupROI, fgMaskPeg);
	_pegBox.init(fgMaskPeg);
}
void Main::initializeRingBox(IplImage* img)
{
	Mat fgMaskRing;
	int thresh_val_Ring = getRingthresholdFromUser(img, gui, "Adjust the trackbar above to get all 6 rings and press enter", hueThreshVal_ring, pegGroupROI, fgMaskRing);
	hueThreshVal_ring = thresh_val_Ring;

	_ringBox.init(fgMaskRing, _pegBox);
}
void Main::testPrintScorer(const vector<Activity> &scorer)
{
	// hardcoded file; please change the path
	string filename = string(imAcq->imgPath) + ".txt";
	FILE *resultsFile = fopen(filename.c_str(), "w");
	fprintf(resultsFile, "Output-File for the video -> %s\n", imAcq->imgPath);
	for (int i = 0; i < scorer.size(); ++i)
	{
		Activity act = scorer[i];
		fprintf(resultsFile, "%d. Activity -> %s No of frames(%d) \n", i, act.type.c_str(), act.no_of_frames);
		if (act.type == "No-Activity")
		{
			fprintf(resultsFile, "StartFrame - %d, EndFrame - %d \n", act.s.startFrame, act.s.endFrame);
			if (act.s.hitting.size())
			{
				fprintf(resultsFile, "Hitting Result \n");
				for (int i = 0; i < act.s.hitting.size(); ++i)
				{
					fprintf(resultsFile, "Frame %d, Hitting Value -> %d\n", act.s.hitting[i].first, act.s.hitting[i].second);
				}
				fprintf(resultsFile, "\n \n");
			}
			if (act.s.trackingData.size())
			{
				fprintf(resultsFile, "Tracking Result \n");
				for (int i = 0; i < act.s.trackingData.size(); ++i)
				{
					fprintf(resultsFile, "Frame %d, Tracking Value -> %lf %lf\n", act.s.trackingData[i].first, act.s.trackingData[i].second.first, act.s.trackingData[i].second.second);
				}
				fprintf(resultsFile, "\n \n");
			}

			if (act.s.framesForTrackingFailure.size())
			{
				fprintf(resultsFile, "These frames has tracking failure \n");
				for (int i = 0; i < act.s.framesForTrackingFailure.size(); ++i)
				{
					fprintf(resultsFile, "%d,%t", act.s.framesForTrackingFailure[i]);
				}
				fprintf(resultsFile, "\n \n");
			}

			if (act.s.framesForRingHitting.size())
			{
				fprintf(resultsFile, "These frames has Ring Hitting or wrong placement of the rings\n");
				for (int i = 0; i < act.s.framesForRingHitting.size(); ++i)
				{
					fprintf(resultsFile, "%d,%t", act.s.framesForRingHitting[i]);
				}
				fprintf(resultsFile, "\n \n");
			}
		}
		
		else if (act.type == "Picking-Activity")
		{
			fprintf(resultsFile, "StartFrame - %d, EndFrame - %d \n", act.p.startFrame, act.p.endFrame);
			fprintf(resultsFile, "Picking from the peg No -> %d \n", act.p.from_peg);
			if (act.p.hitting.size())
			{
				fprintf(resultsFile, "Hitting Result \n");
				for (int i = 0; i < act.p.hitting.size(); ++i)
				{
					fprintf(resultsFile, "Frame %d, Hitting Value -> %d\n", act.p.hitting[i].first, act.p.hitting[i].second);
				}
				fprintf(resultsFile, "\n\n");
			}
			if (act.p.trackingData.size())
			{
				fprintf(resultsFile, "Tracking Result \n");
				for (int i = 0; i < act.p.trackingData.size(); ++i)
				{
					fprintf(resultsFile, "Frame %d, Tracking Value -> %lf  %lf\n", act.p.trackingData[i].first, act.p.trackingData[i].second.first, act.p.trackingData[i].second.second);
				}
				fprintf(resultsFile, "\n\n");
			}

			if (act.p.framesForTrackingFailure.size())
			{
				fprintf(resultsFile, "These frames has tracking failure \n");
				for (int i = 0; i < act.p.framesForTrackingFailure.size(); ++i)
				{
					fprintf(resultsFile, "%d,%t", act.p.framesForTrackingFailure[i]);
				}
				fprintf(resultsFile, "\n\n");
			}

			if (act.p.framesForRingHitting.size())
			{
				fprintf(resultsFile, "These frames has Ring Hitting or wrong placement of the rings\n");
				for (int i = 0; i < act.p.framesForRingHitting.size(); ++i)
				{
					fprintf(resultsFile, "%d,%t", act.p.framesForRingHitting[i]);
				}
				fprintf(resultsFile, "\n \n");
			}
		
		}
		else if (act.type == "Moving-Activity")
		{
			fprintf(resultsFile, "StartFrame - %d, EndFrame - %d \n", act.m.startFrame, act.m.endFrame);
			fprintf(resultsFile, "Moving from the peg->%d to the peg-> %d\n", act.m.from_peg, act.m.to_peg);
			if (act.m.hitting.size())
			{
				fprintf(resultsFile, "Hitting Result \n");
				for (int i = 0; i < act.m.hitting.size(); ++i)
				{
					fprintf(resultsFile, "Frame %d, Hitting Value -> %d\n", act.m.hitting[i].first, act.m.hitting[i].second);
				}
				fprintf(resultsFile, "\n\n");
			}
			if (act.m.trackingData.size())
			{
				fprintf(resultsFile, "Tracking Result \n");
				for (int i = 0; i < act.m.trackingData.size(); ++i)
				{
					fprintf(resultsFile, "Frame %d, Tracking Value -> %lf %lf\n", act.m.trackingData[i].first, act.m.trackingData[i].second.first, act.m.trackingData[i].second.second);
				}
				fprintf(resultsFile, "\n\n");
			}

			if (act.m.framesForTrackingFailure.size())
			{
				fprintf(resultsFile, "These frames has tracking failure \n");
				for (int i = 0; i < act.m.framesForTrackingFailure.size(); ++i)
				{
					fprintf(resultsFile, "%d,%t", act.m.framesForTrackingFailure[i]);
				}
				fprintf(resultsFile, "\n\n");
			}

			if (act.m.framesForRingHitting.size())
			{
				fprintf(resultsFile, "These frames has Ring Hitting or wrong placement of the rings\n");
				for (int i = 0; i < act.m.framesForRingHitting.size(); ++i)
				{
					fprintf(resultsFile, "%d,%t", act.m.framesForRingHitting[i]);
				}
				fprintf(resultsFile, "\n \n");
			}
		}
	}
	fclose(resultsFile);
}

void Main::displayResult(const Result &)
{
	int ycursor = 30;
	int xcursor = 10;
	// display the result
	Mat drawing = Mat::zeros(Size(imAcq->width, imAcq->height), CV_8UC3);
	string filename = util::filenameFromPath(imAcq->imgPath);
	string text = "Scoring sheet for the " + filename;
	putText(drawing, text, Point2f(100, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255, 255, 255));
	text = "------------------------------------";
	ycursor = 60;
	putText(drawing, text, Point2f(100, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255, 255, 255));

	if (result.wavymotion.NoFramesMoving.size() >= 2)
	{
		text = "1. Hitting";
		ycursor = 75;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		if (result.hitting.hittingData.size())
		{
			ycursor += 5;
			for (int i = 0; i < result.hitting.hittingData.size(); ++i)
			{
				text = "Frame No -> " + SSTR(result.hitting.hittingData[i].first) + ". with intensity-> " + SSTR(result.hitting.hittingData[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.68, Scalar(0, 255, 0));
			}
			text = "Hitting Score -> " + SSTR(result.hitting.hittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.68, Scalar(0, 0, 255));
		}
		else
		{
			text = "No hitting during the whole activity";
			ycursor += 20;
			putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			text = "Hitting score -> " + SSTR(result.hitting.hittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}


		text = "2. Grasping";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		if (result.grasping.NoFramesPicking.size())
		{
			ycursor += 5;
			for (int i = 0; i < result.grasping.NoFramesPicking.size(); ++i)
			{
				text = "Peg No(" + SSTR(result.grasping.NoFramesPicking[i].first) + ") no of frames-> " + SSTR(result.grasping.NoFramesPicking[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Grasping score -> (" + SSTR(result.grasping.grapspingScore) + "+" + SSTR(result.grasping.grapspingTrackingScore) + ")=" + SSTR(result.grasping.grapspingTrackingScore + result.grasping.grapspingScore) + "/10.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}

		text = "3. Wavy motion";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		bool updateCursor = true;
		if (result.wavymotion.NoFramesMoving.size())
		{
			for (int i = 0; i < result.wavymotion.NoFramesMoving.size(); ++i)
			{
				if (ycursor > imAcq->height - 15 && updateCursor)
				{
					xcursor = 700;
					ycursor = 75;
					updateCursor = false;
				}
				text = "Peg(" + SSTR(result.wavymotion.NoFramesMoving[i].first.first) + "," + SSTR(result.wavymotion.NoFramesMoving[i].first.second) + ") Number of frames-> " + SSTR(result.wavymotion.NoFramesMoving[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Wavy motion score -> (" + SSTR(result.wavymotion.wavyMotionScore) + "+" + SSTR(result.wavymotion.wavyMotionTrackingScore) + ")=" + SSTR(result.wavymotion.wavyMotionScore + result.wavymotion.wavyMotionTrackingScore) + "/10.0";

			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}

		text = "4. Ring hitting or wrong way of placement of the ring";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		updateCursor = true;
		if (result.ringHItting.frameRingHitting.size())
		{
			for (int i = 0; i < result.ringHItting.frameRingHitting.size(); ++i)
			{
				if (ycursor > imAcq->height - 15 && updateCursor)
				{
					xcursor = 700;
					ycursor = 75;
					updateCursor = false;
				}
				text = "Activity(\"" + result.ringHItting.frameRingHitting[i].first + "\") Frame No-> " + SSTR(result.ringHItting.frameRingHitting[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Ring hitting or wrong way of placement of the ring -> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}
		else
		{
			if (ycursor > imAcq->height - 15 && updateCursor)
			{
				xcursor = 700;
				ycursor = 75;
				updateCursor = false;
			}
			text = "Ring hitting or wrong way of placement of the ring -> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}
		text = "5. Sudden movement";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		updateCursor = true;
		if (result.suddenmovement.frameTrackingFailed.size())
		{
			for (int i = 0; i < result.suddenmovement.frameTrackingFailed.size(); ++i)
			{
				if (ycursor > imAcq->height - 15 && updateCursor)
				{
					xcursor = 700;
					ycursor = 75;
					updateCursor = false;
				}
				text = "Activity(\"" + result.suddenmovement.frameTrackingFailed[i].first + "\") Frame No-> " + SSTR(result.suddenmovement.frameTrackingFailed[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Sudden movement score-> " + SSTR(result.suddenmovement.suddenMovementScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}
		else
		{
			if (ycursor > imAcq->height - 15 && updateCursor)
			{
				xcursor = 700;
				ycursor = 75;
				updateCursor = false;
			}
			text = "Sudden movement score-> " + SSTR(result.suddenmovement.suddenMovementScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}
		text = "6. Wrong Moves";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		updateCursor = true;
		if (result.wrongmoves.wrong_moves.size())
		{
			for (int i = 0; i < result.wrongmoves.wrong_moves.size(); ++i)
			{
				if (ycursor > imAcq->height - 15 && updateCursor)
				{
					xcursor = 700;
					ycursor = 75;
					updateCursor = false;
				}
				text = "Wrong moves detected from peg -> " + SSTR(result.wrongmoves.wrong_moves[i].first) + " to peg -> " + SSTR(result.wrongmoves.wrong_moves[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Wrong Moves score-> " + SSTR(result.wrongmoves.wrongMovesScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}
		else
		{
			if (ycursor > imAcq->height - 15 && updateCursor)
			{
				xcursor = 700;
				ycursor = 75;
				updateCursor = false;
			}
			text = "Wrong Moves score-> " + SSTR(result.wrongmoves.wrongMovesScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}
		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}

		text = "7. Wasted movements";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		text = "Total no of frames in the No-Activity -> " + SSTR(result.noactivity.no_frames);
		ycursor += 15;
		putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
		text = "Wasted movements score -> (" + SSTR(result.noactivity.trackingScore) + "+" + SSTR(result.noactivity.trackingScore2) + ")=" + SSTR(result.noactivity.trackingScore + result.noactivity.trackingScore2) + "/10.0";

		ycursor += 15;
		putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}

		text = "Total Score ->" + SSTR(result.totalScore) + "/50.0";
		ycursor += 40;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_SCRIPT_COMPLEX, 1.1, Scalar(255, 255, 255));
	}
	else
	{
		text = "All the activities are not analysed. There should be at least 10 ring movements to generate a valid score.";
		ycursor = 75;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(0, 255, 0));
		text = "Provide the proper video for analysis";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(0, 255, 0));
	}
	
	imshow("Result for the " + filename, drawing);
	char ch = waitKey(0);


}
void Main::writeResult(const Result &)
{
	FILE *resultsFile = NULL;
	if (printResults != NULL)
	{
		resultsFile = fopen(printResults, "w");
		if (!resultsFile)
		{
			fprintf(stderr, "Error: Unable to create results-file \"%s\"\n", printResults);
			exit(-1);
		}
	}

	string filename = util::filenameFromPath(imAcq->imgPath);
	string text = "Scoring sheet for the " + filename + "\n";
	fprintf(resultsFile, text.c_str());

	text = "-----------------------------------------------\n";
	fprintf(resultsFile, text.c_str());

	if (result.wavymotion.NoFramesMoving.size() >= 10)
	{
		text = "\n1. Hitting\n";
		fprintf(resultsFile, text.c_str());
		if (result.hitting.hittingData.size())
		{
			for (int i = 0; i < result.hitting.hittingData.size(); ++i)
			{
				text = "Frame No -> " + SSTR(result.hitting.hittingData[i].first) + ". with intensity-> " + SSTR(result.hitting.hittingData[i].second) + "\n";
				fprintf(resultsFile, text.c_str());
			}
			text = "Hitting Score -> " + SSTR(result.hitting.hittingScore) + "/5.0" + "\n";
			fprintf(resultsFile, text.c_str());
		}
		else
		{
			text = "No hitting during the whole activity\n";
			fprintf(resultsFile, text.c_str());
			text = "Hitting score -> " + SSTR(result.hitting.hittingScore) + "/5.0" + "\n";
			fprintf(resultsFile, text.c_str());
		}


		text = "\n2. Grasping\n";
		fprintf(resultsFile, text.c_str());
		if (result.grasping.NoFramesPicking.size())
		{
			for (int i = 0; i < result.grasping.NoFramesPicking.size(); ++i)
			{
				text = "Peg No(" + SSTR(result.grasping.NoFramesPicking[i].first) + ") no of frames-> " + SSTR(result.grasping.NoFramesPicking[i].second) + "\n";
				fprintf(resultsFile, text.c_str());
			}
			text = "Grasping score -> (" + SSTR(result.grasping.grapspingScore) + "+" + SSTR(result.grasping.grapspingTrackingScore) + ")=" + SSTR(result.grasping.grapspingScore + result.grasping.grapspingTrackingScore) + "/10.0";
			fprintf(resultsFile, text.c_str());
		}

		text = "\n3. Wavy motion\n";
		fprintf(resultsFile, text.c_str());
		if (result.wavymotion.NoFramesMoving.size())
		{
			for (int i = 0; i < result.wavymotion.NoFramesMoving.size(); ++i)
			{
				text = "Peg(" + SSTR(result.wavymotion.NoFramesMoving[i].first.first) + "," + SSTR(result.wavymotion.NoFramesMoving[i].first.second) + ") Number of frames-> " + SSTR(result.wavymotion.NoFramesMoving[i].second) + "\n";
				fprintf(resultsFile, text.c_str());
			}
			text = "Wavy motion score -> (" + SSTR(result.wavymotion.wavyMotionScore) + "+" + SSTR(result.wavymotion.wavyMotionTrackingScore) + ")=" + SSTR(result.wavymotion.wavyMotionScore + result.wavymotion.wavyMotionTrackingScore) + "/10.0";

			fprintf(resultsFile, text.c_str());
		}



		text = "\n4. Ring hitting or wrong way of placement of the ring\n";
		fprintf(resultsFile, text.c_str());
		if (result.ringHItting.frameRingHitting.size())
		{
			for (int i = 0; i < result.ringHItting.frameRingHitting.size(); ++i)
			{
				text = "Activity(\"" + result.ringHItting.frameRingHitting[i].first + "\") Frame No-> " + SSTR(result.ringHItting.frameRingHitting[i].second) + "\n";
				fprintf(resultsFile, text.c_str());
			}
			text = "Ring hitting or wrong way of placement of the ring -> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0\n";
			fprintf(resultsFile, text.c_str());
		}
		else
		{
			text = "Ring hitting or wrong way of placement of the ring -> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0\n";
			fprintf(resultsFile, text.c_str());
		}

		text = "\n5. Sudden movement\n";
		fprintf(resultsFile, text.c_str());
		if (result.suddenmovement.frameTrackingFailed.size())
		{
			for (int i = 0; i < result.suddenmovement.frameTrackingFailed.size(); ++i)
			{
				text = "Activity(\"" + result.suddenmovement.frameTrackingFailed[i].first + "\") Frame No-> " + SSTR(result.suddenmovement.frameTrackingFailed[i].second) + "\n";
				fprintf(resultsFile, text.c_str());
			}
			text = "Sudden movement score-> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0\n";
			fprintf(resultsFile, text.c_str());
		}
		else
		{
			text = "Sudden movement score-> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0\n";
			fprintf(resultsFile, text.c_str());
		}


		text = "\n6. Wrong Moves \n";
		fprintf(resultsFile, text.c_str());
		if (result.wrongmoves.wrong_moves.size())
		{
			for (int i = 0; i < result.wrongmoves.wrong_moves.size(); ++i)
			{
				text = "Wrong moves detected from peg -> " + SSTR(result.wrongmoves.wrong_moves[i].first) + " to peg -> " + SSTR(result.wrongmoves.wrong_moves[i].second) + "\n";
				fprintf(resultsFile, text.c_str());
			}
			text = "Wrong Moves score-> " + SSTR(result.wrongmoves.wrongMovesScore) + "/5.0\n";
			fprintf(resultsFile, text.c_str());
		}
		else
		{
			text = "Wrong Moves score-> " + SSTR(result.wrongmoves.wrongMovesScore) + "/5.0\n";
			fprintf(resultsFile, text.c_str());
		}

		text = "\n7. Wasted movements\n";
		fprintf(resultsFile, text.c_str());
		text = "Total no of frames in the No-Activity -> " + SSTR(result.noactivity.no_frames) + "\n";
		fprintf(resultsFile, text.c_str());
		text = "Wasted movements score ->" + SSTR(result.noactivity.trackingScore + result.noactivity.trackingScore2) + "/10.0" + "\n";
		text = "Wasted movements score -> (" + SSTR(result.noactivity.trackingScore) + "+" + SSTR(result.noactivity.trackingScore2) + ")=" + SSTR(result.noactivity.trackingScore + result.noactivity.trackingScore2) + "/10.0";

		fprintf(resultsFile, text.c_str());
		text = "\n\nTotal Score ->" + SSTR(result.totalScore) + "/50.0";
		fprintf(resultsFile, text.c_str());
	}
	else
	{
		text = "All the activities are not analysed. There should be at least 10 ring movements to generate a valid score.. \n Provide the proper video for analysis\n";
		fprintf(resultsFile, text.c_str());
	}

	if (resultsFile)
	{
		fclose(resultsFile);
	}
}

void Main::computeResult(const vector<Activity> &scorer)
{
	// Hitting Detection
	for (int i = 0; i < scorer.size(); ++i)
	{
		Activity act = scorer[i];
		// for the stationary activity
		if (act.type == "No-Activity")
		{
			result.noactivity.no_frames += act.no_of_frames;
			// check for hitting the board
			if (act.s.hitting.size())
			{
				for (int i = 0; i < act.s.hitting.size(); ++i)
				{
					if (act.s.hitting[i].second >= parameters.HITTING_THRESHOLD)
						result.hitting.hittingData.push_back(make_pair(act.s.hitting[i].first, act.s.hitting[i].second));
				}
			}


			//if (act.m.framesForTrackingFailure.size())
			//{
			//   ***************  TO BE DONE *******************
			//}
			//else
			//{
			vector< vector<pair<double, double> > > fullData;
			vector<pair<double, double> > data;
			bool update = false;
			for (int i = 0; i < act.s.trackingData.size(); ++i)
			{
				if (act.s.trackingData[i].second.first != -1 && act.s.trackingData[i].second.second != -1)
				{
					data.push_back(act.s.trackingData[i].second);
					update = true;
				}
				else
				{
					if (update)
					{
						fullData.push_back(data);
						data.clear();
						update = false;
					}
				}
			}
			for (int i = 0; i < fullData.size(); ++i)
			{
				if (fullData[i].size() > 20)
					result.noactivity.trackingData.push_back(fullData[i]);
			}
			fullData.clear();
			data.clear();

			//}
			// check for ring hitting or wrong placement of the ring
			if (act.s.framesForRingHitting.size())
			{
				for (int i = 0; i < act.s.framesForRingHitting.size(); ++i)
				{
					result.ringHItting.frameRingHitting.push_back(make_pair("No-Activity", act.s.framesForRingHitting[i]));
				}
			}
			// check for tracking failure or sudden change in the apperance of the tool
			if (act.s.framesForTrackingFailure.size())
			{
				for (int i = 0; i < act.s.framesForTrackingFailure.size(); ++i)
				{
					result.suddenmovement.frameTrackingFailed.push_back(make_pair("No-Activity", act.s.framesForTrackingFailure[i]));
				}
			}
		}
		// for the picking activity
		else if (act.type == "Picking-Activity")
		{
			result.grasping.NoFramesPicking.push_back(make_pair(act.p.from_peg, act.no_of_frames));

			// check for hitting the board
			if (act.p.hitting.size())
			{
				for (int i = 0; i < act.p.hitting.size(); ++i)
				{
					if (act.p.hitting[i].second >= parameters.HITTING_THRESHOLD)
						result.hitting.hittingData.push_back(make_pair(act.p.hitting[i].first, act.p.hitting[i].second));
				}
			}

			
			//if (act.p.framesForTrackingFailure.size())
			//{
			//   ***************  TO BE DONE *******************
			//}
			//else
			//{
			vector<pair<double, double> > data;
			for (int i = 0; i < act.p.trackingData.size(); ++i)
			{
				data.push_back(act.p.trackingData[i].second);
			}
			result.grasping.trackingData.push_back(data);
			data.clear();
			//}
			
			
			
			// check for ring hitting or wrong placement of the ring
			if (act.p.framesForRingHitting.size())
			{
				for (int i = 0; i < act.p.framesForRingHitting.size(); ++i)
				{
					result.ringHItting.frameRingHitting.push_back(make_pair("Picking-Activity", act.p.framesForRingHitting[i]));
				}
			}
			// check for tracking failure or sudden change in the apperance of the tool
			if (act.p.framesForTrackingFailure.size())
			{
				for (int i = 0; i < act.p.framesForTrackingFailure.size(); ++i)
				{
					result.suddenmovement.frameTrackingFailed.push_back(make_pair("Picking-Activity", act.p.framesForTrackingFailure[i]));
				}
			}
		}


		// for the moving activity
		else if (act.type == "Moving-Activity")
		{
			result.wavymotion.NoFramesMoving.push_back(make_pair(make_pair(act.m.from_peg, act.m.to_peg), act.no_of_frames));

			// wrong move
			bool wrongMove = true;
			for (int i = 0; i < result.wrongmoves.rightMoves.size(); ++i)
			{
				if (act.m.from_peg == result.wrongmoves.rightMoves[i].first &&
					act.m.to_peg == result.wrongmoves.rightMoves[i].second)
				{
					wrongMove = false;
					break;
				}
			}
			if (wrongMove)
			{
				cout << "I am in wrong move \n";
				result.wrongmoves.wrong_moves.push_back(make_pair(act.m.from_peg, act.m.to_peg));
				wrongMove = true;
			}

			// check for hitting the board
			if (act.m.hitting.size())
			{
				for (int i = 0; i < act.m.hitting.size(); ++i)
				{
					if (act.m.hitting[i].second >= parameters.HITTING_THRESHOLD)
						result.hitting.hittingData.push_back(make_pair(act.m.hitting[i].first, act.m.hitting[i].second));
				}
			}

			// check for ring hitting or wrong placement of the ring
			if (act.m.framesForRingHitting.size())
			{
				for (int i = 0; i < act.m.framesForRingHitting.size(); ++i)
				{
					result.ringHItting.frameRingHitting.push_back(make_pair("Moving-Activity", act.m.framesForRingHitting[i]));
				}
			}

			// put the tracking data in the result struct
			//if (act.m.framesForTrackingFailure.size())
			//{
			//   ***************  TO BE DONE *******************
			//}
			//else
			//{
			vector<pair<double, double> > data;
			for (int i = 0; i < act.m.trackingData.size(); ++i)
			{
				data.push_back(act.m.trackingData[i].second);
			}
			result.wavymotion.trackingData.push_back(data);
			data.clear();
			//}

			// check for tracking failure or sudden change in the apperance of the tool
			if (act.m.framesForTrackingFailure.size())
			{
				for (int i = 0; i < act.m.framesForTrackingFailure.size(); ++i)
				{
					result.suddenmovement.frameTrackingFailed.push_back(make_pair("Moving-Activity", act.m.framesForTrackingFailure[i]));
				}
			}
		}
	}

	// process the tracking data
	processTrackingData();
	
	// process and compute the scores
	// 1.0 Scoring for the hitting
	result.hitting.hittingScore = 5.0;
	for (int i = 0; i < result.hitting.hittingData.size(); ++i)
	{
		if (result.hitting.hittingData[i].second > 30 && result.hitting.hittingData[i].second <= 40)
		{
			result.hitting.hittingScore -= 0.1;
		}
		else if (result.hitting.hittingData[i].second > 40 && result.hitting.hittingData[i].second <= 50)
		{
			result.hitting.hittingScore -= 0.2;
		}
		else
		{
			result.hitting.hittingScore -= 0.3;
		}
	}

	// 2.0 Scoring for the grasping
	result.grasping.grapspingScore = 5.0;
	for(int i = 0; i < result.grasping.NoFramesPicking.size(); ++i)
	{
		if (result.grasping.NoFramesPicking[i].second > 0 && result.grasping.NoFramesPicking[i].second <= 100)
		{
			result.grasping.grapspingScore -= 0.0;
		}
		else if (result.grasping.NoFramesPicking[i].second > 100 && result.grasping.NoFramesPicking[i].second <= 150)
		{
			result.grasping.grapspingScore -= 0.1;
		}
		else if (result.grasping.NoFramesPicking[i].second > 150 && result.grasping.NoFramesPicking[i].second <= 200)
		{
			result.grasping.grapspingScore -= 0.2;
		}
		else if (result.grasping.NoFramesPicking[i].second > 200 && result.grasping.NoFramesPicking[i].second <= 250)
		{
			result.grasping.grapspingScore -= 0.3;
		}
		else
		{
			result.grasping.grapspingScore -= 0.4;
		}
	
	}

	// 3.0 Scoring for the wavyMotion
	result.wavymotion.wavyMotionScore = 5.0;
	for (int i = 0; i < result.wavymotion.NoFramesMoving.size(); ++i)
	{
		if (result.wavymotion.NoFramesMoving[i].second > 0 && result.wavymotion.NoFramesMoving[i].second <= 200)
		{
			result.wavymotion.wavyMotionScore -= 0.0;
		}
		else if (result.wavymotion.NoFramesMoving[i].second > 200 && result.wavymotion.NoFramesMoving[i].second <= 250)
		{
			result.wavymotion.wavyMotionScore -= 0.1;
		}
		else if (result.wavymotion.NoFramesMoving[i].second > 250 && result.wavymotion.NoFramesMoving[i].second <= 300)
		{
			result.wavymotion.wavyMotionScore -= 0.2;
		}
		else if (result.wavymotion.NoFramesMoving[i].second > 300 && result.wavymotion.NoFramesMoving[i].second <= 350)
		{
			result.wavymotion.wavyMotionScore -= 0.3;
		}
		else
		{
			result.wavymotion.wavyMotionScore -= 0.4;
		}
	}

	// 4.0 scoring for the suddent movement
	result.suddenmovement.suddenMovementScore = 5.0;
	if (result.suddenmovement.frameTrackingFailed.size() == 0)
	{
		result.suddenmovement.suddenMovementScore -= 0.0;
	}
	else if (result.suddenmovement.frameTrackingFailed.size() > 0 && result.suddenmovement.frameTrackingFailed.size() <= 5)
	{
		result.suddenmovement.suddenMovementScore -= 1.0;
	}
	else if (result.suddenmovement.frameTrackingFailed.size() > 5 && result.suddenmovement.frameTrackingFailed.size() <= 10)
	{
		result.suddenmovement.suddenMovementScore -= 2.0;
	}
	else if (result.suddenmovement.frameTrackingFailed.size() > 10 && result.suddenmovement.frameTrackingFailed.size() <= 15)
	{
		result.suddenmovement.suddenMovementScore -= 3.0;
	}
	else if (result.suddenmovement.frameTrackingFailed.size() > 15 && result.suddenmovement.frameTrackingFailed.size() <= 20)
	{
		result.suddenmovement.suddenMovementScore -= 4.0;
	}
	else
	{
		result.suddenmovement.suddenMovementScore -= 5.0;
	}
	
	// 5.0 scoring for the ring hitting or wrong placement of the ring
	result.ringHItting.ringHittingScore = 5.0;
	if (result.ringHItting.frameRingHitting.size() == 0)
	{
		result.ringHItting.ringHittingScore -= 0.0;
	}
	else if (result.ringHItting.frameRingHitting.size() > 0 && result.ringHItting.frameRingHitting.size() <= 10)
	{
		result.ringHItting.ringHittingScore -= 0.5;
	}
	else if (result.ringHItting.frameRingHitting.size() > 10 && result.ringHItting.frameRingHitting.size() <= 20)
	{
		result.ringHItting.ringHittingScore -= 1.0;
	}
	else if (result.ringHItting.frameRingHitting.size() > 20 && result.ringHItting.frameRingHitting.size() <= 30)
	{
		result.ringHItting.ringHittingScore -= 2.0;
	}
	else if (result.ringHItting.frameRingHitting.size() > 30 && result.ringHItting.frameRingHitting.size() <= 40)
	{
		result.ringHItting.ringHittingScore -= 3.0;
	}
	else if (result.ringHItting.frameRingHitting.size() > 40 && result.ringHItting.frameRingHitting.size() <= 50)
	{
		result.ringHItting.ringHittingScore -= 4.0;
	}
	else
	{
		result.ringHItting.ringHittingScore -= 5.0;
	}

	// 6.0 scoring for the no activity
	result.noactivity.trackingScore = 5.0;
	if (result.noactivity.no_frames > 0 && result.noactivity.no_frames <= 2000)
	{
		result.noactivity.trackingScore -= 0.0;
	}
	else if (result.noactivity.no_frames > 2000 && result.noactivity.no_frames <= 4000)
	{
		result.noactivity.trackingScore -= 2.0;
	}
	else if (result.noactivity.no_frames > 4000 && result.noactivity.no_frames <= 6000)
	{
		result.noactivity.trackingScore -= 3.0;
	}
	else if (result.noactivity.no_frames > 6000 && result.noactivity.no_frames <= 8000)
	{
		result.noactivity.trackingScore -= 4.0;
	}
	else
	{
		result.noactivity.trackingScore -= 5.0;
	}

	// 7.0 scorig for the wrong moves
	result.wrongmoves.wrongMovesScore = 5.0;
	if (result.wrongmoves.wrong_moves.size() == 0)
	{
		result.wrongmoves.wrongMovesScore -= 0.0;
	}
	else if (result.wrongmoves.wrong_moves.size() > 0 && result.wrongmoves.wrong_moves.size() <= 2)
	{
		result.wrongmoves.wrongMovesScore -= 1.0;
	}
	else if (result.wrongmoves.wrong_moves.size() > 2 && result.wrongmoves.wrong_moves.size() <= 4)
	{
		result.wrongmoves.wrongMovesScore -= 2.0;
	}
	else if (result.wrongmoves.wrong_moves.size() > 4 && result.wrongmoves.wrong_moves.size() <= 6)
	{
		result.wrongmoves.wrongMovesScore -= 3.0;
	}
	else if (result.wrongmoves.wrong_moves.size() > 6 && result.wrongmoves.wrong_moves.size() <= 8)
	{
		result.wrongmoves.wrongMovesScore -= 4.0;
	}
	else
	{
		result.wrongmoves.wrongMovesScore -= 5.0;
	}

	// Total score
	result.totalScore = result.grasping.grapspingScore + result.hitting.hittingScore + result.noactivity.trackingScore
		+ result.ringHItting.ringHittingScore + result.suddenmovement.suddenMovementScore + result.wavymotion.wavyMotionScore
		+ result.wrongmoves.wrongMovesScore + result.grasping.grapspingTrackingScore + result.noactivity.trackingScore2 + result.wavymotion.wavyMotionTrackingScore;
}
void Main::processTrackingData()
{
	double varX = 0;
	double varY = 0;
	double arcLength = 0;
	double curvatureMax = 0;
	int curvatureMaxCount = 0;
	vector<pair <double, double > > x;
	vector<pair <double, double > > dx;
	vector<pair <double, double > > ddx;
	vector<double> Curvature;
	double scoreNoActivity = 0;
	double scorePicking = 0;
	double scoreMoving = 0;

	for (int i = 0; i < result.noactivity.trackingData.size(); ++i)
	{
		vector<pair <double, double > > data = result.noactivity.trackingData[i];
		util::normalize2D(data, x);
		util::diff2D(x, dx);
		util::diff2D(dx, ddx);
		util::var2D(dx, &varX, &varY);
		util::curvature(dx, ddx, Curvature);
		util::curvatureMaxnCount(Curvature, &curvatureMax, &curvatureMaxCount);
		util::arclength(dx, &arcLength);		
		if (curvatureMax > 450 && curvatureMaxCount > 2 && arcLength > 6)
		{
			scoreNoActivity += 0.0;
		}
		else if ((curvatureMax > 450 && curvatureMaxCount > 2 && arcLength > 6))
		{
			scoreNoActivity += 2.0;
		}
		else if ((curvatureMax > 450 && curvatureMaxCount < 2) || (arcLength > 6) )
		{
			scoreNoActivity += 4.0;
		}
		else
		{
			scoreNoActivity += 5.0;
		}
	}
	scoreNoActivity /= result.noactivity.trackingData.size();


	for (int i = 0; i < result.grasping.trackingData.size(); ++i)
	{
		vector<pair <double, double > > data = result.grasping.trackingData[i];
		util::normalize2D(data, x);
		util::diff2D(x, dx);
		util::diff2D(dx, ddx);
		util::var2D(dx, &varX, &varY);
		util::curvature(dx, ddx, Curvature);
		util::curvatureMaxnCount(Curvature, &curvatureMax, &curvatureMaxCount);
		util::arclength(dx, &arcLength);
		if (curvatureMax > 320 && curvatureMaxCount > 2 && arcLength > 5)
		{
			scorePicking += 0.0;
		}
		else if ((curvatureMax > 320 && curvatureMaxCount > 2 && arcLength > 5))
		{
			scorePicking += 2.0;
		}
		else if ((curvatureMax > 320 && curvatureMaxCount < 2) || (arcLength > 5))
		{
			scorePicking += 4.0;
		}
		else
		{
			scorePicking += 5.0;
		}
	}
	scorePicking /= result.grasping.trackingData.size();


	for (int i = 0; i < result.wavymotion.trackingData.size(); ++i)
	{
		vector<pair <double, double > > data = result.wavymotion.trackingData[i];
		util::normalize2D(data, x);
		util::diff2D(x, dx);
		util::diff2D(dx, ddx);
		util::var2D(dx, &varX, &varY);
		util::curvature(dx, ddx, Curvature);
		util::curvatureMaxnCount(Curvature, &curvatureMax, &curvatureMaxCount);
		util::arclength(dx, &arcLength);
		if (curvatureMax > 450 && curvatureMaxCount > 2 && arcLength > 6 && varX > 0.0001 && varY > 0.0001)
		{
			scoreMoving += 0.0;
		}
		else if ((curvatureMax > 450 && curvatureMaxCount > 2 && arcLength > 6) )
		{
			scoreMoving += 2.0;
		}
		else if ((curvatureMax > 450 && curvatureMaxCount < 2) || (arcLength > 6) )
		{
			scoreMoving += 4.0;
		}
		else
		{
			scoreMoving += 5.0;
		}
	}
	scoreMoving /= result.wavymotion.trackingData.size();

	// update the score in result struct
	result.noactivity.trackingScore2 = scoreNoActivity;
	result.grasping.grapspingTrackingScore = scorePicking;
	result.wavymotion.wavyMotionTrackingScore = scoreMoving;
}

void Main::run()
{
	// Read the current frame and convert to the grayscale
	IplImage *img = imAcqGetImg(imAcq);
	IplImage *img1 = cvCloneImage(img);
	Mat grey(img->height, img->width, CV_8UC1);
	cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);

	// update the background image
	///ForegroundDetector *fg = tld->detectorCascade->foregroundDetector;
	//fg->bgImg = grey.clone();

	tld->detectorCascade->imgWidth = grey.cols;
	tld->detectorCascade->imgHeight = grey.rows;
	tld->detectorCascade->imgWidthStep = grey.step;

	// Get the bounding box around the frame
	initializeBoundingBox(img);

	// Do the peg segmentation
	initializePegBox(img);

	// do the ring segmentation
	initializeRingBox(img);


	// update the rois of the pegs
	int rois_size = _ringBox.rings.size();
	assert(rois_size == 6);
	vector<pair<Rect, int> > roi_rings(rois_size);
	for (int i = 0; i < _ringBox.rings.size(); ++i)
	{
		int code = _ringBox.rings[i].code_pos;
		Rect r = _ringBox.rings[i].roi;
		roi_rings[i].first = r;
		roi_rings[i].second = code;
	}
	_pegBox.roi_update(roi_rings);




	bool reuseFrameOnce = false;

	if (loadModel && modelPath != NULL)
	{
		tld->readFromFile(modelPath);
		reuseFrameOnce = true;
	}

	// Get the copy of previous image
	prv_frame = Mat(img1);
	//start processing the frames


	gui->showImageByDestroyingWindow(img);
	while (imAcqHasMoreFrames(imAcq))
	{
		double tic = cvGetTickCount();
		if (!reuseFrameOnce)
		{
			//cout << "current frame -> " << imAcq->currentFrame - 1 << endl;
			cvReleaseImage(&img);
			img = imAcqGetImg(imAcq);
			if (img == NULL)
			{
				printf("current image is NULL, assuming end of input.\n");
				break;
			}
			cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);
		}
		if (parameters.TLD_WITH_TRACKER_ONLY)
		{
			tld->processImageWithTracker(cvarrToMat(img));
		}
		else
		{
			tld->processImage(cvarrToMat(img));
		}
		// if tracking fails then prompt the user to reinitialize the tracking
		if (tld->currBB == NULL)
		{
			if (first_tracking_failed_detection && trackingStart)
			{
				if (mvRingROI.width > 0)
				{
					Point center_ring = Point(mvRingROI.x + (mvRingROI.width / 2.0), mvRingROI.y + mvRingROI.height / 2.0);
					if (center_ring.y > mvRingROI.height)
					{
						CvRect box;
						std::string message = "Tracking failed .. Reinitialize tracking by drawing a bounding box and press enter";
						if (getBBFromUser(img, box, gui, message) == PROGRAM_EXIT)
						{
							break;
						}
						JerKDetection = true;
						Rect r = Rect(box);
						tld->learnPatch(grey, &r);
					}
				}
			}
			else
			{
				first_tracking_failed_detection = true;
			}
		}

		/*  ********** Hitting Detection *****************
		Calculate the adsolute difference between two successive frames
		divide image into 10x10 subimage
		calculate in how many subimages contours are dected
		* ****************************************************/
		// Detection for the hitting on the board

		curr_frame = Mat(img);
		vector<unsigned int > frameHitData;
		HittingDetection(prv_frame, curr_frame, frameHitData);
		if (frameHitData.size() > 30)
		{
			prv_frame = curr_frame.clone();
		}

		// Replace the previous with current frame and reset frameHitData

		/***********************************************************

		/*  ********** Activity Detection *****************
		Detect activity in each frame;
		1. segment the rings
		2. check overlap of the ring with pegs
		3. if any overlap is zero assign moving code to that ring
		4. if placed assign code of corresponding peg
		input: current frame and tracking output
		output : vector<int> state; describes the state of the system
		*****************************************************/
		if (tld->currBB != NULL)
		{
			activityDetection(prv_frame, curr_frame, Rect(tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height));
		}
		else
		{
			activityDetection(prv_frame, curr_frame, Rect(-1, -1, -1, -1));
		}
		//******************************************************
		prv_frame = curr_frame.clone();

		// Scoring update;
		// Get which ring has either picking or stationary status
		for (int i = 0; i < _ringBox.rings.size(); ++i)
		{
			if (_ringBox.rings[i].status == "picking")
			{
				index = i;
				break;
			}
			if (_ringBox.rings[i].status == "moving")
			{
				index = i;
				break;
			}
		}
		if (status == "stationary")
		{
			if (moving_activity_start)
			{
				activity.m.endFrame = imAcq->currentFrame - 2;
				activity.m.to_peg = _ringBox.rings[index].code_pos;
				scorer.push_back(activity);
				activity.clear();
				moving_activity_start = false;
				update_startFrameAndType = true;
			}
			if (update_startFrameAndType)
			{
				activity.type = "No-Activity";
				activity.s.startFrame = imAcq->currentFrame - 1;
				update_startFrameAndType = false;
			}
			activity.no_of_frames++;
			if (frameHitData.size() >= parameters.HITTING_THRESHOLD)
			{
				activity.s.hitting.push_back(make_pair(imAcq->currentFrame - 1, frameHitData.size()));
			}
			if (tld->currBB != NULL)
			{
				activity.s.trackingData.push_back(make_pair(imAcq->currentFrame - 1, make_pair(tld->currBB->br().x, tld->currBB->br().y + 40)));
			}
			else
			{
				activity.s.trackingData.push_back(make_pair(imAcq->currentFrame - 1, make_pair(-1, -1)));
			}
			if (RingHitting)
			{
				activity.s.framesForRingHitting.push_back(imAcq->currentFrame - 1);
				RingHitting = false;
			}
			if (JerKDetection)
			{
				activity.s.framesForTrackingFailure.push_back(imAcq->currentFrame - 1);
				JerKDetection = false;
			}
			if (trackinReinit)
			{
				activity.s.framesForTrackingReinit.push_back(imAcq->currentFrame - 2);
				trackinReinit = false;
			}
			No_activity_start = true;
		}

		else if (status == "picking")
		{
			if (No_activity_start)
			{
				activity.s.endFrame = imAcq->currentFrame - 2;
				scorer.push_back(activity);
				activity.clear();
				No_activity_start = false;
				update_startFrameAndType = true;
			}
			if (update_startFrameAndType)
			{
				activity.type = "Picking-Activity";
				activity.p.startFrame = imAcq->currentFrame - 1;
				activity.p.from_peg = _ringBox.rings[index].code_pos; 
				update_startFrameAndType = false;
			}
			activity.no_of_frames++;
			if (frameHitData.size() >= parameters.HITTING_THRESHOLD)
			{
				activity.p.hitting.push_back(make_pair(imAcq->currentFrame - 1, frameHitData.size()));
			}
			if (tld->currBB != NULL)
			{
				activity.p.trackingData.push_back(make_pair(imAcq->currentFrame - 1, make_pair(tld->currBB->br().x, tld->currBB->br().y + 40)));
			}
			else
			{
				activity.s.trackingData.push_back(make_pair(imAcq->currentFrame - 1, make_pair(-1, -1)));
			}
			picking_activity_start = true;
			if (RingHitting)
			{
				activity.p.framesForRingHitting.push_back(imAcq->currentFrame - 1);
				RingHitting = false;
			}
			if (JerKDetection)
			{
				activity.p.framesForTrackingFailure.push_back(imAcq->currentFrame - 1);
				JerKDetection = false;
			}
			if (trackinReinit)
			{
				activity.p.framesForTrackingReinit.push_back(imAcq->currentFrame - 2);
				trackinReinit = false;
			}
		}
		else if (status == "moving")
		{
			if (picking_activity_start)
			{
				activity.p.endFrame = imAcq->currentFrame - 2;
				scorer.push_back(activity);
				activity.clear();
				picking_activity_start = false;
				update_startFrameAndType = true;
			}
			if (update_startFrameAndType)
			{
				activity.type = "Moving-Activity";
				activity.m.startFrame = imAcq->currentFrame - 1;
				activity.m.from_peg = _ringBox.rings[index].code_pos;
				update_startFrameAndType = false;
			}
			activity.no_of_frames++;
			if (frameHitData.size() >= parameters.HITTING_THRESHOLD)
			{
				activity.m.hitting.push_back(make_pair(imAcq->currentFrame - 1, frameHitData.size()));
			}
			if (tld->currBB != NULL)
			{
				activity.m.trackingData.push_back(make_pair(imAcq->currentFrame - 1, make_pair(tld->currBB->br().x, tld->currBB->br().y + 40)));
			}
			else
			{
				activity.m.trackingData.push_back(make_pair(imAcq->currentFrame - 1, make_pair(-1, -1)));
			}
			moving_activity_start = true;
			if (RingHitting)
			{
				activity.m.framesForRingHitting.push_back(imAcq->currentFrame - 1);
				RingHitting = false;
			}
			if (JerKDetection)
			{
				activity.m.framesForTrackingFailure.push_back(imAcq->currentFrame - 1);
				JerKDetection = false;
			}
			if (trackinReinit)
			{
				activity.m.framesForTrackingReinit.push_back(imAcq->currentFrame - 2);
				trackinReinit = false;
			}
		}
		// Writing results
		if (printResults != NULL)
		{
			vector<pair<string, int> > vv;
			for (int i  = 0; i < _ringBox.rings.size(); ++i)
			{
				vv.push_back(make_pair(_ringBox.rings[i].status, _ringBox.rings[i].code_pos));
			}
		}
		double toc = (cvGetTickCount() - tic) / cvGetTickFrequency();

		toc = toc / 1000000;

		float fps = 1 / toc;

		int confident = (tld->currConf >= threshold) ? 1 : 0;

		if (showOutput || saveDir != NULL)
		{
			char string[128];
			char learningString[10] = "";

			if (tld->learning)
			{
				strcpy(learningString, "Learning");
			}
			sprintf(string, "#%d,Posterior %.2f; fps: %.2f, #numwindows:%d, %s", imAcq->currentFrame - 1, tld->currConf, fps, tld->detectorCascade->numWindows, learningString);
			CvScalar yellow = CV_RGB(255, 255, 0);
			CvScalar blue = CV_RGB(0, 0, 255);
			CvScalar black = CV_RGB(0, 0, 0);
			CvScalar white = CV_RGB(255, 255, 255);
			CvScalar red = CV_RGB(255, 0, 0);
			CvScalar green = CV_RGB(0, 255, 0);
			CvScalar cyan = CV_RGB(255, 0, 255);
			CvScalar magenta = CV_RGB(0, 255, 255);
			if (tld->currBB != NULL)
			{
				CvScalar rectangleColor = (confident) ? blue : yellow;
				cvRectangle(img, tld->currBB->tl(), tld->currBB->br(), rectangleColor, 2, 8, 0);
				cvCircle(img, cvPoint(tld->currBB->br().x /*- (tld->currBB->width / 2.0)*/, tld->currBB->br().y + 40), 10, red, 3, 8, 0);
			}
			CvFont font;
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
			//cvRectangle(img, cvPoint(0, 0), cvPoint(img->width, 50), black, CV_FILLED, 8, 0);
			cvPutText(img, string, cvPoint(25, 25), &font, white);

			// Draw peg Box
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 2, 8);
			for (int i = 0; i < _pegBox.pegs.size(); ++i)
			{
				std::string text = SSTR(_pegBox.pegs[i].code);
				//cvRectangle(img, cvPoint(_pegBox.pegs[i].roi.tl().x, _pegBox.pegs[i].roi.tl().y),
				//	cvPoint(_pegBox.pegs[i].roi.br().x, _pegBox.pegs[i].roi.br().y), green, 1, 8, 0);
				cvPutText(img, text.c_str(), cvPoint(_pegBox.pegs[i].center.x, _pegBox.pegs[i].center.y), &font, black);
			}
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
			// plot the moving ring
			if (mvRingROI.width > 0)
			{
				cvRectangle(img, cvPoint(mvRingROI.x, mvRingROI.y),
					cvPoint(mvRingROI.br().x, mvRingROI.br().y), cyan, 1, 8, 0);
				cvPutText(img, "moving ring", cvPoint(mvRingROI.x + mvRingROI.width / 2, mvRingROI.y + mvRingROI.height / 2), &font, cyan);
			}

			int count = 0;
			for (int i = 0; i < _ringBox.rings.size(); ++i)
			{
				std::string text = "Ring " + SSTR(i) + " On the Peg " + SSTR(_ringBox.rings[i].code_pos) + "  (with Status " + _ringBox.rings[i].status + ")";
				cvPutText(img, text.c_str(), cvPoint(10, (count++ * 15) + 50), &font, white);
			}
			count = 0;
			std::string text = "Hitting Detection Value ->" + SSTR(frameHitData.size());
			cvPutText(img, text.c_str(), Point(10, (6 * 15) + 50), &font, red);
			if (showForeground)
			{
				for (size_t i = 0; i < tld->detectorCascade->detectionResult->fgList->size(); i++)
				{
					Rect r = tld->detectorCascade->detectionResult->fgList->at(i);
					cvRectangle(img, r.tl(), r.br(), white, 1);
				}
			}
			if (showOutput)
			{
				gui->showImage(img);
				char key = gui->getKey();

				if (key == 'q') break;

				//if (key == 'b')
				//{

				//	ForegroundDetector *fg = tld->detectorCascade->foregroundDetector;

				//	if (fg->bgImg.empty())
				//	{
				//		fg->bgImg = grey.clone();
				//	}
				//	else
				//	{
				//		fg->bgImg.release();
				//	}
				//}

				//if (key == 'c')
				//{
				//	//clear everything
				//	tld->release();
				//}

				//if (key == 'l')
				//{
				//	tld->learningEnabled = !tld->learningEnabled;
				//	printf("LearningEnabled: %d\n", tld->learningEnabled);
				//}
				//if (key == 'e')
				//{
				//	tld->writeToFile(modelExportFile);
				//}
				//if (key == 'i')
				//{
				//	tld->readFromFile(modelPath);
				//}
				if (key == 'r')
				{
					trackingStart = true;
					CvRect box;
					std::string message = "Draw a bounding box and press enter";
					if (getBBFromUser(img, box, gui, message) == PROGRAM_EXIT)
					{
						break;
					}
					Rect r = Rect(box);
					tld->selectObject(grey, &r);
					trackinReinit = true;
				}

				if (key == 'z')
				{
					trackingStart = true;
					CvRect box;
					std::string message = "Draw a bounding box and press enter";
					if (getBBFromUser(img, box, gui, message) == PROGRAM_EXIT)
					{
						break;
					}
					Rect r = Rect(box);
					tld->learnPatch(grey, &r);
					trackinReinit = true;
				}
			}
			if (saveDir != NULL)
			{
				char fileName[256];
				sprintf(fileName, "%s/%.5d.png", saveDir, imAcq->currentFrame - 1);

				cvSaveImage(fileName, img);
			}
			if (reuseFrameOnce)
			{
				reuseFrameOnce = false;
			}
		}
	}

	// update scrorer for the last event;
	if (status == "moving")
	{
		if (imAcq->currentFrame - 1 > activity.m.startFrame)
		{
			activity.m.endFrame = imAcq->currentFrame - 1;
			scorer.push_back(activity);
			activity.clear();
		}
	}
	if (status == "stationary")
	{
		if (imAcq->currentFrame - 1 > activity.s.startFrame)
		{
			activity.s.endFrame = imAcq->currentFrame - 1;
			scorer.push_back(activity);
			activity.clear();
		}
	}
	cvReleaseImage(&img);
	img = NULL;
	gui->destroy();

	// print the result of scorer
	//testPrintScorer(scorer);
	// compute the result
	computeResult(scorer);

	// display the result
	displayResult(result);

	// write the result
	writeResult(result);



	if (exportModelAfterRun)
	{
		tld->writeToFile(modelExportFile);
	}
}
