/********************************************************************************
** Form generated from reading UI file 'mydX7292.ui'
**
** Created: Thu 17. May 09:46:48 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MYDX7292_H
#define MYDX7292_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>
#include "qwt_counter.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_Scene;
    QAction *actionSave_Scene;
    QAction *actionLoad_Scene;
    QAction *actionExit;
    QAction *actionGenerate_Random;
    QAction *actionSava_as_HMap;
    QAction *actionLoad_Texture;
    QAction *actionAddEntity;
    QAction *actionDeleteEntity;
    QAction *actionOnline_Guide;
    QAction *actionAbout;
    QAction *actionEnable_Shadow;
    QAction *actionEnable_Animation;
    QAction *actionEnable_Light;
    QAction *actionCreate_Light;
    QWidget *centralwidget;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab_2;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *labelCurrentPos;
    QwtCounter *CounterScaleX;
    QwtCounter *CounterScaleY;
    QwtCounter *CounterScaleZ;
    QwtCounter *CounterRotX;
    QwtCounter *CounterRotY;
    QwtCounter *CounterRotZ;
    QwtCounter *CounterTraY;
    QwtCounter *CounterTraZ;
    QwtCounter *CounterTraX;
    QLabel *label_7;
    QLabel *labelCurrentScale;
    QWidget *tab_Animation;
    QComboBox *comboBox_Animation;
    QListWidget *listWidget;
    QLabel *label_2;
    QLabel *label_5;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuTerrain;
    QMenu *menuModels;
    QMenu *menuHelp;
    QMenu *menuSettings;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 768);
        MainWindow->setMinimumSize(QSize(1024, 768));
        MainWindow->setMaximumSize(QSize(1024, 768));
        actionNew_Scene = new QAction(MainWindow);
        actionNew_Scene->setObjectName(QString::fromUtf8("actionNew_Scene"));
        actionSave_Scene = new QAction(MainWindow);
        actionSave_Scene->setObjectName(QString::fromUtf8("actionSave_Scene"));
        actionLoad_Scene = new QAction(MainWindow);
        actionLoad_Scene->setObjectName(QString::fromUtf8("actionLoad_Scene"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionGenerate_Random = new QAction(MainWindow);
        actionGenerate_Random->setObjectName(QString::fromUtf8("actionGenerate_Random"));
        actionSava_as_HMap = new QAction(MainWindow);
        actionSava_as_HMap->setObjectName(QString::fromUtf8("actionSava_as_HMap"));
        actionLoad_Texture = new QAction(MainWindow);
        actionLoad_Texture->setObjectName(QString::fromUtf8("actionLoad_Texture"));
        actionAddEntity = new QAction(MainWindow);
        actionAddEntity->setObjectName(QString::fromUtf8("actionAddEntity"));
        actionDeleteEntity = new QAction(MainWindow);
        actionDeleteEntity->setObjectName(QString::fromUtf8("actionDeleteEntity"));
        actionOnline_Guide = new QAction(MainWindow);
        actionOnline_Guide->setObjectName(QString::fromUtf8("actionOnline_Guide"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionEnable_Shadow = new QAction(MainWindow);
        actionEnable_Shadow->setObjectName(QString::fromUtf8("actionEnable_Shadow"));
        actionEnable_Shadow->setCheckable(true);
        actionEnable_Animation = new QAction(MainWindow);
        actionEnable_Animation->setObjectName(QString::fromUtf8("actionEnable_Animation"));
        actionEnable_Animation->setCheckable(true);
        actionEnable_Light = new QAction(MainWindow);
        actionEnable_Light->setObjectName(QString::fromUtf8("actionEnable_Light"));
        actionCreate_Light = new QAction(MainWindow);
        actionCreate_Light->setObjectName(QString::fromUtf8("actionCreate_Light"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayoutWidget_2 = new QWidget(centralwidget);
        horizontalLayoutWidget_2->setObjectName(QString::fromUtf8("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(219, 19, 791, 511));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayoutWidget_3 = new QWidget(centralwidget);
        horizontalLayoutWidget_3->setObjectName(QString::fromUtf8("horizontalLayoutWidget_3"));
        horizontalLayoutWidget_3->setGeometry(QRect(220, 540, 791, 181));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(horizontalLayoutWidget_3);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        label = new QLabel(tab_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(35, 20, 46, 13));
        label_3 = new QLabel(tab_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(155, 20, 46, 13));
        label_4 = new QLabel(tab_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(275, 20, 46, 13));
        label_6 = new QLabel(tab_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(390, 20, 101, 16));
        labelCurrentPos = new QLabel(tab_2);
        labelCurrentPos->setObjectName(QString::fromUtf8("labelCurrentPos"));
        labelCurrentPos->setGeometry(QRect(390, 35, 231, 16));
        CounterScaleX = new QwtCounter(tab_2);
        CounterScaleX->setObjectName(QString::fromUtf8("CounterScaleX"));
        CounterScaleX->setGeometry(QRect(10, 40, 101, 20));
        CounterScaleX->setProperty("basicstep", QVariant(0.2));
        CounterScaleX->setMinValue(-9999);
        CounterScaleX->setMaxValue(9999);
        CounterScaleX->setStepButton2(10);
        CounterScaleX->setValue(0);
        CounterScaleY = new QwtCounter(tab_2);
        CounterScaleY->setObjectName(QString::fromUtf8("CounterScaleY"));
        CounterScaleY->setGeometry(QRect(10, 60, 101, 20));
        CounterScaleY->setProperty("basicstep", QVariant(0.2));
        CounterScaleY->setMinValue(-9999);
        CounterScaleY->setMaxValue(9999);
        CounterScaleY->setStepButton2(10);
        CounterScaleY->setValue(0);
        CounterScaleZ = new QwtCounter(tab_2);
        CounterScaleZ->setObjectName(QString::fromUtf8("CounterScaleZ"));
        CounterScaleZ->setGeometry(QRect(10, 80, 101, 20));
        CounterScaleZ->setProperty("basicstep", QVariant(0.2));
        CounterScaleZ->setMinValue(-9999);
        CounterScaleZ->setMaxValue(9999);
        CounterScaleZ->setStepButton2(10);
        CounterScaleZ->setValue(0);
        CounterRotX = new QwtCounter(tab_2);
        CounterRotX->setObjectName(QString::fromUtf8("CounterRotX"));
        CounterRotX->setGeometry(QRect(130, 40, 101, 20));
        CounterRotX->setProperty("basicstep", QVariant(0.2));
        CounterRotX->setMinValue(-9999);
        CounterRotX->setMaxValue(9999);
        CounterRotX->setStepButton2(10);
        CounterRotX->setValue(0);
        CounterRotY = new QwtCounter(tab_2);
        CounterRotY->setObjectName(QString::fromUtf8("CounterRotY"));
        CounterRotY->setGeometry(QRect(130, 60, 101, 20));
        CounterRotY->setProperty("basicstep", QVariant(0.2));
        CounterRotY->setMinValue(-9999);
        CounterRotY->setMaxValue(9999);
        CounterRotY->setStepButton2(10);
        CounterRotY->setValue(0);
        CounterRotZ = new QwtCounter(tab_2);
        CounterRotZ->setObjectName(QString::fromUtf8("CounterRotZ"));
        CounterRotZ->setGeometry(QRect(130, 80, 101, 20));
        CounterRotZ->setProperty("basicstep", QVariant(0.2));
        CounterRotZ->setMinValue(-9999);
        CounterRotZ->setMaxValue(9999);
        CounterRotZ->setStepButton2(10);
        CounterRotZ->setValue(0);
        CounterTraY = new QwtCounter(tab_2);
        CounterTraY->setObjectName(QString::fromUtf8("CounterTraY"));
        CounterTraY->setGeometry(QRect(250, 60, 101, 20));
        CounterTraY->setProperty("basicstep", QVariant(0.2));
        CounterTraY->setMinValue(-9999);
        CounterTraY->setMaxValue(9999);
        CounterTraY->setStepButton2(10);
        CounterTraY->setValue(0);
        CounterTraZ = new QwtCounter(tab_2);
        CounterTraZ->setObjectName(QString::fromUtf8("CounterTraZ"));
        CounterTraZ->setGeometry(QRect(250, 80, 101, 20));
        CounterTraZ->setProperty("basicstep", QVariant(0.2));
        CounterTraZ->setMinValue(-9999);
        CounterTraZ->setMaxValue(9999);
        CounterTraZ->setStepButton2(10);
        CounterTraZ->setValue(0);
        CounterTraX = new QwtCounter(tab_2);
        CounterTraX->setObjectName(QString::fromUtf8("CounterTraX"));
        CounterTraX->setGeometry(QRect(250, 40, 101, 20));
        CounterTraX->setProperty("basicstep", QVariant(0.2));
        CounterTraX->setMinValue(-9999);
        CounterTraX->setMaxValue(9999);
        CounterTraX->setStepButton2(10);
        CounterTraX->setValue(0);
        label_7 = new QLabel(tab_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(390, 60, 101, 16));
        labelCurrentScale = new QLabel(tab_2);
        labelCurrentScale->setObjectName(QString::fromUtf8("labelCurrentScale"));
        labelCurrentScale->setGeometry(QRect(390, 75, 231, 16));
        tabWidget->addTab(tab_2, QString());
        tab_Animation = new QWidget();
        tab_Animation->setObjectName(QString::fromUtf8("tab_Animation"));
        comboBox_Animation = new QComboBox(tab_Animation);
        comboBox_Animation->setObjectName(QString::fromUtf8("comboBox_Animation"));
        comboBox_Animation->setGeometry(QRect(10, 10, 201, 21));
        tabWidget->addTab(tab_Animation, QString());

        horizontalLayout_3->addWidget(tabWidget);

        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(10, 18, 199, 701));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 0, 46, 13));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(220, 0, 61, 16));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1024, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuTerrain = new QMenu(menubar);
        menuTerrain->setObjectName(QString::fromUtf8("menuTerrain"));
        menuModels = new QMenu(menubar);
        menuModels->setObjectName(QString::fromUtf8("menuModels"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName(QString::fromUtf8("menuSettings"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuTerrain->menuAction());
        menubar->addAction(menuModels->menuAction());
        menubar->addAction(menuSettings->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew_Scene);
        menuFile->addAction(actionSave_Scene);
        menuFile->addAction(actionLoad_Scene);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuTerrain->addAction(actionGenerate_Random);
        menuTerrain->addAction(actionSava_as_HMap);
        menuTerrain->addSeparator();
        menuTerrain->addAction(actionLoad_Texture);
        menuModels->addAction(actionAddEntity);
        menuModels->addAction(actionDeleteEntity);
        menuModels->addSeparator();
        menuModels->addAction(actionCreate_Light);
        menuHelp->addAction(actionOnline_Guide);
        menuHelp->addAction(actionAbout);
        menuSettings->addAction(actionEnable_Shadow);
        menuSettings->addAction(actionEnable_Animation);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Level Generator", 0, QApplication::UnicodeUTF8));
        actionNew_Scene->setText(QApplication::translate("MainWindow", "New Scene", 0, QApplication::UnicodeUTF8));
        actionSave_Scene->setText(QApplication::translate("MainWindow", "Save Scene", 0, QApplication::UnicodeUTF8));
        actionLoad_Scene->setText(QApplication::translate("MainWindow", "Load Scene", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        actionGenerate_Random->setText(QApplication::translate("MainWindow", "Generate Random", 0, QApplication::UnicodeUTF8));
        actionSava_as_HMap->setText(QApplication::translate("MainWindow", "Sava as HMap", 0, QApplication::UnicodeUTF8));
        actionLoad_Texture->setText(QApplication::translate("MainWindow", "Load Texture", 0, QApplication::UnicodeUTF8));
        actionAddEntity->setText(QApplication::translate("MainWindow", "Add Entity", 0, QApplication::UnicodeUTF8));
        actionDeleteEntity->setText(QApplication::translate("MainWindow", "Delete Entity", 0, QApplication::UnicodeUTF8));
        actionOnline_Guide->setText(QApplication::translate("MainWindow", "Online Guide", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        actionEnable_Shadow->setText(QApplication::translate("MainWindow", "Enable Shadow", 0, QApplication::UnicodeUTF8));
        actionEnable_Animation->setText(QApplication::translate("MainWindow", "Enable Animation", 0, QApplication::UnicodeUTF8));
        actionEnable_Light->setText(QApplication::translate("MainWindow", "Enable Light", 0, QApplication::UnicodeUTF8));
        actionCreate_Light->setText(QApplication::translate("MainWindow", "Create Light", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Scale", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Rotation", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Traslation", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Current Position", 0, QApplication::UnicodeUTF8));
        labelCurrentPos->setText(QApplication::translate("MainWindow", "[x = 0; y = 0; z = 0]", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Current Scale", 0, QApplication::UnicodeUTF8));
        labelCurrentScale->setText(QApplication::translate("MainWindow", "[x = 0; y = 0; z = 0]", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Modify...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_Animation), QApplication::translate("MainWindow", "Animation", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Items List", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "View Scene", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuTerrain->setTitle(QApplication::translate("MainWindow", "Terrain", 0, QApplication::UnicodeUTF8));
        menuModels->setTitle(QApplication::translate("MainWindow", "Models", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
        menuSettings->setTitle(QApplication::translate("MainWindow", "Settings", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MYDX7292_H
