/*
make account && ./account
*/

#include "util.h"

typedef struct
    char* owner
    int balance
Account

// Opens a new account with an initial balance.
// Owner must not be empty.
// Amount must not be negative.
Account open_account(char* owner, int initial)
    require("not empty", strlen(owner) > 0)
    require("not negative", initial >= 0)
    Account a
    a.owner = owner
    a.balance = initial
    return a

// Deposits amount into account.
// Amount must not be negative.
Account deposit(Account a, int amount)
    require("not negative", amount >= 0)
    ensure_code(int old_balance = a.balance)
    a.balance += amount
    ensure("amount added", a.balance == old_balance + amount)
    return a

// Withdraws amount from account.
// Amount must not be negative.
// Account must have sufficient balance.
Account withdraw(Account a, int amount)
    require("not negative", amount >= 0)
    require("sufficient balance", a.balance >= amount)
    ensure_code(int old_balance = a.balance)
    a.balance -= amount
    ensure("amount subtracted", a.balance == old_balance - amount)
    ensure("balance not negative", a.balance >= 0)
    return a

// Gets the current balance of the account.
int balance(Account a)
    return a.balance

// Gets the owner of the account.
char* owner(Account a)
    return a.owner

union IntOrFLoat
    int i
    float f

union IntOrFloat { int i; float f; }

int main(void)
    Account a = open_account("Ida", 100)
    printf("%d\n", balance(a))
    a = deposit(a, 10)
    printf("%d\n", balance(a))
    a = withdraw(a, 20)
    // a = withdraw(a, 200)
    printf("%d\n", balance(a))
    a = withdraw(a, 80)
    printf("%d\n", balance(a))
    return 0
