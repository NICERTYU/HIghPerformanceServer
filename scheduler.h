//
// Created by GW on 2023/12/31.
//

#ifndef HIGHPERFORMANCESERVER_SCHEDULER_H
#define HIGHPERFORMANCESERVER_SCHEDULER_H
#include <memory>
#include "thread.h"
#include "fiber.h"
namespace superG
{
 class Scheduler
 {
 public:
     typedef std::shared_ptr<Scheduler> ptr;
     typedef Mutex MutexType;
     Scheduler(size_t threadsnum=1,bool user_call= true,const std::string& name="");
     virtual ~Scheduler();
     const std::string& getName() const {return m_name;}
     void start();
     void stop();
     static Scheduler* GetThis();
     static Fiber* GetMainFiber();
     template<class FiberOrCb>
     void schele(FiberOrCb func,int thread_id=-1)
     {
         bool istickle=false;
         {
             MutexType::Lock lock(m_mutex);
             istickle=scheleNoLock(func,thread_id);

         }
         if(istickle)
         {
             tickle();
         }
     }
     template<class InputIterator>
     void schele(InputIterator begin,InputIterator end,int thread_id=-1)
     {
         bool istickle=false;
         {
             MutexType::Lock lock(m_mutex);
              while(begin!=end)
              {
                  istickle= scheleNoLock(&(*begin,thread_id)||istickle;
              }

         }
         if(istickle)
         {
             tickle();
         }
     }

 protected:
     virtual void tickle();
 private:
     template<class FiberOrCb>
     bool scheleNoLock(FiberOrCb fuc,int thread_id)
     {
         bool tickle=m_fibers.empty();
         FiberOrCb fc(fuc,thread_id);
         if(fc.fiber||fc.cb)
         {
             m_fibers.push_back(fc);
         }
         return tickle;
     }
 private:
     struct FiberAndThread
     {
         Fiber::ptr fiber;
         std::function<void()> cb;
         int thread;
         FiberAndThread(Fiber::ptr p,int thread_id):fiber(p),thread(thread_id)
         {

         }
         FiberAndThread(Fiber::ptr* p,int thread_id):thread(thread_id)
         {
          fiber.swap(*p);
         }
         FiberAndThread(std::function<void()> cb,int thread_id):cb(cb),thread(thread_id)
         {

         }
         FiberAndThread(std::function<void()> * p,int thread_id):thread(thread_id)
         {
             cb.swap(*p);
         }

         FiberAndThread():thread(-1)
         {

         }

         void reset()
         {
             fiber= NULL;
             thread=-1;
             cb=NULL;
         }

     };
 private:
    MutexType m_mutex;
    std::vector<Thread::ptr> threadpool;
    std::string m_name;
    std::list<FiberAndThread> m_fibers;
 };
}
#endif //HIGHPERFORMANCESERVER_SCHEDULER_H
