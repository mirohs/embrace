/*
Compile: make line_counting
Run: ./line_counting
make line_counting && ./line_counting
*/

#include <stdio.h>

#ifdef HELLO
    #define HELLO
    #define FOO

    #define BAR
#endif

int main(void)
    int c;\
        int lines = 0
    printf("x")
    printf("abcdef\n")
    while (c = getchar()) != EOF do
        if c == '\n' do
            lines++
        end. if 
    end. while
    printf("%d lines\n", lines)
    return 0
end. main
