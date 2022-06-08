#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

class BankAccount {
    // atomic types are those for which reading and writing are guaranteed to
    // happen in a single instruction, thus data race is not possible on atomic
    // types
    static std::atomic<unsigned int> global_id;
    const unsigned int id;
    int balance;

  public:
    std::mutex balance_mutex;
    BankAccount() = delete;
    explicit BankAccount(int initial_amount)
        : id(global_id++), balance(initial_amount) {}
    unsigned int get_id() const { return id; }
    int get_balance() const { return balance; }
    void set_balance(int amount) { balance = amount; }
};

// class-types (structs, unions as well) are simply blueprints, and does not set
// memory aside (not the same as an instance of a class). so we must define the
// static variable outside the class definition:
std::atomic<unsigned int> BankAccount::global_id(1);

int deposit(BankAccount *from, BankAccount *to, int amount) {
    std::mutex *first;
    std::mutex *second;

    // accounts are the same
    if (from->get_id() == to->get_id()) {
        return -1;
    }

    // ensure proper order for locking (lower id BankAccount is always locked
    // first). mutex cannot be copied, so we utilize pointers and addresses.
    if (from->get_id() < to->get_id()) {
        first = &from->balance_mutex;
        second = &to->balance_mutex;
    } else {
        first = &to->balance_mutex;
        second = &from->balance_mutex;
    }

    std::lock_guard<std::mutex> first_lock(*first);
    std::lock_guard<std::mutex> second_lock(*second);

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
