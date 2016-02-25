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

#include "Main.h"

#include "ImAcq.h"
#include "Gui.h"



void Main::run()
{
	CvScalar white = CV_RGB(255, 255, 255);
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
	// Read the current frame and convert to the grayscale
	IplImage *img = imAcqGetImg(imAcq);
	Mat grey(img->height, img->width, CV_8UC1);
	cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);

	tld->detectorCascade->imgWidth = grey.cols;
	tld->detectorCascade->imgHeight = grey.rows;
	tld->detectorCascade->imgWidthStep = grey.step;

	if (loadModel && modelPath != NULL)
	{
		tld->readFromFile(modelPath);
	}

	while (imAcqHasMoreFrames(imAcq))
	{
		// get the image
		cvReleaseImage(&img);
		img = imAcqGetImg(imAcq);

		string str = "NN has now " + SSTR(tld->detectorCascade->nnClassifier->truePositives->size()) + " positives and " + SSTR(tld->detectorCascade->nnClassifier->falsePositives->size()) + " negatives";
		cvPutText(img, str.c_str(), cvPoint(25, 25), &font, white);
		gui->showImage(img);

		char key = gui->getKey();

		if (key == 'q') break;

		if (key == 'r')
		{
			CvRect box;
			std::string message = "Draw a bounding box around the object to be learn then press enter";
			if (getBBFromUser(img, box, gui, message) == PROGRAM_EXIT)
			{
				break;
			}
			Rect r = Rect(box);
			if (learnFirstFrame && !loadModel)
			{
				// initial learning 
				tld->selectObject(grey, &r);
				learnFirstFrame = false;
			}
			else
			{
				// learn the patch
				tld->learnPatch(grey, &r);
			}
		}
	}
	cvReleaseImage(&img);
	img = NULL;

	if (exportModelAfterRun)
	{
		tld->writeToFile(modelExportFile);
	}
}
