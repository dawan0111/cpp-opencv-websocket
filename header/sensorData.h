#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <map>

struct SensorData {
    cv::Mat image;
    std::map<int, std::string> imageQueueTiming;
    long long imageTiming;
};

#endif