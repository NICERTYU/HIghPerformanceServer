//
// Created by GW on 2023/12/18.
//

#ifndef HIGHPERFORMANCESERVER_UTIL_H
#define HIGHPERFORMANCESERVER_UTIL_H

#include <unistd.h>       // for syscall()
#include <sys/syscall.h>  // for SYS_xxx definitions

//int id = syscall(SYS_thread_selfid); // for mac os x
#include <csignal>
#include<sys/syscall.h>
#include<pthread.h>
#include<sys/types.h>
#include <stdio.h>
namespace superG {
    pid_t GetThreadId();

    uint32_t GetFiberd();



}
#endif //HIGHPERFORMANCESERVER_UTIL_H
