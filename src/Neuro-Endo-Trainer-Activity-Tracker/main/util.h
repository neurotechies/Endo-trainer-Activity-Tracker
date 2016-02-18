#ifndef UTIL_Included
#define UTIL_Included
#include <opencv2/core/core.hpp>
using namespace cv;
using namespace std;

namespace nets
{
	class util
	{
	private:

	public:
		util();
		~util();
		static bool rectOverlap(Rect A, Rect B);
	};
}

#endif



