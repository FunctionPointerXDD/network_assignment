#pragma once

# define ID_SIZE 8
# define NAME_SIZE 20
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

// c++ --> i don't know why that struct size is 72....
typedef struct Row 
{
	size_t	id; // default key
	char name[NAME_SIZE];
	char address[ADDRESS_SIZE];
	char phone[PHONE_SIZE]; // unique key
} Row;

