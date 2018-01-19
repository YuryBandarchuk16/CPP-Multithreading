//
// Created by Юрий Бондарчук on 08/10/2017.
//

#ifndef FUTUREPROMISE_FUTURE_H
#define FUTUREPROMISE_FUTURE_H


#include "shared_object.h"
#include "ThreadPool.h"
#include "promise.h"
#include <type_traits>

template<typename T>
class Promise;

class ThreadPool;

template<typename T>
class Future {
public:
    Future(std::function<T()> get_lambda, std::function<void()> wait_lambda, std::function<bool()> is_ready_lambda) {
        substitute_implementation = true;
        this -> get_lambda = get_lambda;
        this -> wait_lambda = wait_lambda;
        this -> is_ready_lambda = is_ready_lambda;
    }
    Future(Future const&other) = default;
    Future(Future &&other) = default;
    Future<T>& operator=(Future<T> const& other) = delete;
    Future(std::shared_ptr<shared_object<T>> const&shared_data) {
        this -> shared_data = shared_data;
    }
    T Get() const {
        if (substitute_implementation) {
            return this -> get_lambda();
        }
        return shared_data -> get_value();
    }
    bool IsReady() const {
        if (substitute_implementation) {
            return this -> is_ready_lambda();
        }
        return shared_data -> is_ready();
    }
    void Wait() const {
        if (substitute_implementation) {
            this -> wait_lambda();
        } else {
            shared_data->wait();
        }
    }
    template<typename R, typename F>
    friend Future<typename std::result_of_t<F(R)>>
    Map(const Future<R> &future, const F &function) {
        if (future.shared_data -> callback_set) {
            throw std::logic_error("you could not set function to map twice");
        }
        std::shared_ptr<Promise<std::result_of_t<F(R)>>> promise(new Promise<std::result_of_t<F(R)>>());
        future.shared_data -> set_callback_executable([=](R &&r) {
            ThreadPool::sharedInstance().execute(
                        [promise, r = std::move(r), function]() {
                            promise -> Set(std::move(function(r)));
                        }
                );
            }
        );
        return std::move(promise -> GetFuture());
    };
private:
    bool substitute_implementation = false;
    std::function<T()> get_lambda;
    std::function<void()> wait_lambda;
    std::function<bool()> is_ready_lambda;
    std::shared_ptr<shared_object<T>> shared_data;
};

#endif //FUTUREPROMISE_FUTURE_H
