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


#ifndef CONTOURS_FEATURE_HPP
#define CONTOURS_FEATURE_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>

namespace nets
{

	class ContourFeature {

	public:
		//Default constructor
		ContourFeature();

		//Constructs ContourFeature from a set of points
		ContourFeature(const std::vector<cv::Point>& contour);

		//Copy constructor
		ContourFeature(const ContourFeature&);


		ContourFeature& operator=(const ContourFeature& cf) {
		  this->mom = cf.mom;
		  this->eccentricity = cf.eccentricity;
		  this->perimeter = cf.perimeter;
		  this->areaRatio = cf.areaRatio;
		  this->hull = cf.hull;
		  this->hullArea = cf.hullArea;
		  this->convexRatio = cf.convexRatio;
		  this->centre = cf.centre;
		  this->solidity = cf.solidity;


		}

		//Update the contours features
		void update(const std::vector<cv::Point>& contour);

		//Return contour caracteristics
		const cv::Point2d& getCentre() const;
		double getArea() const;
		double getPerimeter() const;
		double getEccentricity() const;
		double getHullArea() const;
		double getAreaRatio() const;
		double getConvexRatio() const;
		double getSolidity() const;

		//Display contour caracteristics
		void info() const;

	protected:
		double perimeter;
		cv::Point2d centre;
		cv::Moments mom;
		std::vector<cv::Point> hull;
		double area;
		double eccentricity;
		double areaRatio;
		double hullArea;
		double solidity;//1 if non-deformable shape
		double convexRatio;
	};

}
#endif
