/* C++ program to check if a given point lies inside a given polygon
   From: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/  */

#include "Polygon.h"

bool onSegment(Point p, Point q, Point r)
{
	return (q.x < max(p.x, r.x) + MY_EPSILON && q.x > min(p.x, r.x) - MY_EPSILON && q.y < max(p.y, r.y) + MY_EPSILON && q.y > min(p.y, r.y) - MY_EPSILON);
}

int orientation(Point p, Point q, Point r)
{
	double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (val > MY_EPSILON) return 1; // clockwise
	else if (val < - MY_EPSILON) return 2; // counter clockwise 
	else return 0;  // colinear 
}

bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false;
}

bool isInside(vector <Point> & polygon, Point p)
{
	// Create a point for line segment from p to infinite 
	Point extreme = { 10000., p.y };

	// Count intersections of the above line with sides of polygon 
	int count = 0;
	int i = 0;
	do
	{
		int next = (i + 1) % polygon.size();

		// Check if the line segment from 'p' to 'extreme' intersects with the line segment from 'polygon[i]' to 'polygon[next]' 
		if (doIntersect(polygon[i], polygon[next], p, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next', 
			// then check if it lies on segment. If it lies, return true, 
			// otherwise false 
			if (orientation(polygon[i], p, polygon[next]) == 0)
				return onSegment(polygon[i], p, polygon[next]);
			count++;
		}
		i = next;
	} while (i != 0);

	// Return true if count is odd, false otherwise 
	return (count % 2 == 1);
}
