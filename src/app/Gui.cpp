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
 * gui.cpp
 *
 *  Created on: Oct 18, 2011
 *      Author: clemensk
 */

#include "Gui.h"

#include "Main.h"

#include <string>

using std::string;

namespace tld
{

	Gui::Gui() :
		m_window_name("Neuro-Endo-Trainer-Activity-Tracker")
	{
	}

	Gui::~Gui()
	{
	}

	void Gui::init()
	{
		cvNamedWindow(m_window_name.c_str(), CV_WINDOW_AUTOSIZE);
		cvMoveWindow(m_window_name.c_str(), 100, 100);
	}

	void Gui::showImage(IplImage *image)
	{
		cvShowImage(m_window_name.c_str(), image);
	}

	void Gui::showImageByDestroyingWindow(IplImage *image)
	{
		cvDestroyWindow(m_window_name.c_str());
		cvNamedWindow(m_window_name.c_str(), CV_WINDOW_AUTOSIZE);
		cvMoveWindow(m_window_name.c_str(), 100, 100);
		cvShowImage(m_window_name.c_str(), image);
	}
	char Gui::getKey()
	{
		return cvWaitKey(10);
	}
	void Gui::destroy()
	{
		cvDestroyWindow(m_window_name.c_str());
	}
	std::string Gui::windowName()
	{
		return m_window_name;
	}

	static string window_name;
	static CvFont font;
	static IplImage *img0;
	static IplImage *img1;
	static CvPoint point;
	static CvRect *bb;
	static int drag = 0;
	static int const slider_max = 255;
	static int const slider_max_hue = 180;
	static int slider_val = 0;;

	static void mouseHandler(int event, int x, int y, int flags, void *param)
	{
		/* user press left button */
		if (event == CV_EVENT_LBUTTONDOWN && !drag)
		{
			point = cvPoint(x, y);
			drag = 1;
		}

		/* user drag the mouse */
		if (event == CV_EVENT_MOUSEMOVE && drag)
		{
			img1 = (IplImage *)cvClone(img0);

			cvRectangle(img1, point, cvPoint(x, y), CV_RGB(255, 0, 0), 1, 8, 0);

			cvShowImage(window_name.c_str(), img1);
			cvReleaseImage(&img1);
		}

		/* user release left button */
		if (event == CV_EVENT_LBUTTONUP && drag)
		{
			*bb = cvRect(point.x, point.y, x - point.x, y - point.y);
			drag = 0;
		}
	}


	// TODO: member of Gui
	// --> problem: callback function mouseHandler as member!
	int getBBFromUser(IplImage *img, CvRect &rect, Gui *gui, const string &message)
	{
		window_name = gui->windowName();
		
		cvDestroyWindow(window_name.c_str());
		cvNamedWindow(window_name.c_str(), CV_WINDOW_AUTOSIZE);
		cvMoveWindow(window_name.c_str(), 100, 100);
		
		img0 = (IplImage *)cvClone(img);
		rect = cvRect(-1, -1, -1, -1);
		bb = &rect;
		bool correctBB = false;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);

		cvSetMouseCallback(window_name.c_str(), mouseHandler, NULL);
		cvPutText(img0, message.c_str(), cvPoint(500, 60),
			&font, cvScalar(255, 255, 0));
		cvShowImage(window_name.c_str(), img0);

		while (!correctBB)
		{
			char key = cvWaitKey(0);

			if (tolower(key) == 'q')
			{
				return PROGRAM_EXIT;
			}

			if (((key == '\n') || (key == '\r') || (key == '\r\n')) && (bb->x != -1) && (bb->y != -1))
			{
				correctBB = true;
			}
		}

		if (rect.width < 0)
		{
			rect.x += rect.width;
			rect.width = abs(rect.width);
		}

		if (rect.height < 0)
		{
			rect.y += rect.height;
			rect.height = abs(rect.height);
		}

		cvSetMouseCallback(window_name.c_str(), NULL, NULL);

		cvReleaseImage(&img0);
		cvReleaseImage(&img1);

		return SUCCESS;
	}

	int getRingthresholdFromUser(IplImage *img, Gui *gui, string message, int RingThreshVal, Rect r, cv::Mat &fgMaskRing)
	{
		cv::Mat element[1];
		int count = 0;

		element[0] = getStructuringElement(MORPH_ELLIPSE, Size(8, 8), Point(0, 0));

		window_name = gui->windowName();
		cvDestroyWindow(window_name.c_str());
		cvNamedWindow(window_name.c_str(), CV_WINDOW_AUTOSIZE);
		cvMoveWindow(window_name.c_str(), 100, 100);

		img0 = (IplImage *)cvClone(img);
		char TrackbarName[50];
		sprintf(TrackbarName, "thresh");

		slider_val = RingThreshVal;
		createTrackbar(TrackbarName, window_name, &slider_val, slider_max_hue, 0);



		Mat src, im1, im3;
		src = Mat(img0);

		im1 = Mat::zeros(src.size(), src.type());
		cvtColor(src, im3, CV_BGR2HSV);
		vector<vector<Point> > pegsI;
		while (1)
		{
			pegsI.clear();
			Mat channel[3];
			split(im3, channel);
			cv::threshold(channel[0], fgMaskRing, slider_val, 255, THRESH_TOZERO_INV); //b1((b1 >= T))= 0; 
			cv::threshold(fgMaskRing, fgMaskRing, 1, 255, THRESH_BINARY); //b1((b1 > 0))= 255;


			// ROI
			for (int y = 0; y < fgMaskRing.rows; y++)
			{
				for (int x = 0; x < fgMaskRing.cols; x++)
				{
					if (!(x >= r.tl().x && x <= r.br().x && y >= r.tl().y && y <= r.br().y))
					{
						fgMaskRing.at<uchar>(Point(x, y)) = 0;
					}
				}
			}
			erode(fgMaskRing, fgMaskRing, element[0]);
			dilate(fgMaskRing, fgMaskRing, element[0]);
			erode(fgMaskRing, fgMaskRing, element[0]);
			dilate(fgMaskRing, fgMaskRing, element[0]);

			//p.copyTo(p, fgMaskPeg);


			Mat mask = fgMaskRing.clone();
			vector<Vec4i> hierarchy_ring;

			//imshow("Initial mask", initial_ring_mask);

			findContours(mask, pegsI, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			count = pegsI.size();

			for (int i = 0; i < pegsI.size(); i++)
			{
				if (pegsI[i].size() < 35)
				{
					Rect r = boundingRect(pegsI[i]);
					for (int y = r.y; y < r.y + r.height; y++)
					{
						for (int x = r.x; x < r.x + r.width; x++)
						{
							fgMaskRing.at<uchar>(Point(x, y)) = 0;
						}
					}
					count--;
				}
			}

			for (int y = 0; y < src.rows; y++)
			{
				for (int x = 0; x < src.cols; x++)
				{
					if (fgMaskRing.at<uchar>(Point(x, y)))
					{
						im1.at<Vec3b>(Point(x, y)) = src.at<Vec3b>(Point(x, y));
					}
					else
					{
						im1.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
					}
				}
			}
			cout << "count Rings->" << count << endl;
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
			putText(im1, message.c_str(), cvPoint(0, 60), CV_FONT_HERSHEY_SIMPLEX, .7, Scalar(255, 255, 0), 1);

			imshow(window_name.c_str(), im1);
			char key = cvWaitKey(40);
			if ((key == '\r' || key == '\n' || key == '\r\n'))
			{
				if (count == 6)
				{
					break;
				}
			}
			count = 0;
		}
		cvReleaseImage(&img0);
		return slider_val;
	}

	int getPegthresholdFromUser(IplImage *img, Gui *gui, string message, int pegThreshVal, Rect r, cv::Mat &fgMaskPeg)
	{
		cv::Mat element[1];
		int count = 0;

		element[0] = getStructuringElement(MORPH_ELLIPSE, Size(8, 8), Point(0, 0));
		
		window_name = gui->windowName();
		cvDestroyWindow(window_name.c_str());
		cvNamedWindow(window_name.c_str(), CV_WINDOW_AUTOSIZE);
		cvMoveWindow(window_name.c_str(), 100, 100);

		img0 = (IplImage *)cvClone(img);
		char TrackbarName[50];
		sprintf(TrackbarName, "thresh x %d", slider_max);

		slider_val = pegThreshVal;
		createTrackbar(TrackbarName, window_name, &slider_val, slider_max, 0);

		

		Mat src, im1, im3;
		src = Mat(img0);

		im1 = Mat::zeros(src.size(), src.type());
		cvtColor(src, im3, CV_BGR2HSV);
		vector<vector<Point> > pegsI;
		while (1)
		{
			pegsI.clear();
			Mat channel[3];
			split(im3, channel);


			//Mat fgMaskRing;
			inRange(channel[2], slider_val, 255, fgMaskPeg);
			// ROI
			for (int y = 0; y < fgMaskPeg.rows; y++)
			{
				for (int x = 0; x < fgMaskPeg.cols; x++)
				{
					if (!(x >= r.tl().x && x <= r.br().x && y >= r.tl().y && y <= r.br().y))
					{
						fgMaskPeg.at<uchar>(Point(x, y)) = 0;
					}
				}
			}
			erode(fgMaskPeg, fgMaskPeg, element[0]);
			dilate(fgMaskPeg, fgMaskPeg, element[0]);
			erode(fgMaskPeg, fgMaskPeg, element[0]);
			dilate(fgMaskPeg, fgMaskPeg, element[0]);

			//p.copyTo(p, fgMaskPeg);
			for (int y = 0; y < src.rows; y++)
			{
				for (int x = 0; x < src.cols; x++)
				{
					if (fgMaskPeg.at<uchar>(Point(x, y)))
					{
						im1.at<Vec3b>(Point(x, y)) = src.at<Vec3b>(Point(x, y));
					}
					else
					{
						im1.at<Vec3b>(Point(x, y)) = Vec3b(0,0,0);
					}
				}
			}

			Mat mask = fgMaskPeg.clone();
			vector<Vec4i> hierarchy_ring;

			//imshow("Initial mask", initial_ring_mask);
			findContours(mask, pegsI, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			count = pegsI.size();



			cout << "count Pegs->" << count << endl;
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
			putText(im1, message.c_str(), cvPoint(0, 60), CV_FONT_HERSHEY_SIMPLEX, .7, Scalar(255, 255, 0), 1);

			imshow(window_name.c_str(), im1);
			char key = cvWaitKey(40);
			if ((key == '\r' || key == '\n' || key == '\r\n'))
			{
				if (count == 12)
				{
					break;
				}
			}
			count = 0;
		} 
		cvReleaseImage(&img0);
		return slider_val;
	}
}
