# embrace

## Debraced C code.

**Embrace** is a program to insert braces into de-braced C source code. De-braced
C source code selectively has curly braces `{...}` removed in favor of
indentation, similar to Python. This work is inspired by [optional braces in
Scala
3](https://docs.scala-lang.org/scala3/reference/other-new-features/indentation.html).
There is a nice [assessment of optional braces in Scala
3](https://youtu.be/Z0w_pITUTyU?t=2196).

Debraced C must only use spaces for indentation, rather than tab characters.

Semicolons at the end of lines may optionally be omitted.

Parentheses around conditions of if statements may be omitted, if the condition
is followed by the `do` keyword. So 

```c
if (x < 5) ...
```

becomes 

```c
    if x < 5 do ...
```

The condition of while loops

```c
    while (x < 5) ...
```

becomes 

```c
    while x < 5 do ...
```

and for loops 

```c
    for (int i = 0; i < 5; i++) ...
```

becomes 

```c
    for int i = 0; i < 5; i++ do
```

The `do` keyword is used here, because it already is a keyword in C and does
not clash with variable names.

The end of indented blocks may optionally be marked with the end marker
`end.`, optionally followed by a token. If it is followed by a token, then the
corresponding opening line must contain this token. Here is an example:

```c
    if x < 5 do
        printf("x is less than 5\n");
    end. if
```
## Examples

Here are a few examples.


### C: count_digits.c

```c
#include <stdio.h>

int main(void) {
    int ndigit[10];
    int i = 0;
    while (i < 10) {
        ndigit[i] = 0;
        i++; 
    }
    while ((i = getchar()) != EOF) {
        ifi (i >= '0' && i <= '9') {
            ndigit[i - '0']++; 
        }
    }
    i = 0;
    while (i < 10) {
        printf("%d: %4d times\n", i, ndigit[i]);
        i++; 
    }
    return 0; 
}
```

### Debraced C: count_digits.d.c

```c
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
```

### C: dowhile.c

```c
#include <stdio.h>

int main(void) {
    int i = 0;
    do {
        printf("%d\n", i);
        i++;
    } while (i < 5);
    return 0;
}
```

### Debraced C: dowhile.d.c

```c
#include <stdio.h>

int main(void)
    int i = 0
    do
        printf("%d\n", i)
        i++
    while (i < 5)
    return 0
```


