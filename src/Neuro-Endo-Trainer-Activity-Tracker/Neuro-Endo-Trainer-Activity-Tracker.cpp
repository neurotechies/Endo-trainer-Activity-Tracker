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
#include <set>
using nets::Gui;

int main(int argc, char **argv)
{
    Main *main = new Main();
    ImAcq *v = imAcqAlloc();
    Gui *gui = new Gui();

    // get the path of the video file
	v->method = IMACQ_VID;
	v->imgPath = IMACQ_VIDEO_PATH;

	main->gui = gui;
    main->imAcq = v;


    srand(main->seed);

    imAcqInit(v);

    gui->init();
    

    main->run();

    delete main;
    main = NULL;
    delete gui;
    gui = NULL;

    return EXIT_SUCCESS;
}
