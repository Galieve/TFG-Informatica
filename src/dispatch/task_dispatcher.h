#pragma once
#ifndef TASK_DISPATCHER_H
#define TASK_DISPATCHER_H
#include <vector>
#include <thread>
#include <future>
#include <memory>
#include <atomic>
#ifndef DEFINE_H
    #include "../define.h"
#endif
#include "../CTPL/ctpl_stl.h"

class task_dispatcher{

protected:

    ctpl::thread_pool tpool;

    std::atomic_int proc = 0;

public:

    static task_dispatcher get_instance(){
        return task_dispatcher();
    }

    void stop();

    void dispatch_task(const task & function);

    void clear();

    bool is_busy();

    task_dispatcher();

    ~task_dispatcher() = default;

    task_dispatcher(const task_dispatcher &);


protected:

    task_dispatcher & operator =(const task_dispatcher &) = delete;

};
#endif