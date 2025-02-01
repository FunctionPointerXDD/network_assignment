#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "PhoneBook.h"
#include "utils.h"
#include "ui.h"

void	print_row(Row* row)
{
	printf("| %08llu | %-20s | %-30s | %-12s |\n", row->id, row->name, row->address, row->phone);
}

STATUS	Select(Table* table)
{
	char	InputBuffer[256];
	char**	str_arr = NULL;
	int		found_cnt = 0;
	boolean	is_and = FALSE;
	KeyValue	set[2] = { 0 };


	printf("SELECT > ");
	fgets(InputBuffer, 256, stdin);
	InputBuffer[strlen(InputBuffer) - 1] = '\0';
	str_arr = ft_split(InputBuffer, ' ');
	if (!str_arr)
		return 0;
	if (check_select_input(str_arr) == -1)
		return 0;

    LARGE_INTEGER	frequency;
    LARGE_INTEGER	start, end;
    long long		elapsed;

    QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
	printf("| ID%6s | NAME%16s | ADDRESS%23s | PHONE%7s |\n", " ", " ", " ", " ");
	printf("-----------------------------------------------------------------------------------\n");
	int i = 0;
	while (str_arr[i])
	{
		if (i % 2 != 0 )
		{
			if (strncmp(str_arr[i], "AND", 4) == 0)
			{
				set[1].key = get_attribute(str_arr[i + 1]);
				if (strcmp(set[0].key, set[1].key) == 0)
					break;
				set[1].value = get_value(str_arr[i + 1]);
			}
		}
		else
		{
			if (str_arr[i + 1] && strncmp(str_arr[i + 1], "AND", 4) == 0)
			{
				set[0].key = get_attribute(str_arr[i]);
				set[0].value = get_value(str_arr[i]);
				is_and = TRUE;
				i++;
				continue;
			}
			else if (!is_and)
			{
				set[0].key = get_attribute(str_arr[i]);
				set[0].value = get_value(str_arr[i]);
			}
			if (strncmp(set[0].key, "phone", 6) == 0)
				found_cnt += index_search(table, &set[0], &set[1]);
			else
				found_cnt += sequential_search(table, &set[0], &set[1]);
			free(set[0].key);
			free(set[0].value);
			if (set[1].key && set[1].value)
			{
				free(set[1].key);
				free(set[1].value);
			}
			memset(set, 0, sizeof(KeyValue) * 2);
		}
		i++;
	}
	QueryPerformanceCounter(&end);
	elapsed = (end.QuadPart - start.QuadPart) * 1000000000LL / frequency.QuadPart;
	printf("time: %lld nano_sec\n", elapsed);

	free_split_arr(str_arr);
	printf("number of found: %d\n", found_cnt);
	return found_cnt ? SUCCESS : FAIL;
}

STATUS	Insert(Table* table)
{
	char* InputBuffer;
	char* tmp;
	Row		row = { 0 };
		
	InputBuffer = calloc(256, sizeof(char));
	printf("INSERT > (Name Address PhoneNumber)\n Value > ");
	fgets(InputBuffer, 256, stdin);
	InputBuffer[strlen(InputBuffer) - 1] = '\0';
	//valid_check();

	tmp = InputBuffer;
	char *name = ft_strsep(&InputBuffer, " ");
	char *address = ft_strsep(&InputBuffer, " ");
	char *phone = ft_strsep(&InputBuffer, " ");

	row.id = table->total_row_cnt + 1;
	if (name)
		strcpy_s(row.name, strlen(name) + 1, name);
	if (address)
		strcpy_s(row.address, strlen(address) + 1, address);
	if (phone)
		strcpy_s(row.phone, strlen(phone) + 1, phone);
	
	if (phone && my_binary_search(table->index_arr, phone, 0, table->total_row_cnt) != SEARCH_FAIL)
	{
		puts("The phone number already exists.");
		free(tmp);
		return FAIL;
	}
	free(tmp);

	FILE* stream = table->fp_start;
	fseek(stream, 0, SEEK_END);
	if (fwrite(&row, ROW_SIZE, 1, stream) == 1)
	{
		table->file_length += ROW_SIZE;
		table->total_row_cnt++;
		update_index_for_insert(table, row.phone);
		return SUCCESS;
	}
	return FAIL;
}
STATUS	Update(Table* table)
{
	char* InputBuffer = NULL;
	char* tmp;
	int		id = 0;
	Row		row = { 0 };
	
	printf("Please enter the ID that you want to update...\n ID > ");
	scanf_s("%d%*c", &id);
	printf(" UPDATE > (Name Address PhoneNumber)\n");
	printf("    SET > ");
	InputBuffer = calloc(256, sizeof(char));
	fgets(InputBuffer, 256, stdin);
	InputBuffer[strlen(InputBuffer) - 1] = '\0';
	//valid_check();
	tmp = InputBuffer;
	char *name = ft_strsep(&InputBuffer, " ");
	char *address = ft_strsep(&InputBuffer, " ");
	char *phone = ft_strsep(&InputBuffer, " ");

	row.id = id;
	if (name)
		strcpy_s(row.name, strlen(name) + 1, name);
	if (address)
		strcpy_s(row.address, strlen(address) + 1, address);
	if (phone)
		strcpy_s(row.phone, strlen(phone) + 1, phone);

	if (phone && my_binary_search(table->index_arr, phone, 0, table->total_row_cnt) != SEARCH_FAIL)
	{
		puts(" The phone number already exists.");
		free(tmp);
		return FAIL;
	}
	free(tmp);

	FILE* stream = table->fp_start;
	long file_offset = (id - 1) * (ROW_SIZE);
	fseek(stream, file_offset, SEEK_SET);
	if (fwrite(&row, ROW_SIZE, 1, stream) == 1)
	{
		strcpy_s(table->index_arr[id - 1]->phone, strlen(row.phone) + 1, row.phone);
		table->index_arr[id - 1]->index_offset = file_offset;
		sort_index_array(table->index_arr, table->index_end);
		return SUCCESS;
	}
	return FAIL;
}

STATUS	Delete(Table* table)
{
	int		id = 0;
	Row		row = { 0 };
	
	printf("Please enter the ID that you want to delete...\n ID > ");
	scanf_s("%d%*c", &id);

	FILE* stream = table->fp_start;
	long file_offset = (id - 1) * (ROW_SIZE);
	fseek(stream, file_offset, SEEK_SET);
	if (fwrite(&row, ROW_SIZE, 1, stream) == 1)
	{
		//deleted index is to be zero...
		memset(table->index_arr[id - 1]->phone, 0, PHONE_SIZE);
		table->index_arr[id - 1]->index_offset = file_offset;
		sort_index_array(table->index_arr, table->index_end);
		return SUCCESS;
	}
	return FAIL;
}

int	PrintUI(void)
{
	char option;

	printf(" [0]Exit\t[1]Select\t[2]Insert\t[3]Update\t[4]Delete\n");
	while (1)
	{
		option = _getch();
		if (option >= '0' && option <= '4')
			break;
	}
	return option - '0';
}

void	event_loop(Table* table)
{
	STATUS (*query[4])(Table*) = { Select, Insert, Update, Delete };

	int	menu = 0;
	while ((menu = PrintUI()) != 0)
	{
		int status = 0;

		printf("\033[2J\033[1;1H");
		status = query[menu - 1](table);
		if (status == FAIL)
			puts("Fail");
		else
			puts("Execute");
	}
}

