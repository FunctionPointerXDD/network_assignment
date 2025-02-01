
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <wincrypt.h>
#include "PhoneBook.h"
#include "utils.h"
#include "random_phonebook.h"

char* get_attribute(const char* str)
{
	char* equal;
	char* res;

	equal = strchr(str, '=');
	*equal = '\0';
	res = _strdup(str);
	*equal = '=';
	return res;
}

char* get_value(const char* str)
{
	char* equal;

	equal = strchr(str, '=');
	return _strdup(equal + 1);
}

int	left_condition(Row *row, KeyValue *left)
{
	if (((strncmp(left->key, "id", 3) == 0) && (row->id == atoll(left->value))) || \
		((strncmp(left->key, "name", 5) == 0) && (strncmp(row->name, left->value, strlen(left->value) + 1) == 0)) || \
		((strncmp(left->key, "address", 8) == 0) && (strncmp(row->address, left->value, strlen(left->value) + 1) == 0)))
		return 1;
	return 0;
}

int	right_condition(Row *row, KeyValue *right)
{
	if (((strncmp(right->key, "id", 3) == 0) && (row->id == atoll(right->value))) || \
		((strncmp(right->key, "name", 5) == 0) && (strncmp(row->name, right->value, strlen(right->value) + 1) == 0)) || \
		((strncmp(right->key, "address", 8) == 0) && (strncmp(row->address, right->value, strlen(right->value) + 1) == 0)))
		return 1;
	return 0;
}

int	sequential_search(Table *table, KeyValue *left, KeyValue *right)
{
	FILE*	stream = table->fp_start;
	int		cnt = 0;

	fseek(stream, 0, SEEK_SET);
	while (!feof(stream))
	{
		Row		row = { 0 };
		fread(&row, ROW_SIZE, 1, stream);

		if (right->key && right->value)
		{
			if (left_condition(&row, left) && right_condition(&row, right))
			{
				print_row(&row);
				cnt++;
			}
		}
		else
		{
			if (left_condition(&row, left))
			{
				print_row(&row);
				cnt++;
			}
		}
	}

	return cnt;
}

long long my_binary_search(Index** idx_arr, const char* value, long long low, long long high)
{
	char* pivot;
	size_t	mid = 0;

	high--;
	if (high < 0)
		return SEARCH_FAIL;
	while (low <= high)
	{
		mid = low + (high - low) / 2;
		pivot = idx_arr[mid]->phone;
		if (strcmp(pivot, value) == 0)
			return mid;
		else if (strcmp(pivot, value) > 0)
			high = mid - 1;
		else if (strcmp(pivot, value) < 0)
			low = mid + 1;
	}
	return SEARCH_FAIL;
}

//phone number is unique, so the cnt must be one.
int	index_search(Table* table, KeyValue* left, KeyValue* right)
{
	FILE* stream = table->fp_start;
	Index** idx_arr = table->index_arr;
	long long key_pos;
	size_t	key_offset;
	int		cnt = 0;

	(void)right;
	fseek(stream, 0, SEEK_SET);
	if (!feof(stream))
	{
		Row		row = { 0 };
		key_pos = my_binary_search(idx_arr, left->value, 0, table->total_row_cnt);
		if (key_pos == SEARCH_FAIL)
			return FALSE;
		key_offset = idx_arr[key_pos]->index_offset;
		fseek(stream, key_offset, SEEK_SET);
		fread(&row, ROW_SIZE, 1, stream);
		print_row(&row);
		cnt++;
	}

	return cnt;
}

int	check_select_input(const char** str_arr)
{
	int equal;
	int And = 0;

	for (int i = 0; str_arr[i]; i++)
	{
		if (i % 2 == 0)
		{
			equal = 0;
			for (int j = 0; str_arr[i][j]; j++)
			{
				if (str_arr[i][j] == '=')
					equal++;
			}
			if (!equal || equal > 1)
			{
				printf(" Invalid argument!\n");
				printf(" ex) id=4242 AND/OR name=chansjeo ...\n");
				return -1;
			}
		}
		else if (i % 2 != 0)
		{
			if ((strncmp(str_arr[i], "AND", 4) != 0) && (strncmp(str_arr[i], "OR", 3) != 0))
			{
				printf(" Operator must be AND/OR\n");
				return -1;
			}
			else if (strncmp(str_arr[i], "AND", 4) == 0)
				And++;
		}
	}

	if (And > 2)
	{
		printf(" only one AND operator can be used.\n");
		return -1;
	}
	return 0;
}


int	resize_index_arr(Table *table, size_t new_size)
{
	Index** src_index_arr;
	size_t	old_size = table->index_end;

	src_index_arr = table->index_arr;
	table->index_arr = calloc(new_size + 1, sizeof(Index *));
	if (!table->index_arr)
		error_msg("calloc_failed");

	for (int i = 0; i < old_size; i++)
		table->index_arr[i] = src_index_arr[i];

	for (int i = old_size; i < new_size; i++)
	{
		table->index_arr[i] = calloc(1, sizeof(Index));
		if (!table->index_arr[i])
			error_msg("calloc_failed");
	}
	table->index_arr[new_size] = NULL;
	free(src_index_arr);
	return 0;
}

void	update_index_for_insert(Table* table, const char *value)
{
	if (table->index_end + INDEX_ARRAY_SIZE_START_VALUE > table->index_arr_size)
	{
		table->index_arr_size = table->index_arr_size << 1;
		resize_index_arr(table, table->index_arr_size);
	}
	
	table->index_arr[table->index_end]->index_offset = table->file_length - ROW_SIZE;
	strcpy_s(table->index_arr[table->index_end]->phone, strlen(value) + 1, value);
	table->index_end++;
	sort_index_array(table->index_arr, table->index_end);
}


void	swap_index(Index** idx1, Index** idx2)
{
	Index* addr = NULL;
	
	addr = *idx1;
	*idx1 = *idx2;
	*idx2 = addr;
}

void	sort_index_array(Index** index_arr, size_t index_end)
{
	for (int i = 0; i < index_end - 1; ++i)
	{
		for (int j = i + 1; j < index_end; ++j)
		{
			if (strncmp(index_arr[i]->phone, index_arr[j]->phone, PHONE_SIZE) > 0)
				swap_index(&index_arr[i], &index_arr[j]);
		}
	}
}

void	sort_index_array_for_test(Index** index_arr, size_t index_end)
{
	boolean is_duplicate = FALSE;

	sort_index_array(index_arr, index_end);
	//for (int i = 0; i < index_end; ++i)
	//	printf("phone: %s, offset: %lld\n", index_arr[i]->phone, index_arr[i]->index_offset);
	for (int i = 0; i < index_end - 1; ++i)
	{
		if (strncmp(index_arr[i]->phone, index_arr[i + 1]->phone, PHONE_SIZE) == 0)
		{
			is_duplicate = TRUE;
			puts(" > A duplicate phone number was found");
			puts(" > Try regenerate number...");
			GenRandomNumber(index_arr[i]->phone);
			GenRandomNumber(index_arr[i + 1]->phone);
		}
	}
	if (is_duplicate == FALSE)
		puts(" > Unique Numbers!");
	else if (is_duplicate == TRUE)
		sort_index_array_for_test(index_arr, index_end);
}

Index** GetIndexArray(Table *table)
{
	FILE*	stream;
	Index** index_arr;

	if (table->index_end == 0)
	{
		table->index_arr_size = INDEX_ARRAY_SIZE_START_VALUE;
		index_arr = calloc(table->index_arr_size + 1, sizeof(Index*));
		if (!index_arr)
			error_msg("calloc_failed!");
		for (size_t i = 0; i < table->index_arr_size; i++)
		{
			index_arr[i] = calloc(1, sizeof(Index));
			if (!index_arr[i])
				error_msg("calloc_failed");
			index_arr[i]->index_offset = ((size_t)ROW_SIZE * i);
		}
		return index_arr;
	}
	else
	{
		if (table->index_arr_size < INDEX_ARRAY_SIZE_START_VALUE)
			table->index_arr_size = INDEX_ARRAY_SIZE_START_VALUE;
		index_arr = calloc(table->index_arr_size + 1, sizeof(Index*));
		if (!index_arr)
			error_msg("calloc_failed!");
	}

	stream = table->fp_start;
	fseek(stream, 0, SEEK_SET);
	for (size_t i = 0; i < table->index_end; ++i)
	{
		index_arr[i] = calloc(1, sizeof(Index));
		if (!index_arr[i])
			error_msg("calloc_failed!");
		
		index_arr[i]->index_offset = ((size_t)ROW_SIZE * i);
		fseek(stream, PHONE_OFFSET, SEEK_CUR);
		fread(index_arr[i]->phone, PHONE_SIZE, 1, stream);
	}
	index_arr[table->index_arr_size] = NULL;

	if (table->is_test)
		sort_index_array_for_test(index_arr, table->index_end);
	else
		sort_index_array(index_arr, table->index_end);

	return index_arr;
}

void open_db(Table *table)
{
	FILE*	fp = NULL;
	errno_t err = 0;

	err = fopen_s(&fp, "TEST_PhoneBook.dat", "rb+");
	if (err)
	{
		char input;
		printf("Do you want to create a Test address book? [Y/n]\n");
		input = _getch();
		if (input == 'Y' || input == 'y' || input == '\r' || input == '\n')
		{
			table->is_test = TRUE;
			CreateRandomPhoneBook(table);
		}
		else //void file
		{
			fopen_s(&fp, "TEST_PhoneBook.dat", "wb+");
			table->fp_start = fp;
			table->file_length = 0;
			table->total_row_cnt = 0;
			table->index_end = 0;
			table->index_arr = GetIndexArray(table);
		}
	}
	else
	{
		fseek(fp, 0, SEEK_END);
		table->file_length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		table->fp_start = fp;
		table->total_row_cnt = table->file_length / ROW_SIZE;
		table->index_end = table->total_row_cnt;
		table->index_arr_size = table->total_row_cnt;
		table->index_arr = GetIndexArray(table);
	}
}

void	convert_file(Table *table)
{
	FILE* temp_fp = NULL;
	FILE* change_fp = NULL;
	errno_t err1 = 0;
	errno_t err2 = 0;

	if (rename("TEST_PhoneBook.dat", ".temp_file.dat") != 0)
		error_msg("rename_failed");
	err1 = fopen_s(&temp_fp, ".temp_file.dat", "rb+");
	err2 = fopen_s(&change_fp, "TEST_PhoneBook.dat", "wb+");

	if (!err1 && !err2)
	{
		fseek(temp_fp, 0, SEEK_SET);
		size_t new_id = 0;
		size_t null_cnt = 0;
		while (!feof(temp_fp) && table->file_length)
		{
			Row row = { 0 };
			fread(&row, ROW_SIZE, 1, temp_fp);
			if (row.id == 0)
				null_cnt++;
			else
			{
				row.id = ++new_id;
				fwrite(&row, ROW_SIZE, 1, change_fp);
			}
			table->file_length -= ROW_SIZE;
		}
		fclose(temp_fp);
		_unlink(".temp_file.dat");
		fclose(change_fp);
		printf(" > %lld bytes clear.\n", null_cnt * ROW_SIZE);
	}
}

void	close_db(Table* table)
{
	size_t deleted_rows = 0;
	double deleted_rate = 0;

	fseek(table->fp_start, 0, SEEK_SET);
	while (!feof(table->fp_start))
	{
		Row row = { 0 };
		fread(&row, ROW_SIZE, 1, table->fp_start);
		if (row.id == 0)
			deleted_rows++;
	}
	fclose(table->fp_start);

	deleted_rate = (double)deleted_rows / (double)table->total_row_cnt;
	if (table->is_test)
		convert_file(table);
	else if(deleted_rate >= 10.0)
		convert_file(table);
	
	for (int i = 0; i < table->index_arr_size; i++)
		free(table->index_arr[i]);
	free(table->index_arr);
	table->index_arr = 0;
}

