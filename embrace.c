/*
Embrace is a program to insert braces into de-braced C source code. De-braced C
source code selectively has curly braces {...} removed in favor of indentation,
similar to Python. This work is inspired by optional braces in Scala 3.
https://docs.scala-lang.org/scala3/reference/other-new-features/indentation.html

Debraced C must only use spaces for indentation, rather than tab characters.

Semicolons at the end of lines may optionally be omitted.

Parentheses around conditions of if statements may be removed, if the conditino
is followed by the ``do'' keyword. So 
    if (x < 5) ... 
becomes 
    if x < 5 do ...,

    while (x < 5) ... 
becomes 
    while x < 5 do ..., 

and 
    for (int i = 0; i < 5; i++) ... 
becomes 
    for int i = 0; i < 5; i++ do. 

The ``do'' keyword is used here, because it already is a keyword in C and does
not clash with variable names.

The end of indented blocks may optionally be marked with the end marker
``end.'', optionally followed by a token. If it is followed by a token, then the
corresponding opening line must contain this token. Here is an example:

if x < 5 do
    printf("x is less than 5\n");
end. if

@author: Michael Rohs
@date: November 28, 2021
*/

#include "util.h"
#include "embrace.h"

/*
Returns the number of spaces at the beginning of s. Returns -1 if a tab
character has been found at the beginning of s, because tab is invalid for
indentation.
*/
int indentation(String s) {
    for (int i = 0; i < s.len; i++) {
        char c = s.s[i];
        // error, no tabs allowed for indentation
        if (c == '\t') return -1;
        // first non-space character?
        if (c != ' ') return i;
        // assert(c == ' ')
    }
    return s.len;
}

void indentation_test(void) {
    test_equal_i(indentation(make_string("")), 0);
    test_equal_i(indentation(make_string(" ")), 1);
    test_equal_i(indentation(make_string("x")), 0);
    test_equal_i(indentation(make_string("\t")), -1); // error
    test_equal_i(indentation(make_string("\tx")), -1); // error
    test_equal_i(indentation(make_string("\t x")), -1); // error
    test_equal_i(indentation(make_string(" \tx")), -1); // error
    test_equal_i(indentation(make_string(" x")), 1);
    test_equal_i(indentation(make_string("x \t")), 0);
    test_equal_i(indentation(make_string(" x \t")), 1);
    test_equal_i(indentation(make_string("  x \t")), 2);
    test_equal_i(indentation(make_string("    x \t")), 4);
    test_equal_i(indentation(make_string("  \t")), -1); // error
    test_equal_i(indentation(make_string("  ")), 2);
    test_equal_i(indentation(make_string("    ")), 4);
    test_equal_i(indentation(make_string("    hello \t ")), 4);
}

/*
Line continuations in strings and in line comments are possible in C, but we
disallow them in debraced C.

    printcln('\
y');

    prints\
ln("abc\
    def");

// abc\
def
*/
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
};

/*
Computes the next state given the current state and two subsequent characters
from the input.
*/
int next_state(int state, char c, char d) {
    require("valid state", 0 <= state && state < 8);
    int input = 0;
    switch (c) {
        case '"': input = 0; break;
        case '\'': input = 1; break;
        case '\\': 
            switch (d) {
                case '\n': case '\0': input = 6; break; // line continuation
                default: input = 2; break;
            }
            break;
        case '/': 
            switch (d) {
                case '/': input = 3; break;
                case '*': input = 4; break;
                default: input = 7; break;
            }
            break;
        case '*': if (d == '/') input = 5; else input = 7; break;
        default: input = 7; break;
    }
    ensure("valid input", 0 <= input && input < 8);
    state = states[state][input];
    ensure("valid state", 0 <= state && state < 8);
    return state;
}

void next_state_test(void) {
    test_equal_i(next_state(0, '"', 'y'), 1);
    test_equal_i(next_state(0, '\'', 'y'), 6);
    test_equal_i(next_state(0, '\\', 'y'), 0);
    test_equal_i(next_state(0, '/', '/'), 3);
    test_equal_i(next_state(0, '/', '*'), 4);
    test_equal_i(next_state(0, '*', '/'), 0);
    test_equal_i(next_state(0, 'x', 'y'), 0);
    test_equal_i(next_state(1, '"', 'y'), 0);
    test_equal_i(next_state(1, '\'', 'y'), 1);
    test_equal_i(next_state(6, '\'', 'y'), 0);
    test_equal_i(next_state(1, '\\', 'y'), 2);
    test_equal_i(next_state(1, '/', '/'), 1);
    test_equal_i(next_state(1, '/', '*'), 1);
    test_equal_i(next_state(1, '*', '/'), 1);
    test_equal_i(next_state(1, 'x', 'y'), 1);
    // if line ends with backslash (line continuation)
    test_equal_i(next_state(0, '\\', '\0'), 5);
}

/*
Checks if the given character may appear in a C identifier.
*/
bool is_identifier_char(char c) {
    return isalnum(c) || c == '_';
}

/*
Checks whether token appears at position i in line and is surrounded by a
boundary (a non-identifier char or the beginning or end of the string).
*/
bool matches_token(String line, int i, String token) {
    if (i < 0) return false;
    bool boundary_before = (i == 0 || 
            !is_identifier_char(line.s[i-1]));
    if (!boundary_before) return false;
    if (i + token.len > line.len) return false; 
    bool boundary_after = (i + token.len == line.len || 
            !is_identifier_char(line.s[i + token.len]));
    if (!boundary_after) return false;
    return strncmp(line.s + i, token.s, token.len) == 0;
}

const String token_if = {"if", 2};
const String token_for = {"for", 3 };
const String token_while = {"while", 5};
const String token_do = {"do", 2};
const String token_end = {"end.", 4};

/*
Counts each opening brace as +1 and each closing brace as -1.
*/
void parse_line(/*in*/String line, /*inout*/LineInfo* li) {
    require_not_null(li);
    li->line = line;
    // reset state if previos line ended in a line continuation
    if (li->state == 5) li->state = 0;
    assert("valid state", li->state == 0 || li->state == 4);
    li->indent = indentation(li->line);
    li->preprocessor_line = (li->state == 0 && line.s[li->indent] == '#');
    li->end_marker = matches_token(line, li->indent, token_end);
    li->line_comment_index = line.len;
    // replace "if ... do" with "if (...)", same for "for" and "while"
    int itoken = -1;
    for (int i = li->indent; i < line.len; i++) {
        char c = line.s[i];
        char d = line.s[i + 1];
        li->state = next_state(li->state, c, d);
        if (li->state == 0) {
            if (c == '(' || c == '[' || c == '{') {
                li->braces++;
            } else if (c == ')' || c == ']' || c == '}') {
                li->braces--;
            } else if (c == 'i' && d == 'f' && line.s[i + 2] == ' ') {
                if (matches_token(line, i, token_if)) {
                    itoken = i + 2;
                    i += 2;
                }
            } else if (c == 'f' && d == 'o' && line.s[i + 3] == ' ') {
                if (matches_token(line, i, token_for)) {
                    itoken = i + 3;
                    i += 3;
                }
            } else if (c == 'w' && d == 'h' && line.s[i + 5] == ' ') {
                if (matches_token(line, i, token_while)) {
                    itoken = i + 5;
                    i += 5;
                }
            } else if (c == 'd' && d == 'o' && itoken >= 0) {
                if (matches_token(line, i, token_do)) {
                    line.s[itoken] = '(';
                    line.s[i] = ')';
                    line.s[i + 1] = ' ';
                    itoken = -1;
                }
            }
        } else if (li->state == 3) {
            //printf("[/%d/%s]", i, line.s);
            li->line_comment_index = i;
            // strip line comment
            li->line.len = i;
            line.len = i;
            // reset state for line comment (since we are at end of line)
            li->state = 0;
            break;
        }
    }
}

// Prints n spaces (n >= 0).
void print_spaces(int n) {
    assert("not negative", n >= 0);
    char spaces[n + 1];
    for (int i = 0; i < n; i++) {
        spaces[i] = ' ';
    }
    spaces[n] = '\0';
    printf("%s", spaces);
}

// Pushes a copy of value onto stack.
void push(/*inout*/LineInfo** stack, /*in*/LineInfo* value) {
    require_not_null(stack);
    require_not_null(value);
    LineInfo* new = xcalloc(1, sizeof(LineInfo));
    memcpy(new, value, sizeof(LineInfo));
    new->next = *stack;
    *stack = new;
}

// Removes the top element from stack and returns it.
LineInfo pop(/*inout*/LineInfo** stack) {
    require_not_null(stack);
    require("not empty", *stack != NULL);
    LineInfo result = **stack;
    LineInfo* next = (*stack)->next;
    free(*stack);
    *stack = next;
    return result;
}

// Returns a pointer to the top stack element.
LineInfo* top(/*in*/LineInfo* stack) {
    require("not empty", stack != NULL);
    return stack;
}

// Returns the indentation of the top stack element.
int top_indent(/*in*/LineInfo* stack) {
    require("not empty", stack != NULL);
    return stack->indent;
}

// Checks if the stack is empty.
bool is_empty(/*in*/LineInfo* stack) {
    return stack == NULL;
}

// Prints s with indent leading spaces.
void println_indented(char* s, int indent) {
    print_spaces(indent);
    printf("%s\n", s);
}

#define PRINT_EMPTY_LINES \
    while (empty_lines > 0) { println_indented(";", li.indent); empty_lines--; }

/*
Reintroduces braces {...} based on indentation of the de-braced source code. It
uses the following algorithm:
    make indentation stack
    push 0
    for each line:
      if line is blank or only spaces:
        emit(line)
      elif indentation(line) > top_of_stack:
        emit('{')
        emit(line)
        push indentation(line)
      elif indentation(line) < top_of_stack:
        while not stack_empty && top_of_stack != indentation(line):
          emit('}')
          pop()
        if stack_empty: error!
          # error if stack is empty, which will happen if no matching 
          # indentation level was found
        emit(line)
      else:
        emit(line)
Additionally the algorithm ensures that line continuations inside brackets
(...), [...], and {...} do not trigger re-bracing. Moreover, string and
character literals and line and block comments are ignored.
*/
/*
int states[8][8] = { // rows: states, columns: inputs
    //"  '  \  // /+ +/ \<eos> other
    //0  1  2  3  4  5  6  7
    { 1, 6, 0, 3, 4, 0, 5, 0 }, // 0 start
    { 0, 1, 2, 1, 1, 1, 1, 1 }, // 1 in_string
    { 1, 1, 1, 1, 1, 1, 1, 1 }, // 2 string_escape
    { 3, 3, 3, 3, 3, 3, 3, 3 }, // 3 line_comment
    { 4, 4, 4, 4, 4, 0, 4, 4 }, // 4 block_comment
    { 5, 5, 5, 5, 5, 5, 5, 5 }, // 5 line_continuation
    { 6, 0, 7, 6, 6, 6, 6, 6 }, // 6 in_char
    { 6, 6, 6, 6, 6, 6, 6, 6 }, // 7 char_escape
};
typedef struct {
    int length;
    int indent;
    int state;
    int line_comment_index;
    int braces;
    bool preprocessor_line;
} LineInfo;
*/

// add semicolon to prev line if this line is on same or lower indentation level
//     and if prev line is not a preprocessor line or a comment 
// treat file end as line at level 0
// take care of semicolons after struct and union definitions as well as array literals
void print_semicolon(LineInfo* li) {
    if (li->state == 0 && !li->preprocessor_line && 
        li->line.len > 0 && li->line.s[li->line.len - 1] != ';') 
    {
        printf(";");
    }
}

void embrace(StringArray* source_code) {
    require_not_null(source_code);
    LineInfo* indent_stack = NULL;
    // The number of empty (or all-whitespace) lines. These are emitted once the
    // next indentation level is clear.
    int empty_lines = 0;
    LineInfo li = {{NULL, 0}, 0, 0, 0, 0, false, false};
    LineInfo prev_li = li;
    // push(indent_stack, &line_info);
    int current_indent = 0;
    bool is_first_time = true;
    for (int i = 1; i <= source_code->len; i++) {
        String line = source_code->a[i - 1];
        prev_li = li;
        parse_line(line, &li);
        line = li.line;
        // printf("%d(i=%d, b=%d, s=%d): %s\n", i, line_indent, braces, state, line);
        if (li.indent < 0) {
            printf("\nLine %d: Tab used for indentation. De-braced C-Code "
                   "must only use spaces for indentation.\n", i);
            exit(1);
        }
        if (li.braces < 0) {
            printf("\nLine %d: More closing braces than opening braces.\n", i);
            exit(1);
        }
        if (li.state == 1 || li.state == 2 || li.state == 6 || li.state == 7) {
            printf("\nLine %d: Unterminated string or character literal.\n", i);
            exit(1);
        }
        if (li.end_marker && li.indent >= current_indent) {
            printf("\nLine %d: Wrong indentation of end marker.\n", i);
            exit(1);
        }
        if (li.line.len == 0 || li.line.len == li.indent) {
            empty_lines++;
            // preserve previous line as this is an empty line
            li = prev_li;
        } else if (prev_li.braces > 0 || prev_li.state != 0 || prev_li.preprocessor_line) {
            printf("\n");
            PRINT_EMPTY_LINES
            print_string(line);
        } else if (li.indent > current_indent) {
            printf(" {\n");
            PRINT_EMPTY_LINES
            print_string(line);
            push(&indent_stack, &prev_li);
            //printf("(pushed: %d, %s)", prev_li.indent, prev_li.line);
            current_indent = li.indent;
        } else if (li.indent < current_indent) {
            print_semicolon(&prev_li);
            printf(" ");
            while (!is_empty(indent_stack) && top_indent(indent_stack) != li.indent) {
                printf("}");
                pop(&indent_stack);
            }
            if (is_empty(indent_stack)) {
                fprintf(stderr, "\nLine %d: No matching indentation level found.\n", i);
                exit(1);
            }
            assert("matching indentation level found", top_indent(indent_stack) == li.indent);
            LineInfo match = pop(&indent_stack);
            // printf("[match: %.*s]", match.line.len, match.line.s);
            if (li.end_marker) {
                printf("\n"); 
                PRINT_EMPTY_LINES
                print_spaces(li.indent); printf("}");
                int offset = li.indent + token_end.len;
                String marker = make_string2(line.s + offset, line.len - offset);
                marker = trim(marker);
                // printf("[marker: %.*s]", marker.len, marker.s);
                if (!contains(match.line, marker)) {
                    fprintf(stderr, "\nLine %d: End marker '%.*s' does not match.\n", i, marker.len, marker.s);
                    exit(1);
                }
            } else {
                printf("}\n");
                PRINT_EMPTY_LINES
                print_string(line);
            }
            current_indent = li.indent;
        } else {
            print_semicolon(&prev_li);
            if (is_first_time) {
                is_first_time = false;
            } else {
                printf("\n");
            }
            PRINT_EMPTY_LINES
            print_string(line);
        }
    }

    // at end of file need to close any open blocks
    print_semicolon(&prev_li);
    printf(" ");
    while (!is_empty(indent_stack)) {
        pop(&indent_stack);
        printf("}");
    }
    printf("\n");

    //l_free(indent_stack);
}

int main(int argc, char* argv[]) {
    // split_test();
    // indentation_test();
    // next_state_test();
    // trim_test();
    // index_of_test();
    // exit(0);
    if (argc != 2) {
        printf("Usage: embrace <filename de-braced C file>\n");
        exit(1);
    }
    char* filename = argv[1];
    // printf("embracing %s\n", filename);
    String str = read_file(filename);
    StringArray* source_code = split(str.s, '\n');
    /*
    for (int i = 0; i < source_code->len; i++) {
        String line = source_code->a[i];
        printf("%3d: %.*s\n", line.len, line.len, line.s);
    }
    */
    embrace(source_code);
    return 0;
}
