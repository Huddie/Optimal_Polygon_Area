
#ifndef poly_helper_hpp
#define poly_helper_hpp

//---------------include-------------------------//
#include <math.h> 
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>
#include <set>
#include <CGAL/Polygon_2.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_set_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>

// #include "bso_rational_nt.h"
#include <CGAL/Cartesian.h>
#include <CGAL/Boolean_set_operations_2.h>

#include "./pocket.hpp" 

//---------------include-end---------------------//

//---------------typedef-------------------------//
typedef Kernel::Point_2 Point_2;
typedef Kernel::Segment_2 Segment_2;
typedef Kernel::Line_2 Line_2;
typedef Kernel::Vector_2 Vector_2;
typedef Kernel::Triangle_2 Triangle_2;
typedef Kernel::Triangle_2 Triangle_2;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef std::vector<Point_2> Points;
typedef std::vector<Segment_2> Segments;
typedef std::vector<Polygon_2> Pockets;
typedef std::vector<Triangle_WPT> Triangles;
typedef std::vector<Segment_2> Swap_Edges;
typedef Polygon_2::Edge_const_iterator EdgeIterator;

typedef CGAL::Polygon_set_2<Kernel, std::vector<Kernel::Point_2>> Polygon_set_2;


struct FaceInfo2 {
    FaceInfo2(){}
    int nesting_level;
    bool in_domain(){ 
        return nesting_level%2 == 1;
    }
};

// Triangualtion

typedef CGAL::Triangulation_vertex_base_2<Kernel>                       Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, Kernel>    Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<Kernel ,Fbb>        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<Kernel, TDS, Itag>  CDT;
typedef CDT::Point                                                Point;


//---------------typedef-end---------------------//


//---------------POLY-------------------------//


namespace poly 
{

    void mark_domains(CDT& ct, 
                CDT::Face_handle start, 
                int index, 
                std::list<CDT::Edge>& border );
    //explore set of facets connected with non constrained edges,
    //and attribute to each such set a nesting level.
    //We start from facets incident to the infinite vertex, with a nesting
    //level of 0. Then we recursively consider the non-explored facets incident 
    //to constrained edges bounding the former set and increase the nesting level by 1.
    //Facets in the domain are those with an odd nesting level.
    void mark_domains(CDT& cdt);

    bool triangle_shares_2_edges(Triangle_2 triangle, Polygon_2 polygon);
    Triangle_WPT shared_edges(Triangle_2 triangle, Polygon_2 polygon);

    struct slopeCompare {
        Point_2 center, anchor;
        slopeCompare(Point_2 a, Point_2 b) : center(a), anchor(b) {}

        bool operator()(Point_2 a, Point_2 b) const { 

            Vector_2 v1(a, center);
            Vector_2 v2(b, center);
            Vector_2 v_anchor(anchor, center);

            double angle_a = compute_angle_2(
                v1, 
                v_anchor
            );

            double angle_b = compute_angle_2(
                v2, 
                v_anchor
            );

            if (a == b) return false;

            if (angle_a == angle_b) {
                //std::cout << a << " " << b << " " << center << "\n";
                return a.x() < b.x();
            }

            return angle_a < angle_b;
     
        }   

        double compute_angle_2(Vector_2 v1, Vector_2 v2) const {
            double dot =  v1 * v2;
            double norm_a = sqrt( pow(v1.x(), 2) + pow(v1.y(), 2) );
            double norm_b = sqrt( pow(v2.x(), 2) + pow(v2.y(), 2) );
            double angle = acos( dot / (norm_a * norm_b) );

            return angle;
        }


        double compute_slope(Point_2 a, Point_2 b) const {
            double dy = a.y() - b.y();
            double dx = a.x() - b.x();
            return dy/dx;
        }
    };


    Points visabile_points(Point_2 p, Segments segs, Points check_against);

    Triangles find_viable_triangles(Polygon_2 pockets);
    Swap_Edges find_viable_swaps_for_triangle(Triangle_WPT triangle, Polygon_2 polygon);

    Pockets calculate_pockets(Polygon_2 polygon);
    Points find_hidden_points(Points pts);
    Points find_convex_hull(Points pts);
    Points find_convex_hull(Polygon_2 polygon);
    Points peel(Points pts);
    Points make_pointset(std::ifstream &stream);


    Polygon_2 fekete_half(Points pts);
    Polygon_2 fekete_half_givenPt(Points pts, Points hull, size_t index);
    Polygon_2 fekete_half_best_ch(std::string filename, Points pts);
    Polygon_2 fekete_half_best_all(Points pts);

}

//---------------POLY end---------------------//


#endif /* poly_helper_hpp */

