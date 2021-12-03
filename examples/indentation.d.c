/*
make indentation && ./indentation
*/

#include "util.h"

/*
Returns the number of spaces at the beginning of s. Returns -1 if a tab
character has been found at the beginning of s, because tab is invalid for
indentation.
*/
int indentation(char* s)
    require_not_null(s)
    int spaces = 0
    char c
    while (c = *s++) != '\0' do
        // error, no tabs allowed for indentation
        if c == '\t' do return -1
        // first non-space character?
        if c != ' ' do return spaces
        // assert(c == ' ')
        spaces++
    return spaces

void indentation_test(void)
    test_equal_i(indentation(""), 0)
    test_equal_i(indentation(" "), 1)
    test_equal_i(indentation("x"), 0)
    test_equal_i(indentation("\t"), -1) // error
    test_equal_i(indentation("  "), 2)
    test_equal_i(indentation("x "), 0)
    test_equal_i(indentation("\t "), -1) // error
    test_equal_i(indentation(" x"), 1)
    test_equal_i(indentation("xy"), 0)
    test_equal_i(indentation("\tx"), -1) // error
    test_equal_i(indentation(" \t"), -1)
    test_equal_i(indentation("  \t"), -1) // error
    test_equal_i(indentation("x\t"), 0)
    test_equal_i(indentation("\t\t"), -1) // error
    test_equal_i(indentation("    \tx"), -1) // error
    test_equal_i(indentation("    \t x"), -1) // error
    test_equal_i(indentation("xy \t"), 0)
    test_equal_i(indentation(" xyz \t"), 1)
    test_equal_i(indentation("  abcd \t"), 2)
    test_equal_i(indentation("    a \t"), 4)
    test_equal_i(indentation("  "), 2)
    test_equal_i(indentation("    "), 4)
    test_equal_i(indentation("    hello \t "), 4)

int main(void)
    indentation_test()
    return 0
