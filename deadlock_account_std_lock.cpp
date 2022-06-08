#include <iostream>
#include <mutex>
#include <thread>

class BankAccount {
    int balance;

  public:
    std::mutex balance_mutex;
    BankAccount() = delete;
    explicit BankAccount(int initial_amount) : balance(initial_amount) {}
    int get_balance() const { return balance; }
    void set_balance(int amount) { balance = amount; }
};

int deposit(BankAccount *from, BankAccount *to, int amount) {
    // lock both mutexes with deadlock avoidance algo
    std::lock(from->balance_mutex, to->balance_mutex);
    // make sure both already-locked mutexes are unlocked at the end of scope
    // std::adopt_lock assumes the calling thread already has ownership of the
    // mutex
    std::lock_guard<std::mutex> from_lock(from->balance_mutex, std::adopt_lock);
    std::lock_guard<std::mutex> to_lock(to->balance_mutex, std::adopt_lock);

    // insufficient funds
    if (from->get_balance() < amount) {
        return -1;
    }

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

    std::thread t1(
        [=](BankAccount *ba1, BankAccount *ba2, int amount) {
            for (int i = 0; i < 1000; i++) {
                deposit(ba1, ba2, amount);
            }
        },
        &ba1, &ba2, 1);
    std::thread t2(
        [=](BankAccount *ba1, BankAccount *ba2, int amount) {
            for (int i = 0; i < 1000; i++) {
                deposit(ba1, ba2, amount);
            }
        },
        &ba2, &ba1, 1);

    t1.join();
    t2.join();
    return 0;
}
