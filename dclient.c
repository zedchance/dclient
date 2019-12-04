#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsocket/libinetsocket.h>

FILE * connect_to_server();
void menu();
char get_choice();
void list_files(FILE *s);
void download(FILE *s);
void download_all(FILE *s);
void save_file(FILE *s, char file_name[], int size);
void quit(FILE *s);

typedef struct file
{
    char name[25];
    int size;
} file;

int main()
{
    // Connect
    FILE *s = connect_to_server();
    
    while (1)
    {
        // Menu
        menu();
        
        // Get choice
        char choice = get_choice();
        
        // Handle choice
        switch(choice)
        {
            case 'l':
            case 'L':
                list_files(s);
                break;
            
            case 'd':
            case 'D':
                download(s);
                break;
            
            case 'a':
            case 'A':
                download_all(s);
                break;
                
            case 'q':
            case 'Q':
                quit(s);
                exit(0);
                break;
                
            default:
                printf("Choice must be d, l, a, or q\n");
        }
    }
}

/*
 * Connect to server. Returns a FILE pointer that the
 * rest of the program will use to send/receive data.
 */
FILE * connect_to_server()
{
    // Open socket and check
    int socketnum = create_inet_stream_socket("runwire.com", "1234", LIBSOCKET_IPv4, 0);
    if (socketnum == -1)
    {
        fprintf(stderr, "Couldn't connect.\n");
        exit(1);
    }
    
    // Convert socket number to FILE *
    FILE *f = fdopen(socketnum, "r+");
    if (!f)
    {
        fprintf(stderr, "Couldn't convert socket number.\n");
        exit(2);
    }
    
    // Check for greeting
    char response[100];
    fgets(response, 100, f);
    if (strcmp(response, "+OK Greetings\n") != 0)
    {
        fprintf(stderr, "Didn't get the OK\n");
        exit(3);
    }
    
    return f;
}

/*
 * Display menu of choices.
 */
void menu()
{
    printf("-- MENU --\n");
    printf("L) List files\n");
    printf("D) Download a file\n");
    printf("A) Download all files\n");
    printf("Q) Quit\n");
    printf("\n");
}

/*
 * Get the menu choice from the user. Allows the user to
 * enter up to 100 characters, but only the first character
 * is returned.
 */
char get_choice()
{
    printf("Your choice: ");
    char buf[100];
    fgets(buf, 100, stdin);
    return buf[0];
}

/*
 * Display a file list to the user.
 */
void list_files(FILE *s)
{
    // LIST command
    fprintf(s, "LIST\n");
    
    // Get response and check for error
    char response[1000];
    fgets(response, 1000, s);
    if (strcmp(response, "+OK\n") != 0)
    {
        fprintf(stderr, "Something went wrong!\n");
        exit(4);
    }
    
    // Print response
    int file_num = 1;
    printf("Num\tFilename\tSize\n");
    while (fgets(response, 1000, s))
    {
        if (strcmp(response, ".\n") == 0) break;
        char num[20];
        char name[20];
        sscanf(response, "%s %s", num, name);
        printf("%d\t%s\t%s", file_num, name, num);
        printf("\n");
        file_num++;
    }
    printf("\n");
}

/*
 * Download a file.
 * Prompt the user to enter a filename.
 * Download it from the server and save it to a file with the
 * same name.
 */
void download(FILE *s)
{
    // Prompt user
    printf("What file? (num/filename) ");
    char file_name[100];
    fgets(file_name, 100, stdin);
    file_name[strlen(file_name) - 1] = '\0';
    
    // SIZE command
    char size_response[1000];
    fprintf(s, "SIZE %s\n", file_name);
    fgets(size_response, 1000, s);
    int size;
    sscanf(size_response, "+OK %d", &size);
    
    // GET command
    fprintf(s, "GET %s\n", file_name);
    char response[1000];
    fgets(response, 1000, s);
    if (strcmp(response, "+OK\n") != 0)
    {
        fprintf(stderr, "Something went wrong!\n");
        exit(4);
    }
    
    // Save file
    save_file(s, file_name, size);
}

/*
 * Download all files.
 */
void download_all(FILE *s)
{
    // LIST command
    fprintf(s, "LIST\n");
    
    // Get response and check for error
    char list[1000];
    fgets(list, 1000, s);
    if (strcmp(list, "+OK\n") != 0)
    {
        fprintf(stderr, "Something went wrong!\n");
        exit(5);
    }
    
    // Create array of filenames and sizes
    file *files = malloc(20 * sizeof(file));
    int file_count = 0;
    for (int i = 0; ; i++)
    {
        // Break at end of list
        if (strcmp(list, ".\n") == 0) break;
        
        // TODO
        // Check if files array needs to be longer
        
        // Determine filename and size
        fgets(list, 1000, s);
        char file_name[20];
        int size;
        sscanf(list, "%d %s", &size, file_name);
        strcpy(files[i].name, file_name);
        files[i].size = size;
        file_count++;
    }
    
    // Save all files
    char response[1000];
    for (int i = 0; i < file_count - 1; i++)
    {
        // GET command
        fprintf(s, "GET %s\n", files[i].name);
        fgets(response, 1000, s);
        if (strcmp(response, "+OK\n") != 0)
        {
            fprintf(stderr, "Something went wrong!\n");
            exit(6);
        }
        
        save_file(s, files[i].name, files[i].size);
    }
}

/*
 * Used to save file to disk
 * Shows a progress bar
 */
void save_file(FILE *s, char file_name[], int size)
{
    // Start download message
    printf("%s\t\u2592", file_name);
    fflush(stdout);
    
    // Open file for writing and check
    FILE *out = fopen(file_name, "wb");
    if (!out)
    {
        fprintf(stderr, "Can't write to %s\n", file_name);
        exit(1);
    }
    
    // Counters for progress bar
    int tick_size = size / 20;
    int progress = 1;
    
    // Save data to file
    unsigned char data[100];
    int so_far = 0;
    int got;
    while (1)
    {
        if (so_far + 100 < size)
        {
            got = fread(data, sizeof(unsigned char), 100, s);
            fwrite(data, sizeof(unsigned char), got, out);
            so_far += got;
            if (tick_size * progress < so_far)
            {
                printf("\u2588");
                fflush(stdout);
                progress++;
            }
        }
        else
        {
            got = fread(data, sizeof(unsigned char), size - so_far, s);
            fwrite(data, sizeof(unsigned char), got, out);
            if (progress < 19)
            {
                for (int i = 0; i < 20 - progress; i++)
                {
                    printf("\u2588");
                    fflush(stdout);
                }
            }
            break;
        }
    }
    
    // Close file
    fclose(out);
    printf("\u2592 DONE\n");
}

/* 
 * Close the connection to the server.
 */
void quit(FILE *s)
{
    fclose(s);
}