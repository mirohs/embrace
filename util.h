/*
@author: Michael Rohs
@date: November 28, 2021
*/

#ifndef util_h_INCLUDED
#define util_h_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*
A String points to some part of a C string, i.e., it does not have to end with
'\0', but has an explicit length.
*/
typedef struct String String;
struct String {
    char* s;
    int len;
};

String make_string(char* s);
String make_string2(char* s, int n);

void print_string(String str);
void println_string(String str);

String trim(String str);
void trim_test(void);
bool contains(String str, String part);
int index_of(String str, String part);
void index_of_test(void);

typedef struct StringNode StringNode;
struct StringNode {
    String str;
    StringNode* next;
};

StringNode* new_string_node(String str, StringNode* next);

typedef struct StringArray StringArray;
struct StringArray {
    int len;
    int cap;
    String a[]; // variable-sized array
};

StringArray* new_string_array(int cap);

StringArray* split(char* s, char sep);
void split_test(void);

String read_file(char* name);



// #define NO_REQUIRE
// #define NO_ENSURE
// #define NO_ASSERT

#ifdef NO_REQUIRE
#define require_not_null(argument)
#else
/**
Checks that the given argument is not NULL. If so, does nothing. Otherwise
reports the location of the precondition and stops the program. A precondition
is a special type of assertion that has to be valid at the beginning of a
function.

Example use of a precondition:
@code{.c}
    int myfunction(char* s) {
        require_not_null(s);
        ...
    }
@endcode

Example output of failed preconditions:

    myfile.c, line 12: myfunction's precondition "not null" (s) violated

@param[in] argument pointer a pointer that must not be null
*/
#define require_not_null(argument) \
if (argument == NULL) {\
    fprintf(stderr, "%s, line %d: %s's precondition \"not null\" (" #argument ") violated\n", __FILE__, __LINE__, __func__);\
    exit(EXIT_FAILURE);\
}
#endif

#ifdef NO_REQUIRE
#define require(description, condition)
#else
/**
Checks the given precondition. If the condition is true, does nothing. If the
condition is false, reports the location of the precondition and stops the
program. A precondition is a special type of assertion that has to be valid at
the beginning of a function.

Example use of a precondition:
@code{.c}
    int myfunction(int x) {
        require("not too large", x < 100);
        ...
    }
@endcode

Example output of failed preconditions:

    myfile.c, line 12: myfunction's precondition "not too large" (x < 100) violated

@param[in] description char* a description of the condition that has to be valid
@param[in] condition boolean the condition to check
*/
#define require(description, condition) \
if (!(condition)) {\
    fprintf(stderr, "%s, line %d: %s's precondition \"%s\" (%s) violated\n", __FILE__, __LINE__, __func__, description, #condition);\
    exit(EXIT_FAILURE);\
}
#endif

#ifdef NO_ENSURE
#define ensure(description, condition)
#else
/**
Checks the given postcondition. If the condition is true, does nothing. If the
condition is false, reports the location of the postcondition and stops the
program. A postcondition is a special type of assertion that has to be valid
before returning from a function.

Example use of a postcondition:
@code{.c}
    int myfunction(...) {
        int result = 0;
        ...
        ensure("not negative", result >= 0);
        return result;
    }
@endcode

Example output of failed postconditions:

    myfile.c, line 12: myfunction's postcondition "not negative" (result >= 0) violated
    myfile.c, line 12: myfunction's postcondition "sorted" (forall(int i = 0, i < n-1, i++, a[i] <= a[i+1])) violated

@param[in] description char* a description of the condition that has to be valid
@param[in] condition boolean the condition to check
*/
#define ensure(description, condition) \
if (!(condition)) {\
    fprintf(stderr, "%s, line %d: %s's postcondition \"%s\" (%s) violated\n", __FILE__, __LINE__, __func__, description, #condition);\
    exit(EXIT_FAILURE);\
}
#endif

#ifdef NO_ASSERT
#define assert(description, condition)
#else
/**
Checks the given condition. If the condition is true, does nothing. If the
condition is false, reports the file and line of the assertion and stops the
program. Assertions are conditions that have to be valid at a particular point.
If an assertion is false this indicates a bug in the program.

Example use of an assertion:
@code{.c}
    ...
    assert("not too large", x < 3);
    ...
@endcode

Example output of failed assertions:

    myfile.c, line 12: assertion "not too large" (x < 3) violated
    myfile.c, line 12: assertion "sorted" (forall(int i = 0, i < n-1, i++, a[i] <= a[i+1])) violated

@param[in] description char* a description of the condition that has to be valid
@param[in] condition boolean the condition to check
*/
#define assert(description, condition) \
if (!(condition)) {\
    fprintf(stderr, "%s, line %d: assertion \"%s\" (%s) violated\n", __FILE__, __LINE__, description, #condition);\
    exit(EXIT_FAILURE);\
}
#endif

#ifdef NO_ASSERT
#define assert_not_null(pointer)
#else
/**
Checks that the given pointer is not NULL. If so, does nothing. Otherwise
reports the location of the assertion and stops the program. Assertions are
conditions that have to be valid at a particular point. If an assertion is false
this indicates a bug in the program.

Example use of an assertion:
@code{.c}
    ...
    assert_not_null(s);
    ...
@endcode

Example output of failed assertion:

    myfile.c, line 12: assertion "not null" (s) violated

@param[in] pointer a pointer that must not be null
*/
#define assert_not_null(pointer) \
if (pointer == NULL) {\
    fprintf(stderr, "%s, line %d: assertion \"not null\" (" #pointer ") violated\n", __FILE__, __LINE__);\
    exit(EXIT_FAILURE);\
}
#endif

/**
Allows writing code that is meant for use in a postcondition. The code is removed if NO_ENSURE is defined.

Example:
@code{.c}
    int inc(int x) {
        ensure_code(int old_x = x); // save old value for postcondition
        x = x + 1;
        ensure("incremented", x == old_x + 1); // check whether new value is as expected
        return x;
    }
@endcode
*/
#ifdef NO_ENSURE
#define ensure_code(x)
#else
#define ensure_code(x) x
#endif

#ifdef NO_ENSURE
#define ensure_not_null(pointer)
#else
/**
Checks that the given pointer is not NULL. If so, does nothing. Otherwise reports the location of the postcondition and stops the program. A postcondition is a special type of assertion that has to be valid before returning from a function.

Example use of a postcondition:
@code{.c}
    ...
    ensure_not_null(s);
    ...
@endcode

Example output of failed postcondition:

    myfile.c, line 12: myfunction's postcondition "not null" (s) violated

@param[in] pointer a pointer that must not be null
*/
#define ensure_not_null(pointer) \
if (pointer == NULL) {\
    fprintf(stderr, "%s, line %d: %s's postcondition \"not null\" (" #pointer ") violated\n", __FILE__, __LINE__, __func__);\
    exit(EXIT_FAILURE);\
}
#endif



#define panic(message) {\
    fprintf(stderr, "%s:%d, %s: %s\n", __FILE__, __LINE__, __func__, message);\
    exit(EXIT_FAILURE);\
}

#define panicf(format, message) {\
    fprintf(stderr, "%s:%d, %s: " #format "\n", __FILE__, __LINE__, __func__, message);\
    exit(EXIT_FAILURE);\
}

#define panic_if(condition, message) \
if (condition) {\
    fprintf(stderr, "%s:%d, %s: %s\n", __FILE__, __LINE__, __func__, message);\
    exit(EXIT_FAILURE);\
}

#define panicf_if(condition, format, message) \
if (condition) {\
    fprintf(stderr, "%s:%d, %s: " #format "\n", __FILE__, __LINE__, __func__, message);\
    exit(EXIT_FAILURE);\
}



#define xcalloc(count, size) ({\
   void* result = calloc(count, size);\
    if (result == NULL) {\
        panic("Cannot allocate memory.");\
    }\
   result;\
})

#define xmalloc(size) ({\
   void* result = malloc(size);\
    if (result == NULL) {\
        panic("Cannot allocate memory.");\
    }\
   result;\
})



/** 
Checks whether the actual int (first argument) is equal to the expected int
(second argument).
*/
#define test_equal_i(a, e) base_test_equal_i(__FILE__, __LINE__, a, e)

/** 
Checks whether the actual value @c a is equal too the expected value @c e.
*/
bool base_test_equal_i(const char *file, int line, int a, int e);

/** 
Checks whether the actual String (first argument) is equal to the expected
char* (second argument).
*/
#define test_equal_s(a, e) base_test_equal_s(__FILE__, __LINE__, a, e)

/** Checks whether the actual value @c a is equal to the expected value @c e. */
bool base_test_equal_s(const char *file, int line, String a, char* e);

#endif // util_h_INCLUDED
