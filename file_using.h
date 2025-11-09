#ifndef FILE_USING_H
#define FILE_USING_H

bool check_file_founded(int argc, int number_of_files);
bool check_file_opening(const char *input_address, FILE** file_ptr, const char* access_rights);
bool check_file_closing(FILE* input_address);

#endif //FILE_USING_H
