#ifndef PARAMETERESTIMATIONDIALOG_H
#define PARAMETERESTIMATIONDIALOG_H

#include <QDialog>
#include "ui_ParameterEstimationDialog.h"

class ParameterEstimationDialog : public QDialog
{
	Q_OBJECT

public:
	ParameterEstimationDialog(QWidget *parent = 0);
	~ParameterEstimationDialog();

public slots:
	void onTestDataDirectory();
	void onOutputDirectory();
	void onOK();
	void onCancel();

public:
	Ui::ParameterEstimationDialog ui;
};

#endif // PARAMETERESTIMATIONDIALOG_H
