//  imageviewer.cpp
//  highgui
//
//  Created by Romulo Bourget on 2/18/14.
//  Copyright (c) 2014 CAIB. All rights reserved.
//
//  Name: Rômulo Bourget Novas
//  Tel: +55 19 99735-7010
//  Email: romulo.bnovas@gmail.com

#include "imageviewer.h"
#include "plugininterface.h"

caib::ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent) {

    logoLabel = new QLabel();
    logoLabel->setBackgroundRole(QPalette::Base);
    logoLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    logoLabel->setScaledContents(true);

    setCentralWidget(logoLabel);

    createActions();
    createMenus();
    QImage *logo = new QImage(":/images/caib.jpg");
    logoLabel->setPixmap( QPixmap::fromImage( *(logo) ) );
    logoLabel->adjustSize();

    setFixedSize(logo->size());
    setWindowTitle("BIVi");
}

caib::ImageViewer::~ImageViewer() {

}

void caib::ImageViewer::loadPlugins() {

    QDir pluginsDir(qApp->applicationDirPath());

#ifdef Q_OS_MAC
    if(pluginsDir.dirName() == "MacOS")
        pluginsDir.cdUp();
#endif

    if(!pluginsDir.cd("Plugins")) {
        QMessageBox::information(this, tr("ERROR"), tr("Cannot find plugins directory."));
        return;

    } else {

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {

            QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = pluginLoader.instance();

            if (plugin) {
                PluginInterface *pluginInterface = qobject_cast<PluginInterface*>(plugin);
                if (pluginInterface) {

                    QAction *pluginAction = new QAction(pluginInterface->getPluginName(), this);
                    pluginAction->setEnabled(false);

                    pluginManager.append(pluginAction);

                    connect(pluginManager.last(), SIGNAL(triggered()), pluginInterface, SLOT(execute()));

                    connect(pluginInterface, SIGNAL(displayImage(QImage)), this, SLOT(displayImage(QImage)));
                    connect(pluginInterface, SIGNAL(getCurrentImage()), this, SLOT(getCurrentImage()));
                }
            }

        }

    }
}

void caib::ImageViewer::createActions() {
    openAct = new QAction(tr("&Open Image"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save Image"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setEnabled(false);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+N"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    loadPlugins();

}

void caib::ImageViewer::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(exitAct);

    pluginsMenu = new QMenu(tr("Plugins"), this);
    pluginsMenu->addActions(pluginManager);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(pluginsMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);

}

void caib::ImageViewer::scaleImage(const double &factor) {
    Q_ASSERT(activeWindow->imageLabel->pixmap());
    activeWindow->scaleFactor *= factor;
    activeWindow->imageLabel->resize(activeWindow->scaleFactor * activeWindow->imageLabel->pixmap()->size());

    adjustScrollBar(activeWindow->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(activeWindow->scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(activeWindow->scaleFactor < 3.0);
    zoomOutAct->setEnabled(activeWindow->scaleFactor > 0.333);
}

void caib::ImageViewer::adjustScrollBar(QScrollBar *scrollBar, const double &factor) {
   scrollBar->setValue( int( factor * scrollBar->value() + ( (factor - 1) * scrollBar->pageStep()/2 ) ) );
}

void caib::ImageViewer::updateActions() {
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void caib::ImageViewer::open() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath() , tr("All Images (*.jpg *.bmp *.tif );; JPG(*.jpg);; TIFF(*.tif);; BMP(*.bmp)"));

    if (!fileName.isEmpty()) {
        QImage image(fileName);

        if (image.isNull()) {
            QMessageBox::information(this, tr("ERROR"), tr("Cannot load %1.").arg(fileName));
            return;

        } else {
            displayImage(image);
        }
    }
}

void caib::ImageViewer::save() {
    QString savingFile = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(), tr("JPG(*.jpg);; TIFF(*.tif);; BMP(*.bmp)"));
    if(!savingFile.isEmpty())
        QImageWriter(savingFile).write(activeWindow->image);
}

void caib::ImageViewer::zoomIn() {
    scaleImage(1.25);
}

void caib::ImageViewer::zoomOut() {
    scaleImage(0.75);
}


void caib::ImageViewer::normalSize() {
    activeWindow->imageLabel->adjustSize();
    activeWindow->scaleFactor = 1.0;
}

void caib::ImageViewer::fitToWindow() {
    activeWindow->scrollArea->setWidgetResizable( fitToWindowAct->isChecked() );
    if (!fitToWindowAct->isChecked()) {
        normalSize();
    }

    updateActions();

    fitToWindowAct->setChecked(!activeWindow->isFitToWin);
    activeWindow->isFitToWin = fitToWindowAct->isChecked();
}

void caib::ImageViewer::about() {
    QMessageBox::about(this, tr("About BIVi"),
            tr("<p>The <b> BIVi </b> is grafical interface that accept plugins.</p> <p>For adding a plugin just your dinamic library file to your application plugin dir. This application was denveloped by the <b>C</b>omputation <b>A</b>pplied to <b>B</b>iomedical <b>I</b>nformatics Laboratory, <b>U</b>niversity of <b>S</b>ao <b>P</b>aulo, Ribeirao Preto, Brazil</p>"));
}

void caib::ImageViewer::displayImage(QImage image) {
    activeWindow = new ImageWindow(this);

    windowManager.append(activeWindow);
    activeWindow->image = image;
    activeWindow->imageLabel->setPixmap( QPixmap::fromImage( activeWindow->image ) );

    activeWindow->scaleFactor = 1.0;
    activeWindow->isFitToWin = false;

    for (int i = 0; i < pluginManager.size(); i++) {
        pluginManager[i]->setEnabled(true);
    }

    saveAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    fitToWindowAct->setChecked(false);
    updateActions();

    if (!fitToWindowAct->isChecked())
        activeWindow->imageLabel->adjustSize();

#ifdef Q_OS_WIN
    activeWindow->addAction(openAct);
    activeWindow->addAction(saveAct);
    activeWindow->addAction(exitAct);
    activeWindow->addAction(fitToWindowAct);
    activeWindow->addAction(zoomInAct);
    activeWindow->addAction(zoomOutAct);
    activeWindow->addAction(normalSizeAct);
    activeWindow->addAction(aboutAct);
#endif

    activeWindow->setMaximumSize(activeWindow->image.size());
    activeWindow->show();

}

QImage caib::ImageViewer::getCurrentImage() {
    return activeWindow->image;
}

void caib::ImageViewer::managedWindowEvent(ImageWindow *window, QEvent *e) {
   switch ( e->type() ) {

   case QEvent::WindowActivate:

       if( activeWindow->winId() != window->winId() ) {
           activeWindow = window;

           saveAct->setEnabled(true);
           fitToWindowAct->setEnabled(true);
           zoomInAct->setEnabled(activeWindow->scaleFactor < 3.0);
           zoomOutAct->setEnabled(activeWindow->scaleFactor > 0.333);
           normalSizeAct->setEnabled(true);

           for (int i = 0; i < pluginManager.size(); i++) {
               pluginManager[i]->setEnabled(true);
           }

           fitToWindowAct->setChecked(activeWindow->isFitToWin);
           updateActions();
       }

       break;

   case QEvent::Close:

       if ( window->winId() == activeWindow->winId() ) {
           saveAct->setEnabled(false);
           fitToWindowAct->setEnabled(false);
           zoomInAct->setEnabled(false);
           zoomOutAct->setEnabled(false);
           normalSizeAct->setEnabled(false);

           for (int i = 0; i < pluginManager.size(); i++) {
               pluginManager[i]->setEnabled(false);
           }
       }

       for(int i = 0; i < windowManager.size(); i++) {
           if ( windowManager[i]->winId() == window->winId() ) {
               windowManager.removeAt(i);
               break;
           }
       }

       break;

   default:
       break;
   }

}

bool caib::ImageViewer::event(QEvent *e){
    if (e->type() == QEvent::Close) {
        while(!windowManager.empty()) {
            windowManager.last()->close();
        }
    }
    return QMainWindow::event(e);
}
