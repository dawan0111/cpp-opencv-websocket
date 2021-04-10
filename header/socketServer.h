#ifndef SCOEKTSERVER_H
#define SOCKETSERVER_H

#include <libwebsockets.h>
#include "sensorData.h"

class SocketServer
{
private:
    struct lws_context_creation_info info;
    struct SensorData sensorData;
public:
    SocketServer(lws_context_creation_info &_info, const SensorData &_sensorData);
    static void *run(void* socketServer);
};

#endif