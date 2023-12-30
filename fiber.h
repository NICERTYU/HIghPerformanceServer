//
// Created by GW on 2023/12/30.
//

#ifndef HIGHPERFORMANCESERVER_FIBER_H
#define HIGHPERFORMANCESERVER_FIBER_H
#define _XOPEN_SOURCE
#include<ucontext.h>
#include<memory>
#include "thread.h"
#include<functional>
#include "macro.h"
namespace superG
{
class Fiber:public std::enable_shared_from_this<Fiber>
{
public:
    typedef std::shared_ptr<Fiber> ptr;
    enum State
    {
        INIT,
        READY,
        HOLD,
        EXEC,
        TERM,
        EXCEPT
    };
private:
    Fiber();
public:
    Fiber(std::function<void()> cb,size_t stacksize=0);
    ~Fiber();
    void reset(std::function<void()> cb);
    void SwapIn();
    void SwapOut();
    uint64_t getId() const {return m_id;};
public:
     static Fiber::ptr GetThis();
     static void YieldToReady();
     static void YieldToHold();
     static uint64_t TotalFibers();
     static void MainFunc();
     static void SetThis(Fiber* t);
     static uint64_t GetFiberId();
private:
    uint64_t m_id;
    uint32_t m_stacksize;
    State m_state=INIT;
    ucontext_t m_ctx;
    void* m_stack= nullptr;
    std::function<void()> m_cb;

};
}

#endif //HIGHPERFORMANCESERVER_FIBER_H
