#include "Point.h"

void to_json(json& j, const Point& p) 
{
	j = json{p.x,p.y};
}
