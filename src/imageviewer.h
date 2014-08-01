//  imageviewer.h
//  highgui
//
//  Created by Romulo Bourget on 2/18/14.
//  Copyright (c) 2014 CAIB. All rights reserved.
//
//  Name: Rômulo Bourget Novas
//  Tel: +55 19 99735-7010
//  Email: romulo.bnovas@gmail.com

#ifndef __imageviewer__
#define __imageviewer__

#include <QPluginLoader>
#include <QApplication>
#include <QImageWriter>
#include <QResource>
#include <QEvent>
#include <QImage>
#include <QDir>

#include <QMainWindow>
#include <QScrollArea>
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollBar>
#include <QMenuBar>
#include <QAction>
#include <QLabel>
#include <QMenu>

namespace caib {

    class ImageViewer : public QMainWindow
    {
        Q_OBJECT

    public:
        ImageViewer(QWidget *parent = 0);
        ~ImageViewer();     

    private:
        void createMenus();
        void loadPlugins();
        void createActions();
        void updateActions();
        void scaleImage(const double &factor);
        void adjustScrollBar(QScrollBar *scrollBar, const double &factor);

        class ImageWindow : public QMainWindow
        {

        public:
            ImageWindow(ImageViewer *_manager, QWidget *parent = 0) : QMainWindow(parent)
            {
                manager = _manager;
                scaleFactor = 0;

                imageLabel = new QLabel();
                imageLabel->setBackgroundRole(QPalette::Base);
                imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                imageLabel->setScaledContents(true);

                scrollArea = new QScrollArea();
                scrollArea->setBackgroundRole(QPalette::Dark);
                scrollArea->setWidget(imageLabel);
                setCentralWidget(scrollArea);
            }

        public:
            QScrollArea *scrollArea;
            QLabel *imageLabel;
            QImage image;

            double scaleFactor;
            bool isFitToWin;

        protected:
            bool event(QEvent *e)
            {
                manager->managedWindowEvent(this, e);
                return QMainWindow::event(e);
            }

        private:
            ImageViewer *manager;
        };

    protected:
        bool event(QEvent *e);

    private slots:
        void open();
        void save();
        void about();
        void zoomIn();
        void zoomOut();
        void normalSize();
        void fitToWindow();
        QImage getCurrentImage();
        void displayImage(QImage image);
        void managedWindowEvent(ImageWindow* window, QEvent *e);

    private:

        QLabel *logoLabel;

        QAction *openAct;
        QAction *saveAct;
        QAction *exitAct;
        QAction *zoomInAct;
        QAction *zoomOutAct;
        QAction *normalSizeAct;
        QAction *fitToWindowAct;
        QAction *aboutAct;

        QMenu *fileMenu;
        QMenu *pluginsMenu;
        QMenu *viewMenu;
        QMenu *helpMenu;

        QList<ImageWindow *> windowManager;
        QList<QAction *> pluginManager;
        ImageWindow *activeWindow;
    };

};


#endif // __imageviewer__
