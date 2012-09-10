#ifndef CGALQT_FALLSON_H
#define CGALQT_FALLSON_H

#include "config.h"
#include "Enriched_polyhedron.h"
#include <CGAL/Taucs_solver_traits.h>
using namespace CGAL;

template <class Polyhedron,class kernel>
class CSubdivider_fallson
{
	typedef typename kernel::FT FT;
	typedef typename kernel::Point_3 Point;
	typedef typename kernel::Vector_3 Vector;
	typedef typename Polyhedron::Vertex                                   Vertex;
	typedef typename Polyhedron::Vertex_iterator                          Vertex_iterator;
	typedef typename Polyhedron::Vertex_handle                            Vertex_handle;

	typedef typename Polyhedron::Halfedge_handle                          Halfedge_handle;

	typedef typename Polyhedron::Facet                                    Facet;
	typedef typename Polyhedron::Facet_iterator                           Facet_iterator;
	typedef typename Polyhedron::Facet_handle                             Facet_handle;

	typedef typename Polyhedron::Halfedge_around_vertex_circulator        HV_circulator;
	typedef typename Polyhedron::Halfedge_around_facet_circulator         HF_circulator;

	typedef typename Vertex::Normal_3                                     Laplacian_Coord;

public:
	CSubdivider_fallson() {}
	~CSubdivider_fallson() {}

private:
	CSubdivider_fallson(const CSubdivider_fallson&);
	CSubdivider_fallson& operator=(const CSubdivider_fallson&);

public:
	bool dosolver()
	{
		Taucs_solver_traits<FT> solver;
		Taucs_matrix<FT> A(4,false);
		Taucs_vector<FT> x(4);
		Taucs_vector<FT> b(4);
		A.set_coef(0,0,1.0);
		A.set_coef(0,2,1.0);
		A.set_coef(0,3,1.0);
		A.set_coef(1,2,1.0);
		A.set_coef(1,3,1.0);
		A.set_coef(2,0,2.0);
		A.set_coef(2,1,3.0);
		A.set_coef(3,0,1.0);
		A.set_coef(3,1,1.0);
		A.set_coef(3,2,1.0);

		b[0] = 4.0;
		b[1] = 3.0;
		b[2] = 2.0;
		b[3] = 2.0;

		FT d;
		solver.linear_solver(A,b,x,d);
		return true;
	}

	//now only for closed triangle mesh
	bool subdivide(Polyhedron &P, int iter)
	{
		if(P.size_of_facets() == 0)
			return false;

		if(!P.is_pure_triangle())
			return false;

		if(!P.is_closed())
			return false;

		for(int i=0;i<iter;i++)
		{
			subdivide(P);
		}
		return true;
	}

	void subdivide(Polyhedron& P)
	{
		P.set_index_vertices();
		compute_laplacian(P);
		create_center_vertex(P);

		std::size_t vsize = P.size_of_vertices();
		Vertex_iterator last_v = P.vertices_end();
		for(Vertex_iterator v = P.vertices_begin(); v != last_v; ++v)
		{
			int tag = v->tag();
		}

		CGAL_postcondition( P.is_valid());
	}

	void compute_laplacian(Polyhedron& P)
	{
		std::for_each(P.vertices_begin(),P.vertices_end(),ComputeLap());
	}

	void create_center_vertex(Polyhedron& P)
	{
		Facet_iterator last_f = P.facets_end();
		--last_f;
		Facet_iterator f = P.facets_begin();
		do
		{
			Laplacian_Coord lap = CGAL::NULL_VECTOR;
			Vector vec = CGAL::NULL_VECTOR;
			std::size_t order = 0;
			HF_circulator h = f->facet_begin();
			do {
				Vertex_handle v = h->vertex();
				lap = lap + ( v->lap() );
				vec = vec + ( h->vertex()->point() - CGAL::ORIGIN);
				++ order;
			} while ( ++h != f->facet_begin());
			CGAL_assertion( order == 3);
			Point center =  CGAL::ORIGIN + (vec/(FT)order);
			Laplacian_Coord center_lap = lap/(FT)order;
			Halfedge_handle new_center = P.create_center_vertex( f->halfedge());
			new_center->vertex()->point() = center;
			new_center->vertex()->tag() = -1;
			new_center->vertex()->lap() = center_lap;
		}while(f++ != last_f);
	}

	struct ComputeLap
	{
		void operator()( Vertex& v )
		{
			std::size_t degree = CGAL::circulator_size(v.vertex_begin());
			Laplacian_Coord lap = CGAL::NULL_VECTOR;
			HV_circulator h = v.vertex_begin();
			do {
				Vertex_handle nv = h->opposite()->vertex();
				lap = lap + ( nv->point() - v.point());
				++ h;
			} while ( h != v.vertex_begin());
			v.lap() = lap/(FT)degree;
		}
	};

};


#endif