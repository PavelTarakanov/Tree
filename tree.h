#ifndef TREE_H
#define TREE_H

typedef char* tree_elem_t;

struct node_t{tree_elem_t data;
              node_t* left;
              node_t* right;
              node_t* parent;
};
enum tree_errors{
    NO_ERROR = 0,
    ALLOCATION_ERROR = 1,
    FILE_OPENING_ERROR = 2,
    FILE_CLOSING_ERROR = 3,
    GRAPH_MAKING_ERROR = 4,
    FILES_NOT_FOUNDED_ERROR = 5,
    READING_ERROR = 6,
    FILE_STATISTICS_ERROR = 7,
};

tree_errors node_init(node_t** node, char* const value, node_t* parent);
tree_errors basic_init(node_t** root);
void node_destroy(node_t* node);

tree_errors file_print_tree(int argc, char* argv[], node_t* root);
void file_print_node(node_t* node, FILE* const print_address, int root_hight);
tree_errors node_dump(node_t* node, FILE* const dump_address, char* node_way);
tree_errors tree_dump(node_t* root);

void speak(const char *text);

int akinator(node_t* root, char* user_command);
tree_errors make_new_node(node_t* node, char* user_command);

char* find_way(node_t* node, char* value, char* way);
void find_definition(node_t* node, char* value);

bool check_file_founded(int argc, int number_of_files);

tree_errors read_tree(char* file_name, node_t** root);
node_t* read_node(char** buffer, node_t* parent);

#endif //TREE_H
