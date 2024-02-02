#ifndef POINT_H
#define POINT_H
#include <algorithm>
#include <math.h>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct Point
{
	double x = -1.e30;
	double y = -1.e30;

	// distance between two points p1 and p2
	static double distance(const Point & p1, const Point & p2)
	{ 
		return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y)); 
	}

	// distance between a points p and a segment [p1,p2]
	static double distance(const Point & p, const Point & p1, const Point & p2)
	{
		double p1px = p.x - p1.x;
		double p1py = p.y - p1.y;
		double p1p2x = p2.x - p1.x;
		double p1p2y = p2.y - p1.y;

		double dot = p1px * p1p2x + p1py * p1p2y;
		double norm = p1p2x * p1p2x + p1p2y * p1p2y;

		double param = 0;
		if (norm > 0) param = std::max<double>(std::min<double>(1.0,dot/norm),0.0);

		double dx = p.x - (p1.x + param * p1p2x);
		double dy = p.y - (p1.y + param * p1p2y);
		return sqrt(dx * dx + dy * dy);
	}
};

// JSON output
void to_json(json& j, const Point& p);

#endif
