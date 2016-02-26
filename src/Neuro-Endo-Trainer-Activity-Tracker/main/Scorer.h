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


class Scorer
{

public:
	vector<Activity> activityCollector;
	Scorer(){}
	~Scorer(){}
};

#endif