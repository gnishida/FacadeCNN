#include "Utils.h"
#include <math.h>
#include <random>
#include <iostream>

namespace utils {
	const float M_PI = 3.1415926535;

	double genRand() {
		return (double)(rand() % 1000) / 1000.0;
	}

	double genRand(double a, double b) {
		return genRand() * (b - a) + a;
	}

	float gause(float u, float sigma) {
		return 1.0f / 2.0f / M_PI / sigma / sigma * expf(-u * u / 2.0f / sigma / sigma);
	}

	float stddev(std::vector<float> list) {
		if (list.size() <= 1) return 0.0f;

		float avg = mean(list);

		float total = 0.0f;
		for (int i = 0; i < list.size(); ++i) {
			total += (list[i] - avg) * (list[i] - avg);
		}

		return sqrt(total / (list.size() - 1));
	}

	float mean(std::vector<float> list) {
		float sum = 0.0f;
		for (int i = 0; i < list.size(); ++i) {
			sum += list[i];
		}
		return sum / list.size();
	}

	void grayScale(const cv::Mat& img, cv::Mat& grayImg) {
		if (img.channels() == 1) {
			grayImg = img.clone();
		}
		else if (img.channels() == 3) {
			cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
		}
		else if (img.channels() == 4) {
			cv::cvtColor(img, grayImg, cv::COLOR_BGRA2GRAY);
		}
	}

	void scaleToFit(const cv::Mat& src, cv::Mat& dst, const cv::Size& size) {
		cv::Scalar bg_color;
		if (src.channels() == 1) {
			bg_color = cv::Scalar(255);
		}
		else if (src.channels() == 3) {
			bg_color = cv::Scalar(255, 255, 255);
		}
		else if (src.channels() == 4) {
			bg_color = cv::Scalar(255, 255, 255, 255);
		}
		dst = cv::Mat(size, src.type(), bg_color);

		float scale = std::min((float)dst.cols / src.cols, (float)dst.rows / src.rows);
		cv::Size roi_size(src.cols * scale, src.rows * scale);

		cv::Mat roi(dst, cv::Rect((dst.cols - roi_size.width) * 0.5, (dst.rows - roi_size.height) * 0.5, roi_size.width, roi_size.height));
		cv::resize(src, roi, roi_size);
	}

	void distanceMap(const cv::Mat& img, cv::Mat& distMap) {
		cv::Mat gray_img;
		grayScale(img, gray_img);

		cv::threshold(gray_img, gray_img, 254, 255, CV_THRESH_BINARY);
		cv::distanceTransform(gray_img, distMap, CV_DIST_L2, 3);
		distMap.convertTo(distMap, CV_32F);
	}

	void output_vector(const std::vector<float>& values) {
		for (int i = 0; i < values.size(); ++i) {
			if (i > 0) std::cout << ", ";
			std::cout << values[i];
		}
		std::cout << std::endl;
	}

}