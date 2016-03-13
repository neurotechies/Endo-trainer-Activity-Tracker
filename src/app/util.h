#ifndef UTIL_Included
#define UTIL_Included
#include <opencv2/core/core.hpp>
#include <string>
using namespace cv;
using namespace std;

#define STATIONARY "stationary"

namespace tld
{
	class util
	{
	private:

	public:
		util();
		~util();
		static bool rectOverlap(Rect A, Rect B);
		static string filenameFromPath(const string &path);
		static void mean2D(const vector<pair <double, double > > &data, double *meanX, double *meanY);
		static void max2D(const vector<pair <double, double > > &data, double *maxX, double *maxY);
		static void normalize2D(const vector<pair <double, double > > &indata, vector<pair <double, double > > &outdata);
		static void diff2D(const vector<pair <double, double > > &indata, vector<pair <double, double > > &outdata);
		static void var2D(const vector<pair <double, double > > &indata, double *varX, double *varY);
		static void curvature(const vector<pair <double, double > > &dx, 
							  const vector<pair <double, double > > &ddx,
							  vector<double> &curvature);
		static void arclength(const vector<pair <double, double > > &dx, double *arcLength);
		static void curvatureMaxnCount(const vector<double> &curvature, double *curvatureMax, int*curvatureMaxCount);
		
	};
}

#endif



