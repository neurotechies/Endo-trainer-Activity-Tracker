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



#include "pegBox.h"
namespace nets
{
	bool pegBox::init(const cv::Mat &inp)
	{
		vector<vector<Point> > pegsI;
		Mat mask = inp.clone();
		
		vector<Vec4i> hierarchy_ring;
		
		vector<Point2f> centre;
		vector<Rect> initial_roi;

		//imshow("Initial mask", inp);
		//waitKey(0);
		findContours(mask, pegsI, hierarchy_ring, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		unsigned int No_of_pegs = pegsI.size();
		vector<bool> markedPegs(No_of_pegs, true);

		for (int i = 0; i < No_of_pegs; ++i)
		{
			if (pegsI[i].size() < PEG_SIZE_MIN)//TODO (define min threshold)
			{
				markedPegs[i] = false;
			}
		}
		unsigned int count = 0;
		for (int i = 0; i < No_of_pegs; ++i)
		{
			if (markedPegs[i])
			{
				ContourFeature cFeat(pegsI[i]);
				centre.push_back(cFeat.getCentre()); //centre.y += 10;
				initial_roi.push_back(boundingRect(pegsI[i])); // This is a OpenCV function
				++count;
			}
		}
		if (count == 12)
		{
			int no = initial_roi.size();

			count = 0;
			pegs.resize(12);
			vector<pair<Point2f, int> >center_id(no);
			std::unordered_map<int, Rect> roi_id;
			for (int i = 0; i < no; ++i)
			{
				roi_id[i] = initial_roi[i];
				center_id[i] = make_pair(centre[i], i);
			}
			sort(center_id.begin(), center_id.end(), _comparator);
			vector<pair<Point2f, int> > pts(3);
			for (int p = 0; p <= center_id.size() - 3; p = p + 3)
			{
				pts[0] = make_pair(Point2f(center_id[p].first.y, center_id[p].first.x), center_id[p].second);
				pts[1] = make_pair(Point2f(center_id[p + 1].first.y, center_id[p + 1].first.x), center_id[p + 1].second);
				pts[2] = make_pair(Point2f(center_id[p + 2].first.y, center_id[p + 2].first.x), center_id[p + 2].second);
				sort(pts.begin(), pts.end(), _comparator);
				for (int i = 0; i < 3; i++)
				{
					Peg p;
					p.center = Point2f(pts[i].first.y, pts[i].first.x);
					p.roi = roi_id[pts[i].second];
					p.code = count;
					pegs[count] = p;
					count++;
				}
			}
			return true;
		}
		else
		{
			return false;
		}


	}

	void pegBox::print()
	{
		for (int i = 0; i < pegs.size(); ++i)
		{
			Peg peg = pegs[i];
			cout << "Code(" << peg.code << ") center (" << peg.center.x << peg.center.y << ")  ROI (" << peg.roi.x << peg.roi.y << peg.roi.width << peg.roi.height << ")\n";
		}
	}


	bool pegBox::update(int code, const Rect &r)
	{
		if (code > 11)
		{
			return false;
		}
		else
		{
			pegs[code].roi = r;
			return true;
		}
	}
	
	void pegBox::roi_update(const vector<pair<Rect, int> > &rois)
	{
		set<int> s1;
		set<int> s2;
		set<int> s3;
		s1.insert(0);
		s1.insert(1);
		s1.insert(2);
		s1.insert(3);
		s1.insert(4);
		s1.insert(5);

		s2.insert(6);
		s2.insert(7);
		s2.insert(8);
		s2.insert(9);
		s2.insert(10);
		s2.insert(11);

		for (int i = 0; i < rois.size(); ++i)
		{
			s3.insert(rois[i].second);
		}
		std::vector<int> c1;
		std::vector<int> c2;
		set_intersection(s1.begin(), s1.end(), s3.begin(), s3.end(), std::back_inserter(c1));
		set_intersection(s2.begin(), s2.end(), s3.begin(), s3.end(), std::back_inserter(c2));


		if (c2.empty())
		{
			for (int i = 0; i < rois.size(); ++i)
			{
				int code = rois[i].second;
				for (int p = 0; p < pegs.size(); ++p)
				{
					if (pegs[p].code == code)
					{
						Rect r = rois[i].first;
						pegs[p].roi = r;

						Point2f cen = pegs[p + 6].center;
						pegs[p + 6].roi = Rect(cen.x - r.width / 2, cen.y - r.height / 4, r.width, r.height);
					}
				}
			}
		}
		else if (c1.empty())
		{
			for (int i = 0; i < rois.size(); ++i)
			{

				int code = rois[i].second;
				for (int p = 0; p < pegs.size(); ++p)
				{
					if (pegs[p].code == code)
					{

						Rect r = rois[i].first;
						pegs[p].roi = r;

						Point2f cen = pegs[p - 6].center;
						pegs[p - 6].roi = Rect(cen.x - r.width / 2, cen.y - r.height / 4, r.width, r.height);

					}
				}


			}
		}
		else
		{
			cout << "Wrong placement of the rings \n";
			exit(0);
		}
	}
}