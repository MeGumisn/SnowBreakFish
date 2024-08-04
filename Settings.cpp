#include"Settings.h"

const cv::Rect Settings::buttonRect = cv::Rect(1730, 956, 89, 67);
const cv::Rect Settings::fishCircleRect = cv::Rect(1160, 276, 321, 311);

cv::Mat Settings::initImg(const cv::Mat& input) {
    cv::Mat dist;
    cv::cvtColor(input, dist, cv::COLOR_BGR2HSV);
    std::vector<int> lowerGreen({ 10, 70, 220 });
    std::vector<int> upperGreen({ 20, 95, 235 });
    cv::Mat mask;
	cv::inRange(dist, lowerGreen, upperGreen, mask);
    cv::Mat filteredImg;
    cv::bitwise_and(input, input, filteredImg, mask = mask);
    cv::Mat bgrImg;
    cv::cvtColor(filteredImg, bgrImg, cv::COLOR_HSV2BGR);
    cv::Mat out;
    cv::cvtColor(bgrImg, out, cv::COLOR_HSV2BGR);
    return out;
}



cv::Mat Settings::castLineImg = Settings::initImg(cv::imread("png/cast_line.png", cv::IMREAD_UNCHANGED));
cv::Mat Settings::biteImg = Settings::initImg(cv::imread("png/bite.png", cv::IMREAD_UNCHANGED));
cv::Mat Settings::dragImg = Settings::initImg(cv::imread("png/drag.png", cv::IMREAD_UNCHANGED));