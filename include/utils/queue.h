#pragma once

#include <faabric/util/exception.h>
#include <faabric/util/locks.h>
#include <queue>

using namespace std;
using namespace faabric::util;

template<typename T>
class Queue {
public:
    void enqueue(T value) {
        mq.emplace(std::move(value));
    }

    T tryDequeue(T tryValue) {
        if (mq.empty())
            return tryValue;
        T value = std::move(mq.front());
        mq.pop();
        return value;
    }

    void drain() {
        while (!mq.empty()) {
            mq.pop();
        }
    }

    long size() {
        return mq.size();
    }

    void reset() {
        std::queue<T> empty;
        std::swap(mq, empty);
    }

    mutex &getMx() {
        return mx;
    }

private:
    std::queue<T> mq;
    std::mutex mx;
};
