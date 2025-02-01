
#include "Service.h"

Service::Service(void)
	:table(nullptr), ui(nullptr), display_ui(nullptr)
{

}

Service::~Service(void)
{

}

Service& Service::getInstance(void)
{
	static Service instance;
	return instance;
}

void	Service::setService(Table* _table, UI* _ui, DisplayUI* _display_ui)
{
	this->table = _table;
	this->ui = _ui;
	this->display_ui = _display_ui;
}

//parse method
STATUS Service::ParseUserInput_For_Select_And_Search(Table *table, char* InputBuffer)
{
	char** str_arr = nullptr;
	int    found_cnt = 0;
	bool   is_and = false;

	str_arr = ft_split(InputBuffer, ' ');
	if (!str_arr)
		return FAIL;
	if (check_select_input(str_arr) == -1)
		return FAIL;

    LARGE_INTEGER	frequency;
    LARGE_INTEGER	start, end;
    long long		elapsed;

    QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	printf("| ID%6s | NAME%16s | ADDRESS%23s | PHONE%7s |\n", " ", " ", " ", " ");
	printf("-----------------------------------------------------------------------------------\n");

	KeyValue	set[2] = { 0 };
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

STATUS	Service::ParseUserInput_For_Insert(Table *table, char* InputBuffer)
{
	Row	  row = { 0 };

	char* name = ft_strsep(&InputBuffer, " ");
	char* address = ft_strsep(&InputBuffer, " ");
	char* phone = ft_strsep(&InputBuffer, " ");

	row.id = table->getTotalRowCount() + 1;
	if (name)
		memcpy(row.name, name, strlen(name) + 1);
	if (address)
		memcpy(row.address, address, strlen(address) + 1);
	if (phone)
		memcpy(row.phone, phone, strlen(phone) + 1);


	if (phone && my_binary_search(table->getIndexArr(), phone, 0, table->getTotalRowCount()) != SEARCH_FAIL)
	{
		puts("The phone number already exists.");
		return FAIL;
	}
	this->table->UpdateTableForInsert(table, &row);
}

STATUS	Service::ParseUserInput_For_Update(Table* table, char* InputBuffer, int id)
{
	Row		row = { 0 };
	char*	tmp = InputBuffer;

	char *name = ft_strsep(&InputBuffer, " ");
	char *address = ft_strsep(&InputBuffer, " ");
	char *phone = ft_strsep(&InputBuffer, " ");

	row.id = id;
	if (name)
		memcpy(row.name, name, NAME_SIZE);
	if (address)
		memcpy(row.address, address, ADDRESS_SIZE);
	if (phone)
		memcpy(row.phone, phone, PHONE_SIZE);

	if (phone && my_binary_search(table->getIndexArr(), phone, 0, table->getTotalRowCount()) != SEARCH_FAIL)
	{
		puts(" The phone number already exists.");
		delete tmp;
		return FAIL;
	}

	this->table->UpdateTableForUpdate(table, &row);

	delete tmp;
}

STATUS	Service::ParseUserInput_To_Delete(Table* table, char* InputBuffer, int id)
{
	(void)InputBuffer;

	this->table->UpdateTableForDelete(table, id);
	return SUCCESS;
}

char* Service::get_attribute(char* str)
{
	char* equal;
	char* res;

	equal = strchr(str, '=');
	*equal = '\0';
	res = _strdup(str);
	*equal = '=';
	return res;
}

char* Service::get_value(char* str)
{
	char* equal;

	equal = strchr(str, '=');
	return _strdup(equal + 1);
}

int   Service::check_select_input(char** str_arr)
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

//search method
int	Service::left_condition(Row* row, KeyValue* left)
{
	if (((strncmp(left->key, "id", 3) == 0) && (row->id == atoll(left->value))) || \
		((strncmp(left->key, "name", 5) == 0) && (strncmp(row->name, left->value, strlen(left->value) + 1) == 0)) || \
		((strncmp(left->key, "address", 8) == 0) && (strncmp(row->address, left->value, strlen(left->value) + 1) == 0)))
		return 1;
	return 0;
}

int	Service::right_condition(Row* row, KeyValue* right)
{
	if (((strncmp(right->key, "id", 3) == 0) && (row->id == atoll(right->value))) || \
		((strncmp(right->key, "name", 5) == 0) && (strncmp(row->name, right->value, strlen(right->value) + 1) == 0)) || \
		((strncmp(right->key, "address", 8) == 0) && (strncmp(row->address, right->value, strlen(right->value) + 1) == 0)))
		return 1;
	return 0;
}

int	Service::sequential_search(Table* table, KeyValue* left, KeyValue* right)
{
	FILE* stream = table->getFilePointer();
	Row		row = { 0 };
	int		cnt = 0;

	fseek(stream, 0, SEEK_SET);
	while (!feof(stream))
	{
		memset(&row, 0, ROW_SIZE);
		fread(&row, ROW_SIZE, 1, stream);

		if (right->key && right->value)
		{
			if (left_condition(&row, left) && right_condition(&row, right))
			{
				UI::PrintRow(&row);
				cnt++;
			}
		}
		else
		{
			if (left_condition(&row, left))
			{
				UI::PrintRow(&row);
				cnt++;
			}
		}
	}

	return cnt;
}

long long Service::my_binary_search(Index** idx_arr, const char* value, long long low, long long high)
{
	std::string pivot;
	size_t	mid = 0;

	high--;
	if (high < 0)
		return SEARCH_FAIL;
	while (low <= high)
	{
		mid = low + (high - low) / 2;
		pivot = idx_arr[mid]->phone;
		if (pivot == value)
			return mid;
		else if (pivot > value)
			high = mid - 1;
		else if (pivot < value)
			low = mid + 1;
	}
	return SEARCH_FAIL;
}

int	Service::index_search(Table* table, KeyValue* left, KeyValue* right)
{
	FILE* stream = table->getFilePointer();
	Index** idx_arr = table->getIndexArr();
	long long key_pos;
	size_t	key_offset;
	int		cnt = 0;

	(void)right;
	fseek(stream, 0, SEEK_SET);
	if (!feof(stream))
	{
		Row		row = { 0 };
		key_pos = my_binary_search(idx_arr, left->value, 0, table->getTotalRowCount());
		if (key_pos == SEARCH_FAIL)
			return false;
		key_offset = idx_arr[key_pos]->index_offset;
		fseek(stream, key_offset, SEEK_SET);
		fread(&row, ROW_SIZE, 1, stream);
		UI::PrintRow(&row);
		cnt++;
	}

	return cnt;
}

