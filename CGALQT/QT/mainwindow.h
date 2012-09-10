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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include <QMainWindow>

class QAction;
class QMenu;
class QWorkspace;
class QActionGroup;
class GLMdiChild;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);

private:
	//actions op
	void createFileActions();
	void createRenderModeActions();
	void createSubdivisionActions();
	void createSelectActions();
	void createPolygonActions();
	void createSimplificationActions();
	void createFallsonActions();
	void createActions();

	//menus op
	void createFileMenus();
	void createRenderModeMenus();
	void createSubdivisionMenus();
	void createSelectMenus();
	void createPolygonMenus();
	void createSimplificationMenus();
	void createFallsonMenus();
	void createMenus();

	//toolbars op
	void createFileToolBars();
	void createRenderToolBars();
	void createSubdivisionToolBars();
	void createSelectToolBars();
	void createPolygonToolBars();
	void createSimplificationToolBars();
	void createFallsonToolBars();
	void createToolBars();

	void createStatusBar();
	
	void readSettings();
	void writeSettings();
	GLMdiChild *createMdiChild();
	GLMdiChild *activeMdiChild();
	GLMdiChild *findMdiChild(const QString &fileName);

private:
	void updateFileActions();
	void updateRenderModeActions();
	void updateSubdivisionActions();
	void updateSelectActions();
	void updatePolygonActions();
	void updateSimplificationActions();
	void updateFallsonActions();
private slots:
	void updateActions();

	/************************************************************************/
	/* file slots                                                           */
	/************************************************************************/
    void open();
    void saveAs();
	void fileproperty();
	void snapshot();

	/************************************************************************/
	/* rendermode slots                                                     */
	/************************************************************************/
	void pointsRenderMode();
	void linesRenderMode();
	void backlinesRenderMode();
	void flatlinesRenderMode();
	void flatRenderMode();
	void smoothRenderMode();

	void bboxRenderMode();
	void lightRenderMode();
	void selectedRenderMode();
	void numberRenderMode();
	/************************************************************************/
	/* subdivision slots                                                    */
	/************************************************************************/
	void sqrt3Sub();
	void quad_triangleSub();
	void doosabinSub();
	void catmullclarkSub();
	void loopSub();

	/************************************************************************/
	/* select slots                                                         */
	/************************************************************************/
	void doNoSelect();
	void doRectSelect();

	/************************************************************************/
	/* polygon slots                                                         */
	/************************************************************************/
	void simplePoly();
	void convexPoly();
	void orientationPoly();
	void orientedSidePoly();
	void convexHullGen();

	/************************************************************************/
	/* simplification slots                                                */
	/************************************************************************/
	void quadSimplify();

	/************************************************************************/
	/* fallson slots                                                         */
	/************************************************************************/
	void fallsonSub();
	void fallson_EulerSplitFacet();
	void fallson_EulerJoinFacet();
	void fallson_EulerCreateCenterVertex();

private:
    QWorkspace *workspace;

	/************************************************************************/
	/* menu                                                                 */
	/************************************************************************/
    QMenu *fileMenu;
	QMenu *renderModeMenu;
	QMenu *subdivisionMenu;
	QMenu *selectMenu;
	QMenu *fallsonMenu;
	QMenu *polygonMenu;
	QMenu *simplificationMenu;

	/************************************************************************/
	/* toolbar                                                               */
	/************************************************************************/
    QToolBar *fileToolBar;
	QToolBar *renderModeToolBar;
	QToolBar *selectToolBar;

	/************************************************************************/
	/* Actions follow                                                       */
	/************************************************************************/
	/************************************************************************/
	/* file Actions                                                         */
	/************************************************************************/
	QActionGroup* fileActGroup;
    QAction *openAct;
	QAction *closeAct;
    QAction *saveAsAct;
    QAction *exitAct;
	QAction *propertyAct;
	QAction *snapshotAct;

	/************************************************************************/
	/* rendermode Actions                                                   */
	/************************************************************************/
	QActionGroup* renderModeActGroup;
	QAction *pointsAct;
	QAction *linesAct;
	QAction *backlinesAct;
	QAction *flatlinesAct;
	QAction *flatAct;
	QAction *smoothAct;

	QAction *bboxAct;
	QAction *lightAct;
	QAction *selectedRenderAct;
	QAction *numberRenderAct;
	/************************************************************************/
	/* subdivision Actions                                                  */
	/************************************************************************/
	QActionGroup *subdivisionActGroup;
	QAction *sqrt3Act;
	QAction *quad_triangleAct;
	QAction *doosabinAct;
	QAction *catmullclarkAct;
	QAction *loopAct;

	/************************************************************************/
	/*select Actions                                                        */
	/************************************************************************/
	QActionGroup *selectActGroup;
	QAction *noSelectAct;
	QAction *rectSelectAct;

	/************************************************************************/
	/*polygon Actions                                                        */
	/************************************************************************/
	QActionGroup *polygonActGroup;
	QAction *simpleAct;
	QAction *convexAct;
	QAction *orientationAct;
	QAction *orientedSideAct;
	QAction *convexHullGenAct;

	/************************************************************************/
	/*simplification Actions                                                */
	/************************************************************************/
	QActionGroup *simplificationActGroup;
	QAction *quadSimplifyAct;

	/************************************************************************/
	/*fallson Actions                                                        */
	/************************************************************************/
	QActionGroup *fallsonActGroup;
	QAction *fallsonSubAct;
	QAction *fallson_EulerSplitFacetAct;
	QAction *fallson_EulerJoinFacetAct;
	QAction *fallson_EulerCreateCenterVertexAct;

};

#endif
