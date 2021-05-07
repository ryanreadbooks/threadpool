#ifndef _LOCK_GUARD_H_
#define _LOCK_GUARD_H_


#include <pthread.h>
#include <unistd.h>


class LockGuard {
    public:
        LockGuard(pthread_mutex_t& mutex): mutex_(mutex) {
            pthread_mutex_lock(&mutex_);
        }
        ~LockGuard() {
            pthread_mutex_unlock(&mutex_);
        }
    private:
        pthread_mutex_t& mutex_;
};

#endif