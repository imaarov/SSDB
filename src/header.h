#ifndef INPUTBUFF
#define INPUTBUFF

// Structures
typedef struct
{
	char* buffer;
	size_t buffer_length;
	ssize_t input_length;
} InputBuffer;

// Func Prototypes
InputBuffer* new_input_buffer(void);
void print_prompt();
void read_input(InputBuffer* input_buffer);
void close_input_buffer(InputBuffer* input_buffer);

#endif