#include <iostream>
#include <thread>
#include <mutex>

std::mutex mu;

void shared_print(std::string msg, int id) {
    std::lock_guard<std::mutex> guard(mu);
    std::cout << msg << id << std::endl;
}

void function_1() {
    for (int i = 0; i > -100; i--) {
        shared_print("From t1: ", i);
    }
}

int main() {
    std::thread t1(function_1);

    for (int i = 0; i < 100; i++) {
        shared_print("From main: ", i);  
    } 

    t1.join();

    return 0;
}