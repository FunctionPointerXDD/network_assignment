#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <wincrypt.h>
#include "PhoneBook.h"
#include "random_phonebook.h"
#include "ui.h"
#include "utils.h"

int main(void)
{
	//memory leak check in Debug mode
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Table	table;
	
	open_db(&table);
	event_loop(&table);
	close_db(&table);
	return 0;
}

