#include "../uart/uart1.h"
#include "../uart/uart0.h"
#include "printf.h"
#include "custom.h"
#include "mbox.h"


void mbox_buffer_setup(unsigned int buffer_addr, unsigned int tag_identifier,
unsigned int **res_data, unsigned int res_length,
unsigned int req_length, ...) {


    va_list ap;
    va_start(ap, req_length);


    if (tag_identifier == MBOX_TAG_GETCLOCKRATE
         || tag_identifier == MBOX_TAG_SETPHYWH) {
        mBuf[0] = 8 * 4; //Message Buffer Size in bytes
        mBuf[7] = MBOX_TAG_LAST;
    } else if (tag_identifier == MBOX_TAG_GETFIRMWARE 
            || tag_identifier == MBOX_TAG_GETBOARDREVISION 
            || tag_identifier == MBOX_TAG_GETMODEL) {
        mBuf[0] = 7 * 4; //Message Buffer Size in bytes
        mBuf[6] = MBOX_TAG_LAST;
    }

    mBuf[1] = MBOX_REQUEST;

    mBuf[2] = tag_identifier;

    if (res_length > req_length) {// Value buffer size in bytes (max of request and response lengths)
        mBuf[3] = res_length;
    } else {
        mBuf[3] = req_length;
    }

    mBuf[4] = 0; //REQUEST CODE = 0

    if (tag_identifier == MBOX_TAG_GETCLOCKRATE) { // 1 request value, 2 response datas
        for (int i = 5; i < 5 + (res_length / 4 - 1); i++) {
            int x = va_arg(ap, int);

            mBuf[i] = x;
            *(volatile unsigned int **)res_data = &mBuf[i];
        }

        mBuf[6] = 0; // clear output buffer
        *(volatile unsigned int **)(res_data + 1) = &mBuf[6];
    }
    
     else if (tag_identifier == MBOX_TAG_SETPHYWH) {// 2 request values, 2 response datas
        for (int i = 5; i < 5 + (res_length / 4); i++) {
            int x = va_arg(ap, int);

            mBuf[i] = x;
            *(volatile unsigned int **)(res_data + i - 5) = &mBuf[i];
        }
    }
    
     else if (tag_identifier == MBOX_TAG_GETFIRMWARE 
            || tag_identifier == MBOX_TAG_GETBOARDREVISION
            || tag_identifier == MBOX_TAG_GETMODEL) { // no request value, 1 response data
        mBuf[5] = 0; // clear output buffer
        *(volatile unsigned int **)res_data = &mBuf[5];
    }

    va_end(ap);

}



void checkBoardRevision(int mBuf){
    if (mBuf == 0x00a02082) {
        uart_puts(" : rpi-3B BCM2837 1GiB Sony UK");
    } else if (mBuf == 0x00a01041) {
        uart_puts(" : rpi-2B BCM2836 1GiB Sony UK");
    } else if (mBuf == 0x00000010) {
        uart_puts(" : rpi-1B+ BCM2835");
    } else if (mBuf == 0x00900092) {
        uart_puts(" : rpi-Zero BCM2835 512MB Sony UK");
    } else if (mBuf == 0x00b03111) {
        uart_puts(" : rpi-4B BCM2711 2GiB Sony UK");
    } else {
        uart_puts(" : Cannot find the Board Revision");
    }
}



void showinfo() {


    mBuf[0] = 12 * 4; // Message Buffer Size in bytes (12 elements * 4 bytes (32 bit) each)
    mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)

    mBuf[2] = 0x00010002; // TAG Identifier:board revision
    mBuf[3] = 4; // Value buffer size in bytes (max of request and response lengths)
    mBuf[4] = 0; // REQUEST CODE = 0
    mBuf[5] = 0; // clear output buffer 

    mBuf[6] = 0x00010003; // TAG Identifier: MAC address
    mBuf[7] = 6; // Value buffer size in bytes (max of request and response lengths)
    mBuf[8] = 0; // REQUEST CODE = 0
    mBuf[9] = 0; // clear output buffer
    mBuf[10] = 0; // clear output buffer

    mBuf[11] = MBOX_TAG_LAST;


    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP)) {


        uart_puts("\nBoard Revision: ");
        uart_hex(mBuf[5]);
        checkBoardRevision(mBuf[5]);


        uart_puts("\n");
        uart_puts("MAC Address: ");

        uart_MAC(mBuf[9], mBuf[10]);
        uart_puts("\n");

    } else {

        uart_puts("Unable to query!\n");

    }

}




char *auto_completion(char cli_buffer[]) {

    
    const char *commands[] = {
        "help", "setcolor", "clear", "showinfo"
    };

    for (int i = 0; i < 4; i++) {
        if (custom_strncmp(cli_buffer, commands[i], custom_strlen(cli_buffer)) == 1) {

            int length = custom_strlen(cli_buffer);

            while (length > 0) {
				uart_sendc('\b'); // Move cursor back
				uart_sendc(' ');  // Clear character
				uart_sendc('\b'); // Move cursor back again
				length--;
			}

            return (char *)commands[i];
        }
    }

    return NULL;
}




void custom_memset(void* ptr, int value, int num) {
    unsigned char* p = (unsigned char*)ptr;
    for (int i = 0; i < num; i++) {
        *p = (unsigned char)value;
        p++;
    }
}

int custom_strlen(const char* str) {
    int len = 0;
    while (*str != '\0') {
        str++;
        len++;
    }
    return len;
}



void deleteCharacter(int index, char cli_buffer[]) {
    if (index > 0) {
        index--; // Decrement index to move cursor back
        
        
        // Shift the characters in cli_buffer to the left to overwrite the deleted character
        for (int i = index; cli_buffer[i] != '\0'; i++) {
            cli_buffer[i] = cli_buffer[i + 1];
        }
        
        uart_sendc('\b'); // Move cursor back one position
        uart_sendc(' '); // Display a space to erase the character
        uart_sendc('\b'); // Move cursor back again
    }
}

void clearScreen() {

    uart_puts("\033[2J");  // Clear the screen
    uart_puts("\033[H");  // Move cursor to top left corner

}

char* custom_strcpy(char* dest, const char* src) {
    char* original_dest = dest;

    // Copy characters from src to dest until null terminator is reached
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    // Add null terminator to the destination string
    *dest = '\0';

    return original_dest;
}

int custom_strncmp(const char *str1, const char *str2, int n) {
    for (int i = 0; i < n; i++) {
        if (str1[i] != str2[i])
            return 0;
        if (str1[i] == '\0')
            break;
    }
    return 1;
}

char *custom_strstr(const char *haystack, const char *needle) {
    while (*haystack != '\0') {
        const char *h = haystack;
        const char *n = needle;

        while (*n != '\0' && *h == *n) {
            h++;
            n++;
        }

        if (*n == '\0') {
            return (char *)haystack;  // Substring found, return the starting address
        }

        haystack++;
    }

    return NULL;  // Substring not found
}

void changeText(char cli_buffer[]) {

    char *color_t = custom_strstr(cli_buffer, "-t");

    if (color_t != NULL) {
        //uart_puts("\033[0m");
        color_t += 3; // Move past the "-t" part
        if (custom_strncmp(color_t, "black", 5) == 1) {
            // Set text color to yellow
			uart_puts("\033[30m");
        } else if (custom_strncmp(color_t, "yellow", 6) == 1) {
            // Set text color to yellow
			uart_puts("\033[33m");
        } else if (custom_strncmp(color_t, "red", 3) == 1) {
            // Set text color to red
			uart_puts("\033[31m");
        } else if (custom_strncmp(color_t, "blue", 4) == 1) {
            // Set text color to black
			uart_puts("\033[34m");
        } else if (custom_strncmp(color_t, "green", 5) == 1) {
            // Set text color to black
			uart_puts("\033[32m");
        } else if (custom_strncmp(color_t, "purple", 6) == 1) {
            // Set text color to black
			uart_puts("\033[35m");
        } else if (custom_strncmp(color_t, "cyan", 4) == 1) {
            // Set text color to black
			uart_puts("\033[36m");
        } else if (custom_strncmp(color_t, "white", 5) == 1) {
            // Set text color to black
			uart_puts("\033[37m");
        }
    }
}

void changeBackground(char cli_buffer[]) {
    
    char *color_b = custom_strstr(cli_buffer, "-b");
    if (color_b != NULL) {
        //uart_puts("\033[0m");
        color_b += 3; // Move past the "-b" part
        if (custom_strncmp(color_b, "black", 5) == 1) {
            // Set background color to black
            uart_puts("\033[40m");
        } else if (custom_strncmp(color_b, "yellow", 6) == 1) {
            // Set background color to yellow
            uart_puts("\033[43m");
        } else if (custom_strncmp(color_b, "red", 3) == 1) {
            // Set background color to red
            uart_puts("\033[41m");
        } else if (custom_strncmp(color_b, "blue", 4) == 1) {
            // Set background color to blue
            uart_puts("\033[44m");
        } else if (custom_strncmp(color_b, "green", 5) == 1) {
            // Set background color to green
            uart_puts("\033[42m");
        } else if (custom_strncmp(color_b, "purple", 6) == 1) {
            // Set background color to purple
            uart_puts("\033[45m");
        } else if (custom_strncmp(color_b, "cyan", 4) == 1) {
            // Set background color to cyan
            uart_puts("\033[46m");
        } else if (custom_strncmp(color_b, "white", 5) == 1) {
            // Set background color to white
            uart_puts("\033[47m");
        }
    }
}


void help(char cli_buffer[]) {

    const char *commands[] = {
        "help\t\t\t\tShow brief information of all commands",
        "help <command_name>\t\tShow full information of the command",
        "clear\t\t\t\tClear screen",
        "setcolor -t <text color>\tSet text color",
        "setcolor -b <background color\tSet back color",
        "showinfo\t\t\tShow board revision and board MAC address "
    };

    if (custom_strlen(cli_buffer) > 5) { // Check if there's an argument after "help"
        char *command_name = cli_buffer + 5; // Skip "help " to get the command name
        
        // Find and display detailed information for the specified command
        for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
            if (custom_strncmp(command_name, commands[i], custom_strlen(command_name)) == 1) {
                if (custom_strncmp((char *)commands[i], "clear", 5)) {
                    uart_puts("\nClear screen (in our terminal it will scroll down to current position of the cursor).");
                } else if (custom_strncmp((char *)commands[i], "setcolor -t", 11)) {
                    uart_puts("\nSet text color of the console to one of the following colors: BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE");
                } else if (custom_strncmp((char *)commands[i], "setcolor -b", 11)) {
                    uart_puts("\nSet background color of the console to one of the following colors: BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE");
                } else if (custom_strncmp((char *)commands[i], "showinfo", 8)) {
                    uart_puts("\nShow board revision and board MAC address in correct format/ meaningful information");
                }

                uart_puts("\n");
                break;
            }
        }
    } else {
        // Display brief information for all commands
        uart_puts("\nFor more information on a specific command, type help command-name\n");
        for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
            uart_puts((char *)commands[i]);
            uart_puts("\n");
        }
    }
}