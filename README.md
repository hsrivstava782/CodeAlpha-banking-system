# Banking System

Task from the **CodeAlpha C++ Programming Internship** by Himanshu Srivastava.

A console banking application built with object-oriented C++. Classes: `Customer`, `Account`, `Transaction` and `Bank`.

## Features
- Create customers and open multiple accounts per customer
- Deposit, withdraw and transfer funds between accounts
- Transaction history with type, amount, balance after, timestamp and note
- View account information and recent transactions (newest first)
- Validation for amounts, phone numbers, missing accounts and insufficient balance

## How to run
```bash
g++ -std=c++17 main.cpp -o app
./app        # on Windows: app.exe
```

## Tech
C++17, standard library only.
