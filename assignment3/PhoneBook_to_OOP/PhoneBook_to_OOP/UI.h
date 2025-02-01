#pragma once

#include <iostream>
#include <conio.h>
#include "Table.h"
#include "Service.h"
#include "Row.h"
#include "utils.h"

typedef enum STATUS { FAIL, SUCCESS } STATUS;

class Service;
class DisplayUI;
class Table;

class UI
{
public:
	UI(Service* service, DisplayUI* display_ui);
	~UI(void);

	static void	PrintRow(Row* row);
	static int	PrintMenu(void);

	void	EventLoop(Table* table);
	STATUS	Select(Table* table);
	STATUS	Insert(Table* table);
	STATUS	Update(Table* table);
	STATUS	Delete(Table* table);

private:
	Service* service;
	DisplayUI* display_ui;
	
	UI(void);
	UI(const UI& rhs);
};
