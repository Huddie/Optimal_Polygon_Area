#ifndef pocket_hpp
#define pocket_hpp

//---------------include-------------------------//
#include <math.h> 
#include <vector>
#include <iterator>
#include <algorithm>
#include <CGAL/Polygon_2.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Cartesian.h>
//---------------include-end---------------------//

struct Kernel : public CGAL::Cartesian<double> {};
typedef Kernel::Point_2 Point_2;
typedef CGAL::Polygon_2<Kernel> Polygon_2;


struct Triangle_WPT {
    Point_2 pt;
    Polygon_2 triangle;
    Triangle_WPT(Polygon_2 triangle, Point_2 pt) : triangle(triangle),  pt(pt) {}

    bool operator <(const Triangle_WPT& rhs) const
    {
        return this->pt < rhs.pt;
    }
};


#endif /* poly_helper_hpp */