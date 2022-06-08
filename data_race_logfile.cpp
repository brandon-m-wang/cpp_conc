#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

class LogFile {
    std::mutex _mu;
    std::ofstream _f;

  public:
    // constructor
    LogFile() { _f.open("log.txt"); }
    // destructor
    ~LogFile() { _f.close(); }
    void shared_print(std::string msg, int id) {
        std::lock_guard<std::mutex> guard(_mu);
        _f << msg << id << std::endl;
    }
    // now that f is fully protected by the mutex mu (nobody can access f
    // without going through the lock mechanism) f should never be returned to
    // the outside world, like in:
    std::ofstream &getStream() { return _f; }
    // never pass f as argument to user provided function, like in:
    void processf(void fun(std::ofstream &)) { fun(_f); }
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
