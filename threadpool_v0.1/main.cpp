#include <iostream>
#include <memory>
#include <pthread.h>
#include <string>
#include <cstring>

#include "threadpool.h"


using namespace std;

class PrintTask: public Task {
    public:
        PrintTask(const string& name) :Task(name) {}
        PrintTask(const string& name, void* args): Task(name, args) {}
        virtual ~PrintTask() {}
        void run() {
            int i = 0;
            char* str = (char*) get_args();
            string s1(str);
            while (i < 3)
            {
                s1 += "olele";
                printf("%s in thread %lu is running, say %s to you\n", get_name().c_str(), pthread_self(), s1.c_str());
                usleep(10000);
                i ++;
            }
            printf("%s finished working\n", get_name().c_str());
        }

};


int main(int argc, char const *argv[])
{
    ThreadPool pool(3);
    // 添加10个任务
    string base_name = "task-";
    char* strs[] = {"wonderful", "hello world", "ok", "let's go", "cpp is cool"};
    for (int i = 0; i < 5; i ++) {
        string name = base_name + to_string(i);
        PrintTask* task = new PrintTask(name, (void*)(strs[i]));    // 必须用指针，因为里面用了多态
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
 