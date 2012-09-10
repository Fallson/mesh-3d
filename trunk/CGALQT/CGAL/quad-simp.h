#ifndef QUADSIMP_H
#define QUADSIMP_H

#include "config.h"
#include "Enriched_polyhedron.h"

#include <CGAL/Surface_mesh_simplification/HalfedgeGraph_Polyhedron_3.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>

template <class Polyhedron,class Kernel>
class CSimplification_quad
{
	typedef typename Kernel::Point_3 Point;
	typedef typename Polyhedron::Halfedge_handle Halfedge_handle ;

  public:
    CSimplification_quad() {}
    ~CSimplification_quad() {}

  public:

    bool simple(Polyhedron &P, int iter = 1, double ratio = 0.5)
    {
      // check for valid polygon mesh
      if(P.size_of_facets() == 0)
        return false;

      // normalize border
      P.normalize_border();

      for(int i=0;i<iter;i++)
      {
        simple(P, ratio);
      }
      return true;
    }

    void simple(Polyhedron& P, double ratio)
    {
		int index = 0 ;

		for( Polyhedron::Halfedge_iterator eb = P.halfedges_begin()
			, ee = P.halfedges_end()
			; eb != ee
			; ++ eb
			) 
			eb->id() = index++;

		index = 0 ;
		for( Polyhedron::Vertex_iterator vb = P.vertices_begin()
			, ve = P.vertices_end()
			; vb != ve
			; ++ vb
			) 
			vb->id() = index++;

		// In this example, the simplification stops when the number of undirected edges
		// drops below 10% of the initial count
		CGAL::Surface_mesh_simplification::Count_ratio_stop_predicate<Polyhedron> stop(ratio);
		//CGAL::Surface_mesh_simplification::Count_stop_predicate<Polyhedron> stop(1000);

		// This the actual call to the simplification algorithm.
		// The surface and stop conditions are mandatory arguments.
		// The index maps are needed because the vertices and edges
		// of this surface lack an "id()" field.
		int r = CGAL::Surface_mesh_simplification::edge_collapse(P,stop);
    }

};

#endif