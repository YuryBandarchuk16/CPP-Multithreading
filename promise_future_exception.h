//
// Created by Юрий Бондарчук on 08/10/2017.
//

#ifndef FUTUREPROMISE_PROMISE_FUTURE_EXCEPTION_H
#define FUTUREPROMISE_PROMISE_FUTURE_EXCEPTION_H


#include <exception>
#include <stdexcept>

class promise_future_exception: public std::exception {
public:
    const char* what() {
        return "promise_future_exception";
    }
};


#endif //FUTUREPROMISE_PROMISE_FUTURE_EXCEPTION_H
