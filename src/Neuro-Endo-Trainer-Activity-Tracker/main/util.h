#ifndef UTIL_Included
#define UTIL_Included
#include <opencv2/core/core.hpp>
#include <string>
using namespace cv;
using namespace std;

#define STATIONARY "stationary"

namespace nets
{
	class util
	{
	private:

	public:
		util();
		~util();
		static bool rectOverlap(Rect A, Rect B);
		static string filenameFromPath(const string &path);
	};
}

#endif



