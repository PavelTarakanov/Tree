#include <stdio.h>//TODO verify
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "tree.h"
#include "file_using.h"

const int MAX_COMMAND_LEN = 100;
const int FILE_NUBER = 2;

int main(int argc, char* argv[])
{
    node_t* root = NULL;
    char user_command[MAX_COMMAND_LEN] = {0};
    char finding_goal[] = "Полторашка";
    char way[MAX_COMMAND_LEN] = "Z";

    if (check_file_founded(argc, 2))
        return FILES_NOT_FOUNDED_ERROR;

    if (read_tree(argv[1], &root))
        return READING_ERROR;

    tree_dump(root);

    speak("Привет");

    akinator(root, user_command);

    tree_dump(root);

    find_way(root, finding_goal, way);
    printf("way = %s\n", way);

    find_definition(root, finding_goal);

    file_print_tree(argc, argv, root);

    node_destroy(root);

    return 0;
}

void speak(const char *text)
{
    char command[1024] = {0};

    snprintf(command, sizeof(command), "espeak \"%s\"", text);
    system(command);
}

tree_errors read_tree(char* const file_name, node_t** root)//TODO прверять работу функций
{
    assert(file_name);
    assert(root);

    FILE* input_address = NULL;
    struct stat statistics = {};
    char* buffer = NULL;
    char* buffer_begin = NULL;
    int buffer_len = 0;
    char symbol = 0;
    tree_errors error = NO_ERROR;

    if (check_file_opening(file_name, &input_address, "r"))
        return FILE_OPENING_ERROR;

    if(fstat(fileno(input_address), &statistics))
        return FILE_STATISTICS_ERROR;

    buffer = (char*) calloc(statistics.st_size, sizeof(char));

    if (buffer == NULL)
        return ALLOCATION_ERROR;

    for (int i = 0; i < statistics.st_size; i++)
    {
        symbol = fgetc(input_address);//TODO пробелы внутри кавычек?

        if (isgraph(symbol) || symbol < 0)//для русского
        {
            buffer[buffer_len] = symbol;
            buffer_len++;
        }
    }

    buffer_begin = buffer;


    if (check_file_closing(input_address))
    {
        printf("Error while closing file!");
        error = FILE_CLOSING_ERROR;
    }

    *root = read_node(&buffer, NULL);

    if (*root == NULL)
        error = basic_init(root);

    free(buffer_begin);

    return error;
}

tree_errors basic_init(node_t** root)
{
    char value[MAX_COMMAND_LEN] = "Император ведает что";
    tree_errors error = NO_ERROR;

    error = node_init(root, value, NULL);

    return error;
}

node_t* read_node(char** buffer, node_t* parent)//TODO структура буффера
{
    assert(buffer);

    char str[MAX_COMMAND_LEN] = {0};
    node_t* node = NULL;

    if (**buffer == '(')
    {
        *buffer += 2 * sizeof(char);

        sscanf(*buffer, "%[^\"]s", str);
        node_init(&node, str, parent);

        *buffer += ((int) strlen(str) + 1) * sizeof(char);

        node->left = read_node(buffer, node);
        node->right = read_node(buffer, node);

        if (**buffer == ')')
            *buffer += sizeof(char);
        else
        {
            printf("ERROR: buffer = %c\n", **buffer);
            return NULL;
        }

        return node;
    }
    else if ((**buffer) == 'n' && *(*buffer+1) == 'i' && *(*buffer+2) == 'l')
    {
        *buffer += 3 * sizeof(char);
        return NULL;
    }
    else
    {
        printf("ERROR: buffer = %c\n", **buffer);
        return NULL;
    }
}

tree_errors node_init(node_t** node, char* const value, node_t* parent)
{
    *node = (node_t*) calloc(1, sizeof(node_t));

    (*node)->data = (char*) calloc(MAX_COMMAND_LEN, sizeof(char));

    if ((*node)->data == NULL)
        return ALLOCATION_ERROR;

    strcpy((*node)->data, value);

    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->parent = parent;

    return NO_ERROR;
}

tree_errors file_print_tree(int argc, char* argv[], node_t* root)
{
    assert(argv);
    assert(root);

    FILE* print_address = NULL;
    tree_errors error = NO_ERROR;

    if (argc >= 3)
    {
        if (check_file_opening(argv[2], &print_address, "w"))
            return FILE_OPENING_ERROR;
    }
    else if(argc == 2)
    {
        if (check_file_opening(argv[1], &print_address, "w"))
            return FILE_OPENING_ERROR;
    }

    file_print_node(root, print_address, 0);

    if (check_file_closing(print_address))
    {
        error = FILE_CLOSING_ERROR;
        printf("Error while closing file");
    }

    return error;
}

void file_print_node(node_t* node, FILE* const print_address, int root_hight)//TODO tabular
{
    assert(node);
    assert(print_address);

    for (int i = 0; i < root_hight; i++)
        fprintf(print_address, "\t");

    fprintf(print_address, "( ");
    fprintf(print_address, "\"%s\" ", node->data);

    if (node->left != NULL)
    {
        fprintf(print_address, "\n");
        file_print_node(node->left, print_address, root_hight+1);
    }
    else fprintf(print_address, "nil ");

    if (node->right != NULL)
    {
        fprintf(print_address, "\n");
        file_print_node(node->right, print_address, root_hight+1);
    }
    else fprintf(print_address, "nil ");

    fprintf(print_address, ") ");

    return;
}

tree_errors tree_dump(node_t* root)
{
    assert(root);

    FILE* dump_address = NULL;
    char root_address[10] = "Z";

    if (check_file_opening("dump.txt" , &dump_address, "w+"))
        return FILE_OPENING_ERROR;

    fprintf(dump_address, "digraph{\n");

    node_dump(root, dump_address, root_address);

    fprintf(dump_address, "}");

    if (check_file_closing(dump_address))
        return FILE_CLOSING_ERROR;

    if (system("dot dump.txt -T png -o dump.png") != 0)
        return GRAPH_MAKING_ERROR;

    return NO_ERROR;
}

tree_errors node_dump(node_t* node, FILE* const dump_address, char* node_way)
{
    assert(node);
    assert(dump_address);

    char *left_way = (char*) calloc(strlen(node_way) + 2, sizeof(char));
    char *right_way = (char*) calloc(strlen(node_way) + 2 , sizeof(char));
    tree_errors error = NO_ERROR;

    if (left_way == NULL || right_way == NULL)
        return ALLOCATION_ERROR;

    strcpy(left_way, node_way);
    strcpy(right_way, node_way);

    if (node->left == NULL && node->right == NULL)
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{parent = %p | value = %s | {left = %s | right = %s}}\"];\n",
                node_way, node->parent, node->data, "NULL", "NULL");
    else if (node->left == NULL)
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{parent = %p | value = %s | {left = %s | right = %p}}\"];\n",
            node_way, node->parent, node->data, "NULL" , node->right);
    else if (node->right == NULL)
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{parent = %p | value = %s | {left = %p | right = %s}}\"];\n",
            node_way, node->parent, node->data, node->left, "NULL");
    else
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{parent = %p | value = %s | {left = %p | right = %p}}\"];\n",
            node_way, node->parent, node->data, node->left, node->right);

    if (node->left != NULL)
    {
        fprintf(dump_address, "\t%s -> %s\n", node_way, strcat(left_way, "L"));
        error = node_dump(node->left, dump_address, left_way);

        if (error)
            return error;
    }
    if (node->right != NULL)
    {
        fprintf(dump_address, "\t%s -> %s\n", node_way, strcat(right_way, "R"));
        error = node_dump(node->right, dump_address, right_way);

        if (error)
            return error;
    }

    free(left_way);
    free(right_way);

    return error;
}

void node_destroy(node_t* node)
{
    assert(node);

    if (node->left != NULL)
        node_destroy(node->left);
    if (node->right != NULL)
        node_destroy(node->right);

    free(node->data);
    free(node);

    return;
}

int akinator(node_t* root, char* user_command)
{
    assert(root);
    assert(user_command);

    node_t* node = root;
    printf("ДОБРО ПОЖАЛОВАТЬ В ПРОГРАММУ АКИНАТОР!\n");
    while (strcmp(user_command, "Стоп") != 0)
    {
        while (1)
        {
            printf("Это %s?\n", node->data);

            scanf("%s", user_command);

            if (strcmp("Да", user_command) == 0)
                if (node->left != NULL)
                    node = node->left;
                else
                {
                    printf("Очередная победа машинного интеллекта!\n");
                    break;
                }
            else if (strcmp("Нет", user_command) == 0)
                if (node->right != NULL)
                    node = node->right;
                else
                {
                    make_new_node(node, user_command);
                    break;
                }
            else
                printf("Введите да или нет\n");

        }
        printf("Если хотите прекратить, введите \"Стоп\", иначе работа продолжится!\n");
        scanf("%s", user_command);
        node = root;
    }

    return 0;
}

tree_errors make_new_node(node_t* node, char* user_command)
{
    assert(node);
    assert(user_command);

    printf("Кто это?\n");
    while (scanf(" %[^'\n']s", user_command) != 1)
        printf("Кто это?\n");

    if (node_init(&node->left, user_command, node))
        return ALLOCATION_ERROR;
    if (node_init(&node->right, node->data, node))
        return ALLOCATION_ERROR;

    printf("Чем %s отличается от %s? Введите признак с маленькой буквы\n", user_command, node->data);
    while (scanf(" %[^'\n']s", user_command) != 1)
        printf("Чем %s отличается от %s? Введите признак с маленькой буквы\n", user_command, node->data);

    strcpy(node->data, user_command);

    return NO_ERROR;
}

char* find_way(node_t* node, char* value, char* way)
{
    assert(node);
    assert(value);
    assert(way);
    char* new_way = NULL;

    if (strcmp(node->data, value) == 0)
        return way;

    if (node->left != NULL)
    {
        //printf("way = [%s]\n strlen(way) = %lu\n", way, strlen(way));

        new_way = find_way(node->left, value, strcat(way, "L"));

        //printf("way = [%s]\n strlen(way) = %lu\n", way, strlen(way));

        if (new_way != NULL)
        {
            way = new_way;
            return way;
        }
        way[strlen(way) - 1 ] = '\0';
    }

    if (node->right != NULL)
    {
        //printf("way = [%s]\n strlen(way) = %lu\n", way, strlen(way));

        new_way = find_way(node->right, value, strcat(way, "R"));

        //printf("way = [%s]\n strlen(way) = %lu\n", way, strlen(way));

        if (new_way != NULL)
        {
            way = new_way;
            return way;
        }
        way[strlen(way) - 1] = '\0';
    }

    return NULL;
}

void find_definition(node_t* node, char* value)
{
    assert(node);
    assert(value);

    char way[MAX_COMMAND_LEN] = "Z";

    find_way(node, value, way);

    if (strcmp(way, "Z") == 0)
    {
        printf("Такого тут нет\n");
        return;
    }

    printf("%s - это", value);
    for (size_t i = 0; i < strlen(value) - 1; i++)
    {
        if (way[i+1] == 'L')
        {
            printf(" %s,", node->data);
            node = node->left;
        }
        if (way[i+1] == 'R')
        {
            printf(" не %s,", node->data);
            node = node->right;
        }
    }
    printf("\n");

    return;
}

/*
void find_difference(node_t* node, char* value_1, char* value_2)
{
    assert(node);
    assert(value_1);
    assert(value_2);

    char* way_1[MAX_COMMAND_LEN] = "Z";
    char* way_2[MAX_COMMAND_LEN] = "Z";
    size_t i = 0;
    size_t min_way_len = 0;
    size_t max_way_len = 0;

    find_way(node, value_1, way_1);
    find_way(node, value_2, way_2);

    if (strcmp(way_1, "Z") == 0 || strcmp(way_2, "Z"))
        printf("Такого объекта нет!\n");
        return;

    if (strlen(way_1) > strlen(way_2))
    {
        max_way_len = strlen(way_1);
        min_way_len = strlen(way_2);
    }
    else
    {
        min_way_len = strlen(way_1);
        max_way_len = strlen(way_2);
    }

    printf("И %s, и %s - это ", value_1, value_2
    while(way_1[i] = way_2[i] || i < min_way_len)
    {
        printf(
        i++;
    }
}*/

bool check_file_founded(int argc, int number_of_files)
{
    if (argc < number_of_files)
    {
        fprintf(stderr, "Files not founded. Please, pass two files to the program - the file with the original tree"
                        " and the file to save the new tree. If you want the tree to be saved in the file with"
                        " the original tree, do not pass the second file.\n");
        return 1;
    }
    if (argc == number_of_files)
        fprintf(stderr, "New tree will be saved in the file with original tree.\n");

    return 0;
}
