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



#ifndef pegBox_Included
#define pegBox_Included


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ContourFeature.hpp"
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <set>
using namespace cv;
using namespace std;

namespace nets
{

	class pegBox
	{
	public:
		struct Peg
		{
			cv::Rect roi;
			Point2f center;
			int code;
		};

		struct Comparator {
			bool operator() (pair<Point2f, int> pt1, pair<Point2f, int> pt2) { return (pt1.first.x < pt2.first.x); }
		} _comparator;
	
	public:
		vector<Peg> pegs;

	public:
		// initialize pegs;
		bool init(const cv::Mat &inp);

		// print the pegs
		void print();


		// update the roi of given peg
		bool update(int code, const Rect &r);
		
		// update the roi; 
		void roi_update(const vector<pair<Rect, int> > &rings);


		pegBox(){}
		~pegBox(){}
	
	private:
	};

}
#endif