#pragma once
#include<opencv2/opencv.hpp>

class Settings {
public:
	static const cv::Rect buttonRect;
	static const cv::Rect fishCircleRect;
	static cv::Mat castLineImg;
	static cv::Mat biteImg;
	static cv::Mat dragImg;

	static cv::Mat initImg(const cv::Mat& input);
};