#ifndef _DAEMON_H__
#define _DAEMON_H__

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include  <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>

int daemonize(int nochdir, int noclose)
{
    int fd;

    switch (fork()) {
    case -1:
        return (-1);
    case 0:
        break;
    default:
        _exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)
        return (-1);

    if (nochdir == 0) {
        if(chdir("/") != 0) {
            perror("chdir");
            return (-1);
        }
    }

    if (noclose == 0 && (fd = open("/dev/null", O_RDWR, 0)) != -1) {
        if(dup2(fd, STDIN_FILENO) < 0) {
            perror("dup2 stdin");
            return (-1);
        }
        if(dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2 stdout");
            return (-1);
        }
        if(dup2(fd, STDERR_FILENO) < 0) {
            perror("dup2 stderr");
            return (-1);
        }

        if (fd > STDERR_FILENO) {
            if(close(fd) < 0) {
                perror("close");
                return (-1);
            }
        }
    }
    return (0);
}

int close_all_fd(void)
{
    struct rlimit lim;
    unsigned int i;
    if(getrlimit(RLIMIT_NOFILE, &lim) < 0)
    return -1;
    if (lim.rlim_cur == RLIM_INFINITY)
        lim.rlim_cur = 1024;
    for (i = 0; i < lim.rlim_cur; i ++) 
    {
        if (close(i) < 0 && errno != EBADF)
        return -1;
    }
    return 0;
}


int my_system(const char *cmdstring)
{
    int status;
    pid_t pid = fork();
    if (pid < 0) {
        perror("Error when trying to fork");
        return -1;
    }
    if (pid == 0) {
        close_all_fd();
        // If succeeds, execl does not return
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        // With fork() we must not do anything other than execl or _exit
        exit(127);
    }
    while(waitpid(pid, &status, 0) < 0)
    {
        if(errno != EINTR)
        {
            status = -1;
            break; 
        }                  
    }
    return status;
}


#endif
