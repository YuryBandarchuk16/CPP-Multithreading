//
// Created by Юрий Бондарчук on 06/12/2017.
//

#ifndef FUTUREPROMISE_TUPLE_H
#define FUTUREPROMISE_TUPLE_H

#include "future.h"
#include "ThreadPool.h"
#include <type_traits>

template<typename T>
struct get_inner_type {
    typedef T type;
};

template<typename T>
struct get_inner_type<Future<T>> {
    typedef T type;
};

template<typename T>
struct get_inner_type<Future<Future<T>>> {
    typedef typename get_inner_type<T>::type type;
};

template<typename ...Ts>
struct get_inner_type<std::tuple<Ts...>> {
    typedef std::tuple<typename get_inner_type<Ts>::type...> type;
};

template <typename T>
T flattenGet(T &x) {
  return x;
}

template <typename T>
T flattenGet(Future<T> &x) {
  return x.Get();
}

template<typename T>
auto flattenGet(Future<Future<T>> &x) {
    return flattenGet(x.Get());
}

template <size_t... Is, typename... Ts>
auto Get(std::index_sequence<Is...>, std::tuple<Ts...> &t) {
  return std::make_tuple(flattenGet(std::get<Is>(t))...);
}

template <size_t N, typename... Ts>
auto Get(std::tuple<Ts...> &t) {
  return Get(std::make_index_sequence<N>{}, t);
}

template<typename ...Ts, std::size_t... I>
auto flattenMakeTuple(const std::tuple<Ts...> &t, std::index_sequence<I...>) {
    return std::make_tuple(flattenGet(std::get<I>(t))...);
}

template<typename ...Ts, typename Indices = std::make_index_sequence<sizeof...(Ts)>>
auto flattenTuple(std::tuple<Ts...> t) {
    using return_type = typename get_inner_type<std::tuple<Ts...>>::type;
    std::shared_ptr<Promise<return_type>> promise(new Promise<return_type>);
    ThreadPool::sharedInstance().execute([promise, &t]() {
        auto temp = flattenMakeTuple(t, Indices{});
        promise->Set(temp);
    });
    return promise->GetFuture();
}


#endif //FUTUREPROMISE_TUPLE_H
