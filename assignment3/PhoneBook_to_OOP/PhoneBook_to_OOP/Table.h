#pragma once

#include <iostream>
#include <string>
#include "Row.h"
#include "Service.h"
#include "GenRandomPhonebook.h"


//index model
typedef struct Index
{
	char phone[PHONE_SIZE];
	size_t	index_offset;
} Index;

class Service;

class Table
{
public:
	~Table(void) {};

	static Table& getInstance(void);
	void	setMember(Service *service);

	virtual Index** Init_IndexArray(Table* table);
	virtual	void	UpdateTableForInsert(Table* table, Row* row);
	virtual	void	UpdateTableForUpdate(Table* table, Row* row);
	virtual	void	UpdateTableForDelete(Table* table, int id);

	virtual int		resize_index_arr(Table* table, size_t new_size);
	virtual void	update_index_for_insert(Table* table, char *value);
	virtual void	swap_index(Index** idx1, Index** idx2);
	virtual void	sort_index_array(Index** index_arr, size_t index_cnt);
	virtual void	sort_index_array_for_test(Index** index_arr, size_t index_cnt);
	virtual void	open_db(Table* table);
	virtual void	convert_file(Table* table);
	virtual void	close_db(Table* table);

	//getter
	Index** getIndexArr(void) const;
	size_t	getIndexEnd(void) const;
	size_t	getIndexArrSize(void) const;
	FILE*   getFilePointer(void) const;
	size_t  getFileLength(void) const;
	size_t  getTotalRowCount(void) const;
	bool    getIsTest(void) const;

	//setter
	void setIndexArr(Index** index_arr);
	void setIndexEnd(size_t end_pos);
	void setIndexArrSize(size_t size);
	void setFilePointer(FILE* fp);
	void setFileLength(size_t length);
	void setTotalRowCount(size_t count);
	void setIsTest(bool param);

private:
	Index** index_arr = nullptr;
	size_t	index_end = 0; //The last position in an index array that contains information
	size_t	index_arr_size = 0; // allocated index array size;
	FILE*   fp_start = nullptr;
	size_t	file_length = 0;
	size_t	total_row_cnt = 0;
	bool    is_test = false;

	Service* service = nullptr; //throw updated status to service.

	//No use
	Table(void);
	Table(const Table& rhs) = delete;
};

