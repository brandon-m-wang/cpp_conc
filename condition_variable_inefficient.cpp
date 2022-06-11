#include <deque>
#include <iostream>
#include <thread>

std::deque<int> deque;
std::mutex mu;

void producer() {
    int count = 100000;
    while (count > 0) {
        std::unique_lock<std::mutex> lock(mu);
        deque.push_front(count);
        lock.unlock();
        count--;
    }
}

void consumer() {
    int data = 0;
    while (data != 1) {
        std::unique_lock<std::mutex> lock(mu);
        if (!deque.empty()) {
            data = deque.back();
            deque.pop_back();
            lock.unlock();
            std::cout << "t2 got a value from t1: " << data << std::endl;
        } else {
            lock.unlock();
        }
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    return 0;
}