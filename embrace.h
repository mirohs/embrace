/*
@author: Michael Rohs
@date: November 28, 2021
*/

#ifndef embrace_h_INCLUDED
#define embrace_h_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include "util.h"

typedef struct LineInfo LineInfo;
struct LineInfo {
    String* line;
    int indent;
    int state;
    int line_comment_index;
    int braces;
    bool preprocessor_line;
    bool end_marker;
    bool do_open_in_output;
    char* do_open;
    LineInfo* next;
};

#endif // embrace_h_INCLUDED

