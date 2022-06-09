#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

class LogFile {
    std::mutex _mu;
    std::mutex _mu_open;
    std::once_flag _flag;
    std::ofstream _f;

  public:
    // constructor
    LogFile() {}
    // destructor
    ~LogFile() { _f.close(); }
    void shared_print(std::string msg, int id) {
        // {
        //     // need to protect the if check too, since otherwise we risk the
        //     // file being opened twice for no reason causing slowdowns
        //     std::unique_lock<std::mutex> open_locker(_mu_open);
        //     if (!_f.is_open()) {
        //         _f.open("log.txt");
        //     }
        // }
        // creating a unique_lock and checking for _f.is_open() is far too slow
        // everytime, so we can use call_once. this is lazy initialization.
        std::call_once(_flag, [&]() { _f.open("log.txt"); });
        std::unique_lock<std::mutex> locker(_mu, std::defer_lock);
        // do something else

        locker.lock();
        _f << msg << id << std::endl;
        locker.unlock();
        // can lock and unlock arbitrary number of times (not possible with
        // std::lock_guard)
        locker.lock();

        // unique_locks can be moved, lock_guards cannot
        std::unique_lock<std::mutex> locker2 = std::move(locker);
    }
};

void function_1(LogFile &log) {
    for (int i = 0; i > -100; i--) {
        log.shared_print("From t1: ", i);
    }
}

int main() {
    LogFile log;
    std::thread t1(function_1, std::ref(log));

    for (int i = 0; i < 100; i++) {
        log.shared_print("From main: ", i);
    }

    t1.join();

    return 0;
}
