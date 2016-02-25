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


#ifndef MAIN_H_
#define MAIN_H_

#include "ImAcq.h"
#include "Gui.h"
#include "RingBox.h"
#include "ContourFeature.hpp"
#include "pegBox.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include "params.h"
#include "TLD.h"
#include <string>
#include <sstream>

using namespace std;
using namespace nets;
using namespace cv;
using namespace tld;

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	( std::ostringstream() << std::dec << x ) ).str()

enum Retval
{
    PROGRAM_EXIT = 0,
    SUCCESS = 1
};

class Main
{
public:
	
	tld::TLD *tld;
	ImAcq *imAcq;
	nets::Gui *gui;
	bool exportModelAfterRun;
    bool loadModel;
    const char *modelPath;
    const char *modelExportFile;
	int seed;
	bool learnFirstFrame;

public:
	Main()
    {
		tld = new tld::TLD();
		imAcq = NULL;
		gui = NULL;
		seed = SEED;
        loadModel = LOAD_MODEL;
		exportModelAfterRun = EXPORT_MODEL_AFTER_RUN;
        modelExportFile = MODEL_EXPORT_FILE;
		gui = NULL;
        modelPath = MODEL_PATH;
        imAcq = NULL;
		learnFirstFrame = true;
	}

    ~Main()
    {
		delete tld;
        imAcqFree(imAcq);
	}

	void run();

};

#endif /* MAIN_H_ */
