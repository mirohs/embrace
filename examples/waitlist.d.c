#include "util.h"

typedef struct Node
    char* name
    struct Node* next
Node

typedef struct WaitList
    Node* first
    Node* last
WaitList

WaitList* new_waitlist(void)
    WaitList* l = xcalloc(1, sizeof(WaitList))
    return l

Node* new_node(char* name, Node* next)
    Node* n = xcalloc(1, sizeof(Node))
    int len = strlen(name);
    n->name = xmalloc(len + 1);
    memcpy(n->name, name, len + 1);
    n->next = next
    return n

void printlist(WaitList* list)
    for Node* n = list->first; n; n = n->next do
        printf("%s\n", n->name)

void add(WaitList* list, char* name)
    require_not_null(list)
    require_not_null(name)
    require("not empty", strlen(name) > 0)

    Node* new = new_node(name, NULL)
    if list->first == NULL do
        list->first = new
        list->last = new
    else
        list->last->next = new
        list->last = new

int main(void)
    WaitList* w = new_waitlist()
    add(w, "Ada")
    add(w, "Bob")
    add(w, "Carl")
    add(w, "Don ")
    printlist(w)
    return 0
