#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace fs {

	class WindowPos {
	public:
		static enum { INVALID = 0, VALID, UNCERTAIN };
	public:
		int left;
		int right;
		int top;
		int bottom;
		int valid;
		int type;

	public:
		WindowPos() : valid(INVALID), left(0), top(0), right(0), bottom(0) {}
		WindowPos(int left, int top, int right, int bottom) : left(left), top(top), right(right), bottom(bottom), valid(VALID) {}
	};

	void subdivideFacade(cv::Mat img, float floor_height, float column_width, std::vector<float>& y_split, std::vector<float>& x_split);
	std::vector<float> findBoundaries(const cv::Mat& img, cv::Range range1, cv::Range range2, int num_splits, const cv::Mat_<float>& Ver);
	void computeVerAndHor(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor, float alpha);
	void getSplitLines(const cv::Mat_<float>& mat, float threshold, std::vector<float>& split_positions);
	cv::Scalar getDominantColor(const cv::Mat& img, int cluster_count);
	void getWallImage(cv::Mat img, std::vector<float> y_splits, std::vector<float> x_splits, std::vector<std::vector<WindowPos>> win_rects, cv::Mat& result);
	bool isLocalMinimum(const cv::Mat& mat, int index, float threshold);

	// visualization
	void generateWindowImage(std::vector<float> y_splits, std::vector<float> x_splits, std::vector<std::vector<WindowPos>> win_rects, int line_width, const cv::Scalar& line_color, cv::Mat& window_img);
	void generateFacadeSubdivisionImage(std::vector<float> y_splits, std::vector<float> x_splits, int line_width, const cv::Scalar& line_color, cv::Mat& result_img);
	void outputFacadeStructure(cv::Mat img, const std::vector<float>& y_splits, const std::vector<float>& x_splits, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputFacadeStructure(cv::Mat img, const cv::Mat_<float>& SV_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::vector<float>& y_splits, const cv::Mat_<float>& SH_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::vector<float>& x_splits, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputFacadeAndWindows(const cv::Mat& img, const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputWindows(const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth);
	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat_<float>& ver, const std::vector<float>& ys, const cv::Mat_<float>& hor, const std::vector<float>& xs, const std::string& filename, int lineWidth);
	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat_<float>& ver, const cv::Mat_<float>& hor, const std::string& filename);
	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& h_max, const std::string& filename);
	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::string& filename);
	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::vector<float>& y_splits, const std::string& filename);
	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& w_max, const std::string& filename);
	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::string& filename);
	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::vector<float>& x_splits, const std::string& filename);

}