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


#ifndef GUI_H_
#define GUI_H_

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
using namespace std;
using namespace cv;
namespace nets
{

class Gui
{
public:
    Gui();
    ~Gui();
    void init();
	void destroy();
	void showImage(IplImage *image);
	void showImageByDestroyingWindow(IplImage *image);
    char getKey();
    std::string windowName();

private:
    std::string m_window_name;
};

/**
 * Get a bounding box from the user.
 * @param img image to display
 * @param rect CvRect containing the coordinates of the bounding box
 * @param gui initialized gui
 * @return PROGRAM_EXIT if 'q' or 'Q' pressed, SUCCESS if everything went right
 */
int getBBFromUser(IplImage *img, CvRect &rect, Gui *gui, const string &message);
int getPegthresholdFromUser(IplImage *img, Gui *gui, string message, int pegThreshVal, Rect r, cv::Mat &fgMaskPeg);
int getRingthresholdFromUser(IplImage *img, Gui *gui, string message, int pegThreshVal, Rect r, cv::Mat &fgMaskPeg);

}

#endif /* GUI_H_ */
