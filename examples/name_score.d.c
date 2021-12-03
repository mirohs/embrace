/*
Compile: make name_score
Run: ./name_score
make name_score && ./name_score
*/

#include <ctype.h>
#include "util.h"

// todo: write purpose statement
// Returns the score for the given name. Name can only contain letters a-z 
// (in lower case or in upper case). If it contains any other letter, the 
// function returns -1.
int score(char* name)
    // return -1; // todo: implement
    int n = strlen(name)
    int result = 0
    for int i = 0; i < n; i++ do
        char c = name[i]
        c = tolower(c)
        if c < 'a' || c > 'z' do
            return -1
        int delta = 0
        if c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' do
            delta = 1
        else if c == 'q' || c == 'j' || c == 'p' do
            delta = 5
        else if c >= 'v' do
            delta = 3
        result += delta
    return result

void test_score(void)
    test_equal_i(score("saeiou"), 5)
    test_equal_i(score("aeiroua"), 6)
    test_equal_i(score("qhjp"), 15)
    test_equal_i(score("qjcpv"), 18)
    test_equal_i(score("äöü"), -1)
    test_equal_i(score("a b"), -1)

int main(void)
    test_score()
    return 0
