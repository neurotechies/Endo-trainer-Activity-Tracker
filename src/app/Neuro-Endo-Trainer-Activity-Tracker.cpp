/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/**
  * @author Georg Nebehay
  */

#include "Main.h"
#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include "util.h"

using tld::Config;
using tld::Gui;
using tld::Settings;

int main(int argc, char **argv)
{
	//// test code 
	//vector<pair <double, double > > data;
	//vector<pair <double, double > > data1;
	//vector<pair <double, double > > data2;
	//vector<pair <double, double > > data3;
	//vector<double> data4;
	//data.push_back(make_pair(1, 2));
	//data.push_back(make_pair(2, 3));
	//data.push_back(make_pair(3, 4));
	//data.push_back(make_pair(4, 5));
	//data.push_back(make_pair(5, 6));
	//data.push_back(make_pair(6, 7));
	//data.push_back(make_pair(7, 8));
	//data.push_back(make_pair(8, 9));
	//data.push_back(make_pair(9, 10));
	//data.push_back(make_pair(10, 11));
	//double arclength = 0;
	//util::normalize2D(data, data1);
	//util::diff2D(data1, data2);
	//util::diff2D(data2, data3);
	//util::curvature(data2, data3, data4);
	//util::arclength(data2, &arclength);
	// end test code


    Main *main = new Main();
    Config config;
    ImAcq *imAcq = imAcqAlloc();
    Gui *gui = new Gui();

    main->gui = gui;
    main->imAcq = imAcq;

    if(config.init(argc, argv) == PROGRAM_EXIT)
    {
        return EXIT_FAILURE;
    }

    config.configure(main);

    srand(main->seed);

    imAcqInit(imAcq);

    if(main->showOutput)
    {
        gui->init();
    }

    main->run();

    delete main;
    main = NULL;
    delete gui;
    gui = NULL;

    return EXIT_SUCCESS;
}
