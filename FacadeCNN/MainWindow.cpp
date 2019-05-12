#include "MainWindow.h"
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <boost/filesystem.hpp>
#include "Classifier.h"
#include "Regression.h"
#include "Utils.h"
#include "ImageGenerationDialog.h"
#include "ParameterEstimationDialog.h"
#include "facadeA.h"
#include "facadeB.h"
#include "facadeC.h"
#include "facadeD.h"
#include "facadeE.h"
#include "facadeF.h"
#include "facadeG.h"
#include "facadeH.h"
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <QTextStream>
#include "FacadeSegmentation.h"
#include "WindowPositioning.h"
#include "WindowRecognition.h"
#include "FacadeReconstruction.h"

#ifndef SQR
#define SQR(x)	((x) * (x))
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	canvas = new Canvas(this);
	setCentralWidget(canvas);

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionGenerateTrainingImages, SIGNAL(triggered()), this, SLOT(onGenerateTrainingImages()));
	connect(ui.actionParameterEstimationAll, SIGNAL(triggered()), this, SLOT(onParameterEstimationAll()));
	connect(ui.actionParameterEstimation, SIGNAL(triggered()), this, SLOT(onParameterEstimation()));

	cnns_loaded = false;
}

void MainWindow::onGenerateTrainingImages() {
	generateTrainingImages();
}

/**
* This is called when the user clickes [Tool] -> [Predict]
*/
void MainWindow::onParameterEstimationAll() {
	parameterEstimationAll();
}

void MainWindow::generateTrainingImages() {
	ImageGenerationDialog dlg;
	if (!dlg.exec()) return;

	int NUM_GRAMMARS = 8;

	QString DATA_ROOT = dlg.ui.lineEditOutputDirectory->text();
	int NUM_IMAGES_PER_SNIPPET = dlg.ui.lineEditNumImages->text().toInt();
	int IMAGE_SIZE = dlg.ui.lineEditImageSize->text().toInt();
	bool GRAYSCALE = dlg.ui.checkBoxGrayscale->isChecked();
	float EDGE_DISPLACEMENT = dlg.ui.lineEditEdgeDisplacement->text().toFloat() * 0.01;
	float MISSING_WINDOWS = dlg.ui.lineEditMissingWindows->text().toFloat() * 0.01;

	QString dir = QString(DATA_ROOT);
	if (QDir(dir).exists()) {
		QDir(dir).removeRecursively();
	}
	QDir().mkdir(dir);

	for (int facade_grammar_id = 0; facade_grammar_id < NUM_GRAMMARS; ++facade_grammar_id) {
		srand(0);

		QString dirname = QString(DATA_ROOT + "/%1/").arg(facade_grammar_id + 1, 2, 10, QChar('0'));
		QDir().mkdir(dirname);

		int subdir_id = -1;

		QFile file_param(dirname + "parameters.txt");
		file_param.open(QIODevice::WriteOnly);
		QTextStream out(&file_param);

		printf("Grammar snippet #%d:", facade_grammar_id + 1);
		for (int iter = 0; iter < NUM_IMAGES_PER_SNIPPET; ++iter) {
			printf("\rGrammar snippet #%d: %d", facade_grammar_id + 1, iter + 1);

			if (iter % 100000 == 0) {
				subdir_id++;

				QString subdirname = QString(dirname + "%1/").arg(subdir_id, 6, 10, QChar('0'));
				QDir().mkdir(subdirname);
			}

			std::vector<float> params;
			cv::Mat img = generateFacadeStructure(facade_grammar_id, IMAGE_SIZE, IMAGE_SIZE, params, EDGE_DISPLACEMENT, 1 - MISSING_WINDOWS);

			if (GRAYSCALE) {
				cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
			}

			///////////////////////////////////////////////////
			// save the image to the file
			QString filename = QString(dirname + "%1/%2.png").arg(subdir_id, 6, 10, QChar('0')).arg(iter, 6, 10, QChar('0'));
			cv::imwrite(filename.toUtf8().constData(), img);

			for (int i = 0; i < params.size(); ++i) {
				if (i > 0) out << ",";
				out << params[i];
			}
			out << "\n";
		}
		printf("\n");

		file_param.close();
	}
	printf("\n");
}

cv::Mat MainWindow::generateFacadeStructure(int facade_grammar_id, int width, int height, std::vector<float>& params, float window_displacement, float window_prob) {
	int thickness = 1;
	//int thickness = utils::uniform_rand(1, 4);

	cv::Mat result;
	if (facade_grammar_id == 0) {
		result = FacadeA::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 1) {
		result = FacadeB::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 2) {
		result = FacadeC::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 3) {
		result = FacadeD::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 4) {
		result = FacadeE::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 5) {
		result = FacadeF::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 6) {
		result = FacadeG::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}
	else if (facade_grammar_id == 7) {
		result = FacadeH::generateRandomFacade(width, height, thickness, params, window_displacement, window_prob);
	}

	return result;
}

void MainWindow::parameterEstimationAll() {
	ParameterEstimationDialog dlg;
	if (!dlg.exec()) return;

	loadCNNs();

	QString segmentation_output_dir = dlg.ui.lineEditSegmentationOutputDirectory->text();
	if (!QDir(segmentation_output_dir).exists()) {
		QDir().mkdir(segmentation_output_dir);
	}
	QString initial_output_dir = dlg.ui.lineEditInitialOutputDirectory->text();
	if (!QDir(initial_output_dir).exists()) {
		QDir().mkdir(initial_output_dir);
	}
	QString final_output_dir = dlg.ui.lineEditOutputDirectory->text();
	if (!QDir(final_output_dir).exists()) {
		QDir().mkdir(final_output_dir);
	}

	QDir image_dir(dlg.ui.lineEditTestDataDirectory->text());
	QStringList image_files = image_dir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

	printf("Predicting: ");
	for (int i = 0; i < image_files.size(); i++) {
		printf("\rPredicting: %d", i + 1);

		QStringList strs = image_files[i].split(".");
		if (strs.back() != "png" && strs.back() != "jpg") continue;

		std::vector<float> x_splits;
		std::vector<float> y_splits;
		std::vector<std::vector<fs::WindowPos>> win_rects;
		cv::Mat segmentation_result;
		cv::Mat initial_result;
		cv::Mat final_result;
		parseFacade(dlg.ui.lineEditTestDataDirectory->text() + "/" + image_files[i], x_splits, y_splits, win_rects, segmentation_result, initial_result, final_result);

		cv::imwrite((segmentation_output_dir + "/" + strs[0] + ".png").toUtf8().constData(), segmentation_result);
		cv::imwrite((initial_output_dir + "/" + strs[0] + ".png").toUtf8().constData(), initial_result);
		cv::imwrite((final_output_dir + "/" + strs[0] + ".png").toUtf8().constData(), final_result);

	}
	printf("\n");
}

void MainWindow::onParameterEstimation() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
	if (filename.isEmpty()) return;

	loadCNNs();
	setWindowTitle("Facade CNN - " + filename);

	std::vector<float> x_splits;
	std::vector<float> y_splits;
	std::vector<std::vector<fs::WindowPos>> win_rects;
	cv::Mat segmentation_result;
	cv::Mat initial_result;
	cv::Mat final_result;
	parseFacade(filename, x_splits, y_splits, win_rects, segmentation_result, initial_result, final_result);


	cv::imwrite("result_segmentation.png", segmentation_result);
	cv::imwrite("result_initial.png", initial_result);
	cv::imwrite("result_final.png", final_result);

	canvas->set(filename, y_splits, x_splits, win_rects);
}

void MainWindow::parseFacade(const QString& input_filename, std::vector<float>& x_splits, std::vector<float>& y_splits, std::vector<std::vector<fs::WindowPos>>& win_rects, cv::Mat& segmentation_result, cv::Mat& initial_result, cv::Mat& final_result) {
	cv::Mat facade_img = cv::imread(input_filename.toUtf8().constData());

	// floor height / column width
	cv::Mat resized_facade_img;
	cv::resize(facade_img, resized_facade_img, cv::Size(227, 227));
	cv::cvtColor(resized_facade_img, resized_facade_img, cv::COLOR_BGR2GRAY);
	cv::cvtColor(resized_facade_img, resized_facade_img, cv::COLOR_GRAY2BGR);
	std::vector<float> floor_params = floors_regression->Predict(resized_facade_img);
	int num_floors = std::round(floor_params[0] + 0.3);
	int num_columns = std::round(floor_params[1] + 0.3);
	float average_floor_height = (float)facade_img.rows / num_floors;
	float average_column_width = (float)facade_img.cols / num_columns;

	// subdivide the facade into tiles and windows
	fs::subdivideFacade(facade_img, average_floor_height, average_column_width, y_splits, x_splits);

	// update #floors and #columns
	//num_floors = y_splits.size() - 1;
	//num_columns = x_splits.size() - 1;

	//////////////////////////////////////////////////////////////////////////////////
	// DEBUG
	QFileInfo file_info(input_filename);
	for (int i = 0; i < y_splits.size() - 1; i++) {
		for (int j = 0; j < x_splits.size() - 1; j++) {
			cv::Mat tile_img(facade_img, cv::Rect(x_splits[j], y_splits[i], x_splits[j + 1] - x_splits[j] + 1, y_splits[i + 1] - y_splits[i] + 1));
			cv::Mat tile_img_resized;
			cv::resize(tile_img, tile_img_resized, cv::Size(227, 227));
			QString tile_filename = QString("tiles/" + file_info.baseName() + "_%1_%2.png").arg(i, 2, 10, QChar('0')).arg(j, 2, 10, QChar('0'));
			cv::imwrite(tile_filename.toUtf8().constData(), tile_img_resized);
		}
	}
	//////////////////////////////////////////////////////////////////////////////////


	// gray scale
	cv::Mat facade_gray_img;
	cv::cvtColor(facade_img, facade_gray_img, cv::COLOR_BGR2GRAY);
	cv::cvtColor(facade_gray_img, facade_gray_img, cv::COLOR_GRAY2BGR);

	// use window positioning CNN to locate windows
	win_rects.resize(y_splits.size() - 1, std::vector<fs::WindowPos>(x_splits.size() - 1));
	for (int i = 0; i < y_splits.size() - 1; ++i) {
		for (int j = 0; j < x_splits.size() - 1; ++j) {
			cv::Mat tile_img(facade_gray_img, cv::Rect(x_splits[j], y_splits[i], x_splits[j + 1] - x_splits[j] + 1, y_splits[i + 1] - y_splits[i] + 1));

			cv::Mat resized_tile_img;
			cv::resize(tile_img, resized_tile_img, cv::Size(227, 227));

			// check the existence of window
			std::vector<Prediction> pred_exist = win_exist_classifier->Classify(resized_tile_img, 2);
			if (pred_exist[0].first == 1) {
				win_rects[i][j].valid = fs::WindowPos::VALID;
			}
			else {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}

			if (fs::WindowPos::VALID) {
				// predict the window position
				std::vector<float> pred_params = wp::parameterEstimation(win_pos_regression, resized_tile_img, true, 0.1, 3000);
				//utils::output_vector(pred_params);
				win_rects[i][j].left = std::round(pred_params[0] * tile_img.cols);
				win_rects[i][j].top = std::round(pred_params[1] * tile_img.rows);
				win_rects[i][j].right = std::round(pred_params[2] * tile_img.cols);
				win_rects[i][j].bottom = std::round(pred_params[3] * tile_img.rows);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// 2017/8/23
	// get the facade image excluding windows
	cv::Mat facade_wall_img;
	fs::getWallImage(facade_img, y_splits, x_splits, win_rects, facade_wall_img);
	cv::imwrite("facade_wall.png", facade_wall_img);

	// extract a dominant color for the entire region of facade
	cv::Scalar facade_color = fs::getDominantColor(facade_wall_img, 10);
	std::cout << "Facade color = (" << facade_color[0] << "," << facade_color[1] << "," << facade_color[2] << ")" << std::endl;


	//////////////////////////////////////////////////////////////////////////////////////////
	// HACK:
	// refine the #floors / #columns
	for (int i = 0; i < y_splits.size() - 1;) {
		int win_nums = 0;
		for (int j = 0; j < x_splits.size() - 1; ++j) {
			if (win_rects[i][j].valid) win_nums++;
		}

		// if there are too small number of windows detected on this floor,
		// assume that they are false detection, and remove them.
		if (win_nums < (float)(x_splits.size() - 1) * 0.3) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}
			num_floors--;
			if (i < y_splits.size() - 2) {
				y_splits.erase(y_splits.begin() + i + 1);
			}
			else {
				y_splits.erase(y_splits.begin() + i);
			}
			win_rects.erase(win_rects.begin() + i);
		}
		else {
			i++;
		}
	}
	for (int j = 0; j < x_splits.size() - 1; ++j) {
		int win_nums = 0;
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			if (win_rects[i][j].valid) win_nums++;
		}

		// if there are too small number of windows detected on this column,
		// assume that they are false detection, and remove them.
		if (win_nums < (float)(y_splits.size() - 1) * 0.3) {
			for (int i = 0; i < y_splits.size() - 1; ++i) {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}
			num_columns--;
		}
	}

	// generate facade segmentation image
	segmentation_result = facade_img.clone();
	fs::generateFacadeSubdivisionImage(y_splits, x_splits, 3, cv::Scalar(0, 255, 255), segmentation_result);

	// generate initial facade parsing image
	initial_result = cv::Mat(facade_img.rows, facade_img.cols, CV_8UC3, cv::Scalar(0, 255, 255));
	fs::generateWindowImage(y_splits, x_splits, win_rects, -1, cv::Scalar(0, 0, 255), initial_result);

	cv::Mat initial_facade_parsing = cv::Mat(facade_img.rows, facade_img.cols, CV_8UC3, cv::Scalar(255, 255, 255));// cv::Scalar(0, 255, 255));
	fs::generateWindowImage(y_splits, x_splits, win_rects, -1, cv::Scalar(0, 0, 0), initial_facade_parsing);

	// generate input image for facade CNN
	cv::Mat input_img(227, 227, CV_8UC3, cv::Scalar(255, 255, 255));
	fs::generateWindowImage(y_splits, x_splits, win_rects, 1, cv::Scalar(0, 0, 0), input_img);
	//cv::imwrite("window227.png", input_img);

	// classification
	int facade_id = facarec::recognition(fac_classifier, input_img, -1, num_floors);
	std::cout << "facade grammar: " << facade_id + 1 << std::endl;


	// recognize window styles
	std::vector<int> selected_win_types = winrec::recognition(facade_img, facade_id, y_splits, x_splits, win_rects, win_classifier);


	// parameter estimation
	std::vector<float> predicted_params = facarec::parameterEstimation(facade_id, fac_regressions[facade_id], input_img, facade_img.cols, facade_img.rows, num_floors, num_columns, initial_facade_parsing, selected_win_types);
	utils::output_vector(predicted_params);

	// generate final facade parsing image
	facarec::generateFacadeImage(facade_id, facade_img.cols, facade_img.rows, num_floors, num_columns, predicted_params, selected_win_types, -1, cv::Scalar(0, 255, 255), cv::Scalar(0, 0, 255), final_result);
}

void MainWindow::loadCNNs() {
	if (cnns_loaded) return;

	// load trained CNNs
	fac_classifier = boost::shared_ptr<Classifier>(new Classifier("models/facade/deploy.prototxt", "models/facade/train_iter_40000.caffemodel", "models/facade/mean.binaryproto"));
	fac_regressions.resize(8);
	fac_regressions[0] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_01.prototxt", "models/facade/train_01_iter_40000.caffemodel"));
	fac_regressions[1] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_02.prototxt", "models/facade/train_02_iter_40000.caffemodel"));
	fac_regressions[2] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_03.prototxt", "models/facade/train_03_iter_40000.caffemodel"));
	fac_regressions[3] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_04.prototxt", "models/facade/train_04_iter_40000.caffemodel"));
	fac_regressions[4] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_05.prototxt", "models/facade/train_05_iter_40000.caffemodel"));
	fac_regressions[5] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_06.prototxt", "models/facade/train_06_iter_40000.caffemodel"));
	fac_regressions[6] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_07.prototxt", "models/facade/train_07_iter_40000.caffemodel"));
	fac_regressions[7] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_08.prototxt", "models/facade/train_08_iter_40000.caffemodel"));

	floors_regression = boost::shared_ptr<Regression>(new Regression("models/floors/deploy_01.prototxt", "models/floors/train_01_iter_80000.caffemodel"));
	win_exist_classifier = boost::shared_ptr<Classifier>(new Classifier("models/window_existence/deploy.prototxt", "models/window_existence/train_iter_40000.caffemodel", "models/window_existence/mean.binaryproto"));
	win_pos_regression = boost::shared_ptr<Regression>(new Regression("models/window_position/deploy_01.prototxt", "models/window_position/train_01_iter_80000.caffemodel"));
	win_classifier = boost::shared_ptr<Classifier>(new Classifier("models/window/deploy.prototxt", "models/window/train_iter_40000.caffemodel", "models/window/mean.binaryproto"));

	cnns_loaded = true;
}