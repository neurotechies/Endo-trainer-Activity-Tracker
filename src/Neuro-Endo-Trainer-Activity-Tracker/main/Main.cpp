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


void Main::run()
{
	// Read the current frame and convert to the grayscale
	IplImage *img = imAcqGetImg(imAcq);
	Mat grey(img->height, img->width, CV_8UC1);
	cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);

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
	int count = 0;
	for (auto it = _ringBox.rings.begin(); it != _ringBox.rings.end(); ++it)
	{
		int code = it->first;
		Rect r = it->second.roi;
		roi_rings[count].first = r;
		roi_rings[count++].second = code;
	}
	_pegBox.roi_update(roi_rings);


	 //start processing the frames
	while (imAcqHasMoreFrames(imAcq))
	{
		double tic = cvGetTickCount();
		// get the image
		cvReleaseImage(&img);
		img = imAcqGetImg(imAcq);


		// Do the activity detection
		activityDetection(img);
	}
}

void Main::activityDetection(IplImage* img)
{
	IplImage* im = cvCloneImage(img);
	Mat curr_frame = Mat(im);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat ringsMask;
	ringSegmentation(img, ringsMask);

	imshow("ringsMask", ringsMask);
	waitKey(10);
	findContours(ringsMask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly(contours.size());
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());
	vector<pair<int, int > > codes_set;

	for (int i = contours.size() - 1; i >= 0; i--)
	{
		if (contours[i].size() < 50)
		{
			contours.erase(contours.begin() + i);
		}
	}

	if (contours.size() == 6)
	{
		vector<Rect> vecRect(6);
		for (int i = 0; i < contours.size(); i++)
		{
			Rect ring_roi = boundingRect(contours[i]);
			vecRect[i] = ring_roi;
			for (auto it = _pegBox.pegs.begin(); it != _pegBox.pegs.end(); ++it)
			{
				int code = it->first;
				Rect r = it->second.roi;
				if (util::rectOverlap(r, ring_roi))
				{
					codes_set.push_back(make_pair(code, i));
					break;
				}
			}
		}

		for (int i = 0; i < codes_set.size(); ++i)
		{
			CounterMap::iterator it(count_pegCode_contour.find(codes_set[i].first));
			CounterMap::iterator it_1(count_contour_pegCode.find(codes_set[i].second));
			if (it != count_pegCode_contour.end())
			{
				it->second++;
			}
			else
			{
				count_pegCode_contour[codes_set[i].first] = 1;
			}

			if (it_1 != count_contour_pegCode.end())
			{
				it_1->second++;
			}
			else 
			{
				count_contour_pegCode[codes_set[i].second] = 1;
			}
		}
		int count_1 = 0; int count_2 = 0;
		for (CounterMap::iterator it = count_pegCode_contour.begin(); it != count_pegCode_contour.end(); ++it)
		{
			if (it->second == 1)
			{
				count_1 += it->second;
			}
		}
		for (CounterMap::iterator it = count_contour_pegCode.begin(); it != count_contour_pegCode.end(); ++it)
		{
			if (it->second == 1)
			{
				count_2 += it->second;
			}
		}
		// normal case
		if (count_1 == 6 && count_2 == 6)
		{
			for (int i = 0; i < 6; ++i)
			{
				Rect r1 = vecRect[i];
				for (auto it = _ringBox.rings.begin(); it != _ringBox.rings.end(); ++it)
				{
					int code = it->first;
					Rect r2 = it->second.roi;
					if (util::rectOverlap(r1, r2))
					{
						float rad;
						Point2f cent;
						vector<Point> poly;
						Rect roi;
						approxPolyDP(Mat(contours[i]), poly, 3, true);
						minEnclosingCircle((Mat)poly, cent, rad);
						roi = boundingRect(contours[i]);
						Point2f cc = Point2f((roi.x + (roi.width / 2.0)), (roi.y + (roi.height / 2.0)));

						it->second.velocity = sqrt(((it->second.center.x - cc.x) * (it->second.center.x - cc.x)) +
							((it->second.center.y - cc.y) * (it->second.center.y - cc.y)));

						it->second.center = cc;
						it->second.radius = rad;
						it->second.roi = r1;
						cout << "Ring - " << code << "  velocity -> " << it->second.velocity << endl;
					}
				}
			}
		}
		else
		{
			
		}

		codes_set.clear();
		count_pegCode_contour.clear();
		count_contour_pegCode.clear();
	}
	else
	{
		cout << "else not okay\n";
	}
	
	


	



	char ch = gui->getKey();
	cvReleaseImage(&im);
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
