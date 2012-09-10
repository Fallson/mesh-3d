///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: Enriched_polyhedron                                           //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#ifndef POLYGON_MESH_H
#define POLYGON_MESH_H

#include "config.h"
#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <list>
#include <vector>
#include <string>
#include "uglyfont.h"
#include "stringutils.h"

// tag for processhits
struct processhits_normal{};
struct processhits_plus{};
struct processhits_minus{};


// a refined facet with a normal and a tag
template <class Refs, class T, class P, class Norm>
class Enriched_facet : public CGAL::HalfedgeDS_face_base<Refs, T>
{
	int m_id;

	// tag
	int m_tag; 

	// normal
	Norm m_normal;

	bool m_selected;

public:

	Enriched_facet()
	{
		m_id = -1;
		m_tag = -1;
		m_normal = CGAL::NULL_VECTOR;
		m_selected = false;
	}

	int& id() {return m_id;}
	int id() const {return m_id; }

	const int&  tag() const { return m_tag; }
	void tag(const int& t)  { m_tag = t; }

	// normal
	typedef Norm Normal_3;
	Normal_3& normal() { return m_normal; }
	const Normal_3& normal() const { return m_normal; }

	// selected
	const bool& selected() const{ return m_selected; }
	void selected(bool sel) { m_selected = sel; }
};

// a refined halfedge with a general tag and 
// a binary tag to indicate wether it belongs 
// to the control mesh or not
template <class Refs, class Tprev, class Tvertex, class Tface, class Norm>
class Enriched_halfedge : public CGAL::HalfedgeDS_halfedge_base<Refs,Tprev,Tvertex,Tface>
{
private:
	int m_id;

	// tag
	int m_tag; 

	// option for edge superimposing
	bool m_control_edge; 

public:

	// life cycle
	Enriched_halfedge()
	{
		m_id = 0;
		m_tag = -1;
		m_control_edge = true;
	}

	int& id() {return m_id;}
	int id() const {return m_id; }

	// tag
	const int& tag() const { return m_tag;  }
	int& tag() { return m_tag;  }
	void tag(const int& t)  { m_tag = t; }

	// control edge 
	bool& control_edge()  { return m_control_edge; }
	const bool& control_edge()  const { return m_control_edge; }
	void control_edge(const bool& flag) { m_control_edge  = flag; }
};

// a refined vertex with a normal and a tag
template <class Refs, class T, class P, class Norm>
class Enriched_vertex : public CGAL::HalfedgeDS_vertex_base<Refs, T, P>
{
	int m_id;

	bool m_dirty;

	// tag
	int m_tag; 

	// normal
	Norm m_normal;

	Norm m_lap;

public:
	Enriched_vertex()  
	{
		m_id = -1;
		m_dirty = false;
		m_tag = -1;
		m_normal = CGAL::NULL_VECTOR;
		m_lap = CGAL::NULL_VECTOR;
	}

	Enriched_vertex(const P& pt)
		: CGAL::HalfedgeDS_vertex_base<Refs, T, P>(pt)
	{
		m_id = -1;
		m_dirty = false;
		m_tag = -1;
		m_normal = CGAL::NULL_VECTOR;
		m_lap = CGAL::NULL_VECTOR;
	}

	int& id() {return m_id;}
	int id() const {return m_id; }

	//dirty
	const bool& dirty() const { return m_dirty; }
	bool& dirty() { return m_dirty; }
	void dirty(bool d){ m_dirty = d; }

	// normal
	typedef Norm Normal_3;
	Normal_3& normal() { return m_normal; }
	const Normal_3& normal() const { return m_normal; }

	// tag
	int& tag() {  return m_tag; }
	const int& tag() const {  return m_tag; }
	void tag(const int& t)  { m_tag = t; }

	//lap
	Normal_3& lap() { return m_lap; }
	const Normal_3& lap() const { return m_lap; }
};

// A redefined items class for the Polyhedron_3 
// with a refined vertex class that contains a 
// member for the normal vector and a refined
// facet with a normal vector instead of the 
// plane equation (this is an alternative 
// solution instead of using 
// Polyhedron_traits_with_normals_3).
struct Enriched_items : public CGAL::Polyhedron_items_3
{
    // wrap vertex
    template <class Refs, class Traits>
    struct Vertex_wrapper
    {
        typedef typename Traits::Point_3  Point;
        typedef typename Traits::Vector_3 Normal;
        typedef Enriched_vertex<Refs,
                          CGAL::Tag_true,
                          Point,
                          Normal> Vertex;
    };

    // wrap face
    template <class Refs, class Traits>
    struct Face_wrapper
    {
        typedef typename Traits::Point_3  Point;
        typedef typename Traits::Vector_3 Normal;
        typedef Enriched_facet<Refs,
                         CGAL::Tag_true,
                         Point,
                         Normal> Face;
    };

    // wrap halfedge
    template <class Refs, class Traits>
    struct Halfedge_wrapper
    {
        typedef typename Traits::Vector_3 Normal;
        typedef Enriched_halfedge<Refs,
                            CGAL::Tag_true,
                            CGAL::Tag_true,
                            CGAL::Tag_true,
                            Normal> Halfedge;
    };
};


//*********************************************************
template <class kernel, class items>
class Enriched_polyhedron : public CGAL::Polyhedron_3<kernel,items>
{
public :
	typedef typename kernel::FT FT;
	typedef typename kernel::Point_3 Point;
	typedef typename kernel::Vector_3 Vector;
	typedef typename kernel::Iso_cuboid_3 Iso_cuboid;

public :
	Enriched_polyhedron() 
	{
		m_pure_quad = false;
		m_pure_triangle = false;
	}
	virtual ~Enriched_polyhedron() 
	{
	}

public:
	void compute_normals()
	{
		compute_normals_per_facet();
		compute_normals_per_vertex();
	}

	void compute_bounding_box()
	{
		if(size_of_vertices() == 0)
		{
			CGAL_assertion(false);
			return;
		}

		FT xmin,xmax,ymin,ymax,zmin,zmax;
		Vertex_iterator pVertex = vertices_begin();
		xmin = xmax = pVertex->point().x();
		ymin = ymax = pVertex->point().y();
		zmin = zmax = pVertex->point().z();
		for(;
			pVertex !=  vertices_end();
			pVertex++)
		{
			const Point& p = pVertex->point();

			xmin =  std::min(xmin,p.x());
			ymin =  std::min(ymin,p.y());
			zmin =  std::min(zmin,p.z());

			xmax =  std::max(xmax,p.x());
			ymax =  std::max(ymax,p.y());
			zmax =  std::max(zmax,p.z());
		}
		m_bbox = Iso_cuboid(xmin,ymin,zmin,
			xmax,ymax,zmax);
	}

	void copy_bounding_box(Enriched_polyhedron<kernel,items> *pMesh)
	{
		m_bbox = pMesh->bbox();
	}

	FT xmin() { return m_bbox.xmin(); }
	FT xmax() { return m_bbox.xmax(); }
	FT ymin() { return m_bbox.ymin(); }
	FT ymax() { return m_bbox.ymax(); }
	FT zmin() { return m_bbox.zmin(); }
	FT zmax() { return m_bbox.zmax(); }

	Iso_cuboid& bbox() { return m_bbox; }
	const Iso_cuboid bbox() const { return m_bbox; }

	void compute_type()
	{
		m_pure_quad = is_pure_degree(4);
		m_pure_triangle = is_pure_degree(3);
	}

	bool is_pure_triangle() { return m_pure_triangle; }
	bool is_pure_quad() { return m_pure_quad; }

	// degree of a face
	static unsigned int degree(Facet_handle pFace)
	{
		return CGAL::circulator_size(pFace->facet_begin());    
	}

	// valence of a vertex
	static unsigned int valence(Vertex_handle pVertex)
	{
		return CGAL::circulator_size(pVertex->vertex_begin());
	}

	// check wether a vertex is on a boundary or not
	static bool is_border(Vertex_handle pVertex)
	{
		Halfedge_around_vertex_circulator pHalfEdge = pVertex->vertex_begin();
		if(pHalfEdge == NULL) // isolated vertex
			return true;
		Halfedge_around_vertex_circulator d = pHalfEdge;
		CGAL_For_all(pHalfEdge,d)
			if(pHalfEdge->is_border())
				return true;
		return false;
	}

	// get any border halfedge attached to a vertex
	Halfedge_handle get_border_halfedge(Vertex_handle pVertex)
	{
		Halfedge_around_vertex_circulator pHalfEdge = pVertex->vertex_begin();
		Halfedge_around_vertex_circulator d = pHalfEdge;
		CGAL_For_all(pHalfEdge,d)
			if(pHalfEdge->is_border())
				return pHalfEdge;
		return NULL;
	}

	void set_index_facets()
	{
		int index = 0;
		for(Facet_iterator pFacet = facets_begin();
			pFacet != facets_end();
			pFacet++)
			pFacet->tag(index++);
	}

	void set_index_vertices()
	{
		int index = 0;
		for(Vertex_iterator pVertex = vertices_begin();
			pVertex != vertices_end();
			pVertex++)
			pVertex->tag(index++);
	}

	// tag all halfedges
	void tag_halfedges(const int tag)
	{
		for(Halfedge_iterator pHalfedge = halfedges_begin();
			pHalfedge != halfedges_end();
			pHalfedge++)
			pHalfedge->tag(tag);
	}

	// tag all facets
	void tag_facets(const int tag)
	{
		for(Facet_iterator pFacet = facets_begin();
			pFacet  != facets_end();
			pFacet++)
			pFacet->tag(tag);
	}

	// compute facet center
	void compute_facet_center(Facet_handle pFace,Point& center)
	{
		Halfedge_around_facet_circulator pHalfEdge = pFace->facet_begin();
		Halfedge_around_facet_circulator end = pHalfEdge;
		Vector vec(0.0,0.0,0.0);
		int degree = 0;
		CGAL_For_all(pHalfEdge,end)
		{
			vec = vec + (pHalfEdge->vertex()->point()-CGAL::ORIGIN);
			degree++;
		}
		center = CGAL::ORIGIN + (vec/(kernel::FT)degree);
	}

	unsigned int nb_boundaries()
	{
		unsigned int nb = 0;
		tag_halfedges(0);
		for(Halfedge_iterator he = halfedges_begin();
			he != halfedges_end();
			he++)
		{
			if(he->is_border() && he->tag() == 0)
			{
				nb++;
				Halfedge_handle curr = he;
				do
				{
					curr  = curr->next();
					curr->tag(1);
				}
				while(curr != he);
			}
		}
		return nb;
	}

	void tag_component(Facet_handle pSeedFacet,const int tag_free,const int tag_done)
	{
		pSeedFacet->tag(tag_done);
		std::list<Facet_handle> facets;
		facets.push_front(pSeedFacet);
		while(!facets.empty())
		{
			Facet_handle pFacet = facets.front();
			facets.pop_front();
			pFacet->tag(tag_done);
			Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
			Halfedge_around_facet_circulator end = pHalfedge;
			CGAL_For_all(pHalfedge,end)
			{
				Facet_handle pNFacet = pHalfedge->opposite()->facet();
				if(pNFacet != NULL && pNFacet->tag() == tag_free)
				{
					facets.push_front(pNFacet);
					pNFacet->tag(tag_done);
				}
			}
		}
	}

	unsigned int nb_components()
	{
		unsigned int nb = 0;
		tag_facets(0);
		for(Facet_iterator pFacet = facets_begin();
			pFacet != facets_end();
			pFacet++)
		{
			if(pFacet->tag() == 0)
			{
				nb++;
				tag_component(pFacet,0,1);
			}
		}
		return nb;
	}

	// compute the genus
	// V - E + F + B = 2 (C - G)
	// C -> #connected components
	// G : genus
	// B : #boundaries
	int genus()
	{
		int c = nb_components();
		int b = nb_boundaries();
		int v = size_of_vertices();
		int e = size_of_halfedges()/2;
		int f = size_of_facets();

		return (2*c+e-b-f-v)/2;
	}

	/************************************************************************/
	/* file io                                                              */
	/************************************************************************/
	void write_obj(const char *pFilename,int incr  = 1) // 1-based by default
	{
		std::ofstream stream(pFilename);

		// output vertices
		for(Point_iterator pPoint = points_begin();
			pPoint != points_end(); 
			pPoint++) 
			stream << 'v' << ' ' << pPoint->x() << ' ' <<
			pPoint->y() << ' ' <<
			pPoint->z() << std::endl;

		// precompute vertex indices
		this->set_index_vertices(); 

		// output facets
		for(Facet_iterator pFacet = facets_begin();
			pFacet != facets_end(); 
			pFacet++) 
		{
			stream << 'f';
			Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
			do 
			stream << ' ' << pHalfedge->vertex()->tag()+incr;
			while(++pHalfedge != pFacet->facet_begin());
			stream << std::endl;
		}  
	}

	bool euler_split_facet()
	{
		bool retVal = false;
		for(Facet_iterator pFacet = facets_begin(); pFacet != facets_end(); ++pFacet)
		{
			if(pFacet->selected())
			{
				Halfedge_handle h = pFacet->halfedge();
				Halfedge_handle g = h->next()->next();
				//CGAL_precondition(h!=g&&h->next()!=g&&g->next()!=h);
				if( h == g || h->next() == g || g->next() == h)
					continue;

				Halfedge_handle newhe = split_facet(h,g);
				newhe->facet()->selected(false);
				newhe->opposite()->facet()->selected(false);
				retVal = true;
			}
		}
		return retVal;
	}

	bool euler_join_facet()
	{
		bool retVal = false;
		for(Facet_iterator pFacet = facets_begin(); pFacet != facets_end(); ++pFacet)
		{
			if(pFacet->selected())
			{
				Halfedge_handle h = pFacet->halfedge();
				Vertex_handle v1 = h->vertex();
				Vertex_handle v2 = h->opposite()->vertex();

				if(valence(v1) < 3 || valence(v2) < 3)
					continue;

				Halfedge_handle new_he = join_facet(h);
				new_he->facet()->selected(false);

				retVal = true;
			}
		}
		return retVal;
	}

	bool euler_create_center_vertex()
	{
		bool retVal = false;
		for(Facet_iterator pFacet = facets_begin(); pFacet != facets_end(); ++pFacet)
		{
			if(pFacet->selected())
			{
				if(pFacet->halfedge()->is_border())
					continue;

				Vector vec( 0.0, 0.0, 0.0);
				std::size_t order = 0;
				Halfedge_around_facet_circulator h = pFacet->facet_begin();
				do {
					vec = vec + ( h->vertex()->point() - CGAL::ORIGIN);
					++ order;
				} while ( ++h != pFacet->facet_begin());
				CGAL_assertion( order >= 3); // guaranteed by definition of Polyhedron
				Point center =  CGAL::ORIGIN + (vec / (kernel::FT)order);
				Halfedge_handle new_center = create_center_vertex( pFacet->halfedge());
				new_center->vertex()->point() = center;

				Vertex_handle v = new_center->vertex();
				Halfedge_around_vertex_circulator hv = v->vertex_begin();
				do
				{
					hv->facet()->selected(false);
				}while( ++hv != v->vertex_begin());

				retVal = true;
			}
		}
		return retVal;
	}

	/************************************************************************/
	/* opengl part                                                          */
	/************************************************************************/
	void gl_draw(bool smooth_shading, bool use_normals)
	{
		int i = 0;
		Facet_iterator pFacet = facets_begin();
		for(;pFacet != facets_end();pFacet++)
		{
			glLoadName(i);
			glBegin(GL_POLYGON);
			gl_draw_facet(pFacet,smooth_shading,use_normals);
			glEnd();
			i++;
		}
		glFlush();
	}

	void gl_draw_number()
	{
		std::vector<Facet_handle> facets;

		Facet_iterator pFacet = facets_begin();
		for(;pFacet != facets_end();pFacet++)
		{
			if(pFacet->selected())
			{
				gl_draw_facet_tag(pFacet);
				facets.push_back(pFacet);
			}
		}
		glFlush();

		//set the dirty flag
		std::vector<Facet_handle>::iterator f = facets.begin();
		std::vector<Facet_handle>::iterator last_f = facets.end();
		for(;f != last_f; ++f)
		{
			Halfedge_around_facet_circulator pHalfedge = (*f)->facet_begin();
			do
			{
				Vertex_handle v = pHalfedge->vertex();
				v->dirty(false);
			}
			while(++pHalfedge != (*f)->facet_begin());
		}
	}

	void gl_draw_selectedfaces()
	{
		Facet_iterator pFacet = facets_begin();
		for(;pFacet != facets_end();pFacet++)
		{
			if(pFacet->selected())
			{
				glBegin(GL_POLYGON);
				gl_draw_facet(pFacet, false , false);
				glEnd();
			}
		}
		glFlush();
	}

	void gl_draw_lines()
	{
		glBegin(GL_LINES);
		for(Edge_iterator h = edges_begin();
			h != edges_end();
			h++)
		{
			const Point& p1 = h->prev()->vertex()->point();
			const Point& p2 = h->vertex()->point();
			glVertex3f(p1[0],p1[1],p1[2]);
			glVertex3f(p2[0],p2[1],p2[2]);
		}
		glEnd();
	}

	void gl_draw_points()
	{
		glBegin(GL_POINTS);
		for(Point_iterator pPoint = points_begin();
			pPoint != points_end();
			pPoint++)
			glVertex3f(pPoint->x(),pPoint->y(),pPoint->z());
		glEnd();
	}

	void gl_draw_bounding_box()
	{
		glBegin(GL_LINES);

		// along x axis
		glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmin());
		glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmin());
		glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmax());
		glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmax());
		glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmin());
		glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmin());
		glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmax());
		glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmax());

		// along y axis
		glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmin());
		glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmin());
		glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmax());
		glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmax());
		glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmin());
		glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmin());
		glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmax());
		glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmax());

		// along z axis
		glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmin());
		glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmax());
		glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmin());
		glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmax());
		glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmin());
		glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmax());
		glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmin());
		glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmax());

		glEnd();
	}

	//process hits
	void gl_processhits(GLint hits, GLuint buffer[], processhits_normal)
	{
		if(hits == 0)
			return;

		if(false) //legacy code
		{
			clear_selectedfaces();
			for (int i = 0; i < hits; i++)
			{
				int names = buffer[i*4];
				CGAL_assertion(names == 1);

				int faceidx = buffer[i*4 + 3];
				Facet_iterator pFacet = facets_begin();
				advance(pFacet,faceidx);
				pFacet->selected(true);
			}
		}
		else
		{
			size_t facesz = size_of_facets();
			int* bitmark = new int[facesz];
			memset(bitmark,0,sizeof(int)*facesz);

			for (int i = 0; i < hits; i++)
			{
				int names = buffer[i*4];
				CGAL_assertion(names == 1);

				int faceidx = buffer[i*4 + 3];
				bitmark[faceidx] = 1;
			}

			Facet_iterator pFacet = facets_begin();
			for(int i = 0;pFacet != facets_end();pFacet++,i++)
			{
				pFacet->selected(false);
				if(bitmark[i] == 1)
					pFacet->selected(true);
			}

			CGALQT_DELETE_ARRAY(bitmark);
		}	
	}

	void gl_processhits(GLint hits, GLuint buffer[], processhits_plus)
	{
		if(hits == 0)
			return;

		size_t facesz = size_of_facets();
		int* bitmark = new int[facesz];
		memset(bitmark,0,sizeof(int)*facesz);

		for (int i = 0; i < hits; i++)
		{
			int names = buffer[i*4];
			CGAL_assertion(names == 1);

			int faceidx = buffer[i*4 + 3];
			bitmark[faceidx] = 1;
		}

		Facet_iterator pFacet = facets_begin();
		for(int i = 0;pFacet != facets_end();pFacet++,i++)
		{
			if(bitmark[i] == 1)
				pFacet->selected(true);
		}

		CGALQT_DELETE_ARRAY(bitmark);
	}

	void gl_processhits(GLint hits, GLuint buffer[], processhits_minus)
	{
		if(hits == 0)
			return;

		size_t facesz = size_of_facets();
		int* bitmark = new int[facesz];
		memset(bitmark,0,sizeof(int)*facesz);

		for (int i = 0; i < hits; i++)
		{
			int names = buffer[i*4];
			CGAL_assertion(names == 1);

			int faceidx = buffer[i*4 + 3];
			bitmark[faceidx] = 1;
		}

		Facet_iterator pFacet = facets_begin();
		for(int i = 0;pFacet != facets_end();pFacet++,i++)
		{
			if(bitmark[i] == 1)
				pFacet->selected(!pFacet->selected());
		}

		CGALQT_DELETE_ARRAY(bitmark);
	}

private:
	void clear_selectedfaces(void)
	{
		Facet_iterator pFacet = facets_begin();
		for(;pFacet != facets_end();pFacet++)
		{
			pFacet->selected(false);
		}
	}

	void compute_normals_per_facet()
	{
		std::for_each(facets_begin(),facets_end(),Facet_normal());
	}
	void compute_normals_per_vertex()
	{
		std::for_each(vertices_begin(),vertices_end(),Vertex_normal());
	}

	bool is_pure_degree(unsigned int d)
	{
		for(Facet_iterator pFace  = facets_begin();
			pFace != facets_end();
			pFace++)
			if(degree(pFace) != d)
				return false;
		return true;
	}

	// compute average edge length around a vertex
	FT average_edge_length_around(Vertex_handle pVertex)
	{
		FT sum = 0.0;
		Halfedge_around_vertex_circulator pHalfEdge = pVertex->vertex_begin();
		Halfedge_around_vertex_circulator end = pHalfEdge;
		Vector vec(0.0,0.0,0.0);
		int degree = 0;
		CGAL_For_all(pHalfEdge,end)
		{
			Vector vec = pHalfEdge->vertex()->point()-
				pHalfEdge->opposite()->vertex()->point();
			sum += std::sqrt(vec*vec);
			degree++;
		}
		return sum / (FT) degree;
	}

	/************************************************************************/
	/* opengl part                                                          */
	/************************************************************************/
	void gl_draw_facet_tag(Facet_handle pFacet)
	{
		Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
		//const Point& point  = pHalfedge->vertex()->point();
		static float scale_ratio = -1.0;
		if(scale_ratio < 0.0)
			//scale_ratio = (fabsf(point[0])+fabsf(point[1])+fabsf(point[2]))/(3.0*4);
			scale_ratio = average_edge_length_around(pHalfedge->vertex())/3.0;
		do
		{
			Vertex_handle v = pHalfedge->vertex();
			if(v->dirty())
				continue;
			v->dirty() = true;
			int tag  = v->tag();
			std::string tagstr = StringUtils::to_string(tag);
			const Point& point  = v->point();

			glPushMatrix();
			glTranslatef(point[0],point[1],point[2]);
			glScalef(scale_ratio,scale_ratio,scale_ratio);
			YsDrawUglyFont(tagstr.c_str(),0);
			glPopMatrix();
		}
		while(++pHalfedge != pFacet->facet_begin());
	}

	void gl_draw_facet(Facet_handle pFacet, bool smooth_shading,bool use_normals)
	{
		// one normal per face
		if(use_normals && !smooth_shading)
		{
			const Facet::Normal_3& normal = pFacet->normal();
			glNormal3f(normal[0],normal[1],normal[2]);
		}

		// revolve around current face to get vertices
		Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
		do
		{
			// one normal per vertex
			if(use_normals && smooth_shading)
			{
				const Facet::Normal_3& normal = pHalfedge->vertex()->normal();
				glNormal3f(normal[0],normal[1],normal[2]);
			}

			// polygon assembly is performed per vertex
			const Point& point  = pHalfedge->vertex()->point();
			glVertex3d(point[0],point[1],point[2]);
		}
		while(++pHalfedge != pFacet->facet_begin());
	}


	
#if 0
	/************************************************************************/
	/* legacy opengl code                                                  */
	/************************************************************************/
	// superimpose edges
	void superimpose_edges(bool skip_ordinary_edges = true,
		bool skip_control_edges = false,
		bool voronoi_edge = false)
	{
		glBegin(GL_LINES);
		for(Edge_iterator h = edges_begin();
			h != edges_end();
			h++)
		{
			// ignore this edges
			if(skip_ordinary_edges && !h->control_edge())
				continue;

			// ignore control edges
			if(skip_control_edges && h->control_edge())
				continue;

			if(voronoi_edge)
			{
				Facet_handle pFace1 = h->facet();
				Facet_handle pFace2 = h->opposite()->facet();
				if(pFace1 == NULL || pFace2 == NULL)
					continue;

				const Point &p1 = h->vertex()->point();
				const Point &p2 = h->next()->vertex()->point();
				const Point &p3 = h->next()->next()->vertex()->point();

				kernel k;
				Point d1 = k.construct_circumcenter_3_object()(p1,p2,p3);
				glVertex3f(d1[0],d1[1],d1[2]);

				const Point &pp1 = h->opposite()->vertex()->point();
				const Point &pp2 = h->opposite()->next()->vertex()->point();
				const Point &pp3 = h->opposite()->next()->next()->vertex()->point();
				Point d2 = k.construct_circumcenter_3_object()(pp1,pp2,pp3);
				glVertex3f(d2[0],d2[1],d2[2]);
			}
			else
			{
				// assembly and draw line segment
				const Point& p1 = h->prev()->vertex()->point();
				const Point& p2 = h->vertex()->point();
				glVertex3f(p1[0],p1[1],p1[2]);
				glVertex3f(p2[0],p2[1],p2[2]);
			}
		}
		glEnd();
	}

	// superimpose vertices
	void superimpose_vertices()
	{
		glBegin(GL_POINTS);
		for(Point_iterator pPoint = points_begin();
			pPoint != points_end();
			pPoint++)
			glVertex3f(pPoint->x(),pPoint->y(),pPoint->z());
		glEnd(); // // end point assembly
	}

	// superimpose vertices
	void superimpose_spheres(double scale)
	{
		GLUquadricObj* pQuadric = gluNewQuadric();
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

		for(Vertex_iterator pVertex = vertices_begin();
			pVertex !=  vertices_end();
			pVertex++)
		{
			glPushMatrix();
			double radius = average_edge_length_around(pVertex);
			glTranslatef(pVertex->point().x(),
				pVertex->point().y(),
				pVertex->point().z());
			gluSphere(pQuadric,scale*radius,24,24); 
			glPopMatrix();
		}
		gluDeleteQuadric(pQuadric);
	}
#endif

private :
	Iso_cuboid m_bbox;

	// type
	bool m_pure_quad;
	bool m_pure_triangle;
};

// compute facet normal 
struct Facet_normal // (functor)
{
  template <class Facet>
  void operator()(Facet& f)
  {
    typename Facet::Normal_3 sum = CGAL::NULL_VECTOR;
    typename Facet::Halfedge_around_facet_circulator h = f.facet_begin();
    do
    {
      typename Facet::Normal_3 normal = CGAL::cross_product(
        h->next()->vertex()->point() - h->vertex()->point(),
        h->next()->next()->vertex()->point() - h->next()->vertex()->point());
      double sqnorm = normal * normal;
      if(sqnorm != 0)
        normal = normal / (float)std::sqrt(sqnorm);
      sum = sum + normal;
    }
    while(++h != f.facet_begin());
    float sqnorm = sum * sum;
    if(sqnorm != 0.0)
      f.normal() = sum / std::sqrt(sqnorm);
    else
    {
      f.normal() = CGAL::NULL_VECTOR;
      //TRACE("degenerate face\n");
    }
  }
};


// compute vertex normal 
struct Vertex_normal // (functor)
{
    template <class Vertex>
    void operator()(Vertex& v)
    {
        typename Vertex::Normal_3 normal = CGAL::NULL_VECTOR;
        Vertex::Halfedge_around_vertex_const_circulator pHalfedge = v.vertex_begin();
        Vertex::Halfedge_around_vertex_const_circulator begin = pHalfedge;
        CGAL_For_all(pHalfedge,begin) 
          if(!pHalfedge->is_border())
            normal = normal + pHalfedge->facet()->normal();
        float sqnorm = normal * normal;
        if(sqnorm != 0.0f)
          v.normal() = normal / (float)std::sqrt(sqnorm);
        else
          v.normal() = CGAL::NULL_VECTOR;
    }
};


typedef CGAL::Cartesian<double> Enriched_Polyhedron_kernel;
typedef Enriched_polyhedron<Enriched_Polyhedron_kernel,Enriched_items> Polyhedron;

#endif
