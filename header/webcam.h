#ifndef WEBCAM_H
#define WEBCAM_H
#include <opencv2/opencv.hpp>
#include "./sensorData.h"

class Webcam
{
private:
	SensorData* sensorData;
public:
	Webcam(SensorData* _image);
	static void *run(void* cam);
};

#endif