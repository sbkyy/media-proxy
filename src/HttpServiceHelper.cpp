#include "HttpServiceHelper.h"
using json = nlohmann::json;

HttpServiceHepler::HttpServiceHepler(){
    run = true;
}

HttpServiceHepler::~HttpServiceHepler(){
    run = false;
}

void HttpServiceHepler::Start(int port){
   
    router.GET("/ping", [](HttpRequest* req, HttpResponse* resp) {
        return resp->String("pong");
    });

    router.POST("/proxy", [=](HttpRequest* req, HttpResponse* resp) {
        try {
            std::string body = req->Body();
            hlogi("post body: %s", body.c_str());
        
            // 解析 JSON
            json jn = json::parse(body);

            std::string dest = jn.value("dest", "");
            std::string src = jn.value("src", "");
            TaskData taskData;
            taskData.destUrl = dest;
            taskData.srcUrl = src;
            taskHelper.AddTaskRun(taskData);
            resp->String("0");
            return 200;
        } catch (std::exception& e) {
            resp->String("error");
            return 400;
        }
    });
    
    server.registerHttpService(&router);
    server.setPort(port);
    if(server.start() != 0){
        hloge("failed to start server: %d", server.port);
        return;
    }
    hlogi("server started at port %d successful", server.port);
}

bool HttpServiceHepler::IsRun(){
    return run;
}