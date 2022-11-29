#ifndef TABLE_STRUCTURE
#define TABLE_STRUCTURE

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 100
#define SIZE_OF_ATTRIBUTE(STRUCT, ATTRIBUTE) \
	sizeof(((STRUCT *)0)->ATTRIBUTE)

typedef enum { 
	EXECUTE_SUCCESS,
	EXECUTE_TABLE_FULL
} ExecuteResult;

typedef struct {
	uint32_t id;
	char username[COLUMN_USERNAME_SIZE + 1];
	char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef struct {
	int file_descriptor;
	uint32_t file_length;
	void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
	uint32_t num_rows;
	Pager* pager;
} Table;

typedef struct {
	Table* table;
	uint32_t row_num;
	bool end_of_table;
} Cursor;

const uint32_t ID_SIZE = SIZE_OF_ATTRIBUTE(Row, id);
const uint32_t USERNAME_SIZE = SIZE_OF_ATTRIBUTE(Row, username);
const uint32_t EMAIL_SIZE = SIZE_OF_ATTRIBUTE(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;


void serialize_row(Row* , void* );
void deserialize_row(void* , Row* );
void* cursor_value(Cursor* );
Table* db_open(const char* );
void free_table(Table* );
void print_row(Row* );
Pager* pager_open(const char* );
void* get_page(Pager* , uint32_t );
void db_close(Table* );
Cursor* table_start(Table* );
Cursor* table_end(Table* );
void cursor_advance(Cursor* );



#endif