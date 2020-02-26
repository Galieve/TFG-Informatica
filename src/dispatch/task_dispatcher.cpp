#include "task_dispatcher.h"

task_dispatcher::task_dispatcher()
    : tpool(std::thread::hardware_concurrency()){
}

void task_dispatcher::stop(){
    tpool.stop(true);
}

void task_dispatcher::dispatch_task(const task &function){
    tpool.push([this, function](int id){
        ++proc;
        function();
        --proc;
    });
}

void task_dispatcher::clear(){
    tpool.clear_queue();
}



bool task_dispatcher::is_busy(){
    return proc >= std::thread::hardware_concurrency();
}
