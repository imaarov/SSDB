#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "header.h"
#include "table_structure.h"
#include "command.h"


int main(int argc, char* argv[]) 
{
	// Initalizing
	InputBuffer* input_buffer = new_input_buffer();
	Table* table;
	if(argc < 2) {
		printf("Default db path : %s\n", DEFAULT_DB_PATH);
		char* filename = DEFAULT_DB_PATH;
		table = db_open(filename);
	} else {
		char* filename = argv[1];
		table = db_open(filename);
	}
	
	
	while(true) {
		print_prompt();
		read_input(input_buffer);

		if(input_buffer->buffer[0] == '!') {
			switch(do_meta_command(input_buffer, table)) {
				case (META_COMMAND_SUCCESS):
					continue;
				case (META_COMMAND_UNRECOGNIZED_COMMAND):
					printf("Unrecognized command '%s'\n", input_buffer->buffer);
					continue;
			}
		}

		Statement statement;
		switch (prepare_statement(input_buffer, &statement)) {
			case (PREPARE_SUCCESS):
				break;
			case (PREPARE_NEGATIVE_ID):
				printf("ID must be positive.\n");
				continue;
			case (PREPARE_STRING_TOO_LONG):
				printf("String is too long.\n");
				continue;
			case (PREPARE_SYNTAX_ERROR):
				printf("Syntax error. could not parse statement.");
				continue;
			case (PREPARE_UNRECOGNIZED_STATEMENT):
				printf("Unrecognized command '%s'\n", input_buffer->buffer);
				continue;
		}
	    switch (execute_statement(&statement, table)) {
    		case (EXECUTE_SUCCESS):
        		printf("Executed.\n");
        		break;
      		case (EXECUTE_TABLE_FULL):
				printf("Error: Table full.\n");
				break;
    	}

	}
}

void cursor_advance(Cursor* cursor) {
	cursor->row_num += 1;
	if(cursor->row_num >= cursor->table->num_rows) {
		cursor->end_of_table = true;
	}
}

Cursor* table_start(Table* table) {
	Cursor* cursor = malloc(sizeof(Cursor));
	cursor->table = table;
	cursor->row_num = 0;
	cursor->end_of_table = (table->num_rows == 0);

	return cursor;
}

Cursor* table_end(Table* table) {
	Cursor* cursor = malloc(sizeof(Cursor));
	cursor->table = table;
	cursor->row_num = table->num_rows;
	cursor->end_of_table = true;

	return cursor;
}

void pager_flush(Pager* pager, uint32_t page_num, uint32_t size) {
  if (pager->pages[page_num] == NULL) {
    printf("Tried to flush null page\n");
    exit(EXIT_FAILURE);
  }

  off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

  if (offset == -1) {
    printf("Error seeking: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_written =
      write(pager->file_descriptor, pager->pages[page_num], size);

  if (bytes_written == -1) {
    printf("Error writing: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

void db_close(Table* table) {
	Pager* pager = table->pager;
	uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

	for (uint32_t i = 0; i < num_full_pages; i++)
	{
		if(pager->pages[i] == NULL) {
			continue;
		}
		pager_flush(pager, i, PAGE_SIZE);
		free(pager->pages[i]);
		pager->pages[i] = NULL;
	}

	uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
	if (num_additional_rows > 0) {
		uint32_t page_num = num_full_pages;
		if (pager->pages[page_num] != NULL) {
		pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
		free(pager->pages[page_num]);
		pager->pages[page_num] = NULL;
		}
	}

	int result = close(pager->file_descriptor);
	if (result == -1) {
		printf("Error closing db file.\n");
		exit(EXIT_FAILURE);
	}
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		void* page = pager->pages[i];
		if (page) {
		free(page);
		pager->pages[i] = NULL;
		}
	}
	free(pager);
	free(table);
}

void* get_page(Pager* pager, uint32_t page_num) {
  if (page_num > TABLE_MAX_PAGES) {
    printf("Tried to fetch page number out of bounds. %d > %d\n", page_num,
           TABLE_MAX_PAGES);
    exit(EXIT_FAILURE);
  }

  if (pager->pages[page_num] == NULL) {
    // Cache miss. Allocate memory and load from file.
    void* page = malloc(PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGE_SIZE;

    // We might save a partial page at the end of the file
    if (pager->file_length % PAGE_SIZE) {
      num_pages += 1;
    }

    if (page_num <= num_pages) {
      lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
      if (bytes_read == -1) {
        printf("Error reading file: %d\n", errno);
        exit(EXIT_FAILURE);
      }
    }

    pager->pages[page_num] = page;
  }

  return pager->pages[page_num];
}

Pager* pager_open(const char* filename) {
  int fd = open(filename,
                O_RDWR |      // Read/Write mode
                    O_CREAT,  // Create file if it does not exist
                S_IWUSR |     // User write permission
                    S_IRUSR   // User read permission
                );
	if(fd == -1) {
		printf("Unable to create db file.\n");
		exit(EXIT_FAILURE);
	}

	off_t file_length = lseek(fd, 0, SEEK_END);

	Pager* pager = malloc(sizeof(Pager));
	pager->file_descriptor = fd;
	pager->file_length = file_length;

	for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
		pager->pages[i] = NULL;
	return pager;
}

void print_row(Row* row) {
	printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}


Table* db_open(const char* filename) {
  	Pager* pager = pager_open(filename);
  	uint32_t num_rows = pager->file_length / ROW_SIZE;

  	Table* table = malloc(sizeof(Table));
  	table->pager = pager;
  	table->num_rows = num_rows;

  	return table;
}

void* cursor_value(Cursor* cursor) {
	uint32_t row_num = cursor->row_num;
	uint32_t page_num = row_num / ROWS_PER_PAGE;
	
  	void* page = get_page(cursor->table->pager,  page_num);
  	uint32_t row_offset = row_num % ROWS_PER_PAGE;
  	uint32_t byte_offset = row_offset * ROW_SIZE;
  	return page + byte_offset;
}

void serialize_row(Row* source, void* destination) {
	memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  	memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
  	memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
	memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
  	memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
  	memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}



MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
	if(strcmp(input_buffer->buffer, "!exit") == 0 || strcmp(input_buffer->buffer, "!q") == 0) {
		db_close(table);
		exit(EXIT_SUCCESS);
	} else {
		return META_COMMAND_UNRECOGNIZED_COMMAND;
	}
}

PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
	statement->type = STATEMENT_INSERT;

	char* keyword   = strtok(input_buffer->buffer, " ");
	char* id_string = strtok(NULL, " ");
	char* username  = strtok(NULL, " ");
	char* email 	= strtok(NULL, " ");

	if(id_string == NULL || username == NULL || email == NULL) {
		return PREPARE_SYNTAX_ERROR;
	}

	int id = atoi(id_string);
	if(id < 0)
		return PREPARE_NEGATIVE_ID;
	if(strlen(username) > COLUMN_USERNAME_SIZE) {
		return PREPARE_STRING_TOO_LONG;
	}

	statement->row_to_insert.id = id;
	strcpy(statement->row_to_insert.username, username);
  	strcpy(statement->row_to_insert.email, email);

  	return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
	if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
		return prepare_insert(input_buffer, statement);
	}
	if(strcmp(input_buffer->buffer, "select") == 0) {
		statement->type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}

	return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
 	if (table->num_rows >= TABLE_MAX_ROWS) {
	return EXECUTE_TABLE_FULL;
  	}

	Row* row_to_insert = &(statement->row_to_insert);
	Cursor* cursor = table_end(table);

	serialize_row(row_to_insert, cursor_value(cursor));

  	table->num_rows += 1;
	free(cursor);

 	return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
  	Cursor* cursor = table_start(table);
	Row row;
  	while(! (cursor->end_of_table)) {
		deserialize_row(cursor_value(cursor), &row);
		print_row(&row);
		cursor_advance(cursor);
	}

	free(cursor);

  	return EXECUTE_SUCCESS;
}


ExecuteResult execute_statement(Statement* statement, Table *table) {
 	switch (statement->type) {
    	case (STATEMENT_INSERT):
       		return execute_insert(statement, table);
    	case (STATEMENT_SELECT):
			return execute_select(statement, table);
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
