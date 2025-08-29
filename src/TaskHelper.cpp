#include "TaskHelper.h"

#include <sys/wait.h>
#include <unistd.h>

// 检查子进程是否存活（非阻塞）
// 返回 true = 还在运行，false = 已退出
bool is_process_alive(pid_t pid) {
    if (pid <= 0) return false;

    int status = 0;
    // WNOHANG 表示非阻塞
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        // 子进程还在运行
        return true;
    } else if (result == pid) {
        // 子进程已退出，status 可判断退出码
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            hlogi("子进程 %d 正常退出, code=%d\n", pid, code);
        } else if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            hlogi("子进程 %d 被信号 %d 杀死\n", pid, sig);
        }
        return false;
    } else {
        // 出错
        perror("waitpid error");
        return false;
    }
}


static int exec_ffmpeg(TaskData& taskData){
    pid_t pid = fork();
    switch(pid)
    {
        case -1:
            perror("fork failed");
            exit(1);
            break;
        case 0:
        {
            char *argvs[32] = {"-report","-analyzeduration","15M",NULL};
            int offset = 3;
            argvs[offset++] = (char*)"-re";
            argvs[offset++] = (char*)"-probesize";
            argvs[offset++] = (char*)"80K";  
            argvs[offset++] = (char*)"-i";
			argvs[offset++] = (char*)taskData.srcUrl.c_str();
            argvs[offset++] = (char*)"-vcodec";
			argvs[offset++] = (char*)"copy";
            argvs[offset++] = (char*)"-acodec";
            argvs[offset++] = (char*)"copy";
            argvs[offset++] = (char*)"-f";
            argvs[offset++] = (char*)"flv";
            argvs[offset++] = (char*)taskData.destUrl.c_str();
        	execvp("ffmpeg",argvs);
            printf("son bye\n");
			exit(EXIT_SUCCESS);
            break;
        }
        default:
            break;
    }
    taskData.pid = pid;
    return pid;
}

// 停止 ffmpeg
static void stop_ffmpeg(TaskData& taskData) {
    hlogi("stop ffmpeg(pid=%d)\n", taskData.pid);
    if (taskData.pid > 0) {
        // 先发 SIGTERM 请求子进程正常退出
        kill(taskData.pid, SIGTERM);

        // 等待子进程退出（避免僵尸进程）
        int status = 0;
        waitpid(taskData.pid, &status, 0);

        hlogi("ffmpeg(pid=%d) exited with status %d\n", taskData.pid, status);
        taskData.pid = -1;
    }
}

TaskHelper::~TaskHelper(){

}

bool TaskHelper::AddTask(const TaskData& taskData){
    hlogi("add task pid: %d, dest: %s, src: %s", taskData.pid, taskData.destUrl.c_str(), taskData.srcUrl.c_str());
    std::lock_guard<std::recursive_mutex> lock(rmtx);
    mapTask.insert({taskData.destUrl, taskData});
    return true;
}

bool TaskHelper::DelTask(const string& url){
    std::lock_guard<std::recursive_mutex> lock(rmtx);
    auto it = mapTask.find(url);
    if(it == mapTask.end()){
        hlogi("not find dest: %s", url.c_str());
        return false;
    }
    auto taskData = it->second;
    hlogi("del task pid: %d, dest: %s, src: %s", taskData.pid, taskData.destUrl.c_str(), taskData.srcUrl.c_str());
    mapTask.erase(taskData.destUrl);
    if(is_process_alive(taskData.pid))
        stop_ffmpeg(taskData);
    return true;
}

bool TaskHelper::AddTaskRun(const TaskData& taskData){
    TaskData data = taskData;
    DelTask(data.destUrl);
    data.startTime = time_t();
    int pid = exec_ffmpeg(data);
    if(pid <=  0){
        hlogi("pid: %d, dest: %s, src: %s", taskData.pid, taskData.destUrl.c_str(), taskData.srcUrl.c_str());
        return false;
    }   
    return AddTask(data);
}

