#include <iostream>
#include "safe_queue.h"


int main(int argc, char const *argv[])
{
    
    SafeQueue<int> my_q;
    my_q.enqueue(50);
    my_q.enqueue(10);
    my_q.enqueue(20);
    my_q.enqueue(30);
    int a = my_q.dequeue();
    std::cout << a << " ";
    a = my_q.dequeue();
    std::cout << a << " ";
    a = my_q.dequeue();
    std::cout << a << " ";
    a = my_q.dequeue();
    std::cout << a << " ";

    return 0;
}
