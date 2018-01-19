//
// Created by Юрий Бондарчук on 25/10/2017.
//

#ifndef FUTUREPROMISE_FLATTEN_H
#define FUTUREPROMISE_FLATTEN_H

#include "future.h"

template<typename T>
bool flattenIsReady(Future<T> const& future) {
    return future.IsReady();
}

template<typename T>
bool flattenIsReady(Future<Future<T>> const& future) {
    if (future.IsReady()) {
        return flattenIsReady(future.Get());
    } else {
        return false;
    }
}

template<typename T>
T flattenGet(Future<T> const& future) {
    return future.Get();
}

template<typename T>
auto flattenGet(Future<Future<T>> const& future) -> decltype(flattenGet(future.Get())) {
    return flattenGet(future.Get());
}

template<typename T>
void flattenWait(Future<T> const& future) {
    future.Wait();
}

template<typename T>
void flattenWait(Future<Future<T>> const& future) {
    flattenWait(future.Get());
}

template<typename T>
Future<T> Flatten(Future<T> const& future) {
    return future;
}

template<typename T>
Future<T> Flatten(Future<Future<T>> const& future) {
    std::function<bool()> is_ready = [&]() {
        return flattenIsReady(future);
    };
    std::function<void()> wait = [&]() {
        flattenWait(future);
    };
    return Future<T>([&]() { return flattenGet(future); }, wait, is_ready);
}

template<template<typename...> class C, typename T>
bool flattenIsReady(C<Future<T>> const& collection) {
    for (Future<T> const& item: collection) {
        if (!item.IsReady()) {
            return false;
        }
    }
    return true;
};

template<template<typename...> class C, typename T>
void flattenWait(C<Future<T>> const& collection) {
    for (Future<T> const& item: collection) {
        item.Wait();
    }
};

template<template<typename...> class C, typename T>
C<T> flattenGet(C<Future<T>> const& collection) {
    C<T> result;
    for (Future<T> const& item: collection) {
        result.insert(result.end(), item.Get());
    }
    return result;
};

template<template<typename...> class C, typename T>
Future<C<T>> Flatten(C<Future<T>> const& collection) {
    std::function<bool()> is_ready = [&]() {
        return flattenIsReady(collection);
    };
    auto wait = [&]() {
        flattenWait(collection);
    };
    return Future<C<T>>([&collection]() { return flattenGet(collection); }, wait, is_ready);
};


#endif //FUTUREPROMISE_FLATTEN_H
