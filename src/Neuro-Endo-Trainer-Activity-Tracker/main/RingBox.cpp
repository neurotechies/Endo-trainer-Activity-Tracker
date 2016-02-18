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



#include "RingBox.h"

namespace nets
{

	
	bool RingBox::init(const cv::Mat &inp)
	{
		vector<vector<Point> > contours_ring;
		Mat initial_ring_mask = inp.clone();
		vector<Vec4i> hierarchy_ring;
		vector<Point2f> centre;
		vector<Rect> initial_roi;
		vector<float> radius;


		findContours(initial_ring_mask, contours_ring, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		unsigned int No_of_ring = contours_ring.size();
		vector<bool> markedRing(No_of_ring, true);

		for (int i = 0; i < No_of_ring; ++i)
		{
			if (contours_ring[i].size() < 35)
			{
				markedRing[i] = false;
			}
		}
		unsigned int count = 0;
		for (int i = 0; i < No_of_ring; ++i)
		{
			if (markedRing[i])
			{
				ContourFeature cFeat(contours_ring[i]);
				centre.push_back(cFeat.getCentre()); //centre.y += 10;

				initial_roi.push_back(boundingRect(contours_ring[i])); // This is a OpenCV function
				radius.push_back(sqrt(pow(boundingRect(contours_ring[i]).width, 2.0) + pow(boundingRect(contours_ring[i]).height, 2.0)) / 2);
				++count;
			}
		}
		if (count == 6)
		{
			const int size_rings = 6;
			for (int i = 0; i < size_rings; ++i)
			{
				Ring r;
				r.center = centre[i];
				r.radius = radius[i];
				r.roi = initial_roi[i];
				rings[i] = r;
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	void RingBox::updateKeys(const pegBox &pegBox)
	{
		for (auto it_ring = rings.begin(); it_ring != rings.end(); ++it_ring)
		{
			Ring p = it_ring->second;
			for (auto it_peg = pegBox.pegs.begin(); it_peg != pegBox.pegs.end(); ++it_peg)
			{
				Rect q = it_peg->second.roi;
				if (util::rectOverlap(p.roi, q))
				{
					rings.erase(it_ring->first);
					rings[it_peg->first] = p;
				}
			}
		}
	}
}
