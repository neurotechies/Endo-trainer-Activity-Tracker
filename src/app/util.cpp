#include "util.h"

namespace tld
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

	void util::mean2D(const vector<pair <double, double > > &data, double *meanX, double *meanY)
	{
		double sumX = 0; double sumY = 0;
		for (int i = 0; i < data.size(); ++i)
		{
			sumX += data[i].first;
			sumY += data[i].second;
		}
		*meanX = sumX / (double)data.size();
		*meanY = sumY / (double)data.size();
	}
	void util::max2D(const vector<pair <double, double > > &data, double *maxX, double *maxY)
	{
		if (data.size())
		{
			*maxX = data[0].first;
			*maxY = data[0].second;
			for (int i = 1; i < data.size(); ++i)
			{
				if (abs(data[i].first) > *maxX)
				{
					*maxX = abs(data[i].first);
				}
				if (abs(data[i].second) > *maxY)
				{
					*maxY = abs(data[i].second);
				}
			}
		}
		else
		{
			*maxX = 0;
			*maxY = 0;
		}
	}
	void util::normalize2D(const vector<pair <double, double > > &indata, vector<pair <double, double > > &outdata)
	{
		double meanX = 0; double meanY = 0;
		double maxX = 0; double maxY = 0;
		mean2D(indata, &meanX, &meanY);
		
		outdata.resize(indata.size());
		for (int i = 0; i < indata.size(); ++i)
		{
			outdata[i].first = (indata[i].first - meanX);
			outdata[i].second = (indata[i].second - meanY);
		}
		max2D(outdata, &maxX, &maxY);
		for (int i = 0; i < outdata.size(); ++i)
		{
			outdata[i].first = (outdata[i].first) / maxX;
			outdata[i].second = (outdata[i].second) / maxY;
		}
	}

	void util::diff2D(const vector<pair <double, double > > &indata, vector<pair <double, double > > &outdata)
	{
		outdata.resize(indata.size() - 1);
		for (int i = 0; i < indata.size()-1; ++i)
		{
			outdata[i].first = indata[i + 1].first - indata[i].first;
			outdata[i].second = indata[i + 1].second - indata[i].second;
		}
	}
	void util::var2D(const vector<pair <double, double > > &indata, double *varX, double *varY)
	{
		double meanX = 0; double meanY = 0;
		double sumX = 0; double sumY = 0;
		mean2D(indata, &meanX, &meanY);
		for (int i = 0; i < indata.size(); ++i)
		{
			sumX += (indata[i].first - meanX) * (indata[i].first - meanX);
			sumY += (indata[i].second - meanY) * (indata[i].second - meanY);
		}
		sumX /= indata.size() - 1;
		sumY /= indata.size() - 1;
	}
	void util::curvature(const vector<pair <double, double > > &dx,
		const vector<pair <double, double > > &ddx,
		vector<double> &curvature)
	{
		curvature.resize(ddx.size());
		for (int i = 0; i < ddx.size(); ++i)
		{
			curvature[i] = abs((dx[i].first * ddx[i].second) - (dx[i].second * ddx[i].first))
				/ pow(dx[i].first * dx[i].first + dx[i].second * dx[i].second, 1.5);
		}
	}

	void util::arclength(const vector<pair <double, double > > &dx, double *arcLength)
	{
		*arcLength = 0;
		for (int i = 0; i < dx.size(); ++i)
		{
			*arcLength += pow(dx[i].first * dx[i].first + dx[i].second * dx[i].second, 0.5);
		}
	}
	void util::curvatureMaxnCount(const vector<double > &curvature, double *curvatureMax, int*curvatureMaxCount)
	{
		if (curvature.size())
		{
			*curvatureMax = curvature[0];
			for (int i = 1; i < curvature.size(); ++i)
			{
				if (abs(curvature[i]) > *curvatureMax)
				{
					*curvatureMax = abs(curvature[i]);
				}
				if (abs(curvature[i]) > 300)
				{
					*curvatureMaxCount++;
				}
			}
		}
		else
		{
			*curvatureMax = 0;
			*curvatureMaxCount = 0;
		}
	}
}
