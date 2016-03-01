#ifndef Scorer_Included
#define Scorer_Included

#include <vector>
using namespace std;

struct Activity
{
	// struct definitions
	struct stationary
	{
		int startFrame, endFrame;
		vector<pair<int, int> > hitting;
		vector<pair<int, pair<int, int>> > trackingData;
		vector<int> framesForTrackingFailure;
		vector<int> framesForRingHitting;
		stationary()
		{
			startFrame = -1;
			endFrame = -1;
		}
		void stationary::clear()
		{
			startFrame = -1;
			endFrame = -1;
			hitting.clear();
			trackingData.clear();
			framesForTrackingFailure.clear();
			framesForRingHitting.clear();
		}
	};
	struct Picking
	{
		int startFrame, endFrame;
		int from_peg;
		vector<pair<int, int> > hitting;
		vector<pair<int, pair<int, int> > > trackingData;
		vector<int> framesForTrackingFailure;
		vector<int> framesForRingHitting;
		Picking()
		{
			startFrame = 0;
			endFrame = 0;
			from_peg = 0;
		}
		void Picking::clear()
		{
			startFrame = 0;
			endFrame = 0;
			from_peg = 0;
			hitting.clear();
			trackingData.clear();
			framesForTrackingFailure.clear();
			framesForRingHitting.clear();
		}
	};
	struct Moving
	{
		int startFrame, endFrame, from_peg, to_peg;
		vector<pair<int, int> > hitting;
		vector<pair<int, pair<int, int>> > trackingData;
		vector<int> framesForTrackingFailure;
		vector<int> framesForRingHitting;
		Moving()
		{
			startFrame = 0;
			endFrame = 0;
			from_peg = 0;
			to_peg = 0;
		}
		void clear()
		{
			startFrame = 0;
			endFrame = 0;
			from_peg = 0;
			to_peg = 0;
			hitting.clear();
			trackingData.clear();
			framesForTrackingFailure.clear();
			framesForRingHitting.clear();
		}
	};

	string  type;
	int no_of_frames;
	stationary s;
	Picking p;
	Moving m;

	Activity()
	{
		type = "";
		no_of_frames = 0;
	}
	void clear()
	{
		type = "";
		no_of_frames = 0;
		s.clear();
		p.clear();
		m.clear();
	}
};


struct Result
{
	struct Hitting
	{
		vector<pair <int, int> > hittingData;
		double hittingScore;
		Hitting()
		{
			hittingScore = 0;
			hittingData.clear();
		}
		void clear()
		{
			hittingScore = 0;
			hittingData.clear();
		}
	};

	struct Grasping
	{
		vector<pair <int, int> > NoFramesPicking;
		vector<pair <int, double> > trackingMean;
		double grapspingScore;
		Grasping()
		{
			NoFramesPicking.clear();
			trackingMean.clear();
			grapspingScore = 0;
		}
		void clear()
		{
			NoFramesPicking.clear();
			trackingMean.clear();
			grapspingScore = 0;
		}
	};
	
	struct WavyMotion
	{
		
		vector<pair<pair<int, int>, int> > NoFramesMoving; //               to,  from, No of frames  
		vector<pair<pair<int, int>, double> > trackingMean;//               to,  from, trackingResult  
		double wavyMotionScore;
		WavyMotion()
		{
			NoFramesMoving.clear();
			trackingMean.clear();
			wavyMotionScore = 0;
		}
		void clear()
		{
			NoFramesMoving.clear();
			trackingMean.clear();
			wavyMotionScore = 0;
		}

	};

	struct RingHitting_wrongPlacement
	{
		vector<string> frameRingHitting;
		double ringHittingScore;
		RingHitting_wrongPlacement()
		{
			frameRingHitting.clear();
			ringHittingScore = 0;
		}
		void clear()
		{
			frameRingHitting.clear();
			ringHittingScore = 0;
		}
	};

	struct suddenMovement
	{
		vector<int> frameTrackingFailed;
		double suddenMovementScore;
		suddenMovement()
		{
			frameTrackingFailed.clear();
			suddenMovementScore = 0;
		}
		void clear()
		{
			frameTrackingFailed.clear();
			suddenMovementScore = 0;
		}
	};

	struct wrongMoves
	{
		vector<pair<int, int> > rightMoves;
		double wrongMovesScore;
		wrongMoves()
		{
			wrongMovesScore = 0;
			rightMoves.clear();
			rightMoves.push_back(make_pair(5, 8));
			rightMoves.push_back(make_pair(2, 11));
			rightMoves.push_back(make_pair(4, 7));
			rightMoves.push_back(make_pair(1, 10));
			rightMoves.push_back(make_pair(3, 6));
			rightMoves.push_back(make_pair(0, 9));

			rightMoves.push_back(make_pair(9, 5));
			rightMoves.push_back(make_pair(6, 2));
			rightMoves.push_back(make_pair(10, 1));
			rightMoves.push_back(make_pair(7, 4));
			rightMoves.push_back(make_pair(11, 0));
			rightMoves.push_back(make_pair(8, 3));
		}

		void clear()
		{
			wrongMovesScore = 0;
			rightMoves.clear();
			rightMoves.push_back(make_pair(5, 8));
			rightMoves.push_back(make_pair(2, 11));
			rightMoves.push_back(make_pair(4, 7));
			rightMoves.push_back(make_pair(1, 10));
			rightMoves.push_back(make_pair(3, 6));
			rightMoves.push_back(make_pair(0, 9));

			rightMoves.push_back(make_pair(9, 5));
			rightMoves.push_back(make_pair(6, 2));
			rightMoves.push_back(make_pair(10, 1));
			rightMoves.push_back(make_pair(7, 4));
			rightMoves.push_back(make_pair(11, 0));
			rightMoves.push_back(make_pair(8, 3));
		}
	};
	struct No_Activity
	{
		int no_frames;
		double trackingScore;
		No_Activity()
		{
			no_frames = 0;
			trackingScore = 0;
		}
		void clear()
		{
			no_frames = 0;
			trackingScore = 0;
		}
	};


	double totalScore;
	Hitting hitting;
	Grasping grasping;
	WavyMotion wavymotion;
	RingHitting_wrongPlacement ringHItting;
	suddenMovement suddenmovement;
	wrongMoves wrongmoves;
	No_Activity noactivity;
	Result()
	{
		totalScore = 0;
	}

	void clear()
	{
		totalScore = 0;
		hitting.clear();
		grasping.clear();
		wavymotion.clear();
		ringHItting.clear();
		suddenmovement.clear();
		wrongmoves.clear();
		noactivity.clear();
	}
};

#endif