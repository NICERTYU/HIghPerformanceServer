//
// Created by GW on 2023/12/27.
//

#include "thread.h"
#include "logger.h"

namespace superG
{
    static thread_local Thread* p_thread= nullptr;
    static thread_local std::string p_name="UNKNOW";
    static superG::Logger::ptr logger=SUPERG_LOG_NAME("system");
    Semaphore::Semaphore(uint32_t count)
    {
//     if(sem_init(&m_semaphore,0,count))
//     {
//         throw std::logic_error("semaphore init error");
//     }
     m_name="jkkiio";
     if(!sem_open(m_name.c_str(), O_CREAT, 0644, count ))
     {
         throw std::logic_error("semaphore init error");
     }

    }
    void Semaphore::wait()
    {
        if(!sem_wait(&m_semaphore))
        {
            throw std::logic_error("sem wait error");
        }

    }
    void Semaphore::notify()
    {
        if(!sem_post(&m_semaphore))
        {
            throw std::logic_error("semaphore post error");

        }
    }
    Semaphore::~Semaphore()
    {
//        sem_destroy(&m_semaphore);
         sem_close(&m_semaphore);


    }
    Thread* Thread::GetThis()
    {
        return p_thread;
    }
    const std::string& Thread::GetName()
    {
       return p_name;
    }
    void* Thread::run(void* arg)
    {
     Thread* thread=(Thread*)arg;
     p_thread=thread;
     p_name=thread->m_name;
     thread->m_id= pthread_self();
     pthread_setname_np(thread->m_name.substr(0,15).c_str());

     std::function<void()> cb;
     cb.swap(thread->m_cb);
     thread->semaphore.notify();
     cb();

     return 0;

    }

     void Thread::SetName(const std::string& name)
     {
        if(p_thread)
        {
            p_thread->m_name=name;
        }
         p_name=name;


     }

  Thread::Thread(std::function<void()> cb,const std::string name):m_cb(cb)
  {
        if(name.empty())
        {
            m_name="UNKNOW";
        }
        else
        {
            m_name=name;
        }
        int rt= pthread_create(&m_thread, nullptr,&Thread::run,this);

        if(rt)
        {
            SUPERG_LOG_ERROR(logger)<<"pthread creat error pthreadname::"<<m_name<<" rt="<<rt;
            throw std::logic_error("pthread create error");
        }
        semaphore.wait();



  }
   Thread::~Thread()
   {
     if(m_thread)
     {
         pthread_detach(m_thread);
     }

   }



    void Thread::join()
    {
        if(m_thread)
        {
            int rt=pthread_join(m_thread, nullptr);
            if(rt)
            {
                SUPERG_LOG_ERROR(logger)<<"pthread join error pthreadname::"<<m_name<<" rt="<<rt;
                throw std::logic_error("pthread join error");
            }

        }
        m_thread=0;

    }
}
