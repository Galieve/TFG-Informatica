#pragma once
#ifndef TASK_DISPATCHER_H
#define TASK_DISPATCHER_H
#include <vector>
#include <thread>
#include <future>
#include <memory>
#include <atomic>

class task_dispatcher;

#ifndef DEFINE_H
    #include "../define.h"
#endif
#ifndef __ctpl_stl_thread_pool_H__
    #include "../CTPL/ctpl_stl.h"
#endif

class task_dispatcher{

protected:

    ctpl::thread_pool tpool;

    std::atomic<int> proc;

public:

    inline static task_dispatcher get_instance(){
        return task_dispatcher();
    }

    void stop();

    void dispatch_task(const task & function);

    void clear();

    bool is_busy();

    bool is_empty();

    task_dispatcher();

    ~task_dispatcher() = default;

    task_dispatcher(const task_dispatcher &);


protected:

    inline task_dispatcher & operator =(const task_dispatcher &) = delete;

};
#endif