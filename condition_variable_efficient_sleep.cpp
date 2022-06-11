#include <deque>
#include <iostream>
#include <thread>

std::deque<int> deque;
std::mutex mu;
std::condition_variable cond;

void producer() {
    int count = 100000;
    while (count > 0) {
        std::unique_lock<std::mutex> lock(mu);
        deque.push_front(count);
        lock.unlock();
        cond.notify_one(); // notify one waiting thread, if any
        count--;
    }
}

void consumer() {
    int data = 0;
    while (data != 1) {
        std::unique_lock<std::mutex> lock(mu);
        // sleep thread until cond is notified, release
        // mutex/lock before sleep, reacquire lock after wake
        // lambda function is to guard against spurious wake
        cond.wait(lock, []() { return !deque.empty(); });
        data = deque.back();
        deque.pop_back();
        lock.unlock();
        std::cout << "t2 got a value from t1: " << data << std::endl;
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    return 0;
}