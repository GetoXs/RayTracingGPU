/********************************************************************************
** Form generated from reading UI file 'raytracinggpu.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAYTRACINGGPU_H
#define UI_RAYTRACINGGPU_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RayTracingGPUClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RayTracingGPUClass)
    {
        if (RayTracingGPUClass->objectName().isEmpty())
            RayTracingGPUClass->setObjectName(QStringLiteral("RayTracingGPUClass"));
        RayTracingGPUClass->resize(600, 400);
        menuBar = new QMenuBar(RayTracingGPUClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        RayTracingGPUClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RayTracingGPUClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        RayTracingGPUClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(RayTracingGPUClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        RayTracingGPUClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(RayTracingGPUClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        RayTracingGPUClass->setStatusBar(statusBar);

        retranslateUi(RayTracingGPUClass);

        QMetaObject::connectSlotsByName(RayTracingGPUClass);
    } // setupUi

    void retranslateUi(QMainWindow *RayTracingGPUClass)
    {
        RayTracingGPUClass->setWindowTitle(QApplication::translate("RayTracingGPUClass", "RayTracingGPU", 0));
    } // retranslateUi

};

namespace Ui {
    class RayTracingGPUClass: public Ui_RayTracingGPUClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAYTRACINGGPU_H
