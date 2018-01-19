//
// Created by Юрий Бондарчук on 08/10/2017.
//

#ifndef FUTUREPROMISE_SHARED_OBJECT_H
#define FUTUREPROMISE_SHARED_OBJECT_H


#include <mutex>
#include "promise_future_exception.h"

class base_shared_object {
public:

};

template<typename T>
class shared_object {
public:

    template<typename T1> friend
    class Future;

    template<typename T2> friend
    class Promise;

    shared_object() : value(nullptr), exception_ptr(nullptr), value_set(false), is_promise_dead(false), callback_set(false) {
    }

    ~shared_object() {
        delete value;
    }

    T get_value() {
        wait();
        return *(this -> value);
    }

    void set_callback_executable(std::function<void(T)> callback_executable) {
        this -> callback_set = true;
        this -> callback_executable = callback_executable;
        if (this -> value_set) {
            callback_executable(*this -> value);
        }
    }

    void set_value(T const& value) {
        check_reset();
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this -> value = new T(value);
            this -> value_set = true;
        }
        condition_variable.notify_all();
        if (callback_set) {
            callback_executable(*this -> value);
        }
    }
    void set_value(T &&value) {
        check_reset();
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this -> value = new T(std::move(value));
            this -> value_set = true;
        }
        condition_variable.notify_all();
    }

    void promise_died() {
        is_promise_dead = true;
        value_set = true;
        condition_variable.notify_all();
    }
    bool is_ready() {
        std::lock_guard<std::mutex> lock(mutex); // maybe just make value_set std::atomic<bool>?
        return value_set;
    }
    void wait() {
        std::unique_lock<std::mutex> lock(this -> mutex);
        check_fail();
        this -> condition_variable.wait(lock, [&]{if (this -> value_set) return true; else return false;});
        check_fail();
    }
    void set_exception(std::exception_ptr const& exception_ptr) {
        this -> exception_ptr = new std::exception_ptr(exception_ptr);
    }
private:
    T *value;

    std::atomic<bool> callback_set;
    std::function<void(T)> callback_executable;

    std::atomic<bool> value_set;
    std::atomic<bool> is_promise_dead;
    std::mutex mutex;
    std::exception_ptr *exception_ptr;
    std::condition_variable condition_variable;

    void check_reset() {
        if (value_set) {
            throw promise_future_exception();
        }
    }

    void check_fail() {
        if (this->exception_ptr != nullptr) {
            std::rethrow_exception(*(this->exception_ptr));
        }
        if (this->is_promise_dead && this->value == nullptr) {
            throw promise_future_exception();
        }
    }
};

/** TEMPLATE SPECIALIZATION **/


/*
template<>
class shared_object<void> {
public:
    shared_object(): value_set(false), is_promise_dead(false), exception_ptr(nullptr) {
    }
    void get_value() {
        wait();
        return;
    }
    void set_value() {
        check_reset();
        {
            std::lock_guard<std::mutex> lock(mutex);
            this->value_set = true;
        }
        condition_variable.notify_all();
    }
    void promise_died() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            is_promise_dead = true;
            value_set = true;
        }
        condition_variable.notify_all();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(this -> mutex);
        check_fail();
        this -> condition_variable.wait(lock, [&]{if (this -> value_set) return true; else return false;});
        check_fail();
    }
    void set_exception(std::exception_ptr const& exception_ptr) {
        this -> exception_ptr = new std::exception_ptr(exception_ptr);
    }
    bool is_ready() {
        std::lock_guard<std::mutex> lock(mutex); // maybe just make value_set std::atomic<bool>?
        return value_set;
    }

private:
    std::atomic<bool> value_set;
    std::atomic<bool> is_promise_dead;
    std::mutex mutex;
    std::exception_ptr *exception_ptr;
    std::condition_variable condition_variable;

    void check_reset() {
        if (value_set) {
            throw promise_future_exception();
        }
    }

    void check_fail() {
        if (this->exception_ptr != nullptr) {
            std::rethrow_exception(*(this->exception_ptr));
        }
        if (this->is_promise_dead && !this->value_set) {
            throw promise_future_exception();
        }
    }
};


template<typename T>
class shared_object<T&> {
public:
    shared_object(): value(nullptr), value_set(false), is_promise_dead(false), exception_ptr(nullptr) {
    }
    T& get_value() {
        wait();
        return *(this -> value);
    }
    void set_value(T &value) {
        check_reset();
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this -> value = &value;
            this -> value_set = true;
        }
        condition_variable.notify_all();
    }
    void promise_died() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            is_promise_dead = true;
            value_set = true;
        }
        condition_variable.notify_all();
    }
    bool is_ready() {
        std::lock_guard<std::mutex> lock(mutex); // maybe just make value_set std::atomic<bool>?
        return value_set;
    }
    void wait() {
        std::unique_lock<std::mutex> lock(this -> mutex);
        check_fail();
        this -> condition_variable.wait(lock, [&]{if (this -> value_set) return true; else return false;});
        check_fail();
    }
    void set_exception(std::exception_ptr const& exception_ptr) {
        this -> exception_ptr = new std::exception_ptr(exception_ptr);
    }
private:
    T *value;
    std::atomic<bool> value_set;
    std::atomic<bool> is_promise_dead;
    std::mutex mutex;
    std::exception_ptr *exception_ptr;
    std::condition_variable condition_variable;

    void check_reset() {
        if (value_set) {
            throw promise_future_exception();
        }
    }

    void check_fail() {
        if (this->exception_ptr != nullptr) {
            std::rethrow_exception(*(this->exception_ptr));
        }
        if (this->is_promise_dead && this->value == nullptr) {
            throw promise_future_exception();
        }
    }
};
 */

#endif //FUTUREPROMISE_SHARED_OBJECT_H
