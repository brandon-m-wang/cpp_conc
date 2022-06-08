#include <iostream>
#include <mutex>
#include <thread>

// EXAMPLE OF NOT THREAD SAFE CODE DUE TO DATA RACE ON STACK

class stack {
    int *_data;
    int _size;
    std::mutex _mu;

  public:
    stack(int *lst, int size) : _size(size) {
        _data = (int *)malloc(_size * sizeof(int));
        memcpy(_data, lst, _size * sizeof(int));
    }
    void pop() {
        std::lock_guard<std::mutex> guard(_mu);
        size_t new_size = _size - 1;
        // equivalent to
        // int *new_data = new int[new_size];
        // in C++, we must cast the malloc (or just the new keyword)
        int *new_data = (int *)malloc(new_size * sizeof(int));
        memcpy(new_data, _data, new_size * sizeof(int));
        _size = new_size;
        // equivalent to
        // delete[] _data;
        // delete not only calls free, but also calls the destructor function
        free(_data);
        _data = new_data;
    }
    int &top() { return _data[_size - 1]; }
};

void function_1(stack &st) {
    for (int i = 0; i < 5; i++) {
        int x = st.top();
        st.pop();
        std::cout << x << std::endl;
    }
}

int main() {
    int lst[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    stack st(lst, 10);
    std::thread t1(function_1, std::ref(st));
    std::thread t2(function_1, std::ref(st));
    t1.join();
    t2.join();
    return 0;
}
