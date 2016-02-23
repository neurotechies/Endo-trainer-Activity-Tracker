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
	bool RingBox::init(const cv::Mat &inp, const pegBox &pegBox)
	{
		assert(pegBox.pegs.size() == 12);
		vector<vector<Point> > contours_ring;
		Mat initial_ring_mask = inp.clone();
		vector<Vec4i> hierarchy_ring;
		vector<Rect> initial_roi;


		findContours(initial_ring_mask, contours_ring, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		for (int i = contours_ring.size() - 1; i >= 0; i--)
		{
			if (contours_ring[i].size() < 50)
			{
				contours_ring.erase(contours_ring.begin() + i);
			}
		}
		unsigned int No_of_ring = contours_ring.size();

		if (No_of_ring == 6)
		{
			vector<vector<Point> > contours_poly(No_of_ring);
			vector<Point2f>center_poly(No_of_ring);
			vector<float>radius_poly(No_of_ring);

			for (int i = 0; i < No_of_ring; ++i)
			{

				ContourFeature cFeat(contours_ring[i]);
				approxPolyDP(Mat(contours_ring[i]), contours_poly[i], 3, true);
				minEnclosingCircle((Mat)contours_poly[i], center_poly[i], radius_poly[i]);
				initial_roi.push_back(boundingRect(contours_ring[i])); 
			}
			
			
			
			const int size_rings = 6;
			for (int i = 0; i < size_rings; ++i)
			{
				Ring r;
				r.center = center_poly[i];
				r.radius = radius_poly[i];
				r.roi = initial_roi[i];
				r.velocity = 0; 
				r.status = STATIONARY;
				r.id = i;
				for (auto it_peg = pegBox.pegs.begin(); it_peg != pegBox.pegs.end(); ++it_peg)
				{
					Rect q = it_peg->second.roi;
					if (util::rectOverlap(r.roi, q))
					{
						rings[it_peg->first] = r;
						break;
					}
				}	
			}
			if (rings.size() == 6)
			{
				return true;
			}
			else
			{
				cout << "There is some problem in overlapping\n";
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}
