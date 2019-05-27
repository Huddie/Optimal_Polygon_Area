#include "poly_helper.hpp"


//-------========== FROM CGAL ============-------//
void poly::mark_domains(CDT& cdt) {
    for(CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it){
        it->info().nesting_level = -1;
    }
    std::list<CDT::Edge> border;
    mark_domains(cdt, cdt.infinite_face(), 0, border);
    while(! border.empty()){
        CDT::Edge e = border.front();
        border.pop_front();
        CDT::Face_handle n = e.first->neighbor(e.second);
        if(n->info().nesting_level == -1){
        mark_domains(cdt, n, e.first->info().nesting_level+1, border);
        }
    }
}

void poly::mark_domains(CDT& ct, 
                CDT::Face_handle start, 
                int index, 
                std::list<CDT::Edge>& border )
    {
    if(start->info().nesting_level != -1){
        return;
    }
    std::list<CDT::Face_handle> queue;
    queue.push_back(start);
    while(! queue.empty()){
        CDT::Face_handle fh = queue.front();
        queue.pop_front();
        if(fh->info().nesting_level == -1){
        fh->info().nesting_level = index;
        for(int i = 0; i < 3; i++){
            CDT::Edge e(fh,i);
            CDT::Face_handle n = fh->neighbor(i);
            if(n->info().nesting_level == -1){
            if(ct.is_constrained(e)) border.push_back(e);
            else queue.push_back(n);
            }
        }
        }
    }
}

//-------========== END FROM CGAL ============-------//


Triangle_WPT poly::shared_edges(Triangle_2 triangle, Polygon_2 polygon) {
    int count = 0;
    Segment_2 seg1, seg2;
    Point_2 p1, p2, p3, p4;
    Polygon_2 poly_triangle;

    poly_triangle.push_back(triangle.vertex(0));
    poly_triangle.push_back(triangle.vertex(1));
    poly_triangle.push_back(triangle.vertex(2));

    for (auto e = poly_triangle.edges_begin(); e != poly_triangle.edges_end(); ++e) {
        if(std::find(polygon.edges_begin(), polygon.edges_end(), *e) != polygon.edges_end()) {
            count++;
            if(count == 0) seg1 = *e;
            else seg2 = *e;
        }
    }

    p1 = seg1.source();
    p2 = seg1.target();
    p3 = seg2.source();
    p4 = seg2.target();
    if(p1 == p3) return Triangle_WPT(poly_triangle, p1);
    if(p1 == p4) return Triangle_WPT(poly_triangle, p1);
    return Triangle_WPT(poly_triangle, p2);

}

bool poly::triangle_shares_2_edges(Triangle_2 triangle, Polygon_2 polygon) {
    int count = 0;
    Segment_2 seg1, seg2;
    Point_2 p1, p2, p3, p4;
    Polygon_2 poly_triangle;

    poly_triangle.push_back(triangle.vertex(0));
    poly_triangle.push_back(triangle.vertex(1));
    poly_triangle.push_back(triangle.vertex(2));

    for(auto e = poly_triangle.edges_begin(); e != poly_triangle.edges_end(); e++) {
        if(std::find(polygon.edges_begin(), polygon.edges_end(), *e) != polygon.edges_end() 
        || std::find(polygon.edges_begin(), polygon.edges_end(), (*e).opposite()) != polygon.edges_end()) {
            count++;
            if(count == 0) seg1 = *e;
            else seg2 = *e;
        }
    }
    return count >= 2;
}


Points poly::visabile_points(Point_2 p, Segments segs, Points check_against) {

    Points pts;

    if ( segs.size() <= 0 ) return pts;

    bool visible;
    for(auto pt : check_against) {
        Segment_2 against_seg(p, pt);
        visible = true;
        for(auto seg : segs) {
            if(CGAL::do_intersect(seg, against_seg)) {
                auto res = CGAL::intersection(seg, against_seg);
                const Point_2 exact_pt = boost::get<Point_2>(*res);
                if(exact_pt != pt) {
                    visible = false; 
                    break; 
                }
            } 
        }
        if ( visible ) { pts.push_back(pt); }
    }
    return pts;
}


Pockets poly::calculate_pockets(Polygon_2 polygon) {

    Points hull = find_convex_hull(polygon);
    std::set<Point_2> unordered_hull(
        hull.begin(), 
        hull.end()
    );

    auto poly_it = polygon.vertices_begin();

    Point_2 start, pocket_begin, pocket_end;

    // Find beginning point

    for(; unordered_hull.find(start) != unordered_hull.end(); poly_it++) {}
    start = *poly_it;
    pocket_begin = start;

    std::cout << polygon << "\n";

    // Find Pockets

    Points pts;
    Segments segs;
    Pockets pkts;

    poly_it++;

    do {
        if(poly_it == polygon.vertices_end()) poly_it = polygon.vertices_begin();
        for(; unordered_hull.find(*poly_it) == unordered_hull.end(); poly_it++) {
            if(poly_it == polygon.vertices_end()) poly_it = polygon.vertices_begin();
            if(poly_it+1 == polygon.vertices_end()) segs.push_back(Segment_2(*poly_it, *polygon.vertices_begin()));
            else segs.push_back(Segment_2(*poly_it, *(poly_it+1)));
            pts.push_back(*poly_it);
        }

        Polygon_2 polygon;
        pts.push_back(*poly_it);

        for(auto p : pts) { polygon.push_back(p); }

        pkts.push_back(polygon);

        //for(auto v : visabile_points(pocket_begin, segs, pts)) { std::cout <<  v << "\n"; }
        std::cout << "\n";
        segs.clear();
        pts.clear();
        pocket_begin = *poly_it;
        poly_it++;
    } while(pocket_begin != start);

    return pkts;
}


Points poly::make_pointset(std::ifstream &stream) {
    Points points, result;
    std::string _s;
    long x, y;
    
    while(std::getline(stream, _s)) {
        std::istringstream iss(_s);
        iss >> x >> x >> y; 
        points.push_back(Point_2(x, y));
    }
    sort( points.begin(), points.end() );
    points.erase( unique( points.begin(), points.end() ), points.end() );
    return points;
}

Points poly::peel(Points pts) {
    Points hull = find_convex_hull(pts);
    Points result;
    std::set_difference(
        pts.begin(), pts.end(), hull.begin(), hull.end(), 
        std::inserter(result, result.begin())
    );
    return result;
}

Points poly::find_convex_hull(Polygon_2 polygon) {
    Points result;
    CGAL::convex_hull_2( polygon.vertices_begin() , polygon.vertices_end(), std::back_inserter(result) );
    return result;
}

Points poly::find_convex_hull(Points pts) {
    Points result;
    CGAL::convex_hull_2( pts.begin(), pts.end(), std::back_inserter(result) );
    return result;
}


Polygon_2 poly::fekete_half_givenPt(Points pts, Points hull, size_t index) {

    Polygon_2 polygon;
    Points tracked, diff;
    Point_2 comp;

    polygon.push_back(hull[index]);
    polygon.push_back(hull[(index + 1) % hull.size()]);

    // std::cout << polygon << "\n";

    tracked.push_back(*(polygon.vertices_begin()));
    tracked.push_back(*(polygon.vertices_begin() + 1));

    std::sort(
        pts.begin(),
        pts.end()
    );

    std::sort(
        tracked.begin(),
        tracked.end() 
    );

    std::set_difference(
        pts.begin(), 
        pts.end(), 
        tracked.begin(),
        tracked.end(),
        std::inserter(diff, diff.begin())
    );

    std::sort(
        diff.begin(),
        diff.end(), 
        poly::slopeCompare(polygon[0], polygon[1])
    );
    
    for(auto i : diff) { polygon.push_back(i); }

    return polygon;
}

Polygon_2 poly::fekete_half(Points pts) {
    Points hull = poly::find_convex_hull(pts);
    return poly::fekete_half_givenPt(pts, hull, 0);
}

Triangles poly::find_viable_triangles(Polygon_2 pockets) {
    CDT cdt;
    cdt.insert_constraint(pockets.vertices_begin(), pockets.vertices_end(), true);

    mark_domains(cdt);
    Triangles tri;
    int count=0;
    for (CDT::Finite_faces_iterator fit=cdt.finite_faces_begin();
                                    fit!=cdt.finite_faces_end();++fit)
    {
        if ( fit->info().in_domain() ) {
            ++count;
            Triangle_2 triangle = cdt.triangle(fit);
            if(triangle_shares_2_edges(triangle, pockets)) {
                tri.push_back(shared_edges(triangle, pockets));
            }
        }
    }
    return tri;
}

Swap_Edges poly::find_viable_swaps_for_triangle(Triangle_WPT triangle, Polygon_2 polygon) {
    Swap_Edges edges;
    for(auto edge_it = polygon.edges_begin(); edge_it !=  polygon.edges_end(); edge_it++) {
        for(auto edge_it_2 = polygon.edges_begin(); edge_it_2 !=  polygon.edges_end(); edge_it_2++) {
            if(*edge_it_2 != *edge_it) {
                if(CGAL::do_intersect(
                    Segment_2((*edge_it).source(), triangle.pt), 
                    *edge_it_2)
                    && 
                    CGAL::do_intersect(
                    Segment_2((*edge_it).target(), triangle.pt), 
                    *edge_it_2)
                ) {
                    edges.push_back(*edge_it);
                }
            }
        }
    }
    return edges;
}

Polygon_2 poly::fekete_half_best_ch(std::string filename, Points pts) {
    Polygon_2 best_polygon, polygon;
    std::map<Triangle_WPT, Swap_Edges> triangle_swap_map;
    Points hull = poly::find_convex_hull(pts);

    int count = 0, not_simple;
    for(auto pt : hull) {
        polygon = poly::fekete_half_givenPt(pts, hull, count++);

        // assert(polygon.is_simple() && "Polygon must be simple");

        if (polygon.is_simple()) {
            if (polygon.area() > best_polygon.area()) {
                best_polygon = polygon;
            }
            std::cout << "Simple: " << filename << "\n";
            Pockets pkts = calculate_pockets(polygon);
            Triangles tris;
            Swap_Edges edges;
            for(Polygon_2 pkt : pkts) {
                tris = find_viable_triangles(pkt);
                for(Triangle_WPT tri : tris) {
                    std::cout << tri.triangle << "\n";
                    edges = poly::find_viable_swaps_for_triangle(tri, polygon);
                    triangle_swap_map.insert(std::pair<Triangle_WPT, Swap_Edges>(tri, edges));
                }
            }
        }
        else {
            std::cout << "Not Simple: " << filename << "\n";
        }
    }

    return best_polygon;
}

Polygon_2 poly::fekete_half_best_all(Points pts) {}




