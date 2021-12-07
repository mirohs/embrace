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


const int DEBUG = false;

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
void parse_line(/*inout*/LineInfo* li) {
    require_not_null(li);
    String* line = li->line;
    // reset state if previos line ended in a line continuation
    if (li->state == 5) li->state = 0;
    assert("valid state", li->state == 0 || li->state == 4);
    li->indent = indentation(*line);
    li->preprocessor_line = (li->state == 0 && line->s[li->indent] == '#');
    li->end_marker = matches_token(*line, li->indent, token_end);
    li->line_comment_index = line->len;
    // replace "if ... do" with "if (...)", same for "for" and "while"
    for (int i = li->indent; i < line->len; i++) {
        char c = line->s[i];
        char d = line->s[i + 1];
        li->state = next_state(li->state, c, d);
        if (li->state == 0) {
            if (c == '(' || c == '[' || c == '{') {
                li->braces++;
            } else if (c == ')' || c == ']' || c == '}') {
                li->braces--;
            } else if (c == 'i' && d == 'f' && line->s[i + 2] == ' ') {
                if (matches_token(*line, i, token_if)) {
                    li->do_open = line->s + i + 2;
                    li->do_open_in_output = false;
                    i += 2;
                }
            } else if (c == 'f' && d == 'o' && line->s[i + 3] == ' ') {
                if (matches_token(*line, i, token_for)) {
                    li->do_open = line->s + i + 3;
                    li->do_open_in_output = false;
                    i += 3;
                }
            } else if (c == 'w' && d == 'h' && line->s[i + 5] == ' ') {
                if (matches_token(*line, i, token_while)) {
                    li->do_open = line->s + i + 5;
                    li->do_open_in_output = false;
                    i += 5;
                }
            } else if (c == 'd' && d == 'o' && li->do_open != NULL) {
                if (matches_token(*line, i, token_do)) {
                    *li->do_open = '(';
                    li->do_open = NULL;
                    li->do_open_in_output = false;
                    line->s[i] = ')';
                    line->s[i + 1] = ' ';
                }
            }
        } else if (li->state == 3) {
            //printf("[/%d/%s]", i, line->s);
            li->line_comment_index = i;
            // strip line comment
            line->len = i;
            // reset state for line comment (since we are at end of line)
            li->state = 0;
            break;
        }
    }
    if (li->state == 0) {
        *line = trim_right(*line);
    }
}

// Append n spaces (n >= 0) to str.
bool append_spaces(String* str, int n) {
    require_not_null(str);
    require("not negative", n >= 0);
    int new_len = str->len + n;
    if (new_len > str->cap) return false;
    memset(str->s + str->len, ' ', n);
    str->len = new_len;
    return true;
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

#define APPEND_EMPTY_LINES \
    while (empty_lines > 0) { \
        append_spaces(&output, li.indent); \
        char* s = (prev_li.braces == 0) ? ";\n" : "\n"; \
        append_cstring(&output, s); \
        empty_lines--; \
    }

#define PATCH_DO_OPEN \
    if (li.state == 5 && li.do_open != NULL && !li.do_open_in_output) { \
        int offset = li.do_open - li.line->s; \
        assert("valid offset", 0 <= offset < li.line->len); \
        li.do_open = output.s + output.len + offset; \
        li.do_open_in_output = true; \
    }

/*
Adds ia semicolon to the previous line if this line is on same or lower
indentation level and if the previous line is not a preprocessor line or a
comment.
Treat file end as line at level 0.
Take care of semicolons after struct and union definitions as well as array
literals.
*/
void append_semicolon(String* str, LineInfo* li) {
    require_not_null(str);
    require_not_null(li);
    if (li->state == 0 && !li->preprocessor_line && li->line != NULL) {
        int n = li->line->len;
        if (n > 0 && li->line->s[n - 1] != ';') {
            append_char(str, ';');
        }
    }
}

void check_errors(LineInfo* li, char* filename, int line_number, int current_indent) {
    if (li->indent < 0) {
        fprintf(stderr, "%s:%d: Tab used for indentation. De-braced C-Code "
               "must only use spaces for indentation.\n", filename, line_number);
        exit(1);
    }
    if (li->braces < 0) {
        fprintf(stderr, "%s:%d: More closing braces than opening braces.\n", 
                filename, line_number);
        exit(1);
    }
    if (li->state == 1 || li->state == 2 || li->state == 6 || li->state == 7) {
        fprintf(stderr, "stderr, %s:%d: Unterminated string or character literal.\n", 
                filename, line_number);
        exit(1);
    }
    if (li->end_marker && li->indent >= current_indent) {
        fprintf(stderr, "%s:%d: Wrong indentation of end marker.\n", 
                filename, line_number);
        exit(1);
    }
}

/*
Reintroduces braces {...} based on indentation of the de-braced source code. It
uses (roughly) the following algorithm:
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
String embrace(char* filename, String source_code) {
    require_not_null(filename);
    StringArray* source_code_lines = split_lines(source_code.s);
    String output = new_string(2 * source_code.len);
    int current_indent = 0;
    LineInfo* indent_stack = NULL;
    LineInfo li = {NULL, 0, 0, 0, 0, false, false, false, NULL, NULL};
    LineInfo prev_li = li;
    int empty_lines = 0;
    for (int line_number = 1; line_number <= source_code_lines->len; line_number++) {
        li.line = &source_code_lines->a[line_number - 1];
        parse_line(&li);
        check_errors(&li, filename, line_number, current_indent);
        if (DEBUG) printf("i=%d, ind=%d, b=%d, s=%d, pp=%d, do=%p: ", line_number, li.indent, li.braces, li.state, li.preprocessor_line, li.do_open);
        if (DEBUG) println_string(*li.line);

        if (li.line->len == 0 || li.line->len == li.indent) {
            if (DEBUG) printf("embrace: empty\n");
            // Count the number of empty (or all-whitespace) lines. These are 
            // emitted once the next indentation level is clear.
            empty_lines++;
            // preserve previous line as this is an empty line
            li = prev_li;
        } else if (prev_li.braces > 0 || prev_li.state != 0 || prev_li.preprocessor_line) {
            if (DEBUG) printf("embrace: prev special\n");
            append_char(&output, '\n');
            APPEND_EMPTY_LINES
            PATCH_DO_OPEN
            append_string(&output, *li.line);
        } else if (li.indent > current_indent) {
            if (DEBUG) printf("embrace: larger indent\n");
            append_cstring(&output, " {\n");
            APPEND_EMPTY_LINES
            PATCH_DO_OPEN
            append_string(&output, *li.line);
            push(&indent_stack, &prev_li);
            //printf("(pushed: %d, %s)", prev_li.indent, prev_li.line);
            current_indent = li.indent;
        } else if (li.indent < current_indent) {
            if (DEBUG) printf("embrace: smaller indent\n");
            append_semicolon(&output, &prev_li);
            append_char(&output, ' ');
            while (!is_empty(indent_stack) && top_indent(indent_stack) != li.indent) {
                pop(&indent_stack);
                append_char(&output, '}');
            }
            if (is_empty(indent_stack)) {
                fprintf(stderr, "%s:%d: No matching indentation level found.\n", filename, line_number);
                exit(1);
            }
            assert("matching indentation level found", top_indent(indent_stack) == li.indent);
            LineInfo match = pop(&indent_stack);
            // printf("[match: %.*s]", match.line.len, match.line.s);
            if (li.end_marker) {
                append_char(&output, '\n');
                APPEND_EMPTY_LINES
                append_spaces(&output, li.indent);
                append_char(&output, '}');
                int offset = li.indent + token_end.len;
                String marker = make_string2(li.line->s + offset, li.line->len - offset);
                marker = trim(marker);
                // printf("[marker: %.*s]", marker.len, marker.s);
                if (!contains(*match.line, marker)) {
                    fprintf(stderr, "%s:%d: End marker '%.*s' does not match.\n", 
                            filename, line_number, marker.len, marker.s);
                    exit(1);
                }
            } else {
                append_cstring(&output, "}\n");
                APPEND_EMPTY_LINES
                PATCH_DO_OPEN
                append_string(&output, *li.line);
            }
            current_indent = li.indent;
        } else {
            if (DEBUG) printf("embrace: else: ");
            append_semicolon(&output, &prev_li);
            if (output.len > 0) {
                append_char(&output, '\n');
            }
            APPEND_EMPTY_LINES
            PATCH_DO_OPEN
            if (DEBUG) println_string(*li.line);
            append_string(&output, *li.line);
            if (DEBUG) printf("li.line->len: %d output->len: %d\n", li.line->len, output.len);
        } // if
        prev_li = li;
    } // for

    // at end of file need to close any open blocks
    append_semicolon(&output, &prev_li);
    append_char(&output, ' ');
    while (!is_empty(indent_stack)) {
        pop(&indent_stack);
        append_char(&output, '}');
    }
    append_char(&output, '\n');
    assert("indent stack empty", indent_stack == NULL);
    free(source_code_lines);
    return output;
}

int main(int argc, char* argv[]) {
    // split_test();
    // split_lines_test();
    // indentation_test();
    // next_state_test();
    // trim_test();
    // trim_left_test();
    // trim_right_test();
    // index_of_test();
    // append_test();
    // exit(0);

    if (argc != 2) {
        printf("Usage: embrace <filename de-braced C file>\n");
        exit(1);
    }
    char* filename = argv[1];
    // printf("embracing %s\n", filename);

    String source_code = read_file(filename);
    String embraced_source_code = embrace(filename, source_code);
    print_string(embraced_source_code);

    free(source_code.s);
    free(embraced_source_code.s);
    return 0;
}
