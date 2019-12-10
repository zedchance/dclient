/*
 * Header file for dclient.c
 */

typedef struct file
{
    char name[25];
    int size;
} file;

FILE * connect_to_server();
void menu();
char get_choice();
void list_files(FILE *s);
void view_hash(FILE *s);
void download(FILE *s);
void download_all(FILE *s);
void save_file(FILE *s, char file_name[], int size);
file * get_list(FILE *s, int *file_count);
void convert_size(char *ret, double s);
char * prompt_for_filename(FILE *s);
void quit(FILE *s);