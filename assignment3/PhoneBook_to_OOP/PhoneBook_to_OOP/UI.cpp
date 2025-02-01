
#include "UI.h"

UI::UI(Service* _service, DisplayUI* _display_ui)
	:service(_service), display_ui(_display_ui)
{

}
UI::~UI(void) {}

int	UI::PrintMenu(void)
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

void	UI::EventLoop(Table* table)
{
	STATUS (UI::*query[4])(Table*) = { &UI::Select, &UI::Insert, &UI::Update, &UI::Delete };

	int	menu = 0;
	while ((menu = PrintMenu()) != 0)
	{
		int status = 0;

		printf("\033[2J\033[1;1H");
		status = (this->*query[menu - 1])(table); //horrible c++ code.
		if (status == FAIL)
			puts("Fail");
		else
			puts("Execute");
	}
}

void	UI::PrintRow(Row* row)
{
//	std::cout << " | " << row->id << " | " << row->name << " | " << row->address << " | " << row->phone << " | " << std::endl;
	printf("| %08llu | %-20s | %-30s | %-12s |\n", row->id, row->name, row->address, row->phone);
}

STATUS	UI::Select(Table* table)
{
	char	InputBuffer[256];
	bool	is_and = false;

	printf("SELECT > ");
	fgets(InputBuffer, 256, stdin);
	InputBuffer[strlen(InputBuffer) - 1] = '\0';

	this->service->ParseUserInput_For_Select_And_Search(table, InputBuffer);
}

STATUS	UI::Insert(Table* table)
{
	char* InputBuffer;
		
	InputBuffer = new char[256]();
	printf("INSERT > (Name Address PhoneNumber)\n Value > ");
	fgets(InputBuffer, 256, stdin);
	InputBuffer[strlen(InputBuffer) - 1] = '\0';

	this->service->ParseUserInput_For_Insert(table, InputBuffer);
	delete []InputBuffer;
	return SUCCESS;
}

STATUS	UI::Update(Table* table)
{
	char* InputBuffer = NULL;
	int		id = 0;
	
	printf("Please enter the ID that you want to update...\n ID > ");
	scanf_s("%d%*c", &id);
	printf(" UPDATE > (Name Address PhoneNumber)\n");
	printf("    SET > ");
	InputBuffer = new char[256]();

	fgets(InputBuffer, 256, stdin);
	InputBuffer[strlen(InputBuffer) - 1] = '\0';

	this->service->ParseUserInput_For_Update(table, InputBuffer, id);
	return SUCCESS;
}

STATUS	UI::Delete(Table* table)
{
	int		id = 0;
	char*	InputBuffer = nullptr;
	Row		row = { 0 };
	
	printf("Please enter the ID that you want to delete...\n ID > ");
	scanf_s("%d%*c", &id);

	this->service->ParseUserInput_To_Delete(table, InputBuffer, id);
	return SUCCESS;
}

