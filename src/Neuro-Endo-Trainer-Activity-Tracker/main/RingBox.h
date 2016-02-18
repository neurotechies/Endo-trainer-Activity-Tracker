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




#ifndef Ring_Box_Included
#define Ring_Box_Included

#include "ContourFeature.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "pegBox.h"
#include "util.h"
#include "params.h"
#include <unordered_map>
#include <cassert>
using namespace std;
using namespace cv;

namespace nets
{
	class RingBox
	{
	public:
		struct Ring
		{
			cv::Rect roi;
			Point2f center;
			float radius;
			float velocity;
			string status;
		};

		unordered_map<int, Ring> rings;

	public:
		bool init(const cv::Mat &inp, const pegBox &pegBox);

		RingBox(){}
		~RingBox(){}

	};
}

#endif