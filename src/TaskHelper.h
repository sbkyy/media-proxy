#ifndef _TASKHELPER_H_
#define _TASKHELPER_H_
#include <memory>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>
#include <hv/hv.h>

using namespace std;
using namespace hv;

class TaskData{
public:
    string srcUrl;
    string destUrl;
    time_t startTime;
    int pid = -1;
};

class TaskHelper{
public:
    TaskHelper(){};
    ~TaskHelper();

    bool AddTask(const TaskData& taskData);
    bool DelTask(const string& url);
    bool AddTaskRun(const TaskData& taskData);
private:
    recursive_mutex rmtx; 
    map<string /*dest url*/, TaskData> mapTask;
};
#endif