/****************************************************************************
**
** Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include <QtGui>
#include "glmdichild.h"
//stl
#include <iostream>
#include <fstream>

//cgal
#include "parser_obj.h"
#include "sqrt3.h"
#include "quad-triangle.h"
#include "quad-simp.h"
#include "fallson.h"
#include <CGAL/Subdivision_method_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>

//boost
#include <boost/foreach.hpp>

/************************************************************************/
/*                                                                      */
/************************************************************************/

GLMdiChild::GLMdiChild(QWidget *parent)
: QGLWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
	setCurCursor();

	m_pMesh = NULL;

	m_renderMode = RMFlatLines;
	m_bbox = false;
	m_light = true;
	m_selectedRender = true;
	m_numberRender = false;
	m_selectMode = SMNone;
}

GLMdiChild::~GLMdiChild()
{
	CGALQT_DELETE(m_pMesh);
	
	BOOST_FOREACH(EPolygon* poly, m_pPolys)
		CGALQT_DELETE(poly);
}

/************************************************************************/
/* file operation                                                        */
/************************************************************************/
bool GLMdiChild::loadFile(const QString &fileName)
{
	QString extension = QFileInfo(fileName).suffix();
	extension = extension.toLower();

	// mesh extension
	if(extension == "off" || extension == "obj")
	{
		if(NULL == m_pMesh)
			m_pMesh = new Polyhedron();

		if(extension == "off")
		{
			/************************************************************************/
			//ok, this part code should be written into Polyhedron class
			//such as m_pMesh->read_off()
			/************************************************************************/
			std::ifstream stream(qPrintable(fileName));
			if(!stream)
			{
				QMessageBox::warning(this, tr("CGALQT"),tr("read file error"), QMessageBox::Close);
				return false;
			}
			stream >> *m_pMesh;
		}
		else if(extension == "obj")
		{
			/************************************************************************/
			/* and m_pMesh->read_obj()                                             */
			/************************************************************************/
                        Parser_obj<Enriched_Polyhedron_kernel,Enriched_items> parser;
			parser.read(qPrintable(fileName),m_pMesh);
		}

		m_pMesh->compute_type();
		m_pMesh->compute_normals();
		m_pMesh->compute_bounding_box();
	}
	else if(extension == "pol")//polygon extension
	{
		EPolygon* pPoly = new EPolygon();

		m_pPolys.push_back(pPoly);
		
		
		/************************************************************************/
		/* m_pPolys->read()                                                      */
		/************************************************************************/
		std::ifstream stream(qPrintable(fileName));
		if(!stream)
		{
			QMessageBox::warning(this, tr("CGALQT"),tr("read file error"), QMessageBox::Close);
			return false;
		}
		stream >> *pPoly;
	}
	else
	{
		QMessageBox::warning(this, tr("CGALQT"),tr("unknown extension"), QMessageBox::Close);
		return false;
	}

	setCurrentFile(fileName);
    return true;
}

bool GLMdiChild::saveFile(const QString &fileName)
{
	QString extension = QFileInfo(fileName).suffix();
	extension = extension.toLower();

	if(extension == "off" || extension == "obj")
	{
		if(NULL == m_pMesh)
		{
			QMessageBox::warning(this, tr("CGALQT"),tr("write file error"), QMessageBox::Close);
			return false;
		}

		if(extension == "off")
		{
			/************************************************************************/
			/* m_pMesh->write_off()                                                 */
			/************************************************************************/
			std::ofstream stream(qPrintable(fileName));
			if(!stream)
			{
				QMessageBox::warning(this, tr("CGALQT"),tr("write file error"), QMessageBox::Close);
				return false;
			}
			stream << *m_pMesh;
		}
		else if(extension == "obj")
		{
			m_pMesh->write_obj(qPrintable(fileName));
		}
	}
	else if(extension == "pol")
	{
		if(m_pPolys.empty())
		{
			QMessageBox::warning(this, tr("CGALQT"),tr("write file error"), QMessageBox::Close);
			return false;
		}

		/************************************************************************/
		/* m_pPolys->write()                                                     */
		/************************************************************************/
		for(size_t i = 0; i < m_pPolys.size(); i++)
		{
			QString fn = QFileInfo(fileName).baseName();
			fn += QString(".%d.").arg(i);
			fn += extension;
			std::ofstream stream(qPrintable(fn));
			if(!stream)
			{
				QMessageBox::warning(this, tr("CGALQT"),tr("write file error"), QMessageBox::Close);
				return false;
			}
			stream << *(m_pPolys[i]);
		}
	}

	setCurrentFile(fileName);
	return true;
}

bool GLMdiChild::saveToBmp(const QString &fileName)
{
	QImage img = this->grabFrameBuffer();
	return img.save(fileName,0,100);
}
/////////////////////////////private/////////////////////////////////////////////
void GLMdiChild::setCurrentFile(const QString &fileName)
{
    m_strCurFile = QFileInfo(fileName).canonicalFilePath();

    setWindowModified(false);
    setWindowTitle(m_strCurFile);
}

/************************************************************************/
/* Subdivision part                                                     */
/************************************************************************/
bool GLMdiChild::sqrt3Sub()
{
        CSubdivider_sqrt3<Polyhedron,Enriched_Polyhedron_kernel> subdivider;

	bool ret = subdivider.subdivide(*m_pMesh,1);
	if(ret)
	{
		m_pMesh->compute_type();
		m_pMesh->compute_normals();
		m_pMesh->compute_bounding_box();
	}
	return ret;
}

bool GLMdiChild::quad_triangleSub()
{
        CSubdivider_quad_triangle<Polyhedron,Enriched_Polyhedron_kernel> subdivider;

	// alloc a new mesh
	Polyhedron *pNewMesh = new Polyhedron;

	// subdivide once
	subdivider.subdivide(*m_pMesh,*pNewMesh,true);
	// copy bounding box (approximate, but fast)
	pNewMesh->copy_bounding_box(m_pMesh);
	pNewMesh->compute_normals();
	pNewMesh->compute_type();

	// delete previous mesh
	CGALQT_DELETE(m_pMesh);

	// set new mesh
	m_pMesh = pNewMesh;
	return true;
}

bool GLMdiChild::doosabinSub()
{
	CGAL::Subdivision_method_3::DooSabin_subdivision(*m_pMesh);
	m_pMesh->compute_type();
	m_pMesh->compute_normals();
	m_pMesh->compute_bounding_box();

	return true;
}

bool GLMdiChild::catmullclarkSub()
{
	CGAL::Subdivision_method_3::CatmullClark_subdivision(*m_pMesh);
	m_pMesh->compute_type();
	m_pMesh->compute_normals();
	m_pMesh->compute_bounding_box();
	return true;
}

bool GLMdiChild::loopSub()
{
	CGAL::Subdivision_method_3::Loop_subdivision(*m_pMesh);
	m_pMesh->compute_type();
	m_pMesh->compute_normals();
	m_pMesh->compute_bounding_box();
	return true;
}

/************************************************************************/
/* polygon part                                                         */
/************************************************************************/
bool GLMdiChild::convexHullGen()
{
	if(m_pPolys.empty())
		return false;
        /*
        EPolygon* newPoly = NULL;
        if( m_pPolys[0]->convexHullGen(newPoly) )
            m_pPolys.push_back(newPoly);
        */
	return true;
}

/************************************************************************/
/* simplification part                                                  */
/************************************************************************/
bool GLMdiChild::quadSimplify()
{
#if 0
        typedef CGAL::Polyhedron_3<Enriched_Polyhedron_kernel,Enriched_items> Surface;

        CSimplification_quad<Surface,Enriched_Polyhedron_kernel> simplifier;

	Surface* pMesh = (Surface*)m_pMesh;

	bool ret = simplifier.simple(*pMesh,1,0.3);
	if(ret)
	{
		m_pMesh->compute_type();
		m_pMesh->compute_normals();
		m_pMesh->compute_bounding_box();
	}
	return ret;
#endif
        return true;
}

/************************************************************************/
/* fallson part                                                         */
/************************************************************************/
bool GLMdiChild::fallsonSub()
{
        CSubdivider_fallson<Polyhedron, Enriched_Polyhedron_kernel> sub;

	bool ret = sub.dosolver();
	return ret;
#if 0
	bool ret = sub.subdivide(*m_pMesh,1);
	if(ret)
	{
		m_pMesh->compute_type();
		m_pMesh->compute_normals();
		m_pMesh->compute_bounding_box();
	}
	return ret;
#endif
}

bool GLMdiChild::euler_split_facet()
{
	if( NULL == m_pMesh )
		return false;

	if(m_pMesh->euler_split_facet())
	{
		m_pMesh->compute_type();
		m_pMesh->compute_normals();
		m_pMesh->compute_bounding_box();
		return true;
	}
	return false;
}

bool GLMdiChild::euler_join_facet()
{
	if( NULL == m_pMesh )
		return false;

	if(m_pMesh->euler_join_facet())
	{
		m_pMesh->compute_type();
		m_pMesh->compute_normals();
		m_pMesh->compute_bounding_box();
		return true;
	}
	return false;
}

bool GLMdiChild::euler_create_center_vertex()
{
	if( NULL == m_pMesh )
		return false;

	if(m_pMesh->euler_create_center_vertex())
	{
		m_pMesh->compute_type();
		m_pMesh->compute_normals();
		m_pMesh->compute_bounding_box();
		return true;
	}
	return false;
}
/************************************************************************/
/* the UI part                                                          */
/************************************************************************/
QSize GLMdiChild::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize GLMdiChild::sizeHint() const
{
	return QSize(400, 400);
}

void GLMdiChild::setCurCursor(CursorType type)
{
	static QMap<CursorType,QCursor> curMap;
	if(curMap.isEmpty())
	{
		curMap[CTPlain]=QCursor(QPixmap(":/images/cursors/plain.png"),1,1);	
		curMap[CTPlain_Pan]=QCursor(QPixmap(":/images/cursors/plain_pan.png"),1,1);	
		curMap[CTPlain_Rotate]=QCursor(QPixmap(":/images/cursors/plain_rotate.png"),1,1);	
		curMap[CTPlain_Zoom]=QCursor(QPixmap(":/images/cursors/plain_zoom.png"),1,1);	
		curMap[CTSel_Rect]=QCursor(QPixmap(":/images/cursors/sel_rect.png"),1,1);	
		curMap[CTSel_Rect_Plus]=QCursor(QPixmap(":/images/cursors/sel_rect_plus.png"),1,1);	
		curMap[CTSel_Rect_Minus]=QCursor(QPixmap(":/images/cursors/sel_rect_minus.png"),1,1);
	}
	
	setCursor(curMap[type]);
}

void GLMdiChild::mousePressEvent(QMouseEvent *event)
{
	switch(m_selectMode)
	{
		case SMNone:
			if(event->buttons() & Qt::LeftButton)
			{
				setCurCursor(CTPlain_Rotate);
			}
			else if(event->buttons() & Qt::RightButton)
			{
				setCurCursor(CTPlain_Pan);
			}
			break;
		case SMRectSel:
			if(event->buttons() & Qt::LeftButton)
			{
				if(event->modifiers() & Qt::ShiftModifier)
					setCurCursor(CTSel_Rect_Plus);
				else if(event->modifiers() & Qt::ControlModifier)
					setCurCursor(CTSel_Rect_Minus);
				else
					setCurCursor(CTSel_Rect);
			}
			break;
		case SMSketchSel: //not done yet
			break;
	}

	m_ptCurPos = event->pos();
	m_ptLastPos = m_ptCurPos;
	m_ptStartPos = m_ptCurPos;
	
}

void GLMdiChild::mouseReleaseEvent(QMouseEvent *event)
{
	m_ptCurPos = event->pos();
	m_ptLastPos = m_ptCurPos;
	m_ptStartPos = m_ptCurPos;
	setCurCursor();
	updateGL();
}

void GLMdiChild::mouseMoveEvent(QMouseEvent *event)
{
	m_ptCurPos = event->pos();
	switch(m_selectMode)
	{
		case SMNone:
			{
				int dx = m_ptCurPos.x() - m_ptLastPos.x();
				int dy = m_ptCurPos.y() - m_ptLastPos.y();
				if (event->buttons() & Qt::LeftButton)//rotate the view
				{
					m_modelview.rotate(dx,dy);

					updateGL();
				} 
				else if (event->buttons() & Qt::RightButton) //translate the view
				{
					m_modelview.translate(dx,dy);

					updateGL();
				}
			}
			break;
		case SMRectSel:
			{
				if(event->buttons() & Qt::LeftButton)
				{
					if(event->modifiers() & Qt::ShiftModifier)//do plus select;
						doRectSelect(m_ptStartPos,m_ptCurPos,PTPlus);
					else if(event->modifiers() & Qt::ControlModifier)//do minus select;
						doRectSelect(m_ptStartPos,m_ptCurPos,PTMinus);
					else//do normal select;
						doRectSelect(m_ptStartPos,m_ptCurPos,PTNormal);

					updateGL();
				}
			}
			break;
		case SMSketchSel: //not done yet
			break;
	}
	m_ptLastPos = m_ptCurPos;
}

void GLMdiChild::wheelEvent(QWheelEvent * event)
{
	m_modelview.scale(event->delta());
	updateGL();
}


/************************************************************************/
/* the opengl part                                                      */
/************************************************************************/
void GLMdiChild::initializeGL()
{
	glClearColor( (float)(BGCOLOR.red())/(float)255, 
				  (float)(BGCOLOR.green())/(float)255, 
			      (float)(BGCOLOR.blue())/(float)255, 
				  0.5f);								// Black Background

	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	
	glEnable(GL_NORMALIZE);                             // Enable lighting after scale

	//antialiasing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glLineWidth(LINEWIDTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glPointSize(POINTWIDTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	//lighting
	GLfloat LightAmbient[]=		{ 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat LightDiffuse[]=		{ 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat LightSpecular[]=    { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]=	{ 0.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0,GL_POSITION,LightPosition);
	glLightfv(GL_LIGHT0,GL_AMBIENT,LightAmbient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,LightDiffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,LightSpecular);
	GLfloat MatAmbient[]=  { 0.11f, 0.06f, 0.11f, 1.00f };
	GLfloat MatDiffuse[]=  { 0.43f, 0.47f, 0.54f, 1.00f };
	GLfloat MatSpecular[]= { 0.33f, 0.33f, 0.52f, 1.00f };
	GLfloat MatEmission[]= { 0.00f, 0.00f, 0.00f, 0.00f };
	GLfloat MatShininess[] = { 10.0f };
	glMaterialfv(GL_FRONT,GL_AMBIENT,MatAmbient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,MatDiffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,MatSpecular);
	glMaterialfv(GL_FRONT,GL_EMISSION,MatEmission);
	glMaterialfv(GL_FRONT,GL_SHININESS,MatShininess);
	glEnable(GL_LIGHT0);
}

void GLMdiChild::resizeGL(int width, int height)
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
}

void GLMdiChild::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	glTranslatef(m_modelview.get_xTranslate(), m_modelview.get_yTranslate(),-6.0f);
	glRotated(m_modelview.get_xRot() / 16.0, 1.0, 0.0, 0.0);
	glRotated(m_modelview.get_yRot() / 16.0, 0.0, 1.0, 0.0);
	glRotated(m_modelview.get_zRot() / 16.0, 0.0, 0.0, 1.0);
	glScaled(m_modelview.get_xyzScale(), m_modelview.get_xyzScale(), m_modelview.get_xyzScale());

	if(m_renderMode == RMPoints)
		paintGL_Points();
	else if(m_renderMode == RMLines)
		paintGL_Lines();
	else if(m_renderMode == RMBackLines)
		paintGL_BackLines();
	else if(m_renderMode == RMFlatLines)
		paintGL_FlatLines();
	else if(m_renderMode == RMFlat)
		paintGL_Flat();
	else if(m_renderMode == RMSmooth)
		paintGL_Smooth();
	else
		paintGL_Smooth();

	if(m_numberRender)
		paintGL_Number();

	if(m_selectedRender)
		paintGL_Selected();

	if(m_bbox)
		paintGL_BBox();

	if(m_selectMode == SMRectSel)
		drawXORRect(m_ptStartPos, m_ptCurPos);

}

/************************************************************************/
/*opengl private part                                                     */
/************************************************************************/
void GLMdiChild::paintGL_Points()
{
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glColor3ub(POINTSCOLOR.red(),POINTSCOLOR.green(),POINTSCOLOR.blue());
	if(m_pMesh)
		m_pMesh->gl_draw_points();
	BOOST_FOREACH(EPolygon* pPoly, m_pPolys)
		pPoly->gl_draw_points();
}

void GLMdiChild::paintGL_Lines()
{
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glColor3ub(LINESCOLOR.red(),LINESCOLOR.green(),LINESCOLOR.blue());
	if(m_pMesh)
		m_pMesh->gl_draw_lines();
	BOOST_FOREACH(EPolygon* pPoly, m_pPolys)
		pPoly->gl_draw_lines();
}

void GLMdiChild::paintGL_BackLines()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f,1.0f);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	if(m_pMesh)
		m_pMesh->gl_draw(false,false);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glColor3ub(LINESCOLOR.red(),LINESCOLOR.green(),LINESCOLOR.blue());
	if(m_pMesh)
		m_pMesh->gl_draw(false,false);
	BOOST_FOREACH(EPolygon* pPoly, m_pPolys)
		pPoly->gl_draw_lines();
}

void GLMdiChild::paintGL_FlatLines()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f,1.0f);
	if(m_light)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glColor3ub(MESHCOLOR.red(),MESHCOLOR.green(),MESHCOLOR.blue());
	if(m_pMesh)
		m_pMesh->gl_draw(false,true);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glColor3ub(LINESCOLOR.red(),LINESCOLOR.green(),LINESCOLOR.blue());
	if(m_pMesh)
		m_pMesh->gl_draw(false,false);
	BOOST_FOREACH(EPolygon* pPoly, m_pPolys)
		pPoly->gl_draw(false,false);
}

void GLMdiChild::paintGL_Flat()
{
	if(m_light)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glColor3ub(MESHCOLOR.red(),MESHCOLOR.green(),MESHCOLOR.blue());
	if(m_pMesh)
		m_pMesh->gl_draw(false,true);
	BOOST_FOREACH(EPolygon* pPoly, m_pPolys)
		pPoly->gl_draw(false,false);
}

void GLMdiChild::paintGL_Smooth()
{
	if(m_light)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glColor3ub(MESHCOLOR.red(),MESHCOLOR.green(),MESHCOLOR.blue());
	if(m_pMesh)
		m_pMesh->gl_draw(true,true);
	BOOST_FOREACH(EPolygon* pPoly, m_pPolys)
		pPoly->gl_draw(false,false);
}

void GLMdiChild::paintGL_Number()
{
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glColor4f( float(FONTCOLOR.red())/float(255),
		float(FONTCOLOR.green())/float(255),
		float(FONTCOLOR.blue())/float(255),
		1.0f);
	if(m_pMesh)
		m_pMesh->gl_draw_number();
	BOOST_FOREACH(EPolygon* pPoly, m_pPolys)
		pPoly->gl_draw_number();
}

void GLMdiChild::paintGL_Selected()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0, -1.0);

	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glDepthMask(GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glColor4f( float(SELECTEDCOLOR.red())/float(255),
		float(SELECTEDCOLOR.green())/float(255),
		float(SELECTEDCOLOR.blue())/float(255),
		0.3f);
	if(m_pMesh)
		m_pMesh->gl_draw_selectedfaces();
	glDepthMask(GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void GLMdiChild::paintGL_BBox()
{
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glColor3ub(BBOXCOLOR.red(),BBOXCOLOR.green(),BBOXCOLOR.blue());
	if(m_pMesh)
		m_pMesh->gl_draw_bounding_box();
}

void GLMdiChild::drawXORRect(QPoint start, QPoint cur)
{  
	if(start == cur)
		return;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,viewport[2],viewport[3],0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(start.x(),start.y());
	glVertex2f(cur.x(),start.y());
	glVertex2f(cur.x(),cur.y());
	glVertex2f(start.x(),cur.y());
	glEnd();
	glDisable(GL_LOGIC_OP);

	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void GLMdiChild::doRectSelect(QPoint start, QPoint cur, ProcesshitsType type)
{
	//calculate the mouse's rectangle
	QPoint mid=(cur+start)/2;
	QPoint wid=(start-cur);
	if(wid.x()<0)
		wid.setX(-wid.x());
	if(wid.y()<0)  
		wid.setY(-wid.y());
	int x = mid.x();
	int y = mid.y();
	int width = wid.x();
	int height = wid.y();
	
	if( width==0 || height==0 ) 
		return; 

	makeCurrent();
	
	long hits;	
	int sz=m_pMesh->size_of_facets()*5;
	GLuint *selectBuf =new GLuint[sz];
	glSelectBuffer(sz, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	double mp[16];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX ,mp);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix(x, viewport[3]-y, width, height, viewport);
	glMultMatrixd(mp);

	m_pMesh->gl_draw(false,false);

	glPopMatrix();
	hits = glRenderMode(GL_RENDER);

	if(type == PTPlus)
		m_pMesh->gl_processhits(hits,selectBuf,processhits_plus());
	if(type == PTMinus)
		m_pMesh->gl_processhits(hits,selectBuf,processhits_minus());
	if(type == PTNormal)
		m_pMesh->gl_processhits(hits,selectBuf,processhits_normal());

	CGALQT_DELETE_ARRAY(selectBuf);
}
