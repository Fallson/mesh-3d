///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: Enriched_polyhedron                                           //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#ifndef POLYGON_2D_H
#define POLYGON_2D_H

#include "config.h"
#include <CGAL/Cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/ch_graham_andrew.h>
#include <fstream>
#include "uglyfont.h"
#include "stringutils.h"

//*********************************************************
template <class kernel>
class Enriched_polygon : public CGAL::Polygon_2<kernel>
{
public :
	typedef typename kernel::FT FT;
	typedef typename kernel::Point_2 Point;
	typedef typename kernel::Vector_2 Vector;

public :
	Enriched_polygon() 
	{
	}
	virtual ~Enriched_polygon()
	{
	}

public:

	/************************************************************************/
	/*
	It's so complicated, a plane sweep algorithm, it's implemented by CGAL and only cost O(n*logn) time
	check P.19 of the book <<computational geometry, algorithms and applications>>, a good book, en:)
	//

	the basic idea is following the definition of simple polygon:
	for segment in polygon
		for other_segment in polygon
			if intersection(segment,other_segment) with nonadjacent vertex
	and the O(n*n) time.
	*/
	/************************************************************************/
	bool is_simple() const
	{
		return CGAL::Polygon_2<kernel>::is_simple();
	}

	/************************************************************************/
	/* 
	the main idea is:
	for vertex in polygon
	  compute the orientation of adjacent vertices
	  if there's exsiting a clockwise and counterclockwise orientation in the polygon
	  then it's concave
	  else it's convex
	*/
	/************************************************************************/
	bool is_convex() const
	{
		return CGAL::Polygon_2<kernel>::is_convex();
	}

	/************************************************************************/
	/* ok, it's simple:
	find leftmost or rightmost or topmost or bottommost vertex
	then the polygon's orientation is the same as the two adjacent vertices orientation.
	Time is only O(n)

	another way is computing the poly area
	if area > 0 then it's counterclockwise
	else it's clockwise
	*/
	/************************************************************************/
	CGAL::Orientation orientation() const
	{
		return CGAL::Polygon_2<kernel>::orientation();
	}


	/************************************************************************/
	/* ok, it's use the classic odd-even ray tracing algorithm
	   check P.239 of the book <<computational geometry in C>>, a good book 2:)

	   The time complexition is O(n)
	*/
	/************************************************************************/
	CGAL::Bounded_side bounded_side( const Point& q )
	{
		return CGAL::Polygon_2<kernel>::bounded_side(q);
	}

	/************************************************************************/
	/*  point in left side of polygon orientation is positive
		      in right side of polygon orientation is negative
			  in boundary is just in boundary: )
	*/
	/************************************************************************/
	CGAL::Oriented_side oriented_side( const Point& q )
	{
		return CGAL::Polygon_2<kernel>::oriented_side(q);
	}

	/************************************************************************/
	/* signed area
	   check P.27 of the book <<Computational Geometry in C>>
	*/
	/************************************************************************/
	FT area()
	{
		return CGAL::Polygon_2<kernel>::area();
	}

	Point left_vertex()
	{
		return *CGAL::Polygon_2<kernel>::left_vertex();
	}

	Point right_vertex()
	{
		return *CGAL::Polygon_2<kernel>::right_vertex();
	}

	Point top_vertex()
	{
		return *CGAL::Polygon_2<kernel>::top_vertex();
	}

	Point bottom_vertex()
	{
		return *CGAL::Polygon_2<kernel>::bottom_vertex();
	}


	/************************************************************************/
	/* convex generate                                                      */
	/************************************************************************/
        bool convexHullGen(Enriched_polygon*& newPoly)
	{
		std::vector<Point> pts;
		pts.resize(this->size());
		for(int i = 0; i < this->size(); i++)
			pts[i] = vertex(i);
		
		std::vector<Point> chpts;
		CGAL::ch_graham_andrew(pts.begin(),pts.end(),back_inserter(chpts));

                newPoly = new Enriched_polygon();
		for(size_t i = 0; i < chpts.size(); i++)
			newPoly->push_back(chpts[i]);

                return true;
	}

	/************************************************************************/
	/* file io                                                              */
	/************************************************************************/
	void write(const char *pFilename)
	{
		std::ofstream stream(pFilename);
	}

	void read(const char *pFilename)
	{
		std::ifstream stream(pFilename);
	}

	/************************************************************************/
	/* opengl part                                                          */
	/************************************************************************/
	void gl_draw(bool smooth_shading, bool use_normals)
	{
		gl_draw_lines();
	}

	void gl_draw_lines()
	{
		glBegin(GL_LINE_LOOP);
		Vertex_iterator iter = vertices_begin();
		Vertex_iterator endv = vertices_end();
		for( ; iter != endv; ++iter)
		{
			Point pnt = *iter;
			glVertex3f(pnt[0],pnt[1],0.0);
		}
		glEnd();
	}

	void gl_draw_points()
	{
		glBegin(GL_POINTS);
		Vertex_iterator iter = vertices_begin();
		Vertex_iterator endv = vertices_end();
		for( ; iter != endv; ++iter)
		{
			Point pnt = *iter;
			glVertex3f(pnt[0],pnt[1],0.0);
		}
		glEnd();
	}

	void gl_draw_number()
	{
		Vertex_iterator iter = vertices_begin();
		Vertex_iterator endv = vertices_end();

		if(size() < 1)
			return;
		Point pnt = *iter;
		static float scale_ratio = -1.0;
		if(scale_ratio < 0.0)
			scale_ratio = (fabsf(pnt[0])+fabsf(pnt[1]))/(2.0*4);

		for( int i = 0; iter != endv; ++iter,++i)
		{
			pnt = *iter;
			glPushMatrix();
			glTranslatef(pnt[0],pnt[1],0.0);
			glScalef(scale_ratio,scale_ratio,scale_ratio);
			std::string str = StringUtils::to_string(i);
			str += "(";
			str += StringUtils::to_string(pnt[0]);
			str += ",";
			str += StringUtils::to_string(pnt[1]);
			str += ")"; 
			YsDrawUglyFont(str.c_str(),0);
			glPopMatrix();
		}
		glEnd();
	}
};

typedef CGAL::Cartesian<double> Enriched_Polygon_kernel;
typedef Enriched_polygon<Enriched_Polygon_kernel> EPolygon;

#endif
