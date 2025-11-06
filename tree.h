#ifndef TREE_H
#define TREE_H

typedef int tree_elem_t;

struct node_t{tree_elem_t data;
              node_t* left;
              node_t* right;
              int number;
};

int node_init(node_t** node, int value);
void print_node(node_t* node);
void node_dump(node_t* node, FILE* dump_address);
int node_dump(node_t* root, char* file_name);
int node_insert(node_t** node, int value);
void node_destroy(node_t* node);

#endif //TREE_H
