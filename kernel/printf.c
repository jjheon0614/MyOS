#include "printf.h"
#include "../uart/uart0.h"
#include "../uart/uart1.h"

#define MAX_PRINT_SIZE 256


void printf(char *string,...) {

	va_list ap;
	va_start(ap, string);

	char buffer[MAX_PRINT_SIZE];
	for (int i = 0; i < MAX_PRINT_SIZE; i++) {
		buffer[i] = 0;
	}

	int buffer_index = 0;

	char temp_buffer[MAX_PRINT_SIZE];

	while(1) {
		if (*string == 0)
			break;

		if (*string == '%') {
			string++;

			int widthPadding = 0;

			if (*string == '*') {
				string++;

				widthPadding = va_arg(ap, int);
			}

			if (*string == '0') {
				// Handle zero padding
				string++;

				int zeroPadding = 0;
				while (*string >= '0' && *string <= '9') {
					zeroPadding = zeroPadding * 10 + (*string - '0');
					string++;
				}

				if (*string == 'd') {
					string++;
					int x = va_arg(ap, int);
					int temp_index = MAX_PRINT_SIZE - 1;


					int isNegative = 0;
					if (x < 0) {
						buffer[buffer_index] = '-';
						buffer_index++;
						x = -x; // Make x positive for the conversion
						isNegative = 1;
					}


					do {
						temp_buffer[temp_index] = (x % 10) + '0';
						temp_index--;
						x /= 10;
					} while(x != 0);

					int cur_index = buffer_index;

					// Add zero padding
					while (buffer_index <= temp_index - MAX_PRINT_SIZE + zeroPadding + cur_index - isNegative) {
						buffer[buffer_index] = '0';
						buffer_index++;
					}

					for(int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
						buffer[buffer_index] = temp_buffer[i];
						buffer_index++;
					}
				} else if (*string == 'x') {
					string++;
					long x = va_arg(ap, int);
					int temp_index = MAX_PRINT_SIZE - 1;

					int num_digits = 0;
					int temp_x = x;
					do {
						temp_x /= 16;
						num_digits++;
					} while (temp_x != 0);

					int isNegative = 0;
					if (x < 0) {
						if (zeroPadding > 0) {
							for (int i = 0; i < zeroPadding - 8; i++) {
								buffer[buffer_index] = '0';
								buffer_index++;
							}
						}


						isNegative = 1;
						x += 4294967296;
					}

					if (x == 0) {
						temp_buffer[temp_index] = '0';
						temp_index--;
						num_digits++;
					} else {
						do {
							int remainder = x % 16;
							if (remainder < 10) {
								temp_buffer[temp_index] = remainder + '0';
							} else {
								temp_buffer[temp_index] = remainder - 10 + 'a';
							}
							temp_index--;
							x /= 16;
						} while (x != 0);
					}

					int cur_index = buffer_index;

					// Add zero padding

					if (!isNegative) {
						while (buffer_index <= temp_index - MAX_PRINT_SIZE + zeroPadding + cur_index) {

							buffer[buffer_index] = '0';
							buffer_index++;
						}
					}

					// Copy the converted integer to the buffer
					for (int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
						buffer[buffer_index] = temp_buffer[i];
						buffer_index++;
					}
				} else if (*string == 'f') {
					string++;
					double f = va_arg(ap, double);


					// Handle negative numbers
					int isNegative = 0;
					if (f < 0) {
						buffer[buffer_index] = '-';
						buffer_index++;
						f = -f;
						isNegative = 1;
					}

					
					int int_part = (int)f;
					double frac_part = f - int_part;

					// Print integer part
					int temp_index = MAX_PRINT_SIZE - 1;
					do {
						temp_buffer[temp_index] = (int_part % 10) + '0';
						temp_index--;
						int_part /= 10;
					} while (int_part != 0);

					int cur_index = buffer_index;

					// Add zero padding
					while (buffer_index <= temp_index - MAX_PRINT_SIZE + zeroPadding + cur_index - isNegative - 7) {

						buffer[buffer_index] = '0';
						buffer_index++;
					}


					for (int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
						buffer[buffer_index] = temp_buffer[i];
						buffer_index++;
					}

					// Print decimal point
					buffer[buffer_index] = '.';
					buffer_index++;

					// Print fractional part (up to 6 digits)
					for (int i = 0; i < 6; i++) {
						frac_part *= 10;
						int digit = (int)frac_part;
						buffer[buffer_index] = digit + '0';
						buffer_index++;
						frac_part -= digit;
					}
				}
			} else if (*string == '.') {
				// Handle zero padding
				string++;

				
				int precisionPadding = 0;
				while (*string >= '0' && *string <= '9') {
					precisionPadding = precisionPadding * 10 + (*string - '0');
					string++;
				}


				if (*string == 'd') {
					string++;
					int x = va_arg(ap, int);
					int temp_index = MAX_PRINT_SIZE - 1;


					if (x < 0) {
						buffer[buffer_index] = '-';
						buffer_index++;
						x = -x; // Make x positive for the conversion
					}


					do {
						temp_buffer[temp_index] = (x % 10) + '0';
						temp_index--;
						x /= 10;
					} while(x != 0);

					int cur_index = buffer_index;

					// Add precision padding
					while (buffer_index <= temp_index - MAX_PRINT_SIZE + precisionPadding + cur_index) {

						buffer[buffer_index] = '0';
						buffer_index++;
					}

					for(int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
						buffer[buffer_index] = temp_buffer[i];
						buffer_index++;
					}
				} else if (*string == 'x') {
					string++;
					long x = va_arg(ap, int);
					int temp_index = MAX_PRINT_SIZE - 1;

					int num_digits = 0;
					int temp_x = x;
					do {
						temp_x /= 16;
						num_digits++;
					} while (temp_x != 0);

					int isNegative = 0;
					if (x < 0) {
						if (precisionPadding > 0) {
							for (int i = 0; i < precisionPadding - 8; i++) {
								buffer[buffer_index] = '0';
								buffer_index++;
							}
						}


						isNegative = 1;
						x += 4294967296;
					}

					if (x == 0) {
						temp_buffer[temp_index] = '0';
						temp_index--;
						num_digits++;
					} else {
						do {
							int remainder = x % 16;
							if (remainder < 10) {
								temp_buffer[temp_index] = remainder + '0';
							} else {
								temp_buffer[temp_index] = remainder - 10 + 'a';
							}
							temp_index--;
							x /= 16;
						} while (x != 0);
					}

					int cur_index = buffer_index;

					// Add precision padding

					if (!isNegative) {
						while (buffer_index <= temp_index - MAX_PRINT_SIZE + precisionPadding + cur_index) {

							buffer[buffer_index] = '0';
							buffer_index++;
						}
					}

					// Copy the converted integer to the buffer
					for (int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
						buffer[buffer_index] = temp_buffer[i];
						buffer_index++;
					}
				} else if (*string == 'f') {
					string++;
					double f = va_arg(ap, double);


					// Handle negative numbers
					if (f < 0) {
						buffer[buffer_index] = '-';
						buffer_index++;
						f = -f;
					}

					
					int int_part = (int)f;
					double frac_part = f - int_part;

					// Print integer part
					int temp_index = MAX_PRINT_SIZE - 1;
					do {
						temp_buffer[temp_index] = (int_part % 10) + '0';
						temp_index--;
						int_part /= 10;
					} while (int_part != 0);


					for (int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
						buffer[buffer_index] = temp_buffer[i];
						buffer_index++;
					}

					// Print decimal point
					buffer[buffer_index] = '.';
					buffer_index++;

					// Print fractional part (up to 6 digits)
					for (int i = 0; i < precisionPadding; i++) {
						frac_part *= 10;
						int digit = (int)frac_part;
						buffer[buffer_index] = digit + '0';
						buffer_index++;
						frac_part -= digit;
					}
				} else if (*string == 's') {
					string++;
					char *s = va_arg(ap, char*);
					int s_index = 0;

					while (s[s_index] != '\0') {
						s_index++;
					}

					for (int i = 0; i < widthPadding - s_index; i++) {
						buffer[buffer_index] = ' ';
						buffer_index++;
					}

					s_index = 0;

					for (int i = 0; i < precisionPadding; i++) {
						buffer[buffer_index] = s[s_index];
						buffer_index++;
						s_index++;
					}
				}
			} else if (*string == 'd') {
				string++;
				int x = va_arg(ap, int);
				int temp_index = MAX_PRINT_SIZE - 1;

				int isNegative = 0;
				if (x < 0) {
					x = -x; // Make x positive for the conversion
					isNegative = 1;
				}


				do {
					temp_buffer[temp_index] = (x % 10) + '0';
					temp_index--;
					x /= 10;
				} while(x != 0);

				if (widthPadding != 0) {
					int cur_index = buffer_index;

					while (buffer_index <= temp_index - MAX_PRINT_SIZE + widthPadding + cur_index - isNegative) {

						buffer[buffer_index] = ' ';
						buffer_index++;
					}
				}

				if (isNegative) {
					buffer[buffer_index] = '-';
					buffer_index++;
				}

				for(int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
					buffer[buffer_index] = temp_buffer[i];
					buffer_index++;
				}
			} else if (*string == 'c') {
				string++;
				char c = va_arg(ap, int); // Characters are promoted to int in variadic arguments
				

				for (int i = 0; i < widthPadding - 1; i++) {
					buffer[buffer_index] = ' ';
					buffer_index++;
				}
				
				buffer[buffer_index] = c;
				buffer_index++;
			} else if (*string == 's') {
				string++;

				char *s = va_arg(ap, char*);


				int s_index = 0;


				while (s[s_index] != '\0') {
					s_index++;
				}

				for (int i = 0; i < widthPadding - s_index; i++) {
					buffer[buffer_index] = ' ';
					buffer_index++;
				}

				s_index = 0;

				while (s[s_index] != '\0') {
					buffer[buffer_index] = s[s_index];
					buffer_index++;
					s_index++;
				}
			} else if (*string == 'f') {
				string++;
				double f = va_arg(ap, double);


				// Handle negative numbers
				int isNegative = 0;

				if (f < 0) {
					f = -f;
					isNegative = 1;
				}

				
				int int_part = (int)f;
				double frac_part = f - int_part;

				// Print integer part
				int temp_index = MAX_PRINT_SIZE - 1;


				do {
					temp_buffer[temp_index] = (int_part % 10) + '0';
					temp_index--;
					int_part /= 10;
				} while (int_part != 0);

				int cur_index = buffer_index;

				while (buffer_index <= temp_index - MAX_PRINT_SIZE + widthPadding + cur_index - isNegative - 7) {
					buffer[buffer_index] = ' ';
					buffer_index++;
				}


				if (isNegative) {
					buffer[buffer_index] = '-';
					buffer_index++;
				}

				for (int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
					buffer[buffer_index] = temp_buffer[i];
					buffer_index++;
				}

				// Print decimal point
				buffer[buffer_index] = '.';
				buffer_index++;

				// Print fractional part (up to 6 digits)
				for (int i = 0; i < 6; i++) {
					frac_part *= 10;
					int digit = (int)frac_part;
					buffer[buffer_index] = digit + '0';
					buffer_index++;
					frac_part -= digit;
				}


			} 
			else if (*string == '%') {
				buffer[buffer_index] = '%';
				buffer_index++;
				string++;

			} 
			else if (*string == 'x') {
				string++;
				long x = va_arg(ap, int);
				int temp_index = MAX_PRINT_SIZE - 1;

				int num_digits = 0;
				int temp_x = x;


				do {
					temp_x /= 16;
					num_digits++;
				} while (temp_x != 0);



				int isNegative = 0;

				if (x < 0) {
					x += 4294967296;
					isNegative = 1;
				}

				if (isNegative) {
					for (int i = 0; i < widthPadding - 8; i++) {
						buffer[buffer_index] = ' ';
						buffer_index++;
					}
				} else {
					for (int i = 0; i < widthPadding - num_digits; i++) {
						buffer[buffer_index] = ' ';
						buffer_index++;
					}
				}

				if (x == 0) {
					temp_buffer[temp_index] = '0';
					temp_index--;
					num_digits++;
				} else {
					do {
						int remainder = x % 16;
						if (remainder < 10) {
							temp_buffer[temp_index] = remainder + '0';
						} else {
							temp_buffer[temp_index] = remainder - 10 + 'a';
						}
						temp_index--;
						x /= 16;
					} while (x != 0);
				}

				// Copy the converted integer to the buffer
				for (int i = temp_index + 1; i < MAX_PRINT_SIZE; i++) {
					buffer[buffer_index] = temp_buffer[i];
					buffer_index++;
				}
			}

			
		}
		else {
			buffer[buffer_index] = *string;
			buffer_index++;
			string++;
		}

		if (buffer_index == MAX_PRINT_SIZE - 1)
			break;
	}

	va_end(ap);


	//Print out formated string
	uart_puts(buffer);
}
