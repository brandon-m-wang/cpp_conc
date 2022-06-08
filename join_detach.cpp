#include <iostream>
#include <thread>

void function_1() {
    for (int i = 0; i < 20; i++) {
        std::cout << "Long print." << std::endl;
    }
}

void function_2() { std::cout << "Short print." << std::endl; }

int main() {
    std::thread t1(function_1);
    std::thread t2(function_2);
    t2.detach();
    t1.join();
    return 0;
}