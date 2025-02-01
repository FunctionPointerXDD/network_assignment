#pragma once

# define ID_SIZE 8
# define NAME_SIZE 21
# define ADDRESS_SIZE 30
# define PHONE_SIZE 12 
# define ROW_SIZE (ID_SIZE + NAME_SIZE + ADDRESS_SIZE + PHONE_SIZE)

# define ID_OFFSET 0
# define NAME_OFFSET ID_SIZE;
# define ADDRESS_OFFSET (ID_SIZE + NAME_SIZE)
# define PHONE_OFFSET (ID_SIZE + NAME_SIZE + ADDRESS_SIZE)

# define TOTAL_TEST_ROWS 10000
# define INDEX_ARRAY_SIZE_START_VALUE 10000

# define SEARCH_FAIL -1

//Row size = 70 [ 8 + 20 + 30 + 12 ]
typedef struct Row
{
	size_t	id; // default key
	char	name[NAME_SIZE];
	char	address[ADDRESS_SIZE];
	char	phone[PHONE_SIZE]; // unique key
} Row;

typedef struct Index
{
	char	phone[PHONE_SIZE];
	size_t	index_offset;
} Index;

typedef struct Table
{
	Index**	index_arr;
	size_t	index_end; //last location in index array that exist;
	size_t	index_arr_size; // allocated index array size;
	FILE*	fp_start;
	size_t	file_length;
	size_t	total_row_cnt;
	boolean is_test;
} Table;

typedef struct KeyValue
{
	char* key;
	char* value;
} KeyValue;

char*		get_attribute(const char* str);
char*		get_value(const char* str);
int			left_condition(Row* row, KeyValue* left);
int			right_condition(Row* row, KeyValue* right);
int			sequential_search(Table* table, KeyValue* left, KeyValue* right);
long long	my_binary_search(Index** idx_arr, const char* value, long long low, long long high);
int			index_search(Table* table, KeyValue* left, KeyValue* right);
int			check_select_input(const char** str_arr);
int			resize_index_arr(Table* table, size_t new_size);
void		update_index_for_insert(Table* table, const char* value);
void		swap_index(Index** idx1, Index** idx2);
void		sort_index_array(Index** index_arr, size_t index_cnt);
void		sort_index_array_for_test(Index** index_arr, size_t index_cnt);
Index**		GetIndexArray(Table* table);
void		open_db(Table *table);
void		convert_file(Table* table);
void		close_db(Table* table);

