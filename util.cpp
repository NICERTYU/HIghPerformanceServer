//
// Created by GW on 2023/12/18.
//
#include "util.h"
namespace superG
{
    pid_t GetThreadId()
    {
//        return syscall(SYS_thread_selfid);


        static uint64_t id=0;
        if(id!=0)return  id;
        pthread_threadid_np(0, &id);
        return id;
    }
    uint32_t GetFiberd()
    {
        return 0;
    }
}
