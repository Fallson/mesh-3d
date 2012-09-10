/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
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

#include "filepropertydialog.h"

FilePropertyDialog::FilePropertyDialog(const QString &fileName, int vernum, int edgenum, int facenum, QWidget *parent)
    : QDialog(parent)
{
    QFileInfo fileInfo(fileName);

    tabWidget = new QTabWidget;
    tabWidget->addTab(new GeneralTab(fileInfo), tr("General"));
	tabWidget->addTab(new MeshTab(vernum,edgenum,facenum), tr("Meshinfo"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("File Property Dialog"));
}

/************************************************************************/
/* General Tab                                                          */
/************************************************************************/
GeneralTab::GeneralTab(const QFileInfo &fileInfo, QWidget *parent)
    : QWidget(parent)
{
    QLabel *fileNameLabel = new QLabel(tr("File Name:"));
    QLabel *fileNameValueLabel = new QLabel(fileInfo.fileName());
	fileNameValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *pathLabel = new QLabel(tr("Path:"));
    QLabel *pathValueLabel = new QLabel(fileInfo.absoluteFilePath());
    pathValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *sizeLabel = new QLabel(tr("Size:"));
    qlonglong size = fileInfo.size()/1024;
    QLabel *sizeValueLabel = new QLabel(tr("%1 K").arg(size));
    sizeValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *lastReadLabel = new QLabel(tr("Last Read:"));
    QLabel *lastReadValueLabel = new QLabel(fileInfo.lastRead().toString());
    lastReadValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *lastModLabel = new QLabel(tr("Last Modified:"));
    QLabel *lastModValueLabel = new QLabel(fileInfo.lastModified().toString());
    lastModValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(fileNameLabel);
    mainLayout->addWidget(fileNameValueLabel);
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(pathValueLabel);
    mainLayout->addWidget(sizeLabel);
    mainLayout->addWidget(sizeValueLabel);
    mainLayout->addWidget(lastReadLabel);
    mainLayout->addWidget(lastReadValueLabel);
    mainLayout->addWidget(lastModLabel);
    mainLayout->addWidget(lastModValueLabel);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

MeshTab::MeshTab(int vernum, int edgenum, int facenum, QWidget *parent)
	: QWidget(parent)
{
	QLabel *vernumLabel = new QLabel(tr("Vertices number:"));
	QLabel *vernumValueLabel = new QLabel(QString::number(vernum));
	vernumValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QLabel *edgenumLabel = new QLabel(tr("Edges number:"));
	QLabel *edgenumValueLabel = new QLabel(QString::number(edgenum));
	edgenumValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QLabel *facenumLabel = new QLabel(tr("Facets number:"));
	QLabel *facenumValueLabel = new QLabel(QString::number(facenum));
	facenumValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(vernumLabel);
	mainLayout->addWidget(vernumValueLabel);
	mainLayout->addWidget(edgenumLabel);
	mainLayout->addWidget(edgenumValueLabel);
	mainLayout->addWidget(facenumLabel);
	mainLayout->addWidget(facenumValueLabel);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
