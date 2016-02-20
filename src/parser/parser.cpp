#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	( std::ostringstream() << std::dec << x ) ).str()



struct activity_info 
{
	// struct definitions
	struct stationary
	{
		int startFrame, endFrame;
		vector<pair<int, int> > hitting;
		vector<pair<int, pair<int, int>> > trackingData;
		stationary()
		{
			startFrame = 0;
			endFrame = 0;
		}
		void stationary::clear()
		{
			startFrame = 0;
			endFrame = 0;
			hitting.clear();
			trackingData.clear();
		}
	};
	struct Picking
	{
		int startFrame, endFrame;
		int from_peg;
		vector<pair<int, int> > hitting;
		vector<pair<int, pair<int, int> > > trackingData;
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
		}
	};
	struct Moving
	{
		int startFrame, endFrame, from_peg, to_peg;
		vector<pair<int, int> > hitting;
		vector<pair<int, pair<int, int>> > trackingData;
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
		}
	};
	
	string  type; 
	int no_of_frames;
	stationary s;
	Picking p;
	Moving m;

	activity_info()
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

struct file_parse
{
	int frame_no;
	pair<string, int> ring_code_status[6];
	int hitting_detection;
	pair<int, int> tool_tip_center;
	double tracking_confidence;
	file_parse()
	{
		frame_no = -1;
		ring_code_status[0].first = "";
		ring_code_status[0].second = -1;
		hitting_detection = -1;
		tool_tip_center.first = -1;
		tool_tip_center.second = -1;
	}
};
struct rect
{
	int x, y, width, height;
};
int main()
{
	vector<activity_info> vec_activity;
	activity_info act;

	// readable parameters
	pair<rect, int> peg_code_status[12];
	string video_filename = "";
	string pegname = "";
	string Allactivity = "";
	vector<file_parse> parse_activity;


	//  
	ifstream ResultFile("D:/Official/Programs/endo_tracking/src_main/Result/DrAnton_2_0_Deg ST_Aux.txt");

	// 
	string line;
	stringstream convertor;
	int lineNo = 1;
	while (getline(ResultFile, line))
	{
		//cout << line << endl;
		if (lineNo == 1)
		{
			// read video filename;
			std::size_t found = line.find_last_of("/");
			video_filename = line.substr(found + 1);
			cout << video_filename << endl;
			//lineNo++;
		}

		if (lineNo > 2 && lineNo <= 14)
		{
			// read the location and code of pegs
			convertor << line;
			string temp1, temp2;
			int temp3;
			//convertor >> a >> b >> c >> d >> e >> f;
			int idx = lineNo - 3;
			convertor >> temp1 >> temp3 >> temp2 >> peg_code_status[idx].first.x >> peg_code_status[idx].first.y >> peg_code_status[idx].first.width >> peg_code_status[idx].first.height >> peg_code_status[idx].second;
			convertor.clear();

		}
		if (lineNo > 17)
		{
			// read the activity
			file_parse obj;
			convertor << line;
			int x, y, width, height;
			convertor >> obj.frame_no >> obj.ring_code_status[0].first >> obj.ring_code_status[0].second
				>> obj.ring_code_status[1].first >> obj.ring_code_status[1].second
				>> obj.ring_code_status[2].first >> obj.ring_code_status[2].second
				>> obj.ring_code_status[3].first >> obj.ring_code_status[3].second
				>> obj.ring_code_status[4].first >> obj.ring_code_status[4].second
				>> obj.ring_code_status[5].first >> obj.ring_code_status[5].second
				>> obj.hitting_detection
				>> x >> y >> width >> height >> obj.tracking_confidence;

			if (x == -1)
			{
				obj.tool_tip_center.first = -1;
				obj.tool_tip_center.second = -1;
			}
			else
			{
				obj.tool_tip_center.first = x + width;
				obj.tool_tip_center.second = y + height + 40;
			}

			parse_activity.push_back(obj);
			convertor.clear();
		}
		lineNo++;
	}

	// 
	string status;
	int count_p = 0;
	int count_m = 0;
	int count_s = 0;
	int index = -1;
	int last_frame;
	int size = parse_activity.size();
	bool No_activity_start = false;
	bool picking_activity_start = false;
	bool moving_activity_start = false;
	bool update_startFrameAndType = true;
	activity_info objActivity;

	for (int i = 0; i < size; ++i)
	{
		file_parse obj = parse_activity[i];
		last_frame = obj.frame_no;
		for (int p = 0; p < 6; ++p)
		{
			if (obj.ring_code_status[p].first == "picking")
			{
				count_p++;
				index = p;
			}
			if (obj.ring_code_status[p].first == "stationary")
			{
				count_s++;
			}
			if (obj.ring_code_status[p].first == "moving")
			{
				count_m++;
				index = p;
			}
		}
		
		status = (count_s == 6) ? "stationary" : (count_s == 5 && count_p == 1) ? "picking" : (count_s == 5 && count_m == 1) ? "moving" : "invalid";
		if (status == "invalid")
		{
			cout << "Invalid Data";
			return -1;
		}

		if (status == "stationary")
		{
			if (moving_activity_start)
			{
				objActivity.m.endFrame = obj.frame_no-1;
				objActivity.m.to_peg = obj.ring_code_status[index].second;
				vec_activity.push_back(objActivity);
				objActivity.clear();
				moving_activity_start = false;
				update_startFrameAndType = true;
			}
			if (update_startFrameAndType)
			{
				objActivity.type = "No-Activity";
				objActivity.s.startFrame = obj.frame_no;
				update_startFrameAndType = false;
			}
			objActivity.no_of_frames++;
			objActivity.s.hitting.push_back(make_pair(i, obj.hitting_detection));
			objActivity.s.trackingData.push_back(make_pair(i, make_pair(obj.tool_tip_center.first, obj.tool_tip_center.second)));
			No_activity_start = true;
		}
		else if (status == "picking")
		{
			if (No_activity_start)
			{
				objActivity.s.endFrame = obj.frame_no-1;
				vec_activity.push_back(objActivity);
				objActivity.clear();
				No_activity_start = false;
				update_startFrameAndType = true;
			}
			if (update_startFrameAndType)
			{
				objActivity.type = "Activity-Picking";
				objActivity.p.startFrame = obj.frame_no;
				objActivity.p.from_peg = obj.ring_code_status[index].second;
				update_startFrameAndType = false;
			}
			objActivity.no_of_frames++;
			objActivity.p.hitting.push_back(make_pair(i, obj.hitting_detection));
			objActivity.p.trackingData.push_back(make_pair(i, make_pair(obj.tool_tip_center.first, obj.tool_tip_center.second)));
			picking_activity_start = true;
		}
		else if (status == "moving")
		{
			if (picking_activity_start)
			{
				objActivity.p.endFrame = obj.frame_no-1;
				vec_activity.push_back(objActivity);
				objActivity.clear();
				picking_activity_start = false;
				update_startFrameAndType = true;
			}
			if (update_startFrameAndType)
			{
				objActivity.type = "Activity-Moving";
				objActivity.m.startFrame = obj.frame_no;
				objActivity.m.from_peg = obj.ring_code_status[index].second;
				update_startFrameAndType = false;
			}
			objActivity.no_of_frames++;
			objActivity.m.hitting.push_back(make_pair(i, obj.hitting_detection));
			objActivity.m.trackingData.push_back(make_pair(i, make_pair(obj.tool_tip_center.first, obj.tool_tip_center.second)));
			moving_activity_start = true;
		}
		count_p = 0;
		count_m = 0;
		count_s = 0;
	}
	if (last_frame > objActivity.s.startFrame)
	{
		objActivity.s.endFrame = last_frame;
		vec_activity.push_back(objActivity);
		objActivity.clear();
	}

	return 0;
}