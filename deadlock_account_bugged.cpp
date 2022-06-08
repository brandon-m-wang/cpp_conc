#include <iostream>
#include <mutex>
#include <thread>

class BankAccount {
    int balance;

  public:
    std::mutex balance_mutex;
    BankAccount() = delete; // deletes default constructor (avoid undefined
                            // behavior with unitialized balance)
    // direct initialization
    explicit BankAccount(int initial_amount) : balance(initial_amount) {}
    // a "const function", denoted with the keyword const after a function
    // declaration, makes it a compiler error for this class function to change
    // a member variable of the class. However, reading of a class variables is
    // okay inside of the function, but writing inside of this function will
    // generate a compiler error.
    int get_balance() const { return balance; }
    void set_balance(int amount) { balance = amount; }
};

int deposit(BankAccount *from, BankAccount *to, int amount) {
    std::lock_guard<std::mutex> from_lock(from->balance_mutex);

    // insufficient funds
    if (from->get_balance() < amount) {
        return -1;
    }
    std::lock_guard<std::mutex> to_lock(to->balance_mutex);

    // update balances
    from->set_balance(from->get_balance() - amount);
    to->set_balance(to->get_balance() + amount);

    // log balances
    std::cout << "From thread " << std::this_thread::get_id()
              << ": from balance = " << from->get_balance() << std::endl;
    std::cout << "From thread " << std::this_thread::get_id()
              << ": to balance = " << to->get_balance() << std::endl;

    return 0;
}

int main() {
    BankAccount ba1(1000);
    BankAccount ba2(1000);

    // lambda function: [=] specifies that the variables in the lambda are
    // captured by value (since we are using pointers here instead of passing by
    // reference as we have done in previous examples). [&] would have specified
    // that the variables in the lambda are captured by reference
    std::thread t1(
        [=](BankAccount *ba1, BankAccount *ba2, int amount) {
            for (int i = 0; i < 1000; i++) {
                deposit(ba1, ba2, amount);
            }
        },
        &ba1, &ba2, 5);
    std::thread t2(deposit, &ba2, &ba1, 1);

    t1.join();
    t2.join();
    return 0;
}
