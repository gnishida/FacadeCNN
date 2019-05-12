/********************************************************************************
** Form generated from reading UI file 'ImageGenerationDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEGENERATIONDIALOG_H
#define UI_IMAGEGENERATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImageGenerationDialog
{
public:
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QLabel *label;
    QLineEdit *lineEditOutputDirectory;
    QPushButton *pushButtonOutputDirectory;
    QLabel *label_2;
    QLineEdit *lineEditNumImages;
    QPushButton *pushButtonNumImages;
    QLabel *label_3;
    QLineEdit *lineEditImageSize;
    QCheckBox *checkBoxGrayscale;
    QLabel *label_4;
    QLineEdit *lineEditEdgeDisplacement;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLineEdit *lineEditMissingWindows;
    QLabel *label_9;

    void setupUi(QWidget *ImageGenerationDialog)
    {
        if (ImageGenerationDialog->objectName().isEmpty())
            ImageGenerationDialog->setObjectName(QStringLiteral("ImageGenerationDialog"));
        ImageGenerationDialog->resize(531, 190);
        pushButtonOK = new QPushButton(ImageGenerationDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(320, 150, 91, 31));
        pushButtonCancel = new QPushButton(ImageGenerationDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(420, 150, 91, 31));
        label = new QLabel(ImageGenerationDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 111, 21));
        lineEditOutputDirectory = new QLineEdit(ImageGenerationDialog);
        lineEditOutputDirectory->setObjectName(QStringLiteral("lineEditOutputDirectory"));
        lineEditOutputDirectory->setGeometry(QRect(130, 20, 341, 20));
        pushButtonOutputDirectory = new QPushButton(ImageGenerationDialog);
        pushButtonOutputDirectory->setObjectName(QStringLiteral("pushButtonOutputDirectory"));
        pushButtonOutputDirectory->setGeometry(QRect(480, 20, 31, 21));
        label_2 = new QLabel(ImageGenerationDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 50, 121, 21));
        lineEditNumImages = new QLineEdit(ImageGenerationDialog);
        lineEditNumImages->setObjectName(QStringLiteral("lineEditNumImages"));
        lineEditNumImages->setGeometry(QRect(130, 50, 341, 20));
        pushButtonNumImages = new QPushButton(ImageGenerationDialog);
        pushButtonNumImages->setObjectName(QStringLiteral("pushButtonNumImages"));
        pushButtonNumImages->setGeometry(QRect(480, 50, 31, 21));
        label_3 = new QLabel(ImageGenerationDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 80, 101, 21));
        lineEditImageSize = new QLineEdit(ImageGenerationDialog);
        lineEditImageSize->setObjectName(QStringLiteral("lineEditImageSize"));
        lineEditImageSize->setGeometry(QRect(130, 80, 51, 20));
        checkBoxGrayscale = new QCheckBox(ImageGenerationDialog);
        checkBoxGrayscale->setObjectName(QStringLiteral("checkBoxGrayscale"));
        checkBoxGrayscale->setGeometry(QRect(310, 82, 70, 17));
        label_4 = new QLabel(ImageGenerationDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(270, 80, 31, 21));
        lineEditEdgeDisplacement = new QLineEdit(ImageGenerationDialog);
        lineEditEdgeDisplacement->setObjectName(QStringLiteral("lineEditEdgeDisplacement"));
        lineEditEdgeDisplacement->setGeometry(QRect(143, 110, 51, 20));
        label_5 = new QLabel(ImageGenerationDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 110, 101, 21));
        label_6 = new QLabel(ImageGenerationDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(200, 110, 20, 21));
        label_7 = new QLabel(ImageGenerationDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(130, 110, 16, 21));
        label_8 = new QLabel(ImageGenerationDialog);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(293, 110, 101, 21));
        lineEditMissingWindows = new QLineEdit(ImageGenerationDialog);
        lineEditMissingWindows->setObjectName(QStringLiteral("lineEditMissingWindows"));
        lineEditMissingWindows->setGeometry(QRect(403, 110, 51, 20));
        label_9 = new QLabel(ImageGenerationDialog);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(459, 110, 20, 21));

        retranslateUi(ImageGenerationDialog);

        QMetaObject::connectSlotsByName(ImageGenerationDialog);
    } // setupUi

    void retranslateUi(QWidget *ImageGenerationDialog)
    {
        ImageGenerationDialog->setWindowTitle(QApplication::translate("ImageGenerationDialog", "ImageGenerationDialog", 0));
        pushButtonOK->setText(QApplication::translate("ImageGenerationDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("ImageGenerationDialog", "Cancel", 0));
        label->setText(QApplication::translate("ImageGenerationDialog", "Outputd directory:", 0));
        pushButtonOutputDirectory->setText(QApplication::translate("ImageGenerationDialog", "...", 0));
        label_2->setText(QApplication::translate("ImageGenerationDialog", "# images / grammar:", 0));
        pushButtonNumImages->setText(QApplication::translate("ImageGenerationDialog", "...", 0));
        label_3->setText(QApplication::translate("ImageGenerationDialog", "Image size:", 0));
        checkBoxGrayscale->setText(QApplication::translate("ImageGenerationDialog", "Grayscale", 0));
        label_4->setText(QApplication::translate("ImageGenerationDialog", "Color:", 0));
        label_5->setText(QApplication::translate("ImageGenerationDialog", "Edge position noise:", 0));
        label_6->setText(QApplication::translate("ImageGenerationDialog", "%", 0));
        label_7->setText(QApplication::translate("ImageGenerationDialog", "~", 0));
        label_8->setText(QApplication::translate("ImageGenerationDialog", "Missing windows:", 0));
        label_9->setText(QApplication::translate("ImageGenerationDialog", "%", 0));
    } // retranslateUi

};

namespace Ui {
    class ImageGenerationDialog: public Ui_ImageGenerationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEGENERATIONDIALOG_H
