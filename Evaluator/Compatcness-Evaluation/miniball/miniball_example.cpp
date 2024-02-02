//    Copright (C) 1999-2013, Bernd Gaertner
//    $Rev: 3581 $
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Contact:
//    --------
//    Bernd Gaertner
//    Institute of Theoretical Computer Science 
//    ETH Zuerich
//    CAB G31.1
//    CH-8092 Zuerich, Switzerland
//    http://www.inf.ethz.ch/personal/gaertner

// =========================================================================
// generates a set of random points, computes their smallest enclosing ball,
// and outputs the characteristics of this ball
// 
// usage: miniball_example [seed]
// =========================================================================

#include <cstdlib>
#include <iostream>
#include "Miniball.hpp"

int main (int argc, char* argv[])
{
  typedef double mytype;            // coordinate type

  int             d = 5;            // dimension
  int             n = 1000000;      // number of points

  double seed;                      // initialize random number generator
  if (argc != 2) {
    seed = 0;
  } else
    seed = std::atoi(argv[1]);
  std::srand (seed);

  // generate random points and store them in a 2-d array
  // ----------------------------------------------------
  mytype** ap = new mytype*[n];
  for (int i=0; i<n; ++i) {
    mytype* p = new mytype[d];
    for (int j=0; j<d; ++j) {
      p[j] = rand();
    }
    ap[i]=p;
  }

  // define the types of iterators through the points and their coordinates
  // ----------------------------------------------------------------------
  typedef mytype* const* PointIterator; 
  typedef const mytype* CoordIterator;

  // create an instance of Miniball
  // ------------------------------
  typedef Miniball::
    Miniball <Miniball::CoordAccessor<PointIterator, CoordIterator> > 
    MB;
  MB mb (d, ap, ap+n);
  
  // output results
  // --------------
  // center
  std::cout << "Center:\n  ";
  const mytype* center = mb.center(); 
  for(int i=0; i<d; ++i, ++center)
    std::cout << *center << " ";
  std::cout << std::endl;

  // squared radius
  std::cout << "Squared radius:\n  ";
  std::cout << mb.squared_radius() <<  std::endl;

  // number of support points
  std::cout << "Number of support points:\n  ";
  std::cout << mb.nr_support_points() << std::endl;

  // support points on the boundary determine the smallest enclosing ball
  std::cout << "Support point indices (numbers refer to the input order):\n  ";
  MB::SupportPointIterator it = mb.support_points_begin();
  for (; it != mb.support_points_end(); ++it) {
    std::cout << (*it)-ap << " "; // 0 = first point
  }
  std::cout << std::endl;
  
  // relative error: by how much does the ball fail to contain all points? 
  //                 tiny positive numbers come from roundoff and are ok
  std::cout << "Relative error:\n  ";
  mytype suboptimality;
  std::cout << mb.relative_error (suboptimality) <<  std::endl;
  
  // suboptimality: by how much does the ball fail to be the smallest
  //                enclosing ball of its support points? should be 0 
  //                in most cases, but tiny positive numbers are again ok
  std::cout << "Suboptimality:\n  ";
  std::cout << suboptimality <<  std::endl;

  // validity: the ball is considered valid if the relative error is tiny
  //           (<= 10 times the machine epsilon) and the suboptimality is zero
  std::cout << "Validity:\n  ";
  std::cout << (mb.is_valid() ? "ok" : "possibly invalid") << std::endl;

  // computation time
  std::cout << "Computation time was "<< mb.get_time() << " seconds\n";

  // clean up
  for (int i=0; i<n; ++i)
    delete[] ap[i];
  delete[] ap;
   
  return 0;
}