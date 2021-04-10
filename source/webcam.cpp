#include <iostream>
#include <algorithm>
#include <libwebsockets.h>
#include <map>
#include <chrono>
#include <thread>

#include "../header/webcam.h"
#include "../header/sensorData.h"
#include "../header/base64.h"

extern int IMAGE_WIDTH;
extern int IMAGE_HEIGHT;
extern std::map<lws *, long long> lwsTime;

Webcam::Webcam(SensorData* _sensorData)
{
    sensorData = _sensorData;
}

void *Webcam::run(void* cam)
{
    std::cout << "OpenCV Version : " << CV_VERSION << std::endl;
    cv::namedWindow("EXAMPLE02", 1);
    cv::VideoCapture cap;
    SensorData & camSensorData = *((Webcam *) cam)->sensorData;
    long long prevMinTiming = 0;

    std::cout <<  "webcam: " << &camSensorData.image << std::endl;
    
    // cap.set(cv::CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
    // cap.set(cv::CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);
    // cap.open("/dev/video4");

    cap.open(-1);

    std::cout << "frame Width: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;

    while (cap.isOpened())
    {
        cap >> camSensorData.image;
        cv::Mat resizeImage;

        float ratio = camSensorData.image.cols / camSensorData.image.rows;
        cv::resize(camSensorData.image, resizeImage, cv::Size( 300 * ratio, 300 ));

        std::string encoded_png;
        std::vector<uchar> buf;
        cv::imencode(".png", resizeImage, buf);
        auto base64_png = reinterpret_cast<const unsigned char*>(buf.data());
        encoded_png = "data:image/jpeg;base64," + base64_encode(base64_png, buf.size());

        camSensorData.imageQueueTiming[camSensorData.imageTiming] = encoded_png;
        long long minTiming = camSensorData.imageTiming;

        for (auto atom : lwsTime) {
            minTiming = std::min(atom.second, minTiming) - 1;
        }
        for (int i = prevMinTiming; i < minTiming; i++) {
            camSensorData.imageQueueTiming.erase(i);
        }

        camSensorData.imageTiming = camSensorData.imageTiming + 1;

        prevMinTiming = minTiming;

        cv::imshow("EXAMPLE02", camSensorData.image);
        if (cv::waitKey(30)==27)
        {
            break;
        }
    }
    cv::destroyWindow("EXAMPLE02");  
}
