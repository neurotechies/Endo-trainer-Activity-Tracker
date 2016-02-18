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
}
