#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
#include "file_using.h"

int main(int argc, char* argv[])
{
    node_t* root = NULL;
    int value = 0;

    if (check_file_founded(argc))
        return 1;

    while (scanf("%d", &value) == 1)
        node_insert(&root, value);

    print_node(root);
    printf("\n");

    node_dump(root, argv[1]);

    node_destroy(root);

    return 0;
}

int node_init(node_t** node, int value)
{
    *node = (node_t*) calloc(1, sizeof(node_t));

    (*node)->data = value;
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->number = 1;

    return 0;
}

int node_insert(node_t** node, int value)
{
    if (*node == NULL)
        node_init(node, value);
    else if ((*node)->data > value)
        node_insert(&(*node)->left, value);
    else if ((*node)->data < value)
       node_insert(&(*node)->right, value);
    else
        (*node)->number++;

    return 0;
}

void print_node(node_t* node)
{
    assert(node);

    if (node->left != NULL) print_node(node->left);
    for (int i = 0; i < node->number; i++)
        printf("%d ", node->data);
    if (node->right != NULL) print_node(node->right);
}

int node_dump(node_t* root, char* file_name)
{
    FILE* dump_address = NULL;

    if (check_file_opening(file_name, &dump_address, "w"))
        return 1;

    fprintf(dump_address, "digraph{");

    node_dump(root, dump_address);

    fprintf(dump_address, "}");

    if (check_file_closing(dump_address))
        return 1;

    if (system("dot dump.txt -T png -o dump.png") != 0)
        return 1;

    return 0;
}

void node_dump(node_t* node, FILE* dump_address)
{
    assert(node);
    assert(dump_address);

    fprintf(dump_address, "\t%d[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{value = %d | number = %d | {left = %p | right = %p}}\"];\n",
            node->data, node->data, node->number, node->left, node->right);
    if (node->left != NULL)
    {
        fprintf(dump_address, "\t%d -> %d\n", node->data, node->left->data);
        node_dump(node->left, dump_address);
    }
    if (node->right != NULL)
    {
        fprintf(dump_address, "\t%d -> %d\n", node->data, node->right->data);
        node_dump(node->right, dump_address);
    }
}

void node_destroy(node_t* node)
{
    assert(node);

    if (node->left != NULL)
        node_destroy(node->left);
    if (node->right != NULL)
        node_destroy(node->right);
    free(node);

    return;
}
