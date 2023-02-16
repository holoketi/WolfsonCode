/********************************************************************************
** Form generated from reading UI file 'holograminteractive.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOLOGRAMINTERACTIVE_H
#define UI_HOLOGRAMINTERACTIVE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HologramInteractiveClass
{
public:
    QAction *actionGeneration;
    QAction *actionReconstruction;
    QAction *actionOpen_Model;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFILE;
    QMenu *menuHologram;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *HologramInteractiveClass)
    {
        if (HologramInteractiveClass->objectName().isEmpty())
            HologramInteractiveClass->setObjectName(QStringLiteral("HologramInteractiveClass"));
        HologramInteractiveClass->resize(1294, 855);
        actionGeneration = new QAction(HologramInteractiveClass);
        actionGeneration->setObjectName(QStringLiteral("actionGeneration"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/HologramInteractive/Resources/hologram_generation.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGeneration->setIcon(icon);
        actionReconstruction = new QAction(HologramInteractiveClass);
        actionReconstruction->setObjectName(QStringLiteral("actionReconstruction"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/HologramInteractive/Resources/hologram_reconstruction.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReconstruction->setIcon(icon1);
        actionOpen_Model = new QAction(HologramInteractiveClass);
        actionOpen_Model->setObjectName(QStringLiteral("actionOpen_Model"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/HologramInteractive/Resources/open.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_Model->setIcon(icon2);
        centralWidget = new QWidget(HologramInteractiveClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        HologramInteractiveClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(HologramInteractiveClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1294, 26));
        menuFILE = new QMenu(menuBar);
        menuFILE->setObjectName(QStringLiteral("menuFILE"));
        menuHologram = new QMenu(menuBar);
        menuHologram->setObjectName(QStringLiteral("menuHologram"));
        HologramInteractiveClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(HologramInteractiveClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        HologramInteractiveClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(HologramInteractiveClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        HologramInteractiveClass->setStatusBar(statusBar);

        menuBar->addAction(menuFILE->menuAction());
        menuBar->addAction(menuHologram->menuAction());
        menuFILE->addAction(actionOpen_Model);
        menuHologram->addAction(actionGeneration);
        menuHologram->addAction(actionReconstruction);
        mainToolBar->addAction(actionOpen_Model);
        mainToolBar->addAction(actionGeneration);
        mainToolBar->addAction(actionReconstruction);

        retranslateUi(HologramInteractiveClass);

        QMetaObject::connectSlotsByName(HologramInteractiveClass);
    } // setupUi

    void retranslateUi(QMainWindow *HologramInteractiveClass)
    {
        HologramInteractiveClass->setWindowTitle(QApplication::translate("HologramInteractiveClass", "HologramInteractive", 0));
        actionGeneration->setText(QApplication::translate("HologramInteractiveClass", "Generation", 0));
        actionGeneration->setIconText(QApplication::translate("HologramInteractiveClass", "Generation CGH", 0));
#ifndef QT_NO_TOOLTIP
        actionGeneration->setToolTip(QApplication::translate("HologramInteractiveClass", "Generation CGH", 0));
#endif // QT_NO_TOOLTIP
        actionGeneration->setShortcut(QApplication::translate("HologramInteractiveClass", "F5", 0));
        actionReconstruction->setText(QApplication::translate("HologramInteractiveClass", "Reconstruction", 0));
        actionReconstruction->setShortcut(QApplication::translate("HologramInteractiveClass", "F6", 0));
        actionOpen_Model->setText(QApplication::translate("HologramInteractiveClass", "Open Model", 0));
        actionOpen_Model->setShortcut(QApplication::translate("HologramInteractiveClass", "F2", 0));
        menuFILE->setTitle(QApplication::translate("HologramInteractiveClass", "File", 0));
        menuHologram->setTitle(QApplication::translate("HologramInteractiveClass", "Hologram", 0));
    } // retranslateUi

};

namespace Ui {
    class HologramInteractiveClass: public Ui_HologramInteractiveClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOLOGRAMINTERACTIVE_H
