#include <iostream>
#include <pthread.h>
#include <string>
#include <cstring>

#include "threadpool.h"


using namespace std;

class PrintTask: public Task {
    public:
        PrintTask(const string& name) :Task(name) {}
        virtual ~PrintTask() {}
        void run() {
            int i = 0;
            while (i < 3)
            {
                printf("%s in thread %lu is running\n", get_name().c_str(), pthread_self());
                usleep(10000);
                i ++;
            }
            printf("%s finished working\n", get_name().c_str());
        }

};


int main(int argc, char const *argv[])
{
    ThreadPool pool(15);
    // 添加10个任务
    string base_name = "task-";
    
    for (int i = 0; i < 10; i ++) {
        string name = base_name + to_string(i);
        // PrintTask task(name);
        PrintTask* task = new PrintTask(name);
        // task.set_name(name);
        pool.add_task(task);
    }

    while(1)  
    {  
        printf("there are still %d tasks need to process\n", pool.task_size());  
        if (pool.task_size() == 0)
        {  
            pool.stop();
            printf("Now I will exit from main\n"); 
            break;
            // exit(0);   
        } else {
            cout << "main thread\n";
        }
        sleep(2);  
    }  

    printf("main thread already ended\n");
    return 0;
}
 