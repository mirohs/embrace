/*
@author: Michael Rohs
@date: November 28, 2021
*/

#include "util.h"

///////////////////////////////////////////////////////////////////////////////
// Strings

String make_string(char* s) {
    require_not_null(s);
    return (String) {s, strlen(s)};
}

String make_string2(char* s, int n) {
    require_not_null(s);
    require("not negative", n >= 0);
    return (String) {s, n};
}

void print_string(String str) {
    printf("%.*s", str.len, str.s);
}

void println_string(String str) {
    printf("%.*s\n", str.len, str.s);
}

/*
Removes spaces and tabs from the beginning and end of str. The underlying
content is not modified, but the String is appropriately shifted and the length
adapted.
*/
String trim(String str) {
    int left = 0, right = str.len - 1;
    for (; left < str.len && (str.s[left] == ' ' || str.s[left] == '\t' ); left++);
    for (; right >= left && (str.s[right] == ' ' || str.s[right] == '\t' ); right--);
    if (left > right) return (String){"", 0};
    return (String){str.s + left, right - left + 1};
}

void trim_test(void) {
    test_equal_s(trim(make_string("")), "");
    test_equal_s(trim(make_string(" ")), "");
    test_equal_s(trim(make_string("  \t\t \t  ")), "");
    test_equal_s(trim(make_string("a")), "a");
    test_equal_s(trim(make_string("a ")), "a");
    test_equal_s(trim(make_string(" a")), "a");
    test_equal_s(trim(make_string("a \t")), "a");
    test_equal_s(trim(make_string("\t a")), "a");
    test_equal_s(trim(make_string("\t a \t")), "a");
    test_equal_s(trim(make_string("abc")), "abc");
    test_equal_s(trim(make_string("a b c")), "a b c");
    test_equal_s(trim(make_string("   a b c ")), "a b c");
}

/*
Returns true iff str contains part.
*/
bool contains(String str, String part) {
    if (str.len < part.len) return false;
    // must use C strings to use strstr, on stack may overflow the stack
    char s[str.len + 1];
    char t[part.len + 1];
    memcpy(s, str.s, str.len);
    memcpy(t, part.s, part.len);
    s[str.len] = '\0';
    t[part.len] = '\0';
    return strstr(s, t) != NULL;
}

/*
Returns the index of part in str or -1 of part does not appear in s.
*/
int index_of(String str, String part) {
    if (str.len < part.len) return -1;
    // must use C strings to use strstr, on stack may overflow the stack
    char s[str.len + 1];
    char t[part.len + 1];
    memcpy(s, str.s, str.len);
    memcpy(t, part.s, part.len);
    s[str.len] = '\0';
    t[part.len] = '\0';
    char* p = strstr(s, t);
    if (p == NULL) return -1;
    return (int)(p - s);
}

void index_of_test(void) {
    test_equal_i(index_of(make_string("abcd"), make_string("ab")), 0);
    test_equal_i(index_of(make_string("abcd"), make_string("bc")), 1);
    test_equal_i(index_of(make_string("abcd"), make_string("cd")), 2);
    test_equal_i(index_of(make_string("abcd"), make_string("bd")), -1);
    test_equal_i(index_of(make_string("abcd"), make_string("x")), -1);
    test_equal_i(index_of(make_string("abcd"), make_string("")), 0);
    test_equal_i(index_of(make_string(""), make_string("a")), -1);
}

StringNode* new_string_node(String str, StringNode* next) {
    StringNode* node = xcalloc(1, sizeof(StringNode));
    node->str = str;
    node->next = next;
    return node;
}

StringArray* new_string_array(int cap) {
    require("not negative", cap >= 0);
    StringArray* arr = xcalloc(1, sizeof(StringArray) + cap * sizeof(String));
    arr->cap = cap;
    return arr;
}

/**
Reads the contents of a file into a string The function fails if the file does
not exist or cannot be read.
@param[in] name file name (including path)
@return a string that points to a newly allocated char* with data read from file
*/
String read_file(char* name) {
    require_not_null(name);
    
    FILE *f = fopen(name, "rt"); // removes \r from read content, only leaves \n
    panicf_if(f == NULL, "Cannot open %s", name);

    fseek (f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    
    char *s = xmalloc(size + 1);
    long sizeRead = fread(s, 1, size, f);
    // assert: size >= sizeRead (> if file contains \r characters)
    // printf("size = %lu, sizeRead = %lu, feof = %d\n", size, sizeRead, feof(f));
    panicf_if(sizeRead < size && feof(f) == 0, "Cannot read %s to end.\n", name); 
    s[sizeRead] = '\0';
    
    fclose(f);
    return make_string2(s, sizeRead);
}

/*
Splits the string using the given separator character. Does not motify the
content of the argument string.
*/
StringArray* split(char* s, char sep) {
    require_not_null(s);
    char* t = s;
    StringNode* lines = NULL;
    int line_count = 0;
    while (*t) {
        if (*t == sep) {
            lines = new_string_node(make_string2(s, (int)(t - s)), lines);
            s = t + 1;
            line_count++;
        }
        t++;
    }
    // last line
    if (lines != NULL || (lines == NULL && t > s)) {
        lines = new_string_node(make_string2(s, (int)(t - s)), lines);
        line_count++;
    }
    StringArray* arr = new_string_array(line_count);
    arr->len = line_count;
    for (int i = line_count - 1; i >= 0; i--) {
        assert_not_null(lines);
        arr->a[i] = lines->str;
        StringNode* node = lines;
        lines = lines->next;
        free(node);
    }
    return arr;
}

void split_test(void) {
    // empty string => empty array
    StringArray* a = split("", ' ');
    test_equal_i(a->len, 0);
    free(a);

    // separator => two empty strings
    a = split(" ", ' ');
    test_equal_i(a->len, 2);
    test_equal_i(a->a[0].len, 0);
    test_equal_i(a->a[1].len, 0);
    free(a);

    // a single non-empty line without line ending
    a = split("abc", '\n');
    test_equal_i(a->len, 1);
    test_equal_i(a->a[0].len, 3);
    test_equal_s(a->a[0], "abc");
    free(a);

    a = split("ab cde", ' ');
    test_equal_i(a->len, 2);
    test_equal_i(a->a[0].len, 2);
    test_equal_s(a->a[0], "ab");
    test_equal_i(a->a[1].len, 3);
    test_equal_s(a->a[1], "cde");
    free(a);

    a = split("ab cde ", ' ');
    test_equal_i(a->len, 3);
    test_equal_i(a->a[0].len, 2);
    test_equal_s(a->a[0], "ab");
    test_equal_i(a->a[1].len, 3);
    test_equal_s(a->a[1], "cde");
    test_equal_i(a->a[2].len, 0);
    test_equal_s(a->a[2], "");
    free(a);
}

///////////////////////////////////////////////////////////////////////////////
// Testing

static int base_check_count = 0;
static int base_check_success_count = 0;
static bool base_atexit_registered = false;
static int exit_status = EXIT_SUCCESS;

// http://www.gnu.org/software/libc/manual/html_node/Cleanups-on-Exit.html#Cleanups-on-Exit
void base_atexit(void) {
    // if not a successful exit, supress further output
    if (exit_status == EXIT_SUCCESS) {
        // summary information about tests (if any)
        if (base_check_count > 0) {
            int fail_count = base_check_count - base_check_success_count;
            if (fail_count <= 0) {
                if (base_check_count == 1) {
                    fprintf(stderr, "The test passed!\n");
                } else if (base_check_count == 2) {
                    fprintf(stderr, "Both tests passed!\n");
                } else if (base_check_count >= 3) {
                    fprintf(stderr, "All %d tests passed!\n", base_check_count);
                }
            } else {
                if (base_check_count == 1) {
                    fprintf(stderr, "The test failed.\n");
                } else {
                    if (base_check_success_count == 0) {
                        fprintf(stderr, "0 of %d tests passed.\n", base_check_count);
                    } else {
                        fprintf(stderr, "%d of %d tests failed.\n", fail_count, base_check_count);
                    }
                }
            }
        }
    }
}

void base_init(void) {
    if (!base_atexit_registered) {
        atexit(base_atexit);
        base_atexit_registered = true;
    }
}

bool base_test_equal_i(const char *file, int line, int a, int e) {
    base_init();
    base_check_count++;
    if (a == e) {
        printf("%s, line %d: Test passed.\n", file, line);
        base_check_success_count++;
        return true;
    } else {
        printf("%s, line %d: Actual value %d differs from expected value %d.\n", file, line, a, e);
        return false;
    }
}

bool base_test_equal_s(const char *file, int line, String a, char* e) {
    base_init();
    base_check_count++;
    if ((a.s != NULL && e != NULL && strlen(e) == a.len && strncmp(a.s, e, a.len) == 0) || (a.s == NULL && e == NULL)) {
        printf("%s, line %d: Test passed.\n", file, line);
        base_check_success_count++;
        return true;
    } else {
        printf("%s, line %d: Actual value \"%.*s\" differs from expected value \"%s\".\n", file, line, a.len, a.s, e);
        return false;
    }
}

