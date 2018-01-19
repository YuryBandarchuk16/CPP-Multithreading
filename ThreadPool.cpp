//
//  ThreadPool.cpp
//  ThreadPool
//
//  Created by Юрий Бондарчук on 28/09/2017.
//  Copyright © 2017 Yury Bandarchuk. All rights reserved.
//

#include "ThreadPool.h"

#include <iostream>

ThreadPool::ThreadPool(size_t num_threads) {
    stopped = false;
    tasksDone = 0;
    this -> num_threads = num_threads;
    threads.reserve(num_threads);
    threads.resize(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        threads[i] = std::thread(&ThreadPool::runnable, this);
    }
}

ThreadPool::~ThreadPool() {
    stopped = true;
    condition_variable.notify_all();
    for (size_t i = 0; i < num_threads; ++i) {
        threads[i].join();
    }
}

void ThreadPool::execute(std::function<void()> const& f) {
    if (stopped) return;
    {
        std::unique_lock<std::mutex> smart_lock(mutex);
        tasks.push(f);
    }
    condition_variable.notify_one();
}

void ThreadPool::runnable() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> smart_lock(mutex);
            condition_variable.wait(smart_lock, [&](){return !(!stopped && tasks.empty());});
            if (stopped) {
                break;
            }
            task = tasks.front();
            tasks.pop();
        }
        task();
        ++tasksDone;
    }

}

ThreadPool& ThreadPool::sharedInstance() {

    static ThreadPool pool(10);
    return pool;
}

