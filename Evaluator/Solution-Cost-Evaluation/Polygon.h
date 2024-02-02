/* C++ program to check if a given point lies inside a given polygon
   From public domain: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/  */

#ifndef POLYGON_H
#define POLYGON_H

#define MY_EPSILON 0.00001
#include "Point.h"
#include <vector>
using namespace std;

// Given three colinear points p, q, r, the function checks if point q lies on line segment 'pr' 
bool onSegment(Point p, Point q, Point r);

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(Point p, Point q, Point r);

// The function that returns true if line segment 'p1q1' and 'p2q2' intersect. 
bool doIntersect(Point p1, Point q1, Point p2, Point q2);

// Returns true if the point p lies inside the polygon[] with n vertices 
bool isInside(vector <Point> & polygon, Point p);

#endif
