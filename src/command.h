#ifndef COMMAND
#define COMMAND

typedef enum {
	META_COMMAND_SUCCESS,
	META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;

typedef enum {
	PREPARE_SUCCESS,
	PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
	STATEMENT_INSERT,
	STATEMENT_SELECT
} StatementType;

typedef struct {
	StatementType type;
} Statement;


MetaCommandResult do_meta_command(InputBuffer* );
PrepareResult prepare_result(InputBuffer* input_buffer, Statement* statement);
void execute_statement(Statement* );
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);

#endif