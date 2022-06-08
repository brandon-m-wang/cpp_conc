#include <iostream>
#include <thread>

// functor add_x is constructed with the x value and creates a x-adder
// its operator takes an integer by reference and mutates it by adding the x value
struct add_x {
    add_x(int x_val) : x(x_val) {}    
    void operator()(int& y) {
        y += x;
    }

private:
    int x;
};

int main() {
    // create the functor instance and our int
    add_x add_42(42);
    int z = 5;

    // t1 should start running add_42 with z's reference as a parameter to the operator
    std::thread t1(add_42, std::ref(z));
    // transfer ownership of t1's process to t2
    std::thread t2 = std::move(t1);
    // wait for t2 to finish
    t2.join();

    std::cout << "z should be 47: " << z << std::endl;
    
    return 0;
}