#include "mbox.h"

int custom_strncmp(const char *str1, const char *str2, int n);
char *custom_strstr(const char *haystack, const char *needle);
void changeText(char cli_buffer[]);
void changeBackground(char cli_buffer[]);
void clearScreen();
void deleteCharacter(int index, char cli_buffer[]);
char* custom_strcpy(char* dest, const char* src);
int custom_strlen(const char* str);
void custom_memset(void* ptr, int value, int num);
void help(char cli_buffer[]);
void showinfo();
char *auto_completion(char cli_buffer[]);
void mbox_buffer_setup(unsigned int buffer_addr, unsigned int tag_identifier,
unsigned int **res_data, unsigned int res_length, unsigned int req_length, ...);
void checkBoardRevision(int mBuf);