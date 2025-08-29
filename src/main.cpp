#include "HttpServiceHelper.h"
#include <thread>
#include <chrono>

class InitHelper{
public:
    InitHelper(){
        hlog_set_file("myapp.log");  // 设置日志文件
        hlog_set_level(LOG_LEVEL_DEBUG);

        hlogi("Program compiled at %s %s", __DATE__, __TIME__);
    }
};

static InitHelper initHelper;

int main() {
    HttpServiceHepler::Instance().Start(8090);
    
    while( HttpServiceHepler::Instance().IsRun()){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    hlogi("Program exit!");
    return 0;
}
