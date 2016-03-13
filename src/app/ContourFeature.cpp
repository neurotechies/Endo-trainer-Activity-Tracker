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



#include "ContourFeature.hpp"

namespace tld
{

	ContourFeature::ContourFeature() :mom(), centre(), hull()
	{
		area = 0;
		eccentricity = 0;
		perimeter = 0;
		areaRatio = 0;
		hullArea = 0;
		convexRatio = 0;
		solidity = 0;
	}

	ContourFeature::ContourFeature(const ContourFeature& cf) :
		mom(cf.mom), hull(cf.hull)
	{

		this->area = cf.area;
		this->eccentricity = ((pow(mom.mu20 - mom.mu02, 2))
			- (4 * pow(mom.mu11, 2)))
			/ (pow(mom.mu20 + mom.mu02, 2));
		cf.eccentricity;
		this->perimeter = cf.perimeter;
		this->areaRatio = cf.areaRatio;
		this->hullArea = cf.hullArea;
		this->convexRatio = cf.convexRatio;
		cv::Point c = cf.getCentre();
		this->centre.x = c.x;
		this->centre.y = c.y;
		this->solidity = cf.solidity;
	}

	ContourFeature::ContourFeature(const std::vector<cv::Point>& contour)
	{
		mom = moments(cv::Mat(contour));
		area = mom.m00;
		eccentricity = ((pow(mom.mu20 - mom.mu02, 2))
			- (4 * pow(mom.mu11, 2)))
			/ (pow(mom.mu20 + mom.mu02, 2));
		perimeter = cv::arcLength(cv::Mat(contour), true);
		areaRatio = (4 * CV_PI * area) / (perimeter*perimeter);
		convexHull(cv::Mat(contour), hull);
		hullArea = cv::contourArea(cv::Mat(hull));
		convexRatio = area / hullArea;
		centre = cv::Point2d(mom.m10 / mom.m00, mom.m01 / mom.m00);
		solidity = area / hullArea;
	}

	void ContourFeature::update(const std::vector<cv::Point>& contour)
	{
		hull.clear();

		mom = moments(cv::Mat(contour));
		area = mom.m00;
		eccentricity = ((pow(mom.mu20 - mom.mu02, 2))
			- (4 * pow(mom.mu11, 2)))
			/ (pow(mom.mu20 + mom.mu02, 2));
		perimeter = cv::arcLength(cv::Mat(contour), true);
		areaRatio = (4 * CV_PI * area) / (perimeter*perimeter);
		convexHull(cv::Mat(contour), hull);
		hullArea = cv::contourArea(cv::Mat(hull));
		convexRatio = area / hullArea;
		centre = cv::Point2d(mom.m10 / mom.m00, mom.m01 / mom.m00);
		solidity = area / hullArea;

	}


	void ContourFeature::info() const
	{
		std::cout << "Perimeter: " << perimeter << std::endl;
		std::cout << "Centre : (" << centre.x << "; " << centre.y << ")" << std::endl;
		std::cout << "Area: " << area << std::endl;
		std::cout << "Eccentricity: " << eccentricity << std::endl;
		std::cout << "Area ratio: " << areaRatio << std::endl;
		std::cout << "Convex hull area: " << hullArea << std::endl;
		std::cout << "Convex ratio: " << convexRatio << std::endl;
		std::cout << "Solidity: " << solidity << std::endl;
		std::cout << std::endl << std::endl;
	}

	const cv::Point2d& ContourFeature::getCentre() const
	{
		return centre;
	}

	double ContourFeature::getArea() const
	{
		return area;
	}

	double ContourFeature::getPerimeter() const
	{
		return perimeter;
	}

	double ContourFeature::getEccentricity() const
	{
		return eccentricity;
	}

	double ContourFeature::getHullArea() const
	{
		return hullArea;
	}

	double ContourFeature::getConvexRatio() const
	{
		return convexRatio;
	}

	double ContourFeature::getAreaRatio() const
	{
		return areaRatio;
	}

	double ContourFeature::getSolidity() const
	{
		return solidity;
	}

}