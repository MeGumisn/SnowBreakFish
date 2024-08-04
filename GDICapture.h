#pragma once
#include<opencv2/opencv.hpp>
#include<Windows.h>

class GDICapture
{
public:
	GDICapture(LPCWSTR windowTitle):windowTitle(windowTitle) {
		hWinDC = GetDC(NULL);
		hCompatibleDC = CreateCompatibleDC(hWinDC);
		refreshWindowPos();
	}

	~GDICapture() {
		ReleaseDC(NULL, hCompatibleDC);
		hCompatibleDC = 0;
		ReleaseDC(NULL, hWinDC);
		hWinDC = 0;
	}

	cv::Mat grab(const cv::Rect& rect);

	cv::Mat grabFullWindow();

	void refreshWindowPos();
private:
	HDC hWinDC;
	HDC hCompatibleDC;
	int wLeft;
	int wTop;
	int wWidth;
	int wHeight;
	LPCWSTR windowTitle;
};
