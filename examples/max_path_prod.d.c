/*
make max_path_prod && ./max_path_prod
*/

#include <limits.h>
#include "util.h"

// Represents a binary tree with integer values.
typedef struct Tree
    int value
    struct Tree *left
    struct Tree *right
Tree

// Creates a new binary tree node.
Tree* new_tree(Tree* left, int value, Tree* right)
    Tree* n = xcalloc(1, sizeof(Tree))
    n->left = left
    n->value = value
    n->right = right
    return n
 
// Prints a tree.
void print_tree(Tree* n)
    if n == NULL do
        printf("_")
    else if n->left == NULL && n->right == NULL do
        printf("(")
        printf("%d", n->value)
        printf(")")
    else
        printf("(")
        print_tree(n->left)
        printf(", ")
        printf("%d", n->value)
        printf(", ")
        print_tree(n->right)
        printf(")")

// Prints a tree followed by a line break.
void print_tree_ln(Tree* n)
    print_tree(n)
    printf("\n")

// Checks if t is a leaf (has no successors).
bool is_leaf(Tree* t)
    return t != NULL && t->left == NULL && t->right == NULL

// Returns the maximum product of a path from the root to any leaf.
int max_path_prod_rec(Tree* t, int max_so_far, int path_prod)
    path_prod *= t->value
    if is_leaf(t) do
        if path_prod > max_so_far do
            return path_prod
        return max_so_far
    if t->left do
        max_so_far = max_path_prod_rec(t->left, max_so_far, path_prod)
    if t->right do
        max_so_far = max_path_prod_rec(t->right, max_so_far, path_prod)
    return max_so_far

// Returns the maximum path product for t.
int max_path_prod(Tree* t)
    require("path product only defined for non-empty trees", t != NULL)
    return max_path_prod_rec(t, INT_MIN, 1)

// Convenience function to create a leaf.
Tree* leaf(int value)
    return new_tree(NULL, value, NULL)

// Convenience function to create a node.
Tree* node(Tree* left, int value, Tree* right)
    return new_tree(left, value, right)

void test(void)
    Tree* t

    // one-element tree
    t = leaf(100)
    test_equal_i(max_path_prod(t), 100)
    
    //    2
    // 3     4
    t = node(leaf(3), 2, leaf(4))
    test_equal_i(max_path_prod(t), 2 * 4)

    //    2
    // 3     -4
    t = node(leaf(3), 2, leaf(-4))
    test_equal_i(max_path_prod(t), 2 * 3)

    //   10
    //  2   5
    // 1 -3 4 -6
    t = node(node(leaf(1), 2, leaf(-3)), 10, node(leaf(4), 5, leaf(-6)))
    test_equal_i(max_path_prod(t), 10 * 5 * 4)

    //   10
    //  2   -5
    // 1 -3 4 -6
    t = node(node(leaf(1), 2, leaf(-3)), 10, node(leaf(4), -5, leaf(-6)))
    test_equal_i(max_path_prod(t), 10 * -5 * -6)

    //   10
    //  2   -5
    // 1 -3 4 6
    t = node(node(leaf(1), 2, leaf(-3)), 10, node(leaf(4), -5, leaf(6)))
    test_equal_i(max_path_prod(t), 10 * 2 * 1)

    //          10
    //       2        5
    //    1     3   4   6
    //  10 -210
    t = node(node(node(leaf(10), 1, leaf(-210)), 2, leaf(3)), 10, node(leaf(4), 5, leaf(6)))
    test_equal_i(max_path_prod(t), 10 * 5 * 6)

    //                1
    //          2           3
    //      3      NULL
    // NULL   -4
    t = node(node(node(NULL, 3, leaf(-4)), 2, NULL), 1, leaf(3))
    test_equal_i(max_path_prod(t), 1 *  3)

    //                1
    //          2           3
    //      -3      NULL
    // NULL   -4
    t = node(node(node(NULL, -3, leaf(-4)), 2, NULL), 1, leaf(3))
    test_equal_i(max_path_prod(t), 1 * 2 * -3 * -4)
    print_tree_ln(t)

int main(void)
    test()
    printf("%d\n", INT_MIN)
    return 0
