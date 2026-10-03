// CodeAlpha C++ Internship - Task 4: Banking System
// Author: Himanshu Srivastava
// Classes: Transaction, Account, Customer, Bank (OOP design)

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

// ---------------- Transaction ----------------
class Transaction {
public:
    int id;
    string type;  // Deposit, Withdrawal, Transfer In, Transfer Out
    double amount;
    double balanceAfter;
    string timestamp;
    string note;

    Transaction(int id, string type, double amount, double balanceAfter, string note = "")
        : id(id), type(move(type)), amount(amount), balanceAfter(balanceAfter), note(move(note)) {
        time_t now = time(nullptr);
        char buf[32];
        strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", localtime(&now));
        timestamp = buf;
    }

    void print() const {
        cout << left << setw(6) << id << setw(15) << type << right << fixed << setprecision(2)
             << setw(12) << amount << setw(14) << balanceAfter << "  " << left << setw(21)
             << timestamp << note << "\n";
    }
};

// ---------------- Account ----------------
class Account {
private:
    int accountNumber;
    double balance;
    vector<Transaction> history;
    int nextTxnId = 1;

public:
    Account(int number, double openingBalance) : accountNumber(number), balance(openingBalance) {
        if (openingBalance > 0)
            history.emplace_back(nextTxnId++, "Deposit", openingBalance, balance, "Opening balance");
    }

    int getNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    const vector<Transaction>& getHistory() const { return history; }

    bool deposit(double amount, const string& note = "") {
        if (amount <= 0) return false;
        balance += amount;
        history.emplace_back(nextTxnId++, "Deposit", amount, balance, note);
        return true;
    }

    bool withdraw(double amount, const string& note = "") {
        if (amount <= 0 || amount > balance) return false;
        balance -= amount;
        history.emplace_back(nextTxnId++, "Withdrawal", amount, balance, note);
        return true;
    }

    // Used only by Bank::transfer so the history shows "Transfer In/Out" instead of plain deposit/withdrawal.
    bool transferOut(double amount, int toAccount) {
        if (amount <= 0 || amount > balance) return false;
        balance -= amount;
        history.emplace_back(nextTxnId++, "Transfer Out", amount, balance, "To A/C " + to_string(toAccount));
        return true;
    }

    void transferIn(double amount, int fromAccount) {
        balance += amount;
        history.emplace_back(nextTxnId++, "Transfer In", amount, balance, "From A/C " + to_string(fromAccount));
    }

    void printInfo() const {
        cout << "Account Number : " << accountNumber << "\n";
        cout << "Balance        : Rs. " << fixed << setprecision(2) << balance << "\n";
        cout << "Transactions   : " << history.size() << "\n";
    }

    void printRecent(int count) const {
        if (history.empty()) {
            cout << "No transactions yet.\n";
            return;
        }
        cout << left << setw(6) << "ID" << setw(15) << "Type" << right << setw(12) << "Amount"
             << setw(14) << "Balance" << "  " << left << setw(21) << "Date & Time" << "Note\n";
        cout << string(78, '-') << "\n";
        int start = max(0, (int)history.size() - count);
        for (int i = (int)history.size() - 1; i >= start; i--) history[i].print();  // newest first
    }
};

// ---------------- Customer ----------------
class Customer {
private:
    int id;
    string name;
    string phone;
    vector<Account> accounts;

public:
    Customer(int id, string name, string phone) : id(id), name(move(name)), phone(move(phone)) {}

    int getId() const { return id; }
    const string& getName() const { return name; }
    const string& getPhone() const { return phone; }
    vector<Account>& getAccounts() { return accounts; }
    const vector<Account>& getAccounts() const { return accounts; }

    void addAccount(const Account& acc) { accounts.push_back(acc); }

    Account* findAccount(int number) {
        for (auto& a : accounts)
            if (a.getNumber() == number) return &a;
        return nullptr;
    }

    void print() const {
        cout << "Customer ID : " << id << "\n";
        cout << "Name        : " << name << "\n";
        cout << "Phone       : " << phone << "\n";
        cout << "Accounts    : " << accounts.size() << "\n";
    }
};

// ---------------- Bank ----------------
class Bank {
private:
    vector<Customer> customers;
    int nextCustomerId = 1001;
    int nextAccountNumber = 50010001;

public:
    Customer& createCustomer(const string& name, const string& phone) {
        customers.emplace_back(nextCustomerId++, name, phone);
        return customers.back();
    }

    Customer* findCustomer(int id) {
        for (auto& c : customers)
            if (c.getId() == id) return &c;
        return nullptr;
    }

    // Looks through every customer to find who owns an account number.
    Account* findAccount(int number) {
        for (auto& c : customers)
            if (Account* a = c.findAccount(number)) return a;
        return nullptr;
    }

    Customer* ownerOf(int number) {
        for (auto& c : customers)
            if (c.findAccount(number)) return &c;
        return nullptr;
    }

    int openAccount(Customer& customer, double openingBalance) {
        int number = nextAccountNumber++;
        customer.addAccount(Account(number, openingBalance));
        return number;
    }

    bool transfer(int from, int to, double amount, string& error) {
        if (from == to) { error = "Cannot transfer to the same account."; return false; }
        Account* src = findAccount(from);
        Account* dst = findAccount(to);
        if (!src) { error = "Source account not found."; return false; }
        if (!dst) { error = "Destination account not found."; return false; }
        if (amount <= 0) { error = "Amount must be greater than zero."; return false; }
        if (amount > src->getBalance()) { error = "Insufficient balance."; return false; }
        src->transferOut(amount, to);
        dst->transferIn(amount, from);
        return true;
    }

    const vector<Customer>& getCustomers() const { return customers; }
};

// ---------------- Input helpers ----------------
int readInt(const string& prompt) {
    while (true) {
        cout << prompt;
        int v;
        if (cin >> v) return v;
        cout << "  Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readAmount(const string& prompt) {
    while (true) {
        cout << prompt;
        double v;
        if (cin >> v && v >= 0) return v;
        cout << "  Invalid amount.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    cin >> ws;
    getline(cin, s);
    return s;
}

bool validPhone(const string& p) {
    if (p.size() != 10) return false;
    return all_of(p.begin(), p.end(), ::isdigit);
}

// ---------------- Menu actions ----------------
void actionCreateCustomer(Bank& bank) {
    cout << "\n--- Create Customer ---\n";
    string name = readLine("Full name: ");
    if (name.empty()) { cout << "[Error] Name cannot be empty.\n"; return; }
    string phone = readLine("Phone (10 digits): ");
    if (!validPhone(phone)) { cout << "[Error] Invalid phone number.\n"; return; }

    Customer& c = bank.createCustomer(name, phone);
    cout << "[Success] Customer created. Customer ID: " << c.getId() << "\n";
}

void actionOpenAccount(Bank& bank) {
    cout << "\n--- Open Account ---\n";
    Customer* c = bank.findCustomer(readInt("Customer ID: "));
    if (!c) { cout << "[Error] Customer not found.\n"; return; }
    double opening = readAmount("Opening deposit (0 for none): ");
    int number = bank.openAccount(*c, opening);
    cout << "[Success] Account opened for " << c->getName() << ". Account Number: " << number << "\n";
}

void actionDeposit(Bank& bank) {
    cout << "\n--- Deposit ---\n";
    Account* a = bank.findAccount(readInt("Account number: "));
    if (!a) { cout << "[Error] Account not found.\n"; return; }
    double amt = readAmount("Amount: ");
    if (a->deposit(amt)) cout << "[Success] Deposited. New balance: Rs. " << fixed << setprecision(2) << a->getBalance() << "\n";
    else cout << "[Error] Amount must be greater than zero.\n";
}

void actionWithdraw(Bank& bank) {
    cout << "\n--- Withdraw ---\n";
    Account* a = bank.findAccount(readInt("Account number: "));
    if (!a) { cout << "[Error] Account not found.\n"; return; }
    double amt = readAmount("Amount: ");
    if (a->withdraw(amt)) cout << "[Success] Withdrawn. New balance: Rs. " << fixed << setprecision(2) << a->getBalance() << "\n";
    else cout << "[Error] Invalid amount or insufficient balance.\n";
}

void actionTransfer(Bank& bank) {
    cout << "\n--- Fund Transfer ---\n";
    int from = readInt("From account number: ");
    int to = readInt("To account number: ");
    double amt = readAmount("Amount: ");
    string err;
    if (bank.transfer(from, to, amt, err)) {
        cout << "[Success] Transferred Rs. " << fixed << setprecision(2) << amt << " from " << from << " to " << to << ".\n";
    } else {
        cout << "[Error] " << err << "\n";
    }
}

void actionAccountInfo(Bank& bank) {
    cout << "\n--- Account Information ---\n";
    int number = readInt("Account number: ");
    Account* a = bank.findAccount(number);
    Customer* c = bank.ownerOf(number);
    if (!a || !c) { cout << "[Error] Account not found.\n"; return; }
    cout << "\n";
    c->print();
    cout << "\n";
    a->printInfo();
}

void actionHistory(Bank& bank) {
    cout << "\n--- Recent Transactions ---\n";
    Account* a = bank.findAccount(readInt("Account number: "));
    if (!a) { cout << "[Error] Account not found.\n"; return; }
    int n = readInt("How many recent transactions to show? ");
    cout << "\n";
    a->printRecent(max(1, n));
}

void actionListCustomers(const Bank& bank) {
    cout << "\n--- All Customers ---\n";
    if (bank.getCustomers().empty()) { cout << "No customers yet.\n"; return; }
    for (const auto& c : bank.getCustomers()) {
        cout << "ID " << c.getId() << " | " << left << setw(20) << c.getName() << " | Accounts:";
        for (const auto& a : c.getAccounts()) cout << " " << a.getNumber();
        cout << "\n";
    }
}

int main() {
    Bank bank;
    while (true) {
        cout << "\n==========================================\n";
        cout << "            BANKING SYSTEM (C++)\n";
        cout << "==========================================\n";
        cout << "1. Create customer\n2. Open account\n3. Deposit\n4. Withdraw\n";
        cout << "5. Fund transfer\n6. Account information\n7. Recent transactions\n";
        cout << "8. List customers\n9. Exit\n";

        int choice = readInt("Choice: ");
        switch (choice) {
            case 1: actionCreateCustomer(bank); break;
            case 2: actionOpenAccount(bank); break;
            case 3: actionDeposit(bank); break;
            case 4: actionWithdraw(bank); break;
            case 5: actionTransfer(bank); break;
            case 6: actionAccountInfo(bank); break;
            case 7: actionHistory(bank); break;
            case 8: actionListCustomers(bank); break;
            case 9: cout << "Thank you for banking with us. Goodbye!\n"; return 0;
            default: cout << "[Error] Invalid choice.\n";
        }
    }
}
