	if (result.wavymotion.NoFramesMoving.size() >= 10)
	{
		text = "1. Hitting";
		ycursor = 75;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		if (result.hitting.hittingData.size())
		{
			ycursor += 5;
			for (int i = 0; i < result.hitting.hittingData.size(); ++i)
			{
				text = "Frame No -> " + SSTR(result.hitting.hittingData[i].first) + ". with intensity-> " + SSTR(result.hitting.hittingData[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.68, Scalar(0, 255, 0));
			}
			text = "Hitting Score -> " + SSTR(result.hitting.hittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.68, Scalar(0, 0, 255));
		}
		else
		{
			text = "No hitting during the whole activity";
			ycursor += 20;
			putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			text = "Hitting score -> " + SSTR(result.hitting.hittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}


		text = "2. Grasping";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		if (result.grasping.NoFramesPicking.size())
		{
			ycursor += 5;
			for (int i = 0; i < result.grasping.NoFramesPicking.size(); ++i)
			{
				text = "Peg No(" + SSTR(result.grasping.NoFramesPicking[i].first) + ") no of frames-> " + SSTR(result.grasping.NoFramesPicking[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Grasping score -> " + SSTR(result.grasping.grapspingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}

		text = "3. Wavy motion";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		if (result.wavymotion.NoFramesMoving.size())
		{
			for (int i = 0; i < result.wavymotion.NoFramesMoving.size(); ++i)
			{
				text = "Peg(" + SSTR(result.wavymotion.NoFramesMoving[i].first.first) + "," + SSTR(result.wavymotion.NoFramesMoving[i].first.second) + ") Number of frames-> " + SSTR(result.wavymotion.NoFramesMoving[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Wavy motion score -> " + SSTR(result.wavymotion.wavyMotionScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}

		text = "4. Ring hitting or wrong way of placement of the ring";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		if (result.ringHItting.frameRingHitting.size())
		{
			for (int i = 0; i < result.ringHItting.frameRingHitting.size(); ++i)
			{
				text = "Activity(\"" + result.ringHItting.frameRingHitting[i].first + "\") Frame No-> " + SSTR(result.ringHItting.frameRingHitting[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Ring hitting or wrong way of placement of the ring -> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}
		else
		{
			text = "Ring hitting or wrong way of placement of the ring -> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}
		text = "5. Sudden movement";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		if (result.suddenmovement.frameTrackingFailed.size())
		{
			for (int i = 0; i < result.suddenmovement.frameTrackingFailed.size(); ++i)
			{
				text = "Activity(\"" + result.suddenmovement.frameTrackingFailed[i].first + "\") Frame No-> " + SSTR(result.suddenmovement.frameTrackingFailed[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Sudden movement score-> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}
		else
		{
			text = "Sudden movement score-> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}
		text = "6. Wrong Moves";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		if (result.wrongmoves.wrong_moves.size())
		{
			for (int i = 0; i < result.wrongmoves.wrong_moves.size(); ++i)
			{
				text = "Wrong moves detected from peg -> " + SSTR(result.wrongmoves.wrong_moves[i].first) + " to peg -> " + SSTR(result.wrongmoves.wrong_moves[i].second);
				ycursor += 15;
				putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
			}
			text = "Wrong Moves score-> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}
		else
		{
			text = "Wrong Moves score-> " + SSTR(result.ringHItting.ringHittingScore) + "/5.0";
			ycursor += 15;
			putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));
		}
		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}

		text = "7. Wasted movements";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 255));
		text = "Total no of frames in the No-Activity -> " + SSTR(result.noactivity.no_frames);
		ycursor += 15;
		putText(drawing, text, Point2f(xcursor + 10, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 255, 0));
		text = "Wasted movements score ->" + SSTR(result.noactivity.trackingScore) + "/5.0";
		ycursor += 15;
		putText(drawing, text, Point2f(xcursor + 20, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0, 0, 255));

		if (ycursor > imAcq->height - 15)
		{
			xcursor = 700;
			ycursor = 75;
		}

		text = "Total Score ->" + SSTR(result.totalScore) + "/35.0";
		ycursor += 40;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_SCRIPT_COMPLEX, 1.1, Scalar(255, 255, 255));

	}
	else
	{
		text = "All the activitis are not analyzed. There should be atleast 10 ring movements to generate a valid score.";
		ycursor = 75;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(0, 255, 0));
		text = "Provide the proper video for analysis";
		ycursor += 20;
		putText(drawing, text, Point2f(xcursor, ycursor), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(0, 255, 0));
	}
