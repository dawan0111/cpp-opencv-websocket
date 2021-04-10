#ifndef WEBCAM_H
#define WEBCAM_H
#include <opencv2/opencv.hpp>

class Webcam
{
private:
	cv::Mat* image;
public:
	Webcam(cv::Mat* _image);
	static void *run(void* cam);
};

#endif