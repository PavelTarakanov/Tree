#ifndef TREE_H
#define TREE_H

typedef char* tree_elem_t;

struct node_t{tree_elem_t data;
              node_t* left;
              node_t* right;
};
enum tree_errors{
    NO_ERROR = 0,
    ALLOCATION_ERROR = 1,
    FILE_OPENING_ERROR = 2,
    FILE_CLOSING_ERROR = 3,
    GRAPH_MAKING_ERROR = 4,
};

tree_errors node_init(node_t** node, char* value);
void print_node(node_t* node);
tree_errors node_dump(node_t* node, FILE* dump_address, char* node_way);
tree_errors tree_dump(node_t* root, char* file_name);
void node_destroy(node_t* node);
int akinator(node_t* root, char* user_command);
tree_errors make_new_node(node_t* node, char* user_command);
char* find_way(node_t* node, char* value, char* way);
void find_definition(node_t* node, char* value);

bool check_file_founded(int argc, int number_of_files);

#endif //TREE_H
