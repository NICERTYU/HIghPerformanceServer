//
// Created by GW on 2023/12/18.
//

#ifndef HIGHPERFORMANCESERVER_UTIL_H
#define HIGHPERFORMANCESERVER_UTIL_H

#include <unistd.h>       // for syscall()
#include <sys/syscall.h>  // for SYS_xxx definitions
#include<vector>
#include<string>
#include "logger.h"
//int id = syscall(SYS_thread_selfid); // for mac os x
#include <csignal>
#include<sys/syscall.h>
#include<pthread.h>
#include<sys/types.h>
#include <stdio.h>
namespace superG {
    pid_t GetThreadId();

    uint32_t GetFiberd();

    void BackTrace(std::vector<std::string> &vec,int size,int skip=1);

    std::string BacktraceToString(int size,const std::string& prefix,int skip=2);



}
#endif //HIGHPERFORMANCESERVER_UTIL_H
