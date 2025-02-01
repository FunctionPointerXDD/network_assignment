#pragma once

#include "Row.h"
#include "Table.h"
#include "UI.h"
#include "DisplayUI.h"
#include "utils.h"
#include "windows.h"

typedef struct KeyValue
{
	char* key;
	char* value;
} KeyValue;

class Table;
class UI;
class DisplayUI;
typedef struct Index Index;
typedef enum STATUS STATUS;

/**
 * 1. parse user input.
 * 2. throw error_msg.
 * 3. throw updated table status to DisplayUI.
 */
class Service
{
public:
	static Service& getInstance(void);
	void	setService(Table* _table, UI* _ui, DisplayUI* _display_ui);

	//parse method
	STATUS	ParseUserInput_For_Select_And_Search(Table* table, char *InputBuffer);
	STATUS	ParseUserInput_For_Insert(Table* table, char *InputBuffer);
	STATUS	ParseUserInput_For_Update(Table* table, char *InputBuffer, int id);
	STATUS	ParseUserInput_To_Delete(Table* table, char *InputBuffer, int id);
	char* get_attribute(char* str);
	char* get_value(char* str);
	int   check_select_input(char** str_arr);

	//search method
	int			left_condition(Row* row, KeyValue* left);
	int			right_condition(Row* row, KeyValue* right);
	int			sequential_search(Table* table, KeyValue* left, KeyValue* right);
	long long	my_binary_search(Index** idx_arr, const char* value, long long low, long long high);
	int			index_search(Table* table, KeyValue* left, KeyValue* right);

private:
	Table* table;
	UI* ui;
	DisplayUI* display_ui; //display nums of recorded row;

	//No use
	Service(void);
	Service(const Service& rhs) = delete;
	~Service(void);
};
