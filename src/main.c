#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "header.h"

int main() 
{
	InputBuffer* input_buffer = new_input_buffer();

	while(true) {
		print_prompt();
		read_input(input_buffer);

		if(
			strcmp(input_buffer->buffer, ".exit") == 0 || 
			strcmp(input_buffer->buffer, "exit")  == 0 ||
			strcmp(input_buffer->buffer, "q")     == 0
			) {
			close_input_buffer(input_buffer);
			return 0;
		} else {
			printf("Unrecognized command '%s'.\n", input_buffer->buffer);
		}
	}
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}


void read_input(InputBuffer* input_buffer) {
	ssize_t bytes_read = 
		getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
	if (bytes_read <= 0) {
		printf("Error reading input\n");
		exit(EXIT_FAILURE);
	}

	input_buffer->input_length = bytes_read - 1;
  	input_buffer->buffer[bytes_read - 1] = 0;
}

void print_prompt() { printf("ssdb > "); }

InputBuffer* new_input_buffer() {
	InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  	input_buffer->buffer = NULL;
  	input_buffer->buffer_length = 0;
  	input_buffer->input_length = 0;

  	return input_buffer;
}