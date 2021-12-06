#include "util.h"

typedef struct LineInfo LineInfo;
struct LineInfo {
    String* line;
    int state;
    bool preprocessor_line;
};

void print_semicolon(LineInfo* li)
    if li->state == 0 && !li->preprocessor_line \
            && li->line->len > 0 \
            && li->line->s[li->line->len - 1] != ';' do
        printf(";")

void print_semicolon2(LineInfo* li)
    if li->state == 0 && !li->preprocessor_line \
            && li->line->len > 0 \
            && 1 < 2 \
            && li->line->s[li->line->len - 1] != ';' do
        printf(";")

int main(void)
    for (int i = 0; i < 5; i++) for (int j = 0; j < 5; j++) printf("(%d, %d)\n", i, j);
    for int i = 0; i < 5; i++ do for int j = 0; j < 5; j++ do printf("(%d, %d)\n", i, j);
    return 0
