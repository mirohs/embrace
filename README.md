# embrace

## Debraced C code.

*Embrace* is a program to insert braces into debraced C source code. Debraced
C source code selectively has curly braces `{...}` removed in favor of
indentation, similar to Python. This work is inspired by [optional braces in
Scala
3](https://docs.scala-lang.org/scala3/reference/other-new-features/indentation.html).
There is a nice [assessment of optional braces in Scala
3](https://youtu.be/Z0w_pITUTyU?t=2196). The described advantages of debracing include:

- "Programs become shorter, clearer, and more beatiful."
- "Writers stay in the flow, no need to go back and forth inserting braces."
- "Program changes are more reliable than with braces."

Indentation is significant in debraced C. Debraced C must only use spaces for
indentation, rather than tab characters. The number of spaces per level of
indentation is up to the programmer, but the number of spaces for lines on the
same indentation level must match.

*Embrace* re-inserts braces `{...}` in such a way that the line numbers do not
change compared to the de-braced source code. Thus line numbers in compiler
error messages are the same as in the de-braced source code.

The embracing process is transparent to the programmer, because it can be
embedded as a rule in the Makefile.

Semicolons `;` at the end of lines may optionally be omitted.

Parentheses `(...)` around the conditions of `if` statements, `for`statements,
and `while`statements  may be omitted, if the condition is followed by the `do`
keyword. So

```c
if (x < 5) ...
```

may be written as 

```c
    if x < 5 do ...
```

The condition of a while loop

```c
    while (x < 5) ...
```

may be written as

```c
    while x < 5 do ...
```

and a for loop

```c
    for (int i = 0; i < 5; i++) ...
```

becomes

```c
    for int i = 0; i < 5; i++ do
```

The `do` keyword is used in all cases, because it already is a keyword in C and
therefore does not clash with variable names.

The end of indented regions may optionally be marked with an *end marker*. End
markers are advisable to improve the readability of longer indented regions or
when a region is indented by multiple levels. The end marker of debraced C is
`end.`, optionally followed by a token. If it is followed by a token, then the
corresponding opening line must contain this token. Here is an example:

```c
    if x < 5 do
        printf("x is less than 5\n")
        ...
        ... many lines
        ...
    end. if
```

The end marker `end.` (with a trailing `.`) makes name collisions unlikely. One
may still use `end` as a variable name, even if it is a struct.



## Examples

Here are a few examples. As a convention, debraced C code has the file extension
`.d.c`.

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
        if (i >= '0' && i <= '9') {
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

```
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

```
#include <stdio.h>

int main(void)
    int i = 0
    do
        printf("%d\n", i)
        i++
    while (i < 5)
    return 0
```



## Compilation

To compile *embrace* perform the following steps:

```
cd embrace
make embrace
```

To compile an example perform these steps:

```
cd examples
make dowhile
./dowhile
```

The `Makefile` in the `examples`directory transparently invokes *embrace* to create a temporary C file, which is then used to compile the program. If you want to see the *rebraced* C source code, use `make dowhile.c`. The resulting `dowhile.c` is:

```c
#include <stdio.h>
;
int main(void) {
    int i = 0;
    do {
        printf("%d\n", i);
        i++; }
    while (i < 5);
    return 0; }
```

The unusual placement of braces ensures that line numbers do not change, which
is important for compiler error messages.
