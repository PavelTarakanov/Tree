#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tree.h"
#include "file_using.h"

const int MAX_COMMAND_LEN = 100;
const int FILE_NUBER = 2;
int main(int argc, char* argv[])
{
    node_t* root = NULL;
    char user_command[MAX_COMMAND_LEN] = {0};
    char root_data[] = "Император ведает что";
    char finding_goal[] = "Полторашка";
    char way[MAX_COMMAND_LEN] = "Z";
    FILE* print_address = NULL;

    if (node_init(&root, root_data))
        return ALLOCATION_ERROR;

    akinator(root, user_command);

    if (check_file_founded(argc, 2))
        return FILES_NOT_FOUNDED_ERROR;

    tree_dump(root, argv[1]);

    find_way(root, finding_goal, way);
    printf("way = %s\n", way);

    find_definition(root, finding_goal);

    print_node(root);

    if (check_file_opening(argv[2], &print_address, "w"))
        return FILE_OPENING_ERROR;

    file_print_node(root, print_address);

    if (check_file_closing(print_address))
        return FILE_CLOSING_ERROR;

    node_destroy(root);

    return 0;
}

tree_errors node_init(node_t** node, char* value)
{
    *node = (node_t*) calloc(1, sizeof(node_t));

    (*node)->data = (char*) calloc(MAX_COMMAND_LEN, sizeof(char));

    if ((*node)->data == NULL)
        return ALLOCATION_ERROR;

    strcpy((*node)->data, value);
    (*node)->left = NULL;
    (*node)->right = NULL;

    return NO_ERROR;
}

void print_node(node_t* node)
{
    assert(node);

    printf("(");
    printf(" %s ", node->data);
    if (node->left != NULL) print_node(node->left);
    if (node->right != NULL) print_node(node->right);
    printf(")");
}

void file_print_node(node_t* node, FILE* print_address)
{
    assert(node);
    assert(print_address);

    fprintf(print_address, "(");
    fprintf(print_address, "\"%s\" ", node->data);
    if (node->left != NULL) file_print_node(node->left, print_address);
    if (node->right != NULL) file_print_node(node->right, print_address);
    fprintf(print_address, ")");

    return;
}

tree_errors tree_dump(node_t* root, char* file_name)
{
    assert(root);
    assert(file_name);

    FILE* dump_address = NULL;
    char root_address[10] = "Z";

    if (check_file_opening(file_name, &dump_address, "w"))
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

tree_errors node_dump(node_t* node, FILE* dump_address, char* node_way)
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
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{value = %s | {left = %s | right = %s}}\"];\n",
                node_way, node->data, "NULL", "NULL");
    else if (node->left == NULL)
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{value = %s | {left = %s | right = %p}}\"];\n",
            node_way, node->data, "NULL" , node->right);
    else if (node->right == NULL)
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{value = %s | {left = %p | right = %s}}\"];\n",
            node_way, node->data, node->left, "NULL");
    else
        fprintf(dump_address, "\t%s[color=\"black\", style=\"filled\",fillcolor=\"lightgrey\", shape = record, label=\"{value = %s | {left = %p | right = %p}}\"];\n",
            node_way, node->data, node->left, node->right);

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

    if (node_init(&node->left, user_command))
        return ALLOCATION_ERROR;
    if (node_init(&node->right, node->data))
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

bool check_file_founded(int argc, int number_of_files)
{
    if (argc < number_of_files)
    {
        fprintf(stderr, "Files not founded\n");
        return 1;
    }

    return 0;
}

