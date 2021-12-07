#include "util.h"

int states[8][8] = { // rows: states, columns: inputs
    //"  '  \  // /* */ \<eos> other
    //0  1  2  3  4  5  6  7
    { 1, 6, 0, 3, 4, 0, 5, 0 }, // 0 start
    { 0, 1, 2, 1, 1, 1, 1, 1 }, // 1 in_string
    { 1, 1, 1, 1, 1, 1, 1, 1 }, // 2 string_escape
    { 3, 3, 3, 3, 3, 3, 3, 3 }, // 3 line_comment
    { 4, 4, 4, 4, 4, 0, 4, 4 }, // 4 block_comment
    { 5, 5, 5, 5, 5, 5, 5, 5 }, // 5 line_continuation
    { 6, 0, 7, 6, 6, 6, 6, 6 }, // 6 in_char
    { 6, 6, 6, 6, 6, 6, 6, 6 }, // 7 char_escape
}

/*
Computes the next state given the current state and two subsequent characters
from the input.
*/
int next_state(int state, char c, char d)
    require("valid state", 0 <= state && state < 8)
    int input = 0;
    switch c do 
        case '"': input = 0; break
        case '\'': input = 1; break
        case '\\': 
            switch (d)
                case '\n': case '\0': input = 6; break // line continuation
                default: input = 2; break
            break
        case '/': 
            switch (d)
                case '/': input = 3; break
                case '*': input = 4; break
                default: input = 7; break
            break;
        case '*': if (d == '/') input = 5; else input = 7; break
        default: input = 7; break
    end. switch
    ensure("valid input", 0 <= input && input < 8)
    state = states[state][input]
    ensure("valid state", 0 <= state && state < 8)
    return state

void next_state_test(void) 
    test_equal_i(next_state(0, '"', 'y'), 1)
    test_equal_i(next_state(0, '\'', 'y'), 6)
    test_equal_i(next_state(0, '\\', 'y'), 0)
    test_equal_i(next_state(0, '/', '/'), 3)
    test_equal_i(next_state(0, '/', '*'), 4)
    test_equal_i(next_state(0, '*', '/'), 0)
    test_equal_i(next_state(0, 'x', 'y'), 0)
    test_equal_i(next_state(1, '"', 'y'), 0)
    test_equal_i(next_state(1, '\'', 'y'), 1)
    test_equal_i(next_state(6, '\'', 'y'), 0)
    test_equal_i(next_state(1, '\\', 'y'), 2)
    test_equal_i(next_state(1, '/', '/'), 1)
    test_equal_i(next_state(1, '/', '*'), 1)
    test_equal_i(next_state(1, '*', '/'), 1)
    test_equal_i(next_state(1, 'x', 'y'), 1)
    // if line ends with backslash (line continuation)
    test_equal_i(next_state(0, '\\', '\0'), 5)

int main(void)
    next_state_test()
    return 0
