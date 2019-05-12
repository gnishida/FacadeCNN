#include "ParameterEstimationDialog.h"
#include <QFileDialog>

ParameterEstimationDialog::ParameterEstimationDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.lineEditTestDataDirectory->setText("facade3");
	ui.lineEditSegmentationOutputDirectory->setText("segmentation");
	ui.lineEditInitialOutputDirectory->setText("initial_parsing");
	ui.lineEditOutputDirectory->setText("final_parsing");

	connect(ui.pushButtonTestDataDirectory, SIGNAL(clicked()), this, SLOT(onTestDataDirectory()));
	connect(ui.pushButtonOutputDirectory, SIGNAL(clicked()), this, SLOT(onOutputDirectory()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

ParameterEstimationDialog::~ParameterEstimationDialog() {
}

void ParameterEstimationDialog::onTestDataDirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditTestDataDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditTestDataDirectory->setText(dir);
	}
}

void ParameterEstimationDialog::onOutputDirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditOutputDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditOutputDirectory->setText(dir);
	}
}

void ParameterEstimationDialog::onOK() {
	accept();
}

void ParameterEstimationDialog::onCancel() {
	reject();
}
