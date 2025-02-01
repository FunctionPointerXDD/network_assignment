#include "GenRandomPhonebook.h"
#include "Row.h"
#include "UI.h"
#include "DisplayUI.h"
#include "Service.h"
#include "Table.h"
#include "utils.h"

int main(void)
{
	Service& service = Service::getInstance();
	Table& table = Table::getInstance();
	DisplayUI display_ui;
	UI ui(&service, &display_ui);

	service.setService(&table, &ui, &display_ui);

	table.open_db(&table);
	ui.EventLoop(&table);
	table.close_db(&table);
	return 0;
}

