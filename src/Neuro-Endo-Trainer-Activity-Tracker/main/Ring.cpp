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



#include "Ring.h"
namespace nets
{

	Ring::Ring() :center(Point2f(0, 0)), radius(0), roi()
	{
	}

	Ring::Ring(const Point2f & cen, float radi, const Rect &region, string s, int code) : center(cen), radius(radi), roi(region), ringState(s), code_pos(code)
	{
	}


	Point2f Ring::getCenter()
	{
		return this->center;
	}

	float Ring::getRadii()
	{
		return this->radius;
	}
	int Ring::getCode()
	{
		return code_pos;
	}
	Rect Ring::getRoi()
	{
		return this->roi;
	}

	void Ring::setCode(int code)
	{
		this->code_pos = code;
	}
	void Ring::setStatus(const string &status)
	{
		this->ringState = status;
	}
	string Ring::getStatus()
	{
		return this->ringState;
	}

}