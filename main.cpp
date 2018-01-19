
#include "promise.h"
#include "flatten.h"
#include "tuple.h"

#include <cstdio>
#include <vector>
#include <numeric>
#include <thread>
#include <cassert>
#include <queue>
#include <deque>
#include <stdlib.h>
#include <tuple>


void test(std::string name) {
    std::shared_ptr<Promise<std::string>> promiseString(new Promise<std::string>);
    Future<std::string> futureString = promiseString -> GetFuture();
    std::shared_ptr<Promise<int>> promiseInt(new Promise<int>);
    Future<int> futureInt = promiseInt -> GetFuture();
    std::tuple<Future<int>, int, Future<std::string>> t = std::make_tuple(futureInt, 100500, futureString);
    ThreadPool pool(10);
    pool.execute([promiseInt]{
        std::this_thread::sleep_for(std::chrono::seconds(2));
        promiseInt -> Set(10);
    });
    pool.execute([promiseString, &name] {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::string hello = "Hello!";
        hello += name;
        promiseString -> Set(hello);
    });
    std::cout << "Start waiting for operations to complete..." << std::endl;
    auto unwrappedTuple = flattenTuple(t).Get();
    std::cout << std::get<0>(unwrappedTuple) << std::endl;
    std::cout << std::get<1>(unwrappedTuple) << std::endl;
    std::cout << std::get<2>(unwrappedTuple) << std::endl;
}

int main() {
    test(" It fucking works!");
    return 0;
}