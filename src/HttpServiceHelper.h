#ifndef _HTTPSERVICEHELPER_H_
#define _HTTPSERVICEHELPER_H_
#include <string>

#include <hv/HttpServer.h>
#include <hv/hv.h>
#include <hv/json.hpp>
#include "TaskHelper.h"
using namespace hv;

class HttpServiceHepler{
public:
    ~HttpServiceHepler();

    static HttpServiceHepler& Instance() {
        static HttpServiceHepler ins;
        return ins;
    }

    void Start(int port = 8080);
    bool IsRun();
private:
    HttpServiceHepler();

private:
    HttpService router;
    HttpServer server;
    TaskHelper taskHelper;
    bool run;
};

#endif