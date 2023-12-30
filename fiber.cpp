//
// Created by GW on 2023/12/30.
//
#include "fiber.h"
#include <atomic>
#include "config.h"
namespace superG
{
    static Logger::ptr g_logger= SUPERG_LOG_NAME("system");
    static std::atomic<uint64_t > s_fiber_id{0};
    static std::atomic<uint64_t > s_fiber_count{0};
    static thread_local Fiber* t_fiber= nullptr;
    static thread_local Fiber::ptr t_thread_fiber= nullptr;
    static ConfigVar<uint32_t>::ptr g_fiber_stack_size=configure::Lookup<uint32_t>("fiber.stack_size",1024*1024,"fiber stack size");

    class MallocStackAllocator
    {
    public:
        static void* Alloc(size_t size)
        {
            return malloc(size);
        }
        static void Dealloc(void* vp,size_t size)
        {
            return free (vp);
        }
    };
    using StackAllocator=MallocStackAllocator;
    Fiber::Fiber()
    {
        m_state=EXEC;
        SetThis(this);
        if(getcontext(&m_ctx))
        {
            SUPERG_ASSERT1(false,"getcontext")
        }
        ++s_fiber_count;
        m_id=s_fiber_id;


    }
    Fiber::Fiber(std::function<void()> cb,size_t stacksize):m_cb(cb),m_id(++s_fiber_id)
   {
   s_fiber_count++;
   m_stacksize=stacksize?stacksize:g_fiber_stack_size->getValue();
   m_stack=StackAllocator::Alloc(m_stacksize);
   if(getcontext(&m_ctx))
   {
           SUPERG_ASSERT1(false,"getcontext")
   }
   //一次执行
   m_ctx.uc_link= nullptr;
   m_ctx.uc_stack.ss_sp=m_stack;
   m_ctx.uc_stack.ss_size=m_stacksize;
   makecontext(&m_ctx,&Fiber::MainFunc,0);


   }
    Fiber::~Fiber()
    {
        if(m_stack)
        {
            SUPERG_ASSERT(m_state==INIT||m_state==TERM||m_state==EXCEPT);
            StackAllocator::Dealloc(m_stack,m_stacksize);
        } else{
         Fiber* cur=t_fiber;
         SUPERG_ASSERT(cur==this);
         SUPERG_ASSERT(!m_cb);
         SetThis(nullptr);



        }
    }
    void Fiber::reset(std::function<void()> cb)
    {
        SUPERG_ASSERT(m_stack);
        SUPERG_ASSERT(m_state==INIT||m_state==TERM||m_state==EXCEPT);
        m_cb=cb;
        if(getcontext(&m_ctx))
        {
            SUPERG_ASSERT1(false,"getcontext")
        }
        m_ctx.uc_link= nullptr;
        m_ctx.uc_stack.ss_sp=m_stack;
        m_ctx.uc_stack.ss_size=m_stacksize;
        makecontext(&m_ctx,&Fiber::MainFunc,0);
        m_state=INIT;

    }

    void Fiber::SwapIn()
    {
        SetThis(this);
        SUPERG_ASSERT(m_state!=EXEC);
        m_state=EXEC;
        if(swapcontext(&t_thread_fiber->m_ctx,&m_ctx))
        {
            SUPERG_ASSERT1(false,"swap context")
        }

    }

    void Fiber::SwapOut()
    {
        SetThis(t_thread_fiber.get());
        if(swapcontext(&m_ctx,&t_thread_fiber->m_ctx))
        {
            SUPERG_ASSERT1(false,"swap context")
        }



    }
    void Fiber::SetThis(Fiber* t)
    {
        t_fiber=t;
    }
     Fiber::ptr Fiber::GetThis()
    {
        if(t_fiber)
        {
            return t_fiber->shared_from_this();
        }
        Fiber::ptr main_fiber(new Fiber);
        SUPERG_ASSERT(t_fiber==main_fiber.get());
        t_thread_fiber=main_fiber;
        return t_fiber->shared_from_this();

    }
    void Fiber::YieldToReady()
    {
        Fiber::ptr pt=GetThis();
        pt->m_state=READY;
        pt->SwapOut();
    }
    void Fiber::YieldToHold()
    {
        Fiber::ptr pt=GetThis();
        pt->m_state=HOLD;
        pt->SwapOut();
    }
    uint64_t Fiber::TotalFibers()
    {
        return s_fiber_count;
    }
    void Fiber::MainFunc()
    {
        Fiber::ptr pt=GetThis();
        SUPERG_ASSERT(pt);
        try
        {
            pt->m_cb();
            pt->m_cb= nullptr;
            pt->m_state=TERM;
        }
        catch(std::exception &ex)
        {
            pt->m_state=EXCEPT;
            SUPERG_LOG_ERROR(g_logger)<<"Fiber Exception"<<ex.what();

        }catch(...)
        {
            pt->m_state=EXCEPT;
            SUPERG_LOG_ERROR(g_logger)<<"Fiber Exception";
        }
        pt->SwapOut();




    }

     uint64_t Fiber::GetFiberId()
    {
        if(t_fiber)
        {
            return t_fiber->m_id;
        }
        return 0;
    }

}