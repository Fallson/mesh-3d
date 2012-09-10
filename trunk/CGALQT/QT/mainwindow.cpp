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
#include "mainwindow.h"
#include "glmdichild.h"
#include "filepropertydialog.h"
#include <vector>
#include <boost/foreach.hpp>

MainWindow::MainWindow()
{
    workspace = new QWorkspace;
    setCentralWidget(workspace);
	connect(workspace,SIGNAL(windowActivated(QWidget *)), this, SLOT(updateActions()));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    readSettings();

    setWindowTitle(tr("CGALQT"));
	updateActions();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    workspace->closeAllWindows();
    if (activeMdiChild()) 
	{
        event->ignore();
    } 
	else 
	{
        writeSettings();
        event->accept();
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		if(workspace->windowList().size() == 0)
			open();
	}
}

void MainWindow::createFileActions()
{
	fileActGroup = new QActionGroup(this);

	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing file"));
	//openAct->setActionGroup(fileActGroup);
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	closeAct = new QAction(tr("&Close"),this);
	closeAct->setShortcut(Qt::CTRL + Qt::Key_C);
	closeAct->setStatusTip(tr("Close current file"));
	closeAct->setActionGroup(fileActGroup);
	connect(closeAct, SIGNAL(triggered()), workspace, SLOT(closeActiveWindow()));

	saveAsAct = new QAction(QIcon(":/images/save.png"), tr("&Save As..."), this);
	saveAsAct->setShortcut(tr("Ctrl+S"));
	saveAsAct->setStatusTip(tr("Save the document to disk"));
	saveAsAct->setActionGroup(fileActGroup);
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	//exitAct->setActionGroup(fileActGroup);
	connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

	propertyAct = new QAction(QIcon(":/images/property.png"), tr("&File Property..."),this);
	propertyAct->setShortcut(Qt::ALT + Qt::Key_Return);
	propertyAct->setStatusTip(tr("Show the file property"));
	propertyAct->setActionGroup(fileActGroup);
	connect(propertyAct,SIGNAL(triggered()), this, SLOT(fileproperty()));

	snapshotAct = new QAction(QIcon(":/images/snapshot.png"), tr("Save Snapsho&t..."), this);
	snapshotAct->setShortcut(Qt::CTRL + Qt::Key_T);
	snapshotAct->setStatusTip(tr("Save Snapshot"));
	snapshotAct->setActionGroup(fileActGroup);
	connect(snapshotAct, SIGNAL(triggered()), this, SLOT(snapshot()));
}

void MainWindow::createRenderModeActions()
{
	//render mode
	renderModeActGroup = new QActionGroup(this);
	renderModeActGroup->setExclusive(false);

	pointsAct = new QAction(QIcon(":/images/points.png"), tr("&Points"), this);
	pointsAct->setStatusTip(tr("Points render mode"));
	pointsAct->setActionGroup(renderModeActGroup);
	pointsAct->setCheckable(true);
	connect(pointsAct, SIGNAL(triggered()), this, SLOT(pointsRenderMode()));

	linesAct = new QAction(QIcon(":/images/lines.png"), tr("L&ines"), this);
	linesAct->setStatusTip(tr("Lines render mode"));
	linesAct->setActionGroup(renderModeActGroup);
	linesAct->setCheckable(true);
	connect(linesAct,SIGNAL(triggered()), this, SLOT(linesRenderMode()));

	backlinesAct = new QAction(QIcon(":/images/backlines.png"), tr("&BackLines"), this);
	backlinesAct->setStatusTip(tr("BackLines render mode"));
	backlinesAct->setActionGroup(renderModeActGroup);
	backlinesAct->setCheckable(true);
	connect(backlinesAct,SIGNAL(triggered()), this, SLOT(backlinesRenderMode()));

	flatlinesAct = new QAction(QIcon(":/images/flatlines.png"), tr("&FlatLines"), this);
	flatlinesAct->setStatusTip(tr("FlatLines render mode"));
	flatlinesAct->setActionGroup(renderModeActGroup);
	flatlinesAct->setCheckable(true);
	connect(flatlinesAct,SIGNAL(triggered()), this, SLOT(flatlinesRenderMode()));

	flatAct = new QAction(QIcon(":/images/flat.png"), tr("Fla&t"), this);
	flatAct->setStatusTip(tr("Flat render mode"));
	flatAct->setActionGroup(renderModeActGroup);
	flatAct->setCheckable(true);
	connect(flatAct,SIGNAL(triggered()), this, SLOT(flatRenderMode()));

	smoothAct = new QAction(QIcon(":/images/smooth.png"), tr("&Smooth"), this);
	smoothAct->setStatusTip(tr("Smooth render mode"));
	smoothAct->setActionGroup(renderModeActGroup);
	smoothAct->setCheckable(true);
	connect(smoothAct,SIGNAL(triggered()), this, SLOT(smoothRenderMode()));

	numberRenderAct = new QAction(QIcon(":/images/number.png"), tr("Show vertex &Number"), this);
	numberRenderAct->setStatusTip(tr("show vertex index number"));
	numberRenderAct->setActionGroup(renderModeActGroup);
	numberRenderAct->setCheckable(true);
	connect(numberRenderAct, SIGNAL(triggered()), this, SLOT(numberRenderMode()));

	selectedRenderAct = new QAction(QIcon(":/images/selected.png"), tr("S&how Selected Faces"),this);
	selectedRenderAct->setStatusTip(tr("Selected face color on or off"));
	selectedRenderAct->setActionGroup(renderModeActGroup);
	selectedRenderAct->setCheckable(true);
	connect(selectedRenderAct, SIGNAL(triggered()), this, SLOT(selectedRenderMode()));

	bboxAct = new QAction(QIcon(":/images/bbox.png"),tr("BB&ox"), this);
	bboxAct->setStatusTip(tr("Bounded box"));
	bboxAct->setActionGroup(renderModeActGroup);
	bboxAct->setCheckable(true);
	connect(bboxAct,SIGNAL(triggered()), this, SLOT(bboxRenderMode()));

	lightAct = new QAction(QIcon(":/images/lighton.png"),tr("&Light"),this);
	lightAct->setStatusTip(tr("Light on or off"));
	lightAct->setActionGroup(renderModeActGroup);
	lightAct->setCheckable(true);
	connect(lightAct,SIGNAL(triggered()), this, SLOT(lightRenderMode()));
}

void MainWindow::createSubdivisionActions()
{
	subdivisionActGroup = new QActionGroup(this);

	sqrt3Act = new QAction(tr("&Sqrt3"),this);
	sqrt3Act->setShortcut(tr("Ctrl+Shift+S"));
	sqrt3Act->setStatusTip(tr("Sqrt3 Subdivision"));
	sqrt3Act->setActionGroup(subdivisionActGroup);
	connect(sqrt3Act,SIGNAL(triggered()), this, SLOT(sqrt3Sub()));

	quad_triangleAct = new QAction(tr("&Quad-Triangle"),this);
	quad_triangleAct->setShortcut(tr("Ctrl+Shift+Q"));
	quad_triangleAct->setStatusTip(tr("Quad-Triangle Subdivision"));
	quad_triangleAct->setActionGroup(subdivisionActGroup);
	connect(quad_triangleAct,SIGNAL(triggered()), this, SLOT(quad_triangleSub()));

	doosabinAct = new QAction(tr("&DooSabin"),this);
	doosabinAct->setShortcut(tr("Ctrl+Shift+D"));
	doosabinAct->setStatusTip(tr("DooSabin Subdivision"));
	doosabinAct->setActionGroup(subdivisionActGroup);
	connect(doosabinAct,SIGNAL(triggered()), this, SLOT(doosabinSub()));

	catmullclarkAct = new QAction(tr("&CatmullClark"),this);
	catmullclarkAct->setShortcut(tr("Ctrl+Shift+C"));
	catmullclarkAct->setStatusTip(tr("CatmullClark Subdivision"));
	catmullclarkAct->setActionGroup(subdivisionActGroup);
	connect(catmullclarkAct,SIGNAL(triggered()), this, SLOT(catmullclarkSub()));

	loopAct = new QAction(tr("&Loop"),this);
	loopAct->setShortcut(tr("Ctrl+Shift+L"));
	loopAct->setStatusTip(tr("Loop Subdivision"));
	loopAct->setActionGroup(subdivisionActGroup);
	connect(loopAct,SIGNAL(triggered()), this, SLOT(loopSub()));
}

void MainWindow::createSelectActions()
{
	selectActGroup = new QActionGroup(this);

	noSelectAct = new QAction(QIcon(":/images/no_select.png"),tr("&No select"),this);
	noSelectAct->setStatusTip(tr("do not select"));
	noSelectAct->setActionGroup(selectActGroup);
	noSelectAct->setCheckable(true);
	connect(noSelectAct, SIGNAL(triggered()), this, SLOT(doNoSelect()));

	rectSelectAct = new QAction(QIcon(":/images/rect_select.png"),tr("&Rect select"),this);
	rectSelectAct->setStatusTip(tr("do rect select"));
	rectSelectAct->setActionGroup(selectActGroup);
	rectSelectAct->setCheckable(true);
	connect(rectSelectAct, SIGNAL(triggered()), this, SLOT(doRectSelect()));
}

void MainWindow::createPolygonActions()
{
	polygonActGroup = new QActionGroup(this);

	simpleAct = new QAction(tr("simple poly?"),this);
	simpleAct->setStatusTip(tr("test a simple polygon"));
	simpleAct->setActionGroup(polygonActGroup);
	connect(simpleAct,SIGNAL(triggered()), this, SLOT(simplePoly()));

	convexAct = new QAction(tr("convex poly?"),this);
	convexAct->setStatusTip(tr("test a convex polygon"));
	convexAct->setActionGroup(polygonActGroup);
	connect(convexAct,SIGNAL(triggered()), this, SLOT(convexPoly()));

	orientationAct = new QAction(tr("poly orientation?"),this);
	orientationAct->setStatusTip(tr("test a polygon orientation"));
	orientationAct->setActionGroup(polygonActGroup);
	connect(orientationAct,SIGNAL(triggered()), this, SLOT(orientationPoly()));

	orientedSideAct = new QAction(tr("oriented side of point?"),this);
	orientedSideAct->setStatusTip(tr(""));
	orientedSideAct->setActionGroup(polygonActGroup);
	connect(orientedSideAct, SIGNAL(triggered()), this, SLOT(orientedSidePoly()));

	convexHullGenAct = new QAction(tr("generate convex hull"),this);
	convexHullGenAct->setStatusTip(tr(""));
	convexHullGenAct->setActionGroup(polygonActGroup);
	connect(convexHullGenAct, SIGNAL(triggered()), this, SLOT(convexHullGen()));
}

void MainWindow::createSimplificationActions()
{
	simplificationActGroup = new QActionGroup(this);

	quadSimplifyAct = new QAction(tr("quad simplify"),this);
	quadSimplifyAct->setStatusTip(tr("quad error simplify"));
	quadSimplifyAct->setActionGroup(simplificationActGroup);
	connect(quadSimplifyAct,SIGNAL(triggered()), this, SLOT(quadSimplify()));
}

void MainWindow::createFallsonActions()
{
	//fallson
	fallsonActGroup = new QActionGroup(this);

	fallsonSubAct = new QAction(tr("subdivision"),this);
	fallsonSubAct->setStatusTip(tr("subdivision test"));
	fallsonSubAct->setActionGroup(fallsonActGroup);
	connect(fallsonSubAct, SIGNAL(triggered()), this, SLOT(fallsonSub()));

	fallson_EulerSplitFacetAct = new QAction(tr("split selected facet"),this);
	fallson_EulerSplitFacetAct->setStatusTip(tr("split facet test"));
	fallson_EulerSplitFacetAct->setActionGroup(fallsonActGroup);
	connect(fallson_EulerSplitFacetAct, SIGNAL(triggered()), this, SLOT(fallson_EulerSplitFacet()));

	fallson_EulerJoinFacetAct = new QAction(tr("join facet"),this);
	fallson_EulerJoinFacetAct->setStatusTip(tr("join facet test"));
	fallson_EulerJoinFacetAct->setActionGroup(fallsonActGroup);
	connect(fallson_EulerJoinFacetAct, SIGNAL(triggered()), this, SLOT(fallson_EulerJoinFacet()));

	fallson_EulerCreateCenterVertexAct = new QAction(tr("create center vertex"),this);
	fallson_EulerCreateCenterVertexAct->setStatusTip(tr("create center vertex test"));
	fallson_EulerCreateCenterVertexAct->setActionGroup(fallsonActGroup);
	connect(fallson_EulerCreateCenterVertexAct, SIGNAL(triggered()), this, SLOT(fallson_EulerCreateCenterVertex()));
}

void MainWindow::createActions()
{
	createFileActions();
	createRenderModeActions();
	createSubdivisionActions();
	createSelectActions();
	createPolygonActions();
	createSimplificationActions();
	createFallsonActions();
}


void MainWindow::updateFileActions()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{
		fileActGroup->setDisabled(false);
	}
	else
	{
		fileActGroup->setDisabled(true);
	}
}
void MainWindow::updateRenderModeActions()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{
		renderModeActGroup->setDisabled(false);

		int sz = renderModeActGroup->actions().size();
		for(int i = 0; i < sz; i++)
			renderModeActGroup->actions()[i]->setChecked(false);
		renderModeActGroup->actions()[pChild->getRenderMode()]->setChecked(true);

		numberRenderAct->setChecked(pChild->getNumberRender());
		selectedRenderAct->setChecked(pChild->getSelectedRender());
		bboxAct->setChecked(pChild->getBBox());

		lightAct->setChecked(pChild->getLight());
		if(lightAct->isChecked())
		{
			QIcon icon(":/images/lighton.png");
			lightAct->setIcon(icon);
		}
		else
		{
			QIcon icon(":/images/lightoff.png");
			lightAct->setIcon(icon);
		}
	}
	else
	{
		renderModeActGroup->setDisabled(true);
	}
}

void MainWindow::updateSubdivisionActions()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{
		subdivisionActGroup->setDisabled(false);
		Polyhedron* pMesh = pChild->getMesh();

		sqrt3Act->setEnabled(pMesh != NULL && pMesh->is_pure_triangle());
		quad_triangleAct->setEnabled(pMesh != NULL);	
		doosabinAct->setEnabled(pMesh != NULL);
		catmullclarkAct->setEnabled(pMesh != NULL);
		loopAct->setEnabled(pMesh != NULL && pMesh->is_pure_triangle());
	}
	else
	{
		subdivisionActGroup->setDisabled(true);
	}
}
void MainWindow::updateSelectActions()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{
		selectActGroup->setDisabled(false);
		selectActGroup->actions()[pChild->getSelectMode()]->setChecked(true);
	}
	else
	{
		selectActGroup->setDisabled(true);
	}
}

void MainWindow::updatePolygonActions()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{
		polygonActGroup->setDisabled(false);
		size_t polysize = pChild->getPolysSize();
		simpleAct->setEnabled(0 != polysize);
		convexAct->setEnabled(0 != polysize);
		orientationAct->setEnabled(0 != polysize);
		orientedSideAct->setEnabled(0 != polysize);
		convexHullGenAct->setEnabled(0 != polysize);
	}
	else
	{
		polygonActGroup->setDisabled(true);
	}
}

void MainWindow::updateSimplificationActions()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{	
		Polyhedron* pMesh = pChild->getMesh();
		simplificationActGroup->setDisabled(false);
		quadSimplifyAct->setEnabled(pMesh != NULL);
	}
	else
	{
		simplificationActGroup->setDisabled(true);
	}
}
void MainWindow::updateFallsonActions()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{	
		Polyhedron* pMesh = pChild->getMesh();
		fallsonActGroup->setDisabled(false);
		fallsonSubAct->setEnabled(pMesh != NULL);
		fallson_EulerSplitFacetAct->setEnabled(pMesh != NULL);
		fallson_EulerJoinFacetAct->setEnabled(pMesh != NULL);
		fallson_EulerCreateCenterVertexAct->setEnabled(pMesh != NULL);
	}
	else
	{
		fallsonActGroup->setDisabled(true);
	}
}

void MainWindow::updateActions()
{
	updateFileActions();
	updateRenderModeActions();
	updateSubdivisionActions();
	updateSelectActions();
	updatePolygonActions();
	updateSimplificationActions();
	updateFallsonActions();
}

void MainWindow::createFileMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(closeAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(propertyAct);
	fileMenu->addAction(snapshotAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
}

void MainWindow::createRenderModeMenus()
{
	renderModeMenu = menuBar()->addMenu(tr("&Render Mode"));
	renderModeMenu->addAction(pointsAct);
	renderModeMenu->addAction(linesAct);
	renderModeMenu->addAction(backlinesAct);
	renderModeMenu->addAction(flatlinesAct);
	renderModeMenu->addAction(flatAct);
	renderModeMenu->addAction(smoothAct);
	renderModeMenu->addSeparator();
	renderModeMenu->addAction(numberRenderAct);
	renderModeMenu->addAction(selectedRenderAct);
	renderModeMenu->addAction(bboxAct);
	renderModeMenu->addAction(lightAct);
}

void MainWindow::createSubdivisionMenus()
{
	subdivisionMenu = menuBar()->addMenu(tr("&Subdivision"));
	subdivisionMenu->addAction(sqrt3Act);
	subdivisionMenu->addAction(quad_triangleAct);
	subdivisionMenu->addAction(doosabinAct);
	subdivisionMenu->addAction(catmullclarkAct);
	subdivisionMenu->addAction(loopAct);
}

void MainWindow::createSelectMenus()
{
	selectMenu = menuBar()->addMenu(tr("S&elect"));
	selectMenu->addAction(noSelectAct);
	selectMenu->addAction(rectSelectAct);
}

void MainWindow::createPolygonMenus()
{
	polygonMenu = menuBar()->addMenu(tr("&Polygon"));
	polygonMenu->addAction(simpleAct);
	polygonMenu->addAction(convexAct);
	polygonMenu->addAction(orientationAct);
	polygonMenu->addAction(orientedSideAct);
	polygonMenu->addAction(convexHullGenAct);
}

void MainWindow::createSimplificationMenus()
{
	simplificationMenu = menuBar()->addMenu(tr("Simpli&fication"));
	simplificationMenu->addAction(quadSimplifyAct);
}
void MainWindow::createFallsonMenus()
{
	fallsonMenu = menuBar()->addMenu(tr("F&allson"));
	fallsonMenu->addAction(fallsonSubAct);
	fallsonMenu->addAction(fallson_EulerSplitFacetAct);
	fallsonMenu->addAction(fallson_EulerJoinFacetAct);
	fallsonMenu->addAction(fallson_EulerCreateCenterVertexAct);
}
void MainWindow::createMenus()
{
	createFileMenus();
	createRenderModeMenus();
	createSubdivisionMenus();
	createSelectMenus();
	createPolygonMenus();
	createSimplificationMenus();
	createFallsonMenus();
}

void MainWindow::createFileToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAsAct);
	fileToolBar->addSeparator();
	fileToolBar->addAction(propertyAct);
	fileToolBar->addAction(snapshotAct);
}

void MainWindow::createRenderToolBars()
{
	renderModeToolBar = addToolBar(tr("Render Mode"));
	renderModeToolBar->addAction(pointsAct);
	renderModeToolBar->addAction(linesAct);
	renderModeToolBar->addAction(backlinesAct);
	renderModeToolBar->addAction(flatlinesAct);
	renderModeToolBar->addAction(flatAct);
	renderModeToolBar->addAction(smoothAct);
	renderModeToolBar->addSeparator();
	renderModeToolBar->addAction(numberRenderAct);
	renderModeToolBar->addAction(selectedRenderAct);
	renderModeToolBar->addAction(bboxAct);
	renderModeToolBar->addAction(lightAct);
}
void MainWindow::createSubdivisionToolBars()
{

}
void MainWindow::createSelectToolBars()
{
	selectToolBar = addToolBar(tr("Select"));
	selectToolBar->addAction(noSelectAct);
	selectToolBar->addAction(rectSelectAct);
}
void MainWindow::createPolygonToolBars()
{

}
void MainWindow::createSimplificationToolBars()
{

}
void MainWindow::createFallsonToolBars()
{

}
void MainWindow::createToolBars()
{
	createFileToolBars();
	createRenderToolBars();
	createSubdivisionToolBars();
	createSelectToolBars();
	createPolygonToolBars();
	createFallsonToolBars();
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("Fallson", "CGALQT");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("Fallson", "CGALQT");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

GLMdiChild *MainWindow::createMdiChild()
{
	GLMdiChild *child = new GLMdiChild;
	workspace->addWindow(child);

	return child;
}

GLMdiChild *MainWindow::activeMdiChild()
{
    return qobject_cast<GLMdiChild *>(workspace->activeWindow());
}

GLMdiChild *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *window, workspace->windowList()) {
        GLMdiChild *mdiChild = qobject_cast<GLMdiChild *>(window);
        if (mdiChild->currentFile() == canonicalFilePath)
            return mdiChild;
    }
    return 0;
}

/************************************************************************/
/* file slots                                                           */
/************************************************************************/
void MainWindow::open()
{
	QStringList filters;
	filters.push_back(tr("All Known Formats(*.obj *.off *.pol)"));
	filters.push_back(tr("Wavefront 3D Object(*.obj)"));
	filters.push_back(tr("3D Mesh Object File Format(*.off)"));
	filters.push_back(tr("Polygon File Format(*.pol)"));

	QString fileName = QFileDialog::getOpenFileName(this,tr("open file"),QString(),filters.join("\n"));
	if (!fileName.isEmpty()) 
	{
		GLMdiChild *existing = findMdiChild(fileName);
		if (existing) 
		{
			workspace->setActiveWindow(existing);
			return;
		}

		GLMdiChild *child = createMdiChild();
		if (child->loadFile(fileName))
		{
			statusBar()->showMessage(tr("File loaded"), 2000);
			child->showMaximized();
		} 
		else 
		{
			child->close();
		}
	}

	updateActions();
}

void MainWindow::saveAs()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{
		size_t polysize = pChild->getPolysSize();
		Polyhedron* pMesh = pChild->getMesh();

		QStringList filters;
		if(pMesh)
		{
			filters.push_back(tr("Wavefront 3D Object(*.obj)"));
			filters.push_back(tr("3D Mesh Object File Format(*.off)"));
		}
		if(0 != polysize)
			filters.push_back(tr("Polygon File Format(*.pol)"));
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), pChild->currentFile(), filters.join("\n"));
		if (fileName.isEmpty())
			return;

		if (pChild->saveFile(fileName))
			statusBar()->showMessage(tr("File saved"), 2000);
	}

	updateActions();
}

void MainWindow::fileproperty()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		Polyhedron* pMesh = pChild->getMesh();
		if(pMesh)
		{
			FilePropertyDialog dialog(pChild->currentFile(),pMesh->size_of_vertices(),
				pMesh->size_of_halfedges()/2, pMesh->size_of_facets(), this);
			dialog.exec();
		}
	}

	updateActions();
}

void MainWindow::snapshot()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{
		QStringList filters;
		filters.push_back(tr("All Known Formats(*.bmp *.jpg *.jpeg *.png)"));
		filters.push_back(tr("Windows Bitmap(*.bmp)"));
		filters.push_back(tr("Joint Photographic Experts Group(*.jpg)"));
		filters.push_back(tr("Joint Photographic Experts Group(*.jpeg)"));
		filters.push_back(tr("Portable Network Graphics(*.png)"));
		QString targetFileName = QString("%1/%2").arg(QFileInfo(pChild->currentFile()).canonicalPath()).arg(QFileInfo(pChild->currentFile()).completeBaseName());
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), targetFileName, filters.join("\n"));
		if (fileName.isEmpty())
			return;

		if (pChild->saveToBmp(fileName))
			statusBar()->showMessage(tr("Snapshot saved"), 2000);
	}
	updateActions();
}

/************************************************************************/
/* rendermode slots                                                     */
/************************************************************************/
void MainWindow::pointsRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setRenderMode(GLMdiChild::RMPoints);
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::linesRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setRenderMode(GLMdiChild::RMLines);
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::backlinesRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setRenderMode(GLMdiChild::RMBackLines);
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::flatlinesRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setRenderMode(GLMdiChild::RMFlatLines);
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::flatRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setRenderMode(GLMdiChild::RMFlat);
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::smoothRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setRenderMode(GLMdiChild::RMSmooth);
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::bboxRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setBBox(!pChild->getBBox());
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::lightRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setLight(!pChild->getLight());
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::selectedRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setSelectedRender(!pChild->getSelectedRender());
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::numberRenderMode()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setNumberRender(!pChild->getNumberRender());
		pChild->updateGL();
	}
	updateActions();
}
/************************************************************************/
/* subdivision slots                                                    */
/************************************************************************/
void MainWindow::sqrt3Sub()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->sqrt3Sub();
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::quad_triangleSub()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->quad_triangleSub();
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::doosabinSub()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->doosabinSub();
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::catmullclarkSub()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->catmullclarkSub();
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::loopSub()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->loopSub();
		pChild->updateGL();
	}
	updateActions();
}

/************************************************************************/
/* select slots                                                         */
/************************************************************************/
void MainWindow::doNoSelect()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setSelectMode(GLMdiChild::SMNone);
	}
	updateActions();
}

void MainWindow::doRectSelect()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->setSelectMode(GLMdiChild::SMRectSel);
	}
	updateActions();
}

/************************************************************************/
/* polygon slots                                                        */
/************************************************************************/
void MainWindow::simplePoly()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		const std::vector<EPolygon*> pPolys = pChild->getPolys();

		if(pPolys.empty())
			return;
		EPolygon* pPoly = pPolys[0];
		QMessageBox::information(this,tr("simple polygon or not"), pPoly->is_simple()?tr("It's simple"):tr("It's not simple"));
	}
	updateActions();
}

void MainWindow::convexPoly()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		const std::vector<EPolygon*> pPolys = pChild->getPolys();

		if(pPolys.empty())
			return;
		EPolygon* pPoly = pPolys[0];
		QMessageBox::information(this,tr("convex polygon or not"), pPoly->is_convex()?tr("It's convex"):tr("It's concave"));
	}
	updateActions();
}

void MainWindow::orientationPoly()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		const std::vector<EPolygon*> pPolys = pChild->getPolys();

		if(pPolys.empty())
			return;
		EPolygon* pPoly = pPolys[0];
		QMessageBox::information(this,tr("polygon orientation"), 
				pPoly->orientation() == CGAL::COUNTERCLOCKWISE?tr("It's counterclockwise"):tr("It's clockwise"));
	}
	updateActions();
}

void MainWindow::orientedSidePoly()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		const std::vector<EPolygon*> pPolys = pChild->getPolys();

		if(pPolys.empty())
			return;

		EPolygon* pPoly = pPolys[0];
		EPolygon::Point pnt(3,4);
		CGAL::Oriented_side side = pPoly->oriented_side(pnt);
		QString msg;
		if(side == CGAL::ON_NEGATIVE_SIDE)
			msg = tr("on negative side");
		else if(side == CGAL::ON_ORIENTED_BOUNDARY)
			msg = tr("on oriented boundary");
		else if(side == CGAL::ON_POSITIVE_SIDE)
			msg = tr("on positive side");

		QMessageBox::information(this,QString("Point(%1, %2) oriented side").arg(pnt[0]).arg(pnt[1]), msg);
	}
	updateActions();
}

void MainWindow::convexHullGen()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->convexHullGen();
		pChild->updateGL();
	}
	updateActions();
}

/************************************************************************/
/* simplification slots                                                */
/************************************************************************/
void MainWindow::quadSimplify()
{
	GLMdiChild* pChild = activeMdiChild();
	if(pChild)
	{
		pChild->quadSimplify();
		pChild->updateGL();
	}
	updateActions();
}

/************************************************************************/
/* fallson slots                                                         */
/************************************************************************/
void MainWindow::fallsonSub()
{
	GLMdiChild * pChild = activeMdiChild();
	if(pChild)
	{
		pChild->fallsonSub();
		pChild->updateGL();
	}
	updateActions();
}

void MainWindow::fallson_EulerSplitFacet()
{
	GLMdiChild *pChild = activeMdiChild();
	if(pChild)
	{
		if(pChild->euler_split_facet())
			pChild->updateGL();
		else
			QMessageBox::information(this, tr("split facet error"), tr("no facet selected or triangle facet"));
	}
	updateActions();
}

void MainWindow::fallson_EulerJoinFacet()
{
	GLMdiChild *pChild = activeMdiChild();
	if(pChild)
	{
		if(pChild->euler_join_facet())
			pChild->updateGL();
		else
			QMessageBox::information(this,tr("join facet error"), tr("no facet selected or topology error"));
	}
	updateActions();
}

void MainWindow::fallson_EulerCreateCenterVertex()
{
	GLMdiChild *pChild = activeMdiChild();
	if(pChild)
	{
		if(pChild->euler_create_center_vertex())
			pChild->updateGL();
		else
			QMessageBox::information(this,tr("create center vertex error"), tr("no facet selected or topology error"));
	}
	updateActions();
}