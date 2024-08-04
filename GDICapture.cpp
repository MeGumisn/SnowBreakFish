#include<dwmapi.h>
#include"GDICapture.h"

#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "Dwmapi.lib")

cv::Mat GDICapture::grab(const cv::Rect& rect) {
	int left = rect.x + wLeft;
	int top = rect.y + wTop;
	int width = rect.width;
	int height = rect.height;
	// 创建位图DC
	HBITMAP hBitmap = CreateCompatibleBitmap(hWinDC, width, height);
	// 将位图对象选入位图DC中
	SelectObject(hCompatibleDC, hBitmap);
	// 将屏幕内容复制到位图DC中
	BitBlt(hCompatibleDC, 0, 0, width, height, hWinDC, left, top, SRCCOPY);
	// 将位图转换为OpenCV对象
	cv::Mat shotData(height, width, CV_8UC4);
	GetBitmapBits(hBitmap, width * height * 4, shotData.data);
	// 释放资源
	DeleteObject(hBitmap);
	cv::Mat bgrData;
	cv::cvtColor(shotData, bgrData, cv::COLOR_BGRA2BGR);
	return bgrData;
}

cv::Mat GDICapture::grabFullWindow() {
	cv::Rect rect(0, 0, wWidth, wHeight);
	return grab(rect);
}

void GDICapture::refreshWindowPos()
{
	HWND hWindow = FindWindow(NULL, windowTitle);
	RECT rect;
	//GetWindowRect(hWindow, &rect);
	DwmGetWindowAttribute(hWindow, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
	wLeft = rect.left;
	wTop = rect.top;
	wWidth = rect.right - rect.left;
	wHeight = rect.bottom - rect.top;
}
