#include "util.h"

namespace nets
{
	util::util(){}

	util::~util(){}

	bool util::rectOverlap(Rect A, Rect B)
	{
		bool xOverlap = (A.x >= B.x) && (A.x <= B.x + B.width) || (B.x >= A.x) && (B.x <= A.x + A.width);
		bool yOverlap = (A.y >= B.y) && (A.y <= B.y + B.height) || (B.y >= A.y) && (B.y <= A.y + A.height);
		return xOverlap && yOverlap;
	}
	string util::filenameFromPath(const string &path)
	{
		string filename = path;
		const size_t last_slash_idx = filename.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			filename.erase(0, last_slash_idx + 1);
		}

		// Remove extension if present.
		const size_t period_idx = filename.rfind('.');
		if (std::string::npos != period_idx)
		{
			filename.erase(period_idx);
		}
		return filename;
	}

}
