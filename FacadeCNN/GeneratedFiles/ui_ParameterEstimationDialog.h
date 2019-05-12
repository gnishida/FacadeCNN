/********************************************************************************
** Form generated from reading UI file 'ParameterEstimationDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARAMETERESTIMATIONDIALOG_H
#define UI_PARAMETERESTIMATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ParameterEstimationDialog
{
public:
    QLineEdit *lineEditTestDataDirectory;
    QPushButton *pushButtonCancel;
    QPushButton *pushButtonOK;
    QLabel *label_15;
    QPushButton *pushButtonTestDataDirectory;
    QLabel *label_17;
    QLineEdit *lineEditOutputDirectory;
    QPushButton *pushButtonOutputDirectory;
    QPushButton *pushButtonSegmentationOutputDirectory;
    QLabel *label_18;
    QLineEdit *lineEditSegmentationOutputDirectory;
    QPushButton *pushButtonInitialOutputDirectory;
    QLabel *label_19;
    QLineEdit *lineEditInitialOutputDirectory;

    void setupUi(QDialog *ParameterEstimationDialog)
    {
        if (ParameterEstimationDialog->objectName().isEmpty())
            ParameterEstimationDialog->setObjectName(QStringLiteral("ParameterEstimationDialog"));
        ParameterEstimationDialog->resize(522, 175);
        lineEditTestDataDirectory = new QLineEdit(ParameterEstimationDialog);
        lineEditTestDataDirectory->setObjectName(QStringLiteral("lineEditTestDataDirectory"));
        lineEditTestDataDirectory->setGeometry(QRect(130, 10, 341, 20));
        pushButtonCancel = new QPushButton(ParameterEstimationDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(420, 140, 91, 31));
        pushButtonOK = new QPushButton(ParameterEstimationDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(320, 140, 91, 31));
        label_15 = new QLabel(ParameterEstimationDialog);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(10, 10, 101, 20));
        label_15->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        pushButtonTestDataDirectory = new QPushButton(ParameterEstimationDialog);
        pushButtonTestDataDirectory->setObjectName(QStringLiteral("pushButtonTestDataDirectory"));
        pushButtonTestDataDirectory->setGeometry(QRect(480, 10, 31, 23));
        label_17 = new QLabel(ParameterEstimationDialog);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(10, 100, 121, 20));
        lineEditOutputDirectory = new QLineEdit(ParameterEstimationDialog);
        lineEditOutputDirectory->setObjectName(QStringLiteral("lineEditOutputDirectory"));
        lineEditOutputDirectory->setGeometry(QRect(130, 100, 341, 20));
        pushButtonOutputDirectory = new QPushButton(ParameterEstimationDialog);
        pushButtonOutputDirectory->setObjectName(QStringLiteral("pushButtonOutputDirectory"));
        pushButtonOutputDirectory->setGeometry(QRect(480, 100, 31, 23));
        pushButtonSegmentationOutputDirectory = new QPushButton(ParameterEstimationDialog);
        pushButtonSegmentationOutputDirectory->setObjectName(QStringLiteral("pushButtonSegmentationOutputDirectory"));
        pushButtonSegmentationOutputDirectory->setGeometry(QRect(480, 40, 31, 23));
        label_18 = new QLabel(ParameterEstimationDialog);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(10, 40, 121, 20));
        lineEditSegmentationOutputDirectory = new QLineEdit(ParameterEstimationDialog);
        lineEditSegmentationOutputDirectory->setObjectName(QStringLiteral("lineEditSegmentationOutputDirectory"));
        lineEditSegmentationOutputDirectory->setGeometry(QRect(130, 40, 341, 20));
        pushButtonInitialOutputDirectory = new QPushButton(ParameterEstimationDialog);
        pushButtonInitialOutputDirectory->setObjectName(QStringLiteral("pushButtonInitialOutputDirectory"));
        pushButtonInitialOutputDirectory->setGeometry(QRect(480, 70, 31, 23));
        label_19 = new QLabel(ParameterEstimationDialog);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(10, 70, 121, 20));
        lineEditInitialOutputDirectory = new QLineEdit(ParameterEstimationDialog);
        lineEditInitialOutputDirectory->setObjectName(QStringLiteral("lineEditInitialOutputDirectory"));
        lineEditInitialOutputDirectory->setGeometry(QRect(130, 70, 341, 20));
        QWidget::setTabOrder(lineEditTestDataDirectory, pushButtonOK);
        QWidget::setTabOrder(pushButtonOK, pushButtonCancel);

        retranslateUi(ParameterEstimationDialog);

        QMetaObject::connectSlotsByName(ParameterEstimationDialog);
    } // setupUi

    void retranslateUi(QDialog *ParameterEstimationDialog)
    {
        ParameterEstimationDialog->setWindowTitle(QApplication::translate("ParameterEstimationDialog", "ParameterEstimationDialog", 0));
        pushButtonCancel->setText(QApplication::translate("ParameterEstimationDialog", "Cancel", 0));
        pushButtonOK->setText(QApplication::translate("ParameterEstimationDialog", "OK", 0));
        label_15->setText(QApplication::translate("ParameterEstimationDialog", "Image directory:", 0));
        pushButtonTestDataDirectory->setText(QApplication::translate("ParameterEstimationDialog", "...", 0));
        label_17->setText(QApplication::translate("ParameterEstimationDialog", "Final parsing output:", 0));
        pushButtonOutputDirectory->setText(QApplication::translate("ParameterEstimationDialog", "...", 0));
        pushButtonSegmentationOutputDirectory->setText(QApplication::translate("ParameterEstimationDialog", "...", 0));
        label_18->setText(QApplication::translate("ParameterEstimationDialog", "Segmentation output:", 0));
        pushButtonInitialOutputDirectory->setText(QApplication::translate("ParameterEstimationDialog", "...", 0));
        label_19->setText(QApplication::translate("ParameterEstimationDialog", "Initial parsing output:", 0));
    } // retranslateUi

};

namespace Ui {
    class ParameterEstimationDialog: public Ui_ParameterEstimationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAMETERESTIMATIONDIALOG_H
