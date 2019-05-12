#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "Classifier.h"
#include "Regression.h"
#include "Canvas.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	Ui::MainWindowClass ui;
	Canvas* canvas;

	boost::shared_ptr<Classifier> fac_classifier;
	std::vector<boost::shared_ptr<Regression>> fac_regressions;
	boost::shared_ptr<Regression> floors_regression;
	boost::shared_ptr<Classifier> win_exist_classifier;
	boost::shared_ptr<Regression> win_pos_regression;
	boost::shared_ptr<Classifier> win_classifier;
	bool cnns_loaded;

public:
	MainWindow(QWidget *parent = 0);

	void generateTrainingImages();
	cv::Mat generateFacadeStructure(int facade_grammar_id, int width, int height, std::vector<float>& params, float window_displacement, float window_prob = 1);
	void parameterEstimationAll();
	void parseFacade(const QString& input_filename, std::vector<float>& x_splits, std::vector<float>& y_splits, std::vector<std::vector<fs::WindowPos>>& win_rects, cv::Mat& segmentation_result, cv::Mat& initial_result, cv::Mat& final_result);
	void loadCNNs();

public slots:
	void onGenerateTrainingImages();
	void onParameterEstimationAll();
	void onParameterEstimation();
};

#endif // MAINWINDOW_H
