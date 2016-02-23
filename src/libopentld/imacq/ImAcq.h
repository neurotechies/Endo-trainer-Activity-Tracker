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


#ifndef IMACQ_IMPL_H_
#define IMACQ_IMPL_H_

#include <opencv/highgui.h>

/**
 * Capturing method
 */
enum ImacqMethod
{
    IMACQ_IMGS, //!< Images
    IMACQ_CAM, //!< Camera
    IMACQ_VID, //!< Video
    IMACQ_LIVESIM, //!< Livesim
    IMACQ_STREAM //!< Stream
};

typedef struct
{
    int method;
    const char *imgPath;
    CvCapture *capture;
    int lastFrame;
    int currentFrame;
    int startFrame;
    int camNo;
    double startTime;
    float fps;
} ImAcq ;

ImAcq *imAcqAlloc();

void imAcqInit(ImAcq *imAcq);

void imAcqRelease(ImAcq *imAcq);

void imAcqVidSetNextFrameNumber(ImAcq *imAcq, int nFrame);
int imAcqVidGetNextFrameNumber(ImAcq *imAcq);
int imAcqVidGetNumberOfFrames(ImAcq *imAcq);
int imAcqHasMoreFrames(ImAcq *imAcq);
IplImage *imAcqGetImgAndAdvance(ImAcq *imAcq);
IplImage *imAcqGetImg(ImAcq *imAcq);
IplImage *imAcqGetImgByFrame(ImAcq *imAcq, int fNo);
IplImage *imAcqGetImgByCurrentTime(ImAcq *imAcq);
IplImage *imAcqLoadImg(ImAcq *imAcq, char *path);
IplImage *imAcqLoadCurrentFrame(ImAcq *imAcq);
IplImage *imAcqLoadVidFrame(CvCapture *capture);
IplImage *imAcqGrab(CvCapture *capture);
void imAcqAdvance(ImAcq *imAcq);
void imAcqFree(ImAcq *);

#endif /* IMACQ_H_ */