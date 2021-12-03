#include <stdio.h>

int main(void)
    int ndigit[10]
    int i = 0
    while i < 10 do
        ndigit[i] = 0
        i++
    while (i = getchar()) != EOF do
        if i >= '0' && i <= '9' do // with comment
            ndigit[i - '0']++
    i = 0
    while i < 10 do
        printf("%d: %4d times\n", i, ndigit[i])
        i++
    return 0
end. main
