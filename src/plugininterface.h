//  plugininterface.h
//  highgui
//
//  Created by Romulo Bourget on 2/18/14.
//  Copyright (c) 2014 CAIB. All rights reserved.
//
//  Name: Rômulo Bourget Novas
//  Tel: +55 19 99735-7010
//  Email: romulo.bnovas@gmail.com

#ifndef __plugininterface__
#define __plugininterface__

#include <QString>
#include <QObject>
#include <QImage>
#include <QList>

namespace caib {

    class PluginInterface : public QObject
    {
        Q_OBJECT

    public:
        virtual ~PluginInterface() {}

    signals:
        void displayImage(QImage image);
        QImage getCurrentImage();

    public slots:
        virtual void execute() = 0;

    public:
        virtual QString getPluginName() = 0;
    };

};

#define PluginInterface_iid "org.caib.MorphometryTool.PluginInterface"
Q_DECLARE_INTERFACE(caib::PluginInterface, PluginInterface_iid)

#endif // __plugininterface__
