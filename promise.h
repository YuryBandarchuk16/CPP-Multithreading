//
// Created by Юрий Бондарчук on 08/10/2017.
//

#ifndef FUTUREPROMISE_PROMISE_H
#define FUTUREPROMISE_PROMISE_H

#include "future.h"
#include "shared_object.h"
#include "promise_future_exception.h"

#include <exception>
#include <iostream>

template<typename T>
class Promise {
public:

    template<typename T1> friend
    class Future;

    Promise() {
        shared_data = std::make_shared<shared_object<T>>();
    }
    ~Promise() {
        if (shared_data) {
            shared_data -> promise_died();
        }
    }
    Promise(Promise<T> &&other) {
        this -> shared_data = std::move(other.shared_data);
    }
    Future<T> GetFuture() {
        Future<T>result(shared_data);
        return result;
    }
    void Set(T const& value) {
        shared_data -> set_value(value);
    }
    void Set(T&& value) {
        shared_data -> set_value(value);
    }
    void SetException(std::exception_ptr const& exception_ptr) {
        shared_data -> set_exception(exception_ptr);
    }

private:
    std::shared_ptr<shared_object<T>> shared_data;
};


/** TEMPLATE SPECIALIZATION! **/

template<>
class Promise<void> {
public:

    template<typename T1> friend
    class Future;

    Promise() {
        shared_data = std::make_shared<shared_object<void>>();
    }
    ~Promise() {
        if (shared_data) {
            shared_data -> promise_died();
        }
    }
    Promise(Promise<void> &&other) {
        this -> shared_data = std::move(other.shared_data);
    }
    Future<void> GetFuture() {
        Future<void>result(shared_data);
        return result;
    }
    void Set() {
        shared_data -> set_value();
    }
    void SetException(std::exception_ptr const& exception_ptr) {
        shared_data -> set_exception(exception_ptr);
    }

private:
    std::shared_ptr<shared_object<void>> shared_data;
};


template<typename T>
class Promise<T&> {
public:

    template<typename T1> friend
    class Future;

    Promise() {
        shared_data = std::make_shared<shared_object<T&>>();
    }
    ~Promise() {
        if (shared_data) {
            shared_data -> promise_died();
        }
    }
    Promise(Promise<T&> &&other) {
        this -> shared_data = std::move(other.shared_data);
    }
    Future<T&> GetFuture() {
        Future<T&>result(shared_data);
        return result;
    }
    void Set(T &value) {
        shared_data -> set_value(value);
    }
    void SetException(std::exception_ptr const& exception_ptr) {
        shared_data -> set_exception(exception_ptr);
    }

private:
    std::shared_ptr<shared_object<T&>> shared_data;
};

#endif //FUTUREPROMISE_PROMISE_H
