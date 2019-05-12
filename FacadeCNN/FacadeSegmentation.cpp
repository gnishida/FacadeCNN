#include "FacadeSegmentation.h"
#include "CVUtils.h"
#include "Utils.h"
#include <fstream>
#include <list>

namespace fs {
	int seq = 0;

	void subdivideFacade(cv::Mat img, float average_floor_height, float average_column_width, std::vector<float>& y_splits, std::vector<float>& x_splits) {
		// gray scale
		cv::Mat gray_img;
		cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
		
		// compute kernel size
		int kernel_size_V = average_floor_height / 8;
		if (kernel_size_V % 2 == 0) kernel_size_V++;
		int kernel_size_H = average_column_width / 8;
		if (kernel_size_H % 2 == 0) kernel_size_H++;

		// blur the image according to the average floor height
		cv::Mat blurred_gray_img;
		if (kernel_size_V > 1) {
			cv::GaussianBlur(gray_img, blurred_gray_img, cv::Size(kernel_size_V, kernel_size_V), kernel_size_V);
		}
		else {
			blurred_gray_img = gray_img.clone();
		}

		// compute Ver and Hor
		cv::Mat_<float> Ver, Hor;
		computeVerAndHor(blurred_gray_img, Ver, Hor, 0.0);

		// smooth Ver and Hor
		if (kernel_size_V > 1) {
			cv::blur(Ver, Ver, cv::Size(kernel_size_V, kernel_size_V));
		}
		if (kernel_size_H > 1) {
			cv::blur(Hor, Hor, cv::Size(kernel_size_H, kernel_size_H));
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////
		// subdivide vertically
		
		// find the floor boundaries
		cv::Range h_range1 = cv::Range(average_floor_height * 0.7, average_floor_height * 1.5);
		cv::Range h_range2 = cv::Range(average_floor_height * 0.5, average_floor_height * 1.95);
		y_splits = findBoundaries(blurred_gray_img, h_range1, h_range2, std::round(img.rows / average_floor_height) + 1, Ver);
		
		////////////////////////////////////////////////////////////////////////////////////////////////
		// subdivide horizontally
		
		// find the floor boundaries
		cv::Range w_range1 = cv::Range(average_column_width * 0.6, average_column_width * 1.3);
		cv::Range w_range2 = cv::Range(average_column_width * 0.3, average_column_width * 1.95);
		x_splits = findBoundaries(blurred_gray_img.t(), w_range1, w_range2, std::round(img.cols / average_column_width) + 1, Hor);
	}

	std::vector<float> findBoundaries(const cv::Mat& img, cv::Range range1, cv::Range range2, int num_splits, const cv::Mat_<float>& Ver) {
		std::vector<std::vector<float>> good_candidates;

		// find the local minima of Ver
		std::vector<float> y_splits_strong;
		getSplitLines(Ver, 0.5, y_splits_strong);

		// ignore the split lines that are too close to the border
		if (y_splits_strong.size() > 0 && y_splits_strong[0] < range2.start) {
			y_splits_strong.erase(y_splits_strong.begin());
		}
		if (y_splits_strong.size() > 0 && img.rows - 1 - y_splits_strong.back() < range2.start) {
			y_splits_strong.pop_back();
		}

		for (int iter = 0; iter < 2; ++iter) {
			// find the local minima of Ver
			std::vector<float> y_splits;
			getSplitLines(Ver, 0.05 / (iter + 1), y_splits);

			// check whether each split is strong or not
			std::map<float, bool> is_strong;
			for (int i = 0; i < y_splits.size(); ++i) {
				if (std::find(y_splits_strong.begin(), y_splits_strong.end(), y_splits[i]) != y_splits_strong.end()) {
					is_strong[y_splits[i]] = true;
				}
				else {
					is_strong[y_splits[i]] = false;
				}
			}

			std::list<std::vector<float>> queue;
			queue.push_back(std::vector<float>(1, 0));
			while (!queue.empty()) {
				std::vector<float> list = queue.back();
				queue.pop_back();

				if (img.rows - list.back() >= range2.start && img.rows - list.back() <= range2.end) {
					std::vector<float> new_list = list;
					new_list.push_back(img.rows - 1);

					// Only if the number of splits so far does not exceed the limit,
					// add this to the candidate list.
					if (std::abs((int)new_list.size() - num_splits) <= 1) {
						good_candidates.push_back(new_list);
					}
				}

				for (int i = 0; i < y_splits.size(); ++i) {
					if (y_splits[i] <= list.back()) continue;
					if (list.size() == 1) {
						// for the top floor, use the wider range to check
						if (y_splits[i] - list.back() < range2.start || y_splits[i] - list.back() > range2.end) continue;
					}
					else {
						// for other floors, use the narrower range to check
						if (y_splits[i] - list.back() < range1.start || y_splits[i] - list.back() > range1.end) continue;
					}

					std::vector<float> new_list = list;
					new_list.push_back(y_splits[i]);

					// Only if the number of splits so far does not exceed the limit,
					// add this to the queue.
					if ((int)new_list.size() + 1 - num_splits <= 1) {
						queue.push_back(new_list);
					}

					// If this split is a strong one, you cannot skip this, so stop here.
					if (is_strong[y_splits[i]]) {
						break;
					}
				}

			}

			// discard the candidates that have too few strong splits
			for (int i = 0; i < good_candidates.size(); ) {
				int num_strong_splits = 0;
				for (int j = 0; j < good_candidates[i].size(); ++j) {
					if (is_strong[good_candidates[i][j]]) num_strong_splits++;
				}

				if ((float)num_strong_splits < y_splits_strong.size() * 0.8) {
					good_candidates.erase(good_candidates.begin() + i);
				}
				else {
					++i;
				}
			}

			// if there is no good candidate found, increase the range and start over the process
			if (good_candidates.size() == 0) {
				continue;
			}

			// if there is only one option, select it.
			if (good_candidates.size() == 1) {
				return good_candidates[0];
			}

			// find the best candidate
			float min_val = std::numeric_limits<float>::max();
			int best_id = -1;
			float alpha = 0.5;
			for (int i = 0; i < good_candidates.size(); ++i) {
				// compute the average Ver/Hor
				float total_Ver = 0;
				for (int k = 1; k < (int)good_candidates[i].size() - 1; ++k) {
					total_Ver += Ver(good_candidates[i][k]);
				}

				float avg_Ver = total_Ver / ((int)good_candidates[i].size() - 2);

				// compute stddev of heights
				std::vector<float> heights;
				for (int k = 0; k < (int)good_candidates[i].size() - 1; ++k) {
					int h = good_candidates[i][k + 1] - good_candidates[i][k];
					heights.push_back(h);
				}
				float stddev = utils::stddev(heights);
				if (heights.size() > 0) {
					float avg_h = utils::mean(heights);
					stddev /= avg_h;
				}

				if (avg_Ver * alpha + stddev * (1 - alpha) < min_val) {
					min_val = avg_Ver  * alpha + stddev * (1 - alpha);
					best_id = i;
				}
			}

			return good_candidates[best_id];
		}
		
		// if there is no good splits found, use the original candidate splits
		std::vector<float> y_splits;
		getSplitLines(Ver, 0.1, y_splits);
		y_splits.insert(y_splits.begin(), 0);
		y_splits.push_back(img.rows - 1);

		return y_splits;
	}

	/**
 	 * imgから、Ver(y)とHor(x)を計算する。
	 *
	 * @param img		image (3-channel color image)
	 * @param Ver		Ver(y)
	 * @param Hor		Hor(x)
	 */
	void computeVerAndHor(const cv::Mat& img, cv::Mat_<float>& Ver, cv::Mat_<float>& Hor, float alpha) {
		cv::Mat grayImg;
		cvutils::grayScale(img, grayImg);

		// smoothing
		//cv::GaussianBlur(grayImg, grayImg, cv::Size(5, 5), 5, 5);

		// compute gradient magnitude
		cv::Mat sobelx;
		cv::Sobel(grayImg, sobelx, CV_32F, 1, 0);
		sobelx = cv::abs(sobelx);
		cv::Mat sobely;
		cv::Sobel(grayImg, sobely, CV_32F, 0, 1);
		sobely = cv::abs(sobely);

		// sum up the gradient magnitude horizontally and vertically
		cv::Mat sobelx_hor, sobelx_ver;
		cv::reduce(sobelx, sobelx_hor, 0, CV_REDUCE_SUM);
		cv::reduce(sobelx, sobelx_ver, 1, CV_REDUCE_SUM);
		cv::Mat sobely_hor, sobely_ver;
		cv::reduce(sobely, sobely_hor, 0, CV_REDUCE_SUM);
		cv::reduce(sobely, sobely_ver, 1, CV_REDUCE_SUM);

		// compute Ver and Hor
		Ver = sobelx_ver - sobely_ver * alpha;
		Hor = sobely_hor - sobelx_hor * alpha;

		// compute min/max
		double min_Ver, max_Ver;
		cv::minMaxLoc(Ver, &min_Ver, &max_Ver);
		double min_Hor, max_Hor;
		cv::minMaxLoc(Ver, &min_Hor, &max_Hor);

		// normalize Ver and Hor
		Ver = (Ver - min_Ver) / (max_Ver - min_Ver);
		Hor = (Hor - min_Hor) / (max_Hor - min_Hor);

		Hor = Hor.t();
	}

	/**
	* 与えられた関数の極小値を使ってsplit lineを決定する。
	*/
	void getSplitLines(const cv::Mat_<float>& val, float threshold, std::vector<float>& split_positions) {
		cv::Mat_<float> mat = val.clone();
		if (mat.rows == 1) {
			mat = mat.t();
		}

		double max_value, min_value;
		cv::minMaxLoc(mat, &min_value, &max_value);
		threshold *= (max_value - min_value);

		for (int r = 0; r < mat.rows; ++r) {
			if (isLocalMinimum(mat, r, threshold)) {
				split_positions.push_back(r);
			}
		}

		// remove the consecutive ones
		for (int i = 0; i < (int)split_positions.size() - 1;) {
			if (split_positions[i + 1] == split_positions[i] + 1) {
				split_positions.erase(split_positions.begin() + i + 1);
			}
			else {
				++i;
			}
		}

		// remove the both end
		if (split_positions.size() > 0) {
			if (split_positions[0] <= 1) {
				split_positions.erase(split_positions.begin());
			}
			if (split_positions.back() >= mat.rows - 2) {
				split_positions.pop_back();
			}
		}

		/*
		// 両端処理
		if (split_positions.size() == 0) {
			split_positions.insert(split_positions.begin(), 0);
		}
		else if (split_positions[0] > 0) {
			if (split_positions[0] < 5) {
				split_positions[0] = 0;
			}
			else {
				split_positions.insert(split_positions.begin(), 0);
			}
		}

		if (split_positions.back() < mat.rows - 1) {
			if (split_positions.back() >= mat.rows - 5) {
				split_positions.back() = mat.rows - 1;
			}
			else {
				split_positions.push_back(mat.rows - 1);
			}
		}
		*/
	}

	/**
	* Get the dominant color in the image.
	* Note: exclude the black color.
	*
	* @param img				image
	* @param cluster_count		#clusters
	* @return					dominant color
	*/
	cv::Scalar getDominantColor(const cv::Mat& img, int cluster_count) {
		cv::Mat lab_img;
		cv::cvtColor(img, lab_img, cv::COLOR_BGR2Lab);

		// k-means
		std::vector<std::vector<float>> samples;
		double avg_l = 0;
		int cnt = 0;
		for (int r = 0; r < img.rows; r++) {
			for (int c = 0; c < img.cols; c++) {
				if (img.at<cv::Vec3b>(r, c) == cv::Vec3b(0, 0, 0)) continue;
				cv::Vec3b& col = lab_img.at<cv::Vec3b>(r, c);

				std::vector<float> v(3);
				for (int k = 0; k < 3; ++k) {
					v[k] = col[k];
				}
				samples.push_back(v);
				avg_l += col[0];
				cnt++;
			}
		}

		// avg lightness
		avg_l /= cnt;

		cv::Mat samples_mat(samples.size(), 3, CV_32F);
		for (int i = 0; i < samples.size(); ++i) {
			for (int k = 0; k < 3; ++k) {
				samples_mat.at<float>(i, k) = samples[i][k];
			}
		}

		cv::Mat labels;
		int attempts = 5;
		cv::Mat centers;
		cv::kmeans(samples_mat, cluster_count, labels, cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, cv::KMEANS_PP_CENTERS, centers);
		std::vector<int> hist(centers.rows, 0);
		for (int i = 0; i < labels.rows; ++i) {
			int l = labels.at<int>(i, 0);
			hist[l]++;
		}

		int max_count = 0;
		int max_idx = -1;
		for (int i = 0; i < hist.size(); ++i) {
			if (hist[i] > max_count) {
				max_count = hist[i];
				max_idx = i;
			}
		}

		cv::Scalar max_color(0, 0, 0);
		for (int k = 0; k < 3; ++k) {
			max_color[k] = centers.at<float>(max_idx, k);
		}

		cv::Mat temp(1, 1, CV_8UC3, max_color);
		cv::cvtColor(temp, temp, cv::COLOR_Lab2BGR);
		cv::Vec3b rgb_col = temp.at<cv::Vec3b>(0, 0);

		return cv::Scalar(std::min(255, (int)(rgb_col[2] * 1.2)), std::min(255, (int)(rgb_col[1] * 1.2)), std::min(255, (int)(rgb_col[0] * 1.2)));
	}

	void getWallImage(cv::Mat img, std::vector<float> y_splits, std::vector<float> x_splits, std::vector<std::vector<WindowPos>> win_rects, cv::Mat& result) {
		result = img.clone();

		for (int i = 0; i < y_splits.size() - 1; ++i) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				if (win_rects[i][j].valid != fs::WindowPos::VALID) continue;

				cv::rectangle(result, cv::Rect(x_splits[j] + win_rects[i][j].left, y_splits[i] + win_rects[i][j].top, win_rects[i][j].right - win_rects[i][j].left + 1, win_rects[i][j].bottom - win_rects[i][j].top + 1), cv::Scalar(0, 0, 0), -1);
			}
		}
	}

	bool isLocalMinimum(const cv::Mat& mat, int index, float threshold) {
		float origin_value = mat.at<float>(index);

		// check upward
		bool local_max_found = false;
		for (int r = index - 1; r >= 0; --r) {
			if (mat.at<float>(r) < origin_value) return false;
			if (fabs(mat.at<float>(r) -origin_value) > threshold) {
				local_max_found = true;
				break;
			}
		}

		if (!local_max_found) return false;

		// check downward
		for (int r = index + 1; r < mat.rows; ++r) {
			if (mat.at<float>(r) < origin_value) return false;
			if (fabs(mat.at<float>(r) - origin_value) > threshold) return true;
		}

		return false;
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// visualization

	/**
	 * Generate window image
	 * Add window boundary lines on top of the given image.
	 *
	 * @param y_splits
	 * @param x_splits
	 * @param win_rects
	 * @param line_width
	 * @param line_color
	 * @param window_img
	 */
	void generateWindowImage(std::vector<float> y_splits, std::vector<float> x_splits, std::vector<std::vector<WindowPos>> win_rects, int line_width, const cv::Scalar& line_color, cv::Mat& window_img) {
		int img_width = x_splits.back() + 1;
		int img_height = y_splits.back() + 1;

		// resize the window coordinates
		for (int i = 0; i < win_rects.size(); ++i) {
			for (int j = 0; j < win_rects[i].size(); ++j) {
				win_rects[i][j].left = win_rects[i][j].left * (float)window_img.cols / img_width;
				win_rects[i][j].right = win_rects[i][j].right * (float)window_img.cols / img_width;
				win_rects[i][j].top = win_rects[i][j].top * (float)window_img.rows / img_height;
				win_rects[i][j].bottom = win_rects[i][j].bottom * (float)window_img.rows / img_height;
			}
		}
		for (int i = 0; i < x_splits.size(); ++i) {
			x_splits[i] = x_splits[i] * (float)window_img.cols / img_width;
		}
		for (int i = 0; i < y_splits.size(); ++i) {
			y_splits[i] = y_splits[i] * (float)window_img.rows / img_height;
		}

		// generate window image with specified size
		//window_img = cv::Mat(window_img_size, CV_8UC3, cv::Scalar(255, 255, 255));
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				if (win_rects[i][j].valid == fs::WindowPos::VALID) {
					int x = std::round(x_splits[j] + win_rects[i][j].left);
					int y = std::round(y_splits[i] + win_rects[i][j].top);
					int w = std::round(win_rects[i][j].right - win_rects[i][j].left + 1);
					int h = std::round(win_rects[i][j].bottom - win_rects[i][j].top + 1);
					cv::rectangle(window_img, cv::Rect(x, y, w, h), line_color, line_width);
				}
			}
		}
	}

	void generateFacadeSubdivisionImage(std::vector<float> y_splits, std::vector<float> x_splits, int line_width, const cv::Scalar& line_color, cv::Mat& result_img) {
		int img_width = x_splits.back() + 1;
		int img_height = y_splits.back() + 1;

		// resize the split line coordinates
		for (int i = 0; i < x_splits.size(); ++i) {
			x_splits[i] = x_splits[i] * (float)result_img.cols / img_width;
		}
		for (int i = 0; i < y_splits.size(); ++i) {
			y_splits[i] = y_splits[i] * (float)result_img.rows / img_height;
		}

		// generate subdivision image
		for (int i = 0; i < y_splits.size(); ++i) {
			cv::line(result_img, cv::Point(0, y_splits[i]), cv::Point(result_img.cols, y_splits[i]), line_color, line_width);
		}
		for (int i = 0; i < x_splits.size(); ++i) {
			cv::line(result_img, cv::Point(x_splits[i], 0), cv::Point(x_splits[i], result_img.rows), line_color, line_width);
		}
	}

	void outputFacadeStructure(cv::Mat img, const std::vector<float>& y_splits, const std::vector<float>& x_splits, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		cv::Mat result;
		if (img.channels() == 1) {
			cv::cvtColor(img, result, cv::COLOR_GRAY2BGR);
		}
		else {
			result = img.clone();
		}

		for (int i = 0; i < y_splits.size(); ++i) {
			if (i < y_splits.size() - 1) {
				cv::line(result, cv::Point(0, y_splits[i]), cv::Point(img.cols, y_splits[i]), lineColor, lineWidth);
			}
			else {
				// For the last line, we need to move the line upward by 1px to make it inside the image.
				cv::line(result, cv::Point(0, y_splits[i] - 1), cv::Point(img.cols, y_splits[i] - 1), lineColor, lineWidth);
			}
		}
		for (int i = 0; i < x_splits.size(); ++i) {
			if (i < x_splits.size() - 1) {
				cv::line(result, cv::Point(x_splits[i], 0), cv::Point(x_splits[i], img.rows), lineColor, lineWidth);
			}
			else {
				// For the last line, we need to move the line upward by 1px to make it inside the image.
				cv::line(result, cv::Point(x_splits[i] - 1, 0), cv::Point(x_splits[i] - 1, img.rows), lineColor, lineWidth);
			}
		}
		cv::imwrite(filename, result);
	}

	void outputFacadeStructure(cv::Mat img, const cv::Mat_<float>& SV_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::vector<float>& y_splits, const cv::Mat_<float>& SH_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::vector<float>& x_splits, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		if (img.channels() == 1) {
			cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
		}

		double min_SV, max_SV;
		double min_Ver, max_Ver;
		double min_h, max_h;
		double min_SH, max_SH;
		double min_Hor, max_Hor;
		double min_w, max_w;
		cv::minMaxLoc(SV_max, &min_SV, &max_SV);
		cv::minMaxLoc(Ver, &min_Ver, &max_Ver);
		cv::minMaxLoc(h_max, &min_h, &max_h);
		cv::minMaxLoc(SH_max, &min_SH, &max_SH);
		cv::minMaxLoc(Hor, &min_Hor, &max_Hor);
		cv::minMaxLoc(h_max, &min_w, &max_w);

		int graphSizeV = img.rows * 0.25;
		int marginV = graphSizeV * 0.2;
		int graphSizeH = std::max(80.0, img.rows * 0.25);
		int marginH = graphSizeH * 0.2;
		cv::Mat result(img.rows + graphSizeH * 2 + max_w + marginH * 4, img.cols + graphSizeV * 2 + max_h + marginV * 4, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw SV_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + marginV + (SV_max(r) - min_SV) / (max_SV - min_SV) * graphSizeV;
			int x2 = img.cols + marginV + (SV_max(r + 1) - min_SV) / (max_SV - min_SV) * graphSizeV;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Ver
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSizeV + marginV * 2 + (Ver(r) - min_Ver) / (max_Ver - min_Ver) * graphSizeV;
			int x2 = img.cols + graphSizeV + marginV * 2 + (Ver(r + 1) - min_Ver) / (max_Ver - min_Ver) * graphSizeV;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw h_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSizeV * 2 + marginV * 3 + h_max(r);
			int x2 = img.cols + graphSizeV * 2 + marginV * 3 + h_max(r + 1);

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw y splits
		for (int i = 0; i < y_splits.size(); ++i) {
			cv::line(result, cv::Point(0, y_splits[i]), cv::Point(img.cols, y_splits[i]), cv::Scalar(0, 255, 255), lineWidth);
		}

		// draw SH_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + marginH + (SH_max(c) - min_SH) / (max_SH - min_SH) * graphSizeH;
			int y2 = img.rows + marginH + (SH_max(c + 1) - min_SH) / (max_SH - min_SH) * graphSizeH;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Hor
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSizeH + marginH * 2 + (Hor(c) - min_Hor) / (max_Hor - min_Hor) * graphSizeH;
			int y2 = img.rows + graphSizeH + marginH * 2 + (Hor(c + 1) - min_Hor) / (max_Hor - min_Hor) * graphSizeH;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw w_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSizeH * 2 + marginH * 3 + w_max(c);
			int y2 = img.rows + graphSizeH * 2 + marginH * 3 + w_max(c + 1);

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw x splits
		for (int i = 0; i < x_splits.size(); ++i) {
			cv::line(result, cv::Point(x_splits[i], 0), cv::Point(x_splits[i], img.rows), cv::Scalar(0, 255, 255), lineWidth);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeAndWindows(const cv::Mat& img, const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		cv::Mat result = img.clone();
#if 0
		for (int i = 0; i < y_split.size(); ++i) {
			if (i < y_split.size() - 1) {
				cv::line(result, cv::Point(0, y_split[i]), cv::Point(result.cols - 1, y_split[i]), cv::Scalar(0, 0, 255), lineWidth);
			}
			else {
				cv::line(result, cv::Point(0, y_split[i] - 1), cv::Point(result.cols - 1, y_split[i] - 1), cv::Scalar(0, 0, 255), lineWidth);
			}
		}
		for (int i = 0; i < x_split.size(); ++i) {
			if (i < x_split.size() - 1) {
				cv::line(result, cv::Point(x_split[i], 0), cv::Point(x_split[i], result.rows - 1), cv::Scalar(0, 0, 255), lineWidth);
			}
			else {
				cv::line(result, cv::Point(x_split[i] - 1, 0), cv::Point(x_split[i] - 1, result.rows - 1), cv::Scalar(0, 0, 255), lineWidth);
			}
		}
#endif
		for (int i = 0; i < y_split.size() - 1; ++i) {
			for (int j = 0; j < x_split.size() - 1; ++j) {
				if (winpos[i][j].valid == WindowPos::VALID) {
					int x = x_split[j] + winpos[i][j].left;
					int y = y_split[i] + winpos[i][j].top;
					int w = winpos[i][j].right - winpos[i][j].left + 1;
					int h = winpos[i][j].bottom - winpos[i][j].top + 1;
					cv::rectangle(result, cv::Rect(x, y, w, h), lineColor, lineWidth);
				}
			}
		}
		cv::imwrite(filename, result);
	}

	void outputWindows(const std::vector<float>& y_split, const std::vector<float>& x_split, const std::vector<std::vector<WindowPos>>& winpos, const std::string& filename, cv::Scalar lineColor, int lineWidth) {
		cv::Mat result(y_split.back(), x_split.back(), CV_8UC3, cv::Scalar(255, 255, 255));
		for (int i = 0; i < y_split.size() - 1; ++i) {
			for (int j = 0; j < x_split.size() - 1; ++j) {
				if (winpos[i][j].valid == WindowPos::VALID) {
					int x1 = x_split[j] + winpos[i][j].left;
					int y1 = y_split[i] + winpos[i][j].top;
					int x2 = x_split[j + 1] - 1 - winpos[i][j].right;
					int y2 = y_split[i + 1] - 1 - winpos[i][j].bottom;
					cv::rectangle(result, cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1), lineColor, lineWidth);
				}
			}
		}
		cv::imwrite(filename, result);
	}

	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat_<float>& ver, const std::vector<float>& ys, const cv::Mat_<float>& hor, const std::vector<float>& xs, const std::string& filename, int lineWidth) {
		int graphSize = std::max(10.0, std::max(img.rows, img.cols) * 0.3);

		cv::Mat result;
		cv::Scalar graph_color;
		cv::Scalar peak_color;

		result = cv::Mat(img.rows + graphSize + 3, img.cols + graphSize + 3, CV_8UC3, cv::Scalar(255, 255, 255));
		graph_color = cv::Scalar(0, 0, 0);
		peak_color = cv::Scalar(0, 255, 255);

		// copy img to result
		cv::Mat color_img;
		if (img.channels() == 1) {
			cv::cvtColor(img, color_img, cv::COLOR_GRAY2BGR);
		}
		else if (img.channels() == 3) {
			color_img = img;
		}
		cv::Mat roi(result, cv::Rect(0, 0, color_img.cols, color_img.rows));
		color_img.copyTo(roi);

		// get the maximum value of Ver(y) and Hor(x)
		float max_ver = cvutils::max(ver);
		float min_ver = cvutils::min(ver);
		float max_hor = cvutils::max(hor);
		float min_hor = cvutils::min(hor);

		// draw vertical graph
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + (ver(r) - min_ver) / (max_ver - min_ver) * graphSize;
			int x2 = img.cols + (ver(r + 1) - min_ver) / (max_ver - min_ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), graph_color, 1, cv::LINE_8);
		}
		for (int i = 0; i < ys.size(); ++i) {
			cv::line(result, cv::Point(0, ys[i]), cv::Point(img.cols - 1, ys[i]), peak_color, lineWidth);
		}

		// draw horizontal graph
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + (hor(c) - min_hor) / (max_hor - min_hor) * graphSize;
			int y2 = img.rows + (hor(c + 1) - min_hor) / (max_hor - min_hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), graph_color, 1, cv::LINE_8);
		}
		for (int i = 0; i < xs.size(); ++i) {
			cv::line(result, cv::Point(xs[i], 0), cv::Point(xs[i], img.rows - 1), peak_color, lineWidth);
		}

		cv::imwrite(filename, result);
	}

	void outputImageWithHorizontalAndVerticalGraph(const cv::Mat& img, const cv::Mat_<float>& ver, const cv::Mat_<float>& hor, const std::string& filename) {
		int graphSize = std::max(10.0, std::max(img.rows, img.cols) * 0.3);

		cv::Mat result;
		cv::Scalar graph_color;
		cv::Scalar peak_color;

		result = cv::Mat(img.rows + graphSize + 3, img.cols + graphSize + 3, CV_8UC3, cv::Scalar(255, 255, 255));
		graph_color = cv::Scalar(0, 0, 0);
		peak_color = cv::Scalar(0, 0, 255);

		// copy img to result
		cv::Mat color_img;
		if (img.channels() == 1) {
			cv::cvtColor(img, color_img, cv::COLOR_GRAY2BGR);
		}
		else if (img.channels() == 3) {
			color_img = img;
		}
		cv::Mat roi(result, cv::Rect(0, 0, color_img.cols, color_img.rows));
		color_img.copyTo(roi);

		// get the maximum value of Ver(y) and Hor(x)
		float max_ver = cvutils::max(ver);
		float min_ver = cvutils::min(ver);
		float max_hor = cvutils::max(hor);
		float min_hor = cvutils::min(hor);

		// draw vertical graph
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + (ver(r) - min_ver) / (max_ver - min_ver) * graphSize;
			int x2 = img.cols + (ver(r + 1) - min_ver) / (max_ver - min_ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), graph_color, 1, cv::LINE_8);
		}

		// draw horizontal graph
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + (hor(c) - min_hor) / (max_hor - min_hor) * graphSize;
			int y2 = img.rows + (hor(c + 1) - min_hor) / (max_hor - min_hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), graph_color, 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	/**
	* Facade画像と合わせて、S_max(y)とh_max(y)を画像として保存する。
	* 論文 Fig 5に相当する画像。
	*
	* @param img		Facade画像
	* @param S_max		S_max
	* @param h_max		h_max
	* @param filename	output file name
	*/
	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& h_max, const std::string& filename) {
		float max_S = cvutils::max(S_max);
		float min_S = cvutils::min(S_max);
		float max_h = cvutils::max(h_max);
		float min_h = cvutils::min(h_max);

		int graphSize = img.rows * 0.25;
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows, img.cols + graphSize + max_h + margin * 3, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + margin + (S_max(r) - min_S) / (max_S - min_S) * graphSize;
			int x2 = img.cols + margin + (S_max(r + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw h_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize + margin * 2 + h_max(r);
			int x2 = img.cols + graphSize + margin * 2 + h_max(r + 1);

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::string& filename) {
		double min_S, max_S;
		double min_Ver, max_Ver;
		double min_h, max_h;
		cv::minMaxLoc(S_max, &min_S, &max_S);
		cv::minMaxLoc(Ver, &min_Ver, &max_Ver);
		cv::minMaxLoc(h_max, &min_h, &max_h);
		
		int graphSize = img.rows * 0.25;
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows, img.cols + graphSize * 2 + max_h + margin * 4, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + margin + (S_max(r) - min_S) / (max_S - min_S) * graphSize;
			int x2 = img.cols + margin + (S_max(r + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Ver
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize + margin * 2 + (Ver(r) - min_Ver) / (max_Ver - min_Ver) * graphSize;
			int x2 = img.cols + graphSize + margin * 2 + (Ver(r + 1) - min_Ver) / (max_Ver - min_Ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw h_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize * 2 + margin * 3 + h_max(r);
			int x2 = img.cols + graphSize * 2 + margin * 3 + h_max(r + 1);

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureV(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Ver, const cv::Mat_<float>& h_max, const std::vector<float>& y_splits, const std::string& filename) {
		double min_S, max_S;
		double min_Ver, max_Ver;
		double min_h, max_h;
		cv::minMaxLoc(S_max, &min_S, &max_S);
		cv::minMaxLoc(Ver, &min_Ver, &max_Ver);
		cv::minMaxLoc(h_max, &min_h, &max_h);

		int graphSize = img.rows * 0.25;
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows, img.cols + graphSize * 2 + max_h + margin * 4, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + margin + (S_max(r) - min_S) / (max_S - min_S) * graphSize;
			int x2 = img.cols + margin + (S_max(r + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Ver
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize + margin * 2 + (Ver(r) - min_Ver) / (max_Ver - min_Ver) * graphSize;
			int x2 = img.cols + graphSize + margin * 2 + (Ver(r + 1) - min_Ver) / (max_Ver - min_Ver) * graphSize;

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw h_max
		for (int r = 0; r < img.rows - 1; ++r) {
			int x1 = img.cols + graphSize * 2 + margin * 3 + h_max(r);
			int x2 = img.cols + graphSize * 2 + margin * 3 + h_max(r + 1);

			cv::line(result, cv::Point(x1, r), cv::Point(x2, r + 1), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw y splits
		for (int i = 0; i < y_splits.size(); ++i) {
			cv::line(result, cv::Point(0, y_splits[i]), cv::Point(img.cols, y_splits[i]), cv::Scalar(0, 255, 255), 3);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& w_max, const std::string& filename) {
		float max_S = cvutils::max(S_max);
		float min_S = cvutils::min(S_max);
		float max_w = cvutils::max(w_max);
		float min_w = cvutils::min(w_max);

		int graphSize = std::max(80.0, img.rows * 0.25);
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows + graphSize + max_w + margin * 3, img.cols, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + margin + (S_max(c) - min_S) / (max_S - min_S) * graphSize;
			int y2 = img.rows + margin + (S_max(c + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw w_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize + margin * 2 + w_max(c);
			int y2 = img.rows + graphSize + margin * 2 + w_max(c + 1);

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::string& filename) {
		double min_S, max_S;
		double min_Hor, max_Hor;
		double min_w, max_w;
		cv::minMaxLoc(S_max, &min_S, &max_S);
		cv::minMaxLoc(Hor, &min_Hor, &max_Hor);
		cv::minMaxLoc(w_max, &min_w, &max_w);

		int graphSize = std::max(80.0, img.rows * 0.25);
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows + graphSize * 2 + max_w + margin * 4, img.cols, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + margin + (S_max(c) - min_S) / (max_S - min_S) * graphSize;
			int y2 = img.rows + margin + (S_max(c + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Hor
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize + margin * 2 + (Hor(c) - min_Hor) / (max_Hor - min_Hor) * graphSize;
			int y2 = img.rows + graphSize + margin * 2 + (Hor(c + 1) - min_Hor) / (max_Hor - min_Hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw w_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize * 2 + margin * 3 + w_max(c);
			int y2 = img.rows + graphSize * 2 + margin * 3 + w_max(c + 1);

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		cv::imwrite(filename, result);
	}

	void outputFacadeStructureH(const cv::Mat& img, const cv::Mat_<float>& S_max, const cv::Mat_<float>& Hor, const cv::Mat_<float>& w_max, const std::vector<float>& x_splits, const std::string& filename) {
		double min_S, max_S;
		double min_Hor, max_Hor;
		double min_w, max_w;
		cv::minMaxLoc(S_max, &min_S, &max_S);
		cv::minMaxLoc(Hor, &min_Hor, &max_Hor);
		cv::minMaxLoc(w_max, &min_w, &max_w);

		int graphSize = std::max(80.0, img.rows * 0.25);
		int margin = graphSize * 0.2;
		cv::Mat result(img.rows + graphSize * 2 + max_w + margin * 4, img.cols, CV_8UC3, cv::Scalar(255, 255, 255));

		// copy img to result
		cv::Mat roi(result, cv::Rect(0, 0, img.cols, img.rows));
		img.copyTo(roi);

		// draw S_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + margin + (S_max(c) - min_S) / (max_S - min_S) * graphSize;
			int y2 = img.rows + margin + (S_max(c + 1) - min_S) / (max_S - min_S) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw Hor
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize + margin * 2 + (Hor(c) - min_Hor) / (max_Hor - min_Hor) * graphSize;
			int y2 = img.rows + graphSize + margin * 2 + (Hor(c + 1) - min_Hor) / (max_Hor - min_Hor) * graphSize;

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw w_max
		for (int c = 0; c < img.cols - 1; ++c) {
			int y1 = img.rows + graphSize * 2 + margin * 3 + w_max(c);
			int y2 = img.rows + graphSize * 2 + margin * 3 + w_max(c + 1);

			cv::line(result, cv::Point(c, y1), cv::Point(c + 1, y2), cv::Scalar(0, 0, 0), 1, cv::LINE_8);
		}

		// draw x splits
		for (int i = 0; i < x_splits.size(); ++i) {
			cv::line(result, cv::Point(x_splits[i], 0), cv::Point(x_splits[i], img.rows), cv::Scalar(0, 255, 255), 3);
		}

		cv::imwrite(filename, result);
	}

}