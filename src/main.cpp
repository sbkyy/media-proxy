#include "HttpServiceHelper.h"
#include "Deamon.h"

class InitHelper{
public:
    InitHelper(){
        hlog_set_file("mediaProxy.log");  // 设置日志文件
        hlog_set_level(LOG_LEVEL_DEBUG);

        hlogi("Program compiled at %s %s", __DATE__, __TIME__);
    }
};

static InitHelper initHelper;

// ./mediaProxt 0 8090
int main(int argc, char **argv) {
    int bflage = 0;
    int port = 8090;
	if(argc >= 3){
        bflage = atoi(argv[1]);
		port = atoi(argv[2]);
		hlogi("bflage: %d, %s\n", bflage, argv[1]);
	}

	if(bflage <= 0)
		daemonize(1,0);
	if(signal(SIGCHLD,SIG_IGN) == SIG_ERR)
    {
        hlogi("signal SIGCHLD error");
        exit(EXIT_FAILURE);
    }
	if(signal(SIGPIPE,SIG_IGN) == SIG_ERR)
    {
        hlogi("signal SIGPIPE error");
        exit(EXIT_FAILURE);
    }

    HttpServiceHepler::Instance().Start(port);
    
    while( HttpServiceHepler::Instance().IsRun()){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    hlogi("Program exit!");
    return 0;
}
