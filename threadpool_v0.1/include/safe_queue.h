#ifndef _SAFE_QUEUE_H_
#define _SAFE_QUEUE_H_

/**
 * 包装一个线程安全的队列
*/

#include <queue>
#include <pthread.h>
#include "lock_gurad.h"

using std::queue;

template <typename T>
class SafeQueue {
    private:
        queue<T> data_;
        pthread_mutex_t mutex_;

    public:
        SafeQueue();
        ~SafeQueue();
        void enqueue(const T& item);
        T dequeue();
};


template <typename T>
SafeQueue<T>::SafeQueue(): mutex_(PTHREAD_MUTEX_INITIALIZER) {
};

template <typename T>
SafeQueue<T>::~SafeQueue() {
    pthread_mutex_destroy(&(this->mutex_));
};

template <typename T>
void SafeQueue<T>::enqueue(const T& item) {
    LockGuard(this->mutex_);
    this->data_.push(item);
};

template <typename T>
T SafeQueue<T>::dequeue() {
    LockGuard(this->mutex_);
    T data = this->data_.front();
    this->data_.pop();
    return data;
};

#endif