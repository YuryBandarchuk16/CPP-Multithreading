//
//  ThreadPool.h
//  ThreadPool
//
//  Created by Юрий Бондарчук on 27/09/2017.
//  Copyright © 2017 Yury Bandarchuk. All rights reserved.
//

#ifndef ThreadPool_h
#define ThreadPool_h

#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <functional>

template<typename T>
class Promise;

template<typename T>
class Future;

class ThreadPool {

public:

    static const size_t num_threads_default = 100;

    static ThreadPool& sharedInstance();

    explicit ThreadPool(size_t num_threads);
    ThreadPool(ThreadPool const& other) = delete;
    ThreadPool& operator=(ThreadPool const& other) = delete;
    ~ThreadPool();
    void execute(std::function<void()> const& f);

    template<typename IterType, typename F>
    using ret_parallel_t = std::vector< Future<std::result_of_t<F(typename IterType::value_type)>>>;

    template<typename IterType, typename F>
    ret_parallel_t<IterType, F> parallel(IterType begin, IterType end, F function) {
        std::vector< Future<std::result_of_t<F(typename IterType::value_type)>>> answer;

        for (; begin != end; ++begin) {

            auto promise = std::make_shared<Promise<typename std::result_of_t<F(typename IterType::value_type)>>>();

            execute([promise, function, begin] {
                promise->Set(function(*begin));
            });

            answer.push_back(promise->GetFuture());
        }

        return answer;
    };

    int getAmountOfTasksDone() {
        return tasksDone;
    }

private:

    size_t num_threads;
    std::atomic<bool> stopped;

    std::atomic<int> tasksDone;

    std::mutex mutex;
    std::condition_variable condition_variable;

    std::vector<std::thread> threads;
    std::queue< std::function<void()> > tasks;

    void runnable();
};


#endif /* ThreadPool_h */
