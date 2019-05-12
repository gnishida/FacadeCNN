/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

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

class Ui_MainWindowClass
{
public:
    QAction *actionOpenImage;
    QAction *actionExit;
    QAction *actionParameterEstimationAll;
    QAction *actionNew;
    QAction *actionOpenCGA;
    QAction *actionOpenContour;
    QAction *actionSaveContour;
    QAction *actionUndo;
    QAction *actionExtractCameraParameter;
    QAction *actionParameterEstimationWithCameraCalibration;
    QAction *actionGenerateTrainingImages;
    QAction *actionParameterEstimation;
    QAction *actionGenerateTrainingImagesFromExamples;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuTool;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(800, 853);
        actionOpenImage = new QAction(MainWindowClass);
        actionOpenImage->setObjectName(QStringLiteral("actionOpenImage"));
        actionExit = new QAction(MainWindowClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionParameterEstimationAll = new QAction(MainWindowClass);
        actionParameterEstimationAll->setObjectName(QStringLiteral("actionParameterEstimationAll"));
        actionNew = new QAction(MainWindowClass);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        actionOpenCGA = new QAction(MainWindowClass);
        actionOpenCGA->setObjectName(QStringLiteral("actionOpenCGA"));
        actionOpenContour = new QAction(MainWindowClass);
        actionOpenContour->setObjectName(QStringLiteral("actionOpenContour"));
        actionSaveContour = new QAction(MainWindowClass);
        actionSaveContour->setObjectName(QStringLiteral("actionSaveContour"));
        actionUndo = new QAction(MainWindowClass);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        actionExtractCameraParameter = new QAction(MainWindowClass);
        actionExtractCameraParameter->setObjectName(QStringLiteral("actionExtractCameraParameter"));
        actionParameterEstimationWithCameraCalibration = new QAction(MainWindowClass);
        actionParameterEstimationWithCameraCalibration->setObjectName(QStringLiteral("actionParameterEstimationWithCameraCalibration"));
        actionGenerateTrainingImages = new QAction(MainWindowClass);
        actionGenerateTrainingImages->setObjectName(QStringLiteral("actionGenerateTrainingImages"));
        actionParameterEstimation = new QAction(MainWindowClass);
        actionParameterEstimation->setObjectName(QStringLiteral("actionParameterEstimation"));
        actionGenerateTrainingImagesFromExamples = new QAction(MainWindowClass);
        actionGenerateTrainingImagesFromExamples->setObjectName(QStringLiteral("actionGenerateTrainingImagesFromExamples"));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuTool = new QMenu(menuBar);
        menuTool->setObjectName(QStringLiteral("menuTool"));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuTool->menuAction());
        menuFile->addAction(actionExit);
        menuTool->addAction(actionGenerateTrainingImages);
        menuTool->addSeparator();
        menuTool->addAction(actionParameterEstimationAll);
        menuTool->addAction(actionParameterEstimation);
        menuTool->addSeparator();

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "Facade CNN", 0));
        actionOpenImage->setText(QApplication::translate("MainWindowClass", "Open Image", 0));
        actionExit->setText(QApplication::translate("MainWindowClass", "Exit", 0));
        actionParameterEstimationAll->setText(QApplication::translate("MainWindowClass", "Parameter Estimation All", 0));
        actionParameterEstimationAll->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+P", 0));
        actionNew->setText(QApplication::translate("MainWindowClass", "New", 0));
        actionNew->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+N", 0));
        actionOpenCGA->setText(QApplication::translate("MainWindowClass", "Open CGA", 0));
        actionOpenContour->setText(QApplication::translate("MainWindowClass", "Open Contour", 0));
        actionOpenContour->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", 0));
        actionSaveContour->setText(QApplication::translate("MainWindowClass", "Save Contour", 0));
        actionSaveContour->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+S", 0));
        actionUndo->setText(QApplication::translate("MainWindowClass", "Undo", 0));
        actionUndo->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+Z", 0));
        actionExtractCameraParameter->setText(QApplication::translate("MainWindowClass", "Extract Camera Parameter", 0));
        actionParameterEstimationWithCameraCalibration->setText(QApplication::translate("MainWindowClass", "Parameter Estimation with Camera Calibration", 0));
        actionGenerateTrainingImages->setText(QApplication::translate("MainWindowClass", "Generate Training Images", 0));
        actionParameterEstimation->setText(QApplication::translate("MainWindowClass", "Parameter Estimation", 0));
        actionGenerateTrainingImagesFromExamples->setText(QApplication::translate("MainWindowClass", "Generate Training Images From Examples", 0));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0));
        menuTool->setTitle(QApplication::translate("MainWindowClass", "Tool", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
