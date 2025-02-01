
#include "Table.h"

Table::Table()
{

}

//getter
Index** Table::getIndexArr(void) const
{
	return index_arr;
}

size_t	Table::getIndexEnd(void) const
{
	return index_end;
}

size_t	Table::getIndexArrSize(void) const
{
	return index_arr_size;
}

FILE* Table::getFilePointer(void) const
{
	return fp_start;
}

size_t Table::getFileLength(void) const
{
	return file_length;
}

size_t Table::getTotalRowCount(void) const
{
	return total_row_cnt;
}

bool   Table::getIsTest(void) const
{
	return is_test;
}

//setter
void Table::setIndexArr(Index** index_arr)
{
	this->index_arr = index_arr;
}

void Table::setIndexEnd(size_t end_pos)
{
	this->index_end = end_pos;
}

void Table::setIndexArrSize(size_t size)
{
	this->index_arr_size = size;
}

void Table::setFilePointer(FILE* fp)
{
	this->fp_start = fp;
}

void Table::setFileLength(size_t length)
{
	this->file_length = length;
}

void Table::setTotalRowCount(size_t count)
{
	this->total_row_cnt = count;
}

void Table::setIsTest(bool param)
{
	this->is_test = param;
}

Table& Table::getInstance(void)
{
	static Table instance;
	return instance;
}

void	Table::setMember(Service* _service)
{
	this->service = _service;
}

void	Table::UpdateTableForInsert(Table* table, Row* row)
{
	FILE* stream = table->getFilePointer();
	fseek(stream, 0, SEEK_END);
	if (fwrite(row, ROW_SIZE, 1, stream) == 1)
	{
		table->setFileLength(table->getFileLength() + ROW_SIZE);
		table->setTotalRowCount(table->getTotalRowCount() + 1);
		update_index_for_insert(table, row->phone);
		//return SUCCESS;
	}
	//return FAIL;
}

void	Table::UpdateTableForUpdate(Table* table, Row* row)
{
	FILE* stream = table->fp_start;
	long file_offset = (row->id - 1) * (ROW_SIZE);
	fseek(stream, file_offset, SEEK_SET);
	if (fwrite(row, ROW_SIZE, 1, stream) == 1)
	{
		memcpy(table->index_arr[row->id - 1]->phone, row->phone, PHONE_SIZE);
		table->index_arr[row->id - 1]->index_offset = file_offset;
		sort_index_array(table->index_arr, table->index_end);
		//return SUCCESS;
	}
	//return FAIL;
}

void	Table::UpdateTableForDelete(Table* table, int id)
{
	FILE* stream = table->fp_start;
	long file_offset = (id - 1) * (ROW_SIZE);
	fseek(stream, file_offset, SEEK_SET);

	Row row = { 0 };
	if (fwrite(&row, ROW_SIZE, 1, stream) == 1)
	{
		//deleted index is to be zero...
		memset(&table->index_arr[id - 1]->phone, 0, PHONE_SIZE);
		table->index_arr[id - 1]->index_offset = file_offset;
		sort_index_array(table->index_arr, table->index_end);
		//return SUCCESS;
	}
	//return FAIL;
}

int	Table::resize_index_arr(Table *table, size_t new_size)
{
	Index** src_index_arr;
	size_t	old_size = table->index_end;

	src_index_arr = table->index_arr;
	table->index_arr = new Index * [new_size + 1];

	for (int i = 0; i < old_size; i++)
		table->index_arr[i] = src_index_arr[i];

	for (int i = old_size; i < new_size; i++)
	{
		table->index_arr[i] = new Index();
	}
	table->index_arr[new_size] = NULL;
	delete src_index_arr;
	return 0;
}

void	Table::update_index_for_insert(Table* table, char* value)
{
	if (table->index_end + INDEX_ARRAY_SIZE_START_VALUE > table->index_arr_size)
	{
		table->index_arr_size = table->index_arr_size << 1;
		resize_index_arr(table, table->index_arr_size);
	}
	
	table->index_arr[table->index_end]->index_offset = table->file_length - ROW_SIZE;
	memcpy(table->index_arr[table->index_end]->phone ,value, PHONE_SIZE);
	table->index_end++;
	sort_index_array(table->index_arr, table->index_end);
}

void	Table::swap_index(Index** idx1, Index** idx2)
{
	Index* addr = NULL;
	
	addr = *idx1;
	*idx1 = *idx2;
	*idx2 = addr;
}

void	Table::sort_index_array(Index** index_arr, size_t index_end)
{
	for (int i = 0; i < index_end - 1; ++i)
	{
		for (int j = i + 1; j < index_end; ++j)
		{
			if (index_arr[i]->phone > index_arr[j]->phone)
				swap_index(&index_arr[i], &index_arr[j]);
		}
	}
}

void	Table::sort_index_array_for_test(Index** index_arr, size_t index_end)
{
	bool is_duplicate = false;

	sort_index_array(index_arr, index_end);
	for (int i = 0; i < index_end - 1; ++i)
	{
		if ((index_arr[i]->phone) == (index_arr[i + 1]->phone))
		{
			is_duplicate = true;
			puts(" > A duplicate phone number was found");
			puts(" > Try regenerate number...");
			GenRandomNumber(index_arr[i]->phone);
			GenRandomNumber(index_arr[i + 1]->phone);
		}
	}
	if (is_duplicate == false)
		puts(" > Unique Numbers!");
	else if (is_duplicate == true)
		sort_index_array_for_test(index_arr, index_end);
}

Index** Table::Init_IndexArray(Table *table)
{
	FILE*	stream;
	Index** index_arr;

	if (table->index_end == 0)
	{
		table->index_arr_size = INDEX_ARRAY_SIZE_START_VALUE;
		index_arr = new Index*[table->index_arr_size + 1]();
		for (size_t i = 0; i < table->index_arr_size; i++)
		{
			index_arr[i] = new Index;
			index_arr[i]->index_offset = ((size_t)ROW_SIZE * i);
		}
		return index_arr;
	}
	else
	{
		if (table->index_arr_size < INDEX_ARRAY_SIZE_START_VALUE)
			table->index_arr_size = INDEX_ARRAY_SIZE_START_VALUE;
		index_arr = new Index*[table->index_arr_size + 1]();
	}

	stream = table->fp_start;
	fseek(stream, 0, SEEK_SET);
	for (size_t i = 0; i < table->index_end; ++i)
	{
		index_arr[i] = new Index();
		
		fseek(stream, PHONE_OFFSET, SEEK_CUR);
		fread(index_arr[i]->phone, PHONE_SIZE, 1, stream);
		index_arr[i]->index_offset = (ROW_SIZE * i);
	}
	index_arr[table->index_arr_size] = NULL;

	if (table->is_test)
		sort_index_array_for_test(index_arr, table->index_end);
	else
		sort_index_array(index_arr, table->index_end);

	return index_arr;
}

void Table::open_db(Table *table)
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
			table->is_test = true;
			CreateRandomPhoneBook(table);
		}
		else //void file
		{
			fopen_s(&fp, "TEST_PhoneBook.dat", "wb+");
			table->fp_start = fp;
			table->file_length = 0;
			table->total_row_cnt = 0;
			table->index_end = 0;
			table->index_arr = Init_IndexArray(table);
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
		table->index_arr = Init_IndexArray(table);
	}
}

void	Table::convert_file(Table *table)
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

void	Table::close_db(Table* table)
{
	Row row = { 0 };
	size_t deleted_rows = 0;
	double deleted_rate = 0;

	fseek(table->fp_start, 0, SEEK_SET);
	while (!feof(table->fp_start))
	{
		memset(&row, 0, ROW_SIZE);
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
		delete (table->index_arr[i]);
	delete (table->index_arr);
	table->index_arr = 0;
}

