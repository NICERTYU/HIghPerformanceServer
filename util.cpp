//
// Created by GW on 2023/12/18.
//
#include "util.h"
#include "execinfo.h"
namespace superG
{
superG::Logger::ptr log2phys= SUPERG_LOG_NAME("system");




    pid_t GetThreadId()
    {
//        return syscall(SYS_thread_selfid);


        static uint64_t id=0;
        if(id!=0)return  id;
        pthread_threadid_np(0, &id);
        return id;
//        pthread_t tid = pthread_self();
//        return tid;

    }
    uint32_t GetFiberd()
    {
        return 0;
    }

    void BackTrace(std::vector<std::string> &vec,int size,int skip)
    {
        void** array=(void**)malloc(sizeof(void*)*size);
        size_t s=::backtrace(array,size);
        char** strings= backtrace_symbols(array,s);
        if(strings== nullptr)
        {
            SUPERG_LOG_ERROR(log2phys)<<"backtrace symbolms,error";
            return;
        }
        for(int i=skip;i<s;i++)
        {
            vec.push_back(strings[i]);
        }
        free(strings);
        free(array);

    }

    std::string BacktraceToString(int size,const std::string& prefix,int skip)
   {
        std::vector<std::string> vec;
       BackTrace(vec,size,skip);
       std::stringstream ss;
       for(int i=0;i<vec.size();i++)
       {
           ss<<prefix<<vec[i]<<std::endl;
       }

       return ss.str();

    }

}

