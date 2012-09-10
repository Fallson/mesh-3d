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
#ifndef GLMDICHILD_H
#define GLMDICHILD_H

#include "config.h"
#include <QGLWidget>
#include <vector>

#include <CGAL/basic.h>
#include <CGAL/enum.h>
#include "enriched_polyhedron.h"
#include "enriched_polygon.h"

class ModelView
{
public:
	ModelView()
	{
		m_xRot = 0;
		m_yRot = 0;
		m_zRot = 0;
		m_xyz_scale = 1.0;
		m_xTranslate = 0.0;
		m_yTranslate = 0.0;
	}
	
	void rotate(int dx, int dy)
	{
		int xangle = m_xRot + 8 * dy;
		int yangle = m_yRot + 8 * dx;

		normalizeAngle(&xangle);
		if (xangle != m_xRot) 
		{
			m_xRot = xangle;
		}

		normalizeAngle(&yangle);
		if (yangle != m_yRot) 
		{
			m_yRot = yangle;
		}
	}

	void translate(int dx, int dy)
	{
		m_xTranslate += double(dx)/double(8*12);
		m_yTranslate += double(-1*dy)/double(8*12);
	}

	void scale(int delta)
	{
		if(delta > 0)
		{
			m_xyz_scale *= double(delta)/double(8*12);
		}
		else
		{
			m_xyz_scale *= double(-1*8*12)/double(delta);
		}
	}

public:
	int get_xRot() { return m_xRot; }
	int get_yRot() { return m_yRot; }
	int get_zRot() { return m_zRot; }
	double get_xyzScale() { return m_xyz_scale; }
	double get_xTranslate() { return m_xTranslate; }
	double get_yTranslate() { return m_yTranslate; }
private:
	void normalizeAngle(int *angle)
	{
		while (*angle < 0)
			*angle += 360 * 16;
		while (*angle > 360 * 16)
			*angle -= 360 * 16;
	}
private:
	int m_xRot;
	int m_yRot;
	int m_zRot;
	double m_xyz_scale;
	double m_xTranslate;
	double m_yTranslate;
};

class GLMdiChild : public QGLWidget
{
	Q_OBJECT

public:	
	enum RenderMode
	{
		RMPoints = 0,
		RMLines = 1,
		RMBackLines = 2,
		RMFlatLines = 3,
		RMFlat = 4,
		RMSmooth = 5
	};
	enum CursorType
	{
		CTPlain = 0,
		CTPlain_Pan = 1,
		CTPlain_Rotate = 2,
		CTPlain_Zoom = 3,
		CTSel_Rect = 4,
		CTSel_Rect_Plus = 5,
		CTSel_Rect_Minus = 6
	};
	enum SelectMode
	{
		SMNone = 0,
		SMRectSel = 1,
		SMSketchSel = 2,
	};
	enum ProcesshitsType
	{
		PTNormal = 0,
		PTPlus = 1,
		PTMinus = 2
	};

public:
	GLMdiChild(QWidget *parent = 0);
	~GLMdiChild();

public:
	/************************************************************************/
	/*public slot functions                                                 */
	/************************************************************************/
	//file
	bool loadFile(const QString &fileName);
	bool saveFile(const QString &fileName);
	bool saveToBmp(const QString &fileName);

	//render mode
	void setRenderMode(RenderMode mode){ m_renderMode = mode; }
	RenderMode getRenderMode(){ return m_renderMode; }
	void setBBox(bool bbox){ m_bbox = bbox; }
	bool getBBox(){ return m_bbox; }
	void setLight(bool light){ m_light = light; }
	bool getLight(){ return m_light; }
	void setSelectedRender(bool sel) { m_selectedRender = sel; }
	bool getSelectedRender() { return m_selectedRender; }
	void setNumberRender(bool num) {m_numberRender = num; }
	bool getNumberRender() {return m_numberRender; }

	//subdivision
	bool sqrt3Sub();
	bool quad_triangleSub();
	bool doosabinSub();
	bool catmullclarkSub();
	bool loopSub();

	//select
	void setSelectMode(SelectMode mode) { m_selectMode = mode; }
	SelectMode getSelectMode() { return m_selectMode; }

	//polygon
	bool convexHullGen();

	//simplification
	bool quadSimplify();

	//fallson
	bool fallsonSub();
	bool euler_split_facet();
	bool euler_join_facet();
	bool euler_create_center_vertex();

	//other
	Polyhedron* getMesh(){ return m_pMesh; }
	const std::vector<EPolygon*>& getPolys(){ return m_pPolys; }
	size_t getPolysSize() { return m_pPolys.size(); } 
	//void setMesh(Polyhedron* pMesh){ m_pMesh = pMesh; }
	QString currentFile() { return m_strCurFile; }

protected:
	//opengl
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	//ui
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent * event);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	

private:
	void setCurrentFile(const QString &fileName);
	void setCurCursor(CursorType type = CTPlain);

	//opengl
	void paintGL_Points();
	void paintGL_Lines();
	void paintGL_BackLines();
	void paintGL_FlatLines();
	void paintGL_Flat();
	void paintGL_Smooth();
	void paintGL_Number();
	void paintGL_Selected();
	void paintGL_BBox();
	void doRectSelect(QPoint start, QPoint cur, ProcesshitsType type);
	void drawXORRect(QPoint start, QPoint cur);

private:
	QString m_strCurFile;
	QPoint m_ptLastPos;//used for modelview, the last mouse move position
	QPoint m_ptStartPos;//used for select, the last mouse press position
	QPoint m_ptCurPos;

	Polyhedron *m_pMesh;
	std::vector<EPolygon*> m_pPolys;

	//opengl
	ModelView m_modelview; //
	RenderMode m_renderMode; //the render mode
	bool m_bbox; //whether show boundbox
	bool m_light;//whether using light
	bool m_selectedRender;//whether show the selected faces
	bool m_numberRender;

	SelectMode m_selectMode; //whether in select mode
};

#endif