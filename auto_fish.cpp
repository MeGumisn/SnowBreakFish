#include<time.h>
#include"Settings.h"
#include"GDICapture.h"

#pragma comment(lib,"IlmImf.lib")
#pragma comment(lib,"ippicvmt.lib")
#pragma comment(lib,"ippiw.lib")
#pragma comment(lib,"ittnotify.lib")
#pragma comment(lib,"libjpeg-turbo.lib")


bool isScrollLockOn() {
	return GetKeyState(VK_SCROLL) == 1;
}

void checkIfCreateDirectory() {
	std::wstring dirName = L"fish_result";
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(dirName.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE) {
		CreateDirectory(dirName.c_str(), NULL);
	}
}

double getPSNR(cv::Mat& I1, cv::Mat& I2)
{
	cv::Mat s1;
	absdiff(I1, I2, s1);
	s1.convertTo(s1, CV_32FC1);
	s1 = s1.mul(s1);

	cv::Scalar s = sum(s1);

	double sse = s.val[0];

	if (sse <= 1e-10) {
		return 0;
	}
	else
	{
		double  mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0 * log10((255.0 * 255.0) / mse);
		return psnr;
	}
}

bool castButtonDetected(GDICapture& capture) {
	cv::Mat img = capture.grab(Settings::buttonRect);
	//cv::imshow("cast", img);
	//cv::waitKey(0);
	//cv::destroyAllWindows();
	cv::Mat filteredImg = Settings::initImg(img);
	float psnr = getPSNR(filteredImg, Settings::castLineImg);
 	return psnr > 25.0;
}


bool biteButtonDetected(GDICapture& capture) {
	cv::Mat img = capture.grab(Settings::buttonRect);
	cv::Mat filteredImg = Settings::initImg(img);
	float psnr = getPSNR(filteredImg, Settings::biteImg);
	return psnr > 25.0;
}

bool dragButtonDetected(GDICapture& capture) {
	cv::Mat img = capture.grab(Settings::buttonRect);
	cv::Mat filteredImg = Settings::initImg(img);

	float psnr = getPSNR(filteredImg, Settings::dragImg);
	return psnr > 25.0;
}

bool qteDetected(GDICapture& capture) {
	cv::Mat srcImg = capture.grab(Settings::fishCircleRect);
	cv::Mat img;
	cv::resize(srcImg, img, { 64,64 });
	cv::Mat hsvImg;
	cv::cvtColor(img, hsvImg, cv::COLOR_BGR2HSV);
	std::vector<int> lowerGreen({ 20,252,255 });
	std::vector<int> upperGreen({ 70,255,255 });
	cv::Mat mask;
	cv::inRange(hsvImg, lowerGreen, upperGreen, mask);
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	return contours.size() >= 2;
}

bool testQTE() {
	cv::Mat srcImg = cv::imread("d:/snowbreak_fish3.png", cv::COLOR_BGRA2BGR);
	cv::Mat img;
	cv::resize(srcImg, img, { 64,64 });
	cv::Mat hsvImg;
	cv::cvtColor(img, hsvImg, cv::COLOR_BGR2HSV);
	std::vector<int> lowerGreen({ 20,252,255 });
	std::vector<int> upperGreen({ 70,255,255 });
	cv::Mat mask;
	cv::inRange(hsvImg, lowerGreen, upperGreen, mask);
	cv::imshow("cast", mask);
	cv::waitKey(0);
	cv::destroyAllWindows();
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	return contours.size() >= 2;
}

int main() {
	//testQTE();
	//return 0;
	Sleep(2000);
	//SetProcessDPIAware();
	checkIfCreateDirectory();
	GDICapture capture(L"尘白禁区");
	bool paused = false;
	while (true) {
		if (isScrollLockOn()) {
			// 挥杆钓鱼
			if (castButtonDetected(capture) && isScrollLockOn()) {
				std::cout << "抛线\n";
				keybd_event(VK_SPACE, 0, 0, 0);
				Sleep(50);
				keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
				Sleep(1000);
				if (castButtonDetected(capture) && isScrollLockOn()) {
					continue;
				}
				while (!dragButtonDetected(capture) && isScrollLockOn()) {
					Sleep(50);
				}
				std::cout << "收杆\n";
				keybd_event(VK_SPACE, 0, 0, 0);
				Sleep(50);
				keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
				while (!biteButtonDetected(capture) && isScrollLockOn()) {
					Sleep(50);
				}
			}
			if (!isScrollLockOn()) {
				continue;
			}
			//qte按键
			while (biteButtonDetected(capture) && isScrollLockOn()) {
				while (qteDetected(capture)) {
					keybd_event(VK_SPACE, 0, 0, 0);
					Sleep(50);
					keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
					std::cout << "qte拉线\n";
					Sleep(900);
				}
			}

			if (!isScrollLockOn()) {
				continue;
			}

			//结束,保存图片
			std::cout << "结算\n";
			Sleep(3000);
			cv::Mat window = capture.grabFullWindow();
			char filename[20] = { 0 };
			time_t cur = std::time(NULL);
			struct tm nowTime;
			localtime_s(&nowTime, &cur);
			std::strftime(filename, sizeof(filename), "%Y%m%d_%H%M%S", &nowTime);
			cv::imwrite("fish_result/" + std::string(filename) + ".png", window);
			keybd_event(VK_ESCAPE, 0, 0, 0);
			Sleep(50);
			keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
			while (!castButtonDetected(capture) && isScrollLockOn()) {
				Sleep(100);
			}
		}
		else {
			if (!paused) {
				std::cout << "暂停,使用前将游戏画面设置为1920*1080窗口模式,切勿遮挡游戏窗口,按下scroll lock开始自动钓鱼\n";
				paused = true;
			}
			while (!isScrollLockOn()) {
				Sleep(500);
			}
			std::cout << "开始自动钓鱼,按下scroll lock可暂停\n";
			paused = false;
			capture.refreshWindowPos();
		}
	}
}
