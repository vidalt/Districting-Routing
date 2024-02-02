#include "District.h"
#include "Block.h"
#include "Params.h"
#include "./miniball/Miniball.hpp"
#include <math.h>

typedef double mytype;            // coordinate type

typedef mytype* const* PointIterator; 
typedef const mytype* CoordIterator;

typedef Miniball::
    Miniball <Miniball::CoordAccessor<PointIterator, CoordIterator> > 
    MiniballSolver;

double getSmallestEnclosingCircleArea(vector<vector<double>> verticesPoints)
{
    int dimension = 2;
    int numberOfPoints = verticesPoints.size();

    double** points = new double*[numberOfPoints];

    for (int i=0; i<numberOfPoints; ++i) {

        mytype* point = new mytype[dimension];
        
        for (int j=0; j<dimension; ++j) {
            point[j] = verticesPoints[i][j];
        }
        
        points[i]=point;
    }

    MiniballSolver solver (dimension, points, points+numberOfPoints);
    
    double squaredRadius = solver.squared_radius();
    double pi = M_PI;

    return M_PI*squaredRadius;
}

static double getCompactnessMeasure(District* district, Params params)
{
    int dimension = 2;
    vector<vector<double>> verticesPoints = vector<vector<double> >();

    double totalArea = 0.;

    for (int blockId : district->blocks)
    {
        Block block = params.blocks[blockId];
        
        totalArea += block.area;

        for (Point vertexPoint: block.verticesPoints)
        {
            vector<double> vertice = {vertexPoint.x, vertexPoint.y};
            verticesPoints.push_back(vertice);
        }
    }

    double minimumEnclosingCircleArea = getSmallestEnclosingCircleArea(verticesPoints);
    double compactness = totalArea/minimumEnclosingCircleArea;

    return compactness;
  
}