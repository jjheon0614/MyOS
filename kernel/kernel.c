#include "../uart/uart1.h"
#include "../uart/uart0.h"
#include "printf.h"
#include "custom.h"
#include "mbox.h"
#define MAX_CMD_SIZE 100
#define HISTORY_SIZE 100
char history[HISTORY_SIZE][MAX_CMD_SIZE];
int historyIndex = 0;
int tempIndex;

void cli()
{
	static char cli_buffer[MAX_CMD_SIZE];
	static int index = 0;

	//read and send back each char
	char c = uart_getc();
	

	//put into a buffer until got new line character
	if (c != '\n'){
		if (c == '_') {
			if (tempIndex != 0) {
				tempIndex--;
				char* command = history[tempIndex];


				while (index > 0) {
					uart_sendc('\b'); // Move cursor back
					uart_sendc(' ');  // Clear character
					uart_sendc('\b'); // Move cursor back again
					index--;
				}
			
				// Display the new command
				uart_puts(command);
				index = custom_strlen(command);


				custom_memset(cli_buffer, 0, custom_strlen(cli_buffer));
				custom_strcpy(cli_buffer, command);
			}
		} else if (c == '+') {
			if (tempIndex < historyIndex) {
				
				tempIndex++;
				char* command = history[tempIndex];


				while (index > 0) {
					uart_sendc('\b'); // Move cursor back
					uart_sendc(' ');  // Clear character
					uart_sendc('\b'); // Move cursor back again
					index--;
				}
			
				// Display the new command
				uart_puts(command);
				index = custom_strlen(command);


				custom_memset(cli_buffer, 0, custom_strlen(cli_buffer));
				custom_strcpy(cli_buffer, command);
			}
		} else if (c == '\b') {
			if (index > 0) {
				deleteCharacter(index, cli_buffer); // Call the deleteCharacter function
				index--;
			}
    	} else if (c == '\t') {

			char* command = auto_completion(cli_buffer);

			custom_memset(cli_buffer, 0, custom_strlen(cli_buffer));
			custom_strcpy(cli_buffer, command);
			uart_puts(cli_buffer);
			index = custom_strlen(cli_buffer);

			
	    }else {
			uart_sendc(c);


			cli_buffer[index] = c; //Store into the buffer
			index++;
		}

	} else if (c == '\n'){
		cli_buffer[index] = '\0';

		custom_strcpy(history[historyIndex], cli_buffer);
		historyIndex++;
		tempIndex = historyIndex;

		if (custom_strncmp(cli_buffer, "setcolor\n", 8) == 1) {
			changeBackground(cli_buffer);
			changeText(cli_buffer);
		} else if (custom_strncmp(cli_buffer, "clear\n", 5) == 1) {
			clearScreen();
		} else if (custom_strncmp(cli_buffer, "help\n", 4) == 1) {
			help(cli_buffer);
		} else if (custom_strncmp(cli_buffer, "showinfo\n", 8) == 1) {
			showinfo();
		}

		uart_puts("\n");

		uart_puts("MyOS> ");


		//Return to command line
		index = 0;
	}
}

void welcomeMessage() {
	uart_puts("\n\n_////////_////////_////////_/// _//////\n"                                            
				"_//      _//      _//           _//      _// _/         _//      _/ _//       _//\n"     
				"_//      _//      _//           _//     _/     _//    _ _//    _//     _//  _//  _//  \n" 
				"_//////  _//////  _//////       _//          _//     _/ _//   _/      _// _//     _// \n" 
				"_//      _//      _//           _//        _//     _//  _//     _/  _//   _//      _//\n" 
				"_//      _//      _//           _//      _//      _//// _/ _//     _//     _//    _// \n" 
				"_////////_////////_////////     _//     _////////       _//      _//         _///     \n" 
				"\n" 
				"_// _//         _/       _///////    _////////     _////       _// //                \n " 
				"_/    _//      _/ //     _//    _//  _//         _//    _//  _//    _//               \n" 
				"_/     _//    _/  _//    _//    _//  _//       _//        _// _//                     \n" 
				"_/// _/      _//   _//   _/ _//      _//////   _//        _//   _//                   \n" 
				"_/     _//  _////// _//  _//  _//    _//       _//        _//      _//                \n" 
				"_/      _/ _//       _// _//    _//  _//         _//     _// _//    _//               \n" 
				"_//// _// _//         _//_//      _//_////////     _////       _// //                 \n" 
	"\n"
	"\tDeveloped by <Jaeheon Jeong> - <s3821004>\n"
	);
}



void main(){
    // set up serial console
	uart_init();


	//Q2

	// printf("\n\ndecimal: %d\n", 5);

	// printf("decimal: %d\n\n", -11);

	// printf("\n\nchracter: %c\n", 'A');

	// printf("chracter: %c\n\n", 'c');


	// printf("\n\nstring: %s\n\n", "Hello World");

	// printf("\n\nhexa: %x\n", 160);

    // printf("hexa: %x\n\n", -177);

	// printf("\n\nfloat: %f\n", 3.12345678);
    // printf("float: %f\n\n", -10.25);

	// printf("\n\n%% is used in string\n\n");
	

	// printf("\n\nPositive decimal: %010d\nNegative decimal: %010d\n\n", 25, -25);
	// printf("Positive float: %010f\nNegative float: %010f\n\n", 10.25, -3.123);
	// printf("Positive hexa: %010x\nNegative hexa: %010x\n\n", 25, -25);

	// printf("\n\ndecimal: %*d\nfloat: %*f\n", 5, -110, 12, 3.25);
	// printf("string: %*s\ncharacter: %*c\n", 8, "Hello", 3, 'c');
	// printf("hexa: %*x\n\n", 5, 16);

	// printf("\n\ndecimal: %.5d\nfloat: %.12f\n", 15, -10.25);
	// printf("string: %.3s\nhexa: %.6x\n\n", "Hello world", 255);

	// printf("\n\n0 flag integer: %05d\nprecision integer: %.5d\n\n", -25, -25);

	// printf("%*.3d", 5, 25);


	//Q3
	// uart_puts("\nQ3. Mailbox setup\n");
	// unsigned int *physize = 0;
	// mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_SETPHYWH, &physize, 8, 8, 1024, 768);
	// mbox_call(ADDR(mBuf), MBOX_CH_PROP);
	// uart_puts("\nGot Actual Physical Width: ");
	// uart_dec(physize[0]);
	// uart_puts("\nGot Actual Physical Height: ");
	// uart_dec(physize[1]);

	// unsigned int *clock_rate = 0;
	// mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETCLOCKRATE, &clock_rate, 8, 4, 3);
	// mbox_call(ADDR(mBuf), MBOX_CH_PROP);
	// uart_puts("\nGot ARM clock rate: ");
	// uart_dec(clock_rate[1]);



	//Q1
	welcomeMessage();

	uart_puts("\nMyOS> ");

    // run CLI
    while(1) {
    	cli();
    }
}
