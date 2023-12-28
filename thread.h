//
// Created by GW on 2023/12/27.
//

#ifndef HIGHPERFORMANCESERVER_THREAD_H
#define HIGHPERFORMANCESERVER_THREAD_H
#include <thread>
#include <functional>
#include<memory>
#include<pthread.h>
#include<semaphore.h>
#include<stdint.h>
#include<atomic>
namespace superG
{
    class Semaphore
    {
    public:

        Semaphore(uint32_t count=0);
        void wait();
        void notify();
        ~Semaphore();

    private:
        Semaphore(Semaphore&)=delete;
        Semaphore(Semaphore&&)=delete;
       const Semaphore& operator=(Semaphore&)=delete;
    private:
        sem_t m_semaphore;
        std::string m_name;



    };

    template<class T>
    struct ScopedLockImpl
    {
    public:
       ScopedLockImpl(T& mutex):m_mutex(mutex)
       {
           m_mutex.lock();
           is_locked=true;
       }
       void lock()
       {
           if(!is_locked)
           {
               m_mutex.lock();
               is_locked= true;

           }
       }
       void unlock()
       {
           if(is_locked)
           {
               m_mutex.unlock();
               is_locked=false;
           }
       }

       ~ScopedLockImpl()
       {
           unlock();
       }
    private:
        T& m_mutex;
        bool is_locked;
    };
    template<class T>
    struct ReadScopedLockImpl
    {
    public:
        ReadScopedLockImpl(T& mutex):m_mutex(mutex)
        {
            m_mutex.rdlock();
            is_locked=true;
        }
        void lock()
        {
            if(!is_locked)
            {
                m_mutex.rdlock();
                is_locked= true;

            }
        }
        void unlock()
        {
            if(is_locked)
            {
                m_mutex.unlock();
                is_locked=false;
            }
        }

        ~ReadScopedLockImpl()
        {
            unlock();
        }
    private:
        T& m_mutex;
        bool is_locked;
    };
    template<class T>
    struct WriteScopedLockImpl
    {
    public:
        WriteScopedLockImpl(T& mutex):m_mutex(mutex)
        {
            m_mutex.wrlock();
            is_locked=true;
        }
        void lock()
        {
            if(!is_locked)
            {
                m_mutex.wrlock();
                is_locked= true;

            }
        }
        void unlock()
        {
            if(is_locked)
            {
                m_mutex.unlock();
                is_locked=false;
            }
        }

        ~WriteScopedLockImpl()
        {
            unlock();
        }
    private:
        T& m_mutex;
        bool is_locked;
    };
    class NullMutex
    {
    public:
        typedef ScopedLockImpl<NullMutex> Lock;
        NullMutex()
        {
        }
        void lock()
        {
        }
        void unlock()
        {
        }
        ~NullMutex()
        {
        }
    };
    class NullReadWriteMutex
    {
    public:
        typedef ReadScopedLockImpl<NullReadWriteMutex> ReadLock;
        typedef WriteScopedLockImpl<NullReadWriteMutex> WriteLock;

        NullReadWriteMutex()
        {
        }
        ~NullReadWriteMutex()
        {
        }
        void rdlock()
        {
        }
        void wrlock()
        {
        }
        void unlock()
        {
        }
    };
   class CASLock
   {
   public:
       typedef  ScopedLockImpl<CASLock> Lock;
       CASLock()
       {
           m_mutex.clear();
       }
       ~CASLock()
       {

       }
       void lock()
       {
           while(std::atomic_flag_test_and_set_explicit(&m_mutex,std::memory_order_acquire));
       }
       void unlock()
       {
           std::atomic_flag_clear_explicit(&m_mutex,std::memory_order_release);

       }

   private:
       volatile std::atomic_flag m_mutex;
   };
    class Mutex
    {
    public:
        typedef ScopedLockImpl<Mutex> Lock;
        Mutex()
        {
            pthread_mutex_init(&mutex, nullptr);
        }
        void lock()
        {
            pthread_mutex_lock(&mutex);
        }
        void unlock()
        {
            pthread_mutex_unlock(&mutex);
        }
        ~Mutex()
        {
            pthread_mutex_destroy(&mutex);
        }
    private:
        pthread_mutex_t mutex;
    };
    class ReadWriteMutex
    {
    public:
        typedef ReadScopedLockImpl<ReadWriteMutex> ReadLock;
        typedef WriteScopedLockImpl<ReadWriteMutex> WriteLock;

        ReadWriteMutex()
        {
            pthread_rwlock_init(&rwlock, nullptr);
        }
        ~ReadWriteMutex()
        {
            pthread_rwlock_destroy(&rwlock);
        }
        void rdlock()
        {
            pthread_rwlock_rdlock(&rwlock);
        }
        void wrlock()
        {
            pthread_rwlock_wrlock(&rwlock);

        }
        void unlock()
        {
            pthread_rwlock_unlock(&rwlock);

        }
    private:
        pthread_rwlock_t rwlock;
    };
    class Thread
    {
    public:
        typedef std::shared_ptr<Thread> ptr;
        Thread(std::function<void()> cb,const std::string name);
        ~Thread();
        pthread_t getId() const {return m_id;}
        const std::string& getName() const{return m_name;}


        void join();
        static Thread* GetThis();
        static const std::string& GetName();
        static void SetName(const std::string& name);



    private:
        Thread(const Thread&)=delete;
        Thread(const Thread&&)=delete;
        Thread& operator=(const Thread&)=delete;
        static void* run(void* arg);

    private:
        pthread_t m_id=(pthread_t)-1;
        pthread_t m_thread;
        std::function<void()> m_cb;
        std::string m_name;
        Semaphore semaphore;

    };
}
#endif //HIGHPERFORMANCESERVER_THREAD_H
