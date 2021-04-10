#include <iostream>
#include <map>
#include <opencv2/opencv.hpp>
#include <libwebsockets.h>
#include <pthread.h>
#include <chrono>
#include <thread>
#include <string>

#include "header/webcam.h"
#include "header/socketServer.h"
#include "header/sensorData.h"
#include "header/base64.h"

#define MAX_BUFFER_SIZE 100

std::map<lws *, long long> lwsTime;

static std::vector<lws *> connectLWS;
static pthread_mutex_t mutex;

extern SensorData sensorData;

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	return 0;
}

static int callback_dumb_increment(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    using namespace std;
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            lws_callback_on_writable(wsi);
            lwsTime[wsi] = sensorData.imageTiming;
            printf("connection established\n");
            break;
        case 11: {
            pthread_mutex_lock(&mutex);
            std::string buffer_string = "";

            if (lwsTime[wsi] + MAX_BUFFER_SIZE <= sensorData.imageTiming) {
                for (int i = lwsTime[wsi]; i <= lwsTime[wsi] + MAX_BUFFER_SIZE; i++) {
                    buffer_string += sensorData.imageQueueTiming[i]+" ";
                }

                lwsTime[wsi] += MAX_BUFFER_SIZE;
            }

            const int len = buffer_string.size();

            if (len > 0) {
                cout << lwsTime[wsi] << " vs " << sensorData.imageTiming << endl;

                unsigned char* buffer = new unsigned char[LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING];
                for (int i = 0; i < len; i++) {
                    buffer[LWS_SEND_BUFFER_PRE_PADDING + i] = buffer_string[i];
                }
                lws_write(wsi, &buffer[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);

                delete buffer;
            }
            
            pthread_mutex_unlock(&mutex);
            lws_callback_on_writable(wsi);
            break;
        }
        case LWS_CALLBACK_CLIENT_CLOSED:
            break;
        default:
            break;
    }
    
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "http-only",
        callback_http,
        0
    },
    {
        "dumb-increment-protocol",
        callback_dumb_increment,
        0
    },
    {
        NULL, NULL, 0
    }
};

static void *send(void *)
{

}

int main()
{
    pthread_t webcamThread, socketThread, sendThread;
    int result;
    int result2;
    int result3;

    pthread_mutex_init(&mutex,NULL);

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = 9000;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;
    info.pt_serv_buf_size = 1024 * 1024 * 3; 

    sensorData.image = cv::Mat(cv::Size(500, 500), CV_8UC3);
    sensorData.imageTiming = 0;

    Webcam myWebCam(&sensorData);
    SocketServer mySocketServer(info, sensorData);

    pthread_create(&webcamThread, NULL, myWebCam.run, &myWebCam);
    pthread_create(&socketThread, NULL, mySocketServer.run, &mySocketServer);
    // pthread_create(&sendThread, NULL, send, NULL);

    pthread_join(webcamThread, (void **)&result);
    pthread_join(socketThread, (void **)&result2);
    // pthread_join(sendThread, (void **)&result3);

    pthread_mutex_destroy(&mutex);

    std::cout << "end!" << std::endl;

    return 0;
}